#include "ShaderUtils.hlsl"

Texture2D <float4> tMaterial0_d : register(t8);
Texture2D <float4> tMaterial0_n : register(t9);
Texture2D <float4> tMaterial1_d : register(t10);
Texture2D <float4> tMaterial1_n : register(t11);
Texture2D <float4> tMaterial2_d : register(t12);
Texture2D <float4> tMaterial2_n : register(t13);

//--------------------------------------------------------------------------------------
// Vertex Shader
v2f_t VertexFunction( vs_input_t input )
{
	v2f_t v2f = (v2f_t)0;

	//Need to add a normal in with same operations vs_input_t should be replaced to hold a normal and named Vertex_PCUTBN
	float4 localPos = float4( input.position, 1);
	float4 worldPos = mul(MODEL, localPos);
	float4 cameraPos = mul(VIEW, worldPos);
	float4 clipPos = mul(PROJECTION, cameraPos);	// might have a w (usually 1 for now)

	//normal is currently in model/local space
	float4 localNormal = float4( input.normal, 0.f );
	float4 localTangent = float4( input.tangent, 0.f );
	float4 localBitangent = float4( input.bitangent, 0.f );
	float4 worldNormal = mul( MODEL, localNormal );
	float4 worldTangent = mul( MODEL, localTangent );
	float4 worldBitangent = mul( MODEL, localBitangent );


	// forward vertex input onto the next stage
	v2f.position = clipPos;
	v2f.color = input.color * TINT; 
	v2f.uv = input.uv; 
	v2f.worldPosition = worldPos.xyz;
	v2f.worldNormal = worldNormal.xyz;
	v2f.worldTangent = worldTangent.xyz;
	v2f.worldBitangent = worldBitangent.xyz;

	return v2f;
}

//--------------------------------------------------------------------------------------
// Fragment Shader
// 
// SV_Target0 at the end means the float4 being returned
// is being drawn to the first bound color target.
float4 FragmentFunction( v2f_t input ) : SV_Target0
{
	float2 z_uv = frac( input.worldPosition.xy );
	//z_uv.y = 1.f - z_uv.y;
	float2 x_uv = frac( input.worldPosition.zy );
	//x_uv.x = 1.f - x_uv.x;
	//x_uv.y = 1.f - x_uv.y;
	float2 y_uv = frac( input.worldPosition.xz );

	//need colors from each plane
	float4 x_color = tMaterial0_d.Sample( sSampler, x_uv );
	float4 y_color = tMaterial1_d.Sample( sSampler, y_uv );
	float4 z_color = tMaterial2_d.Sample( sSampler, z_uv );

	float3 x_normal = tMaterial0_n.Sample( sSampler, x_uv ).xyz;
	float3 y_normal = tMaterial1_n.Sample( sSampler, y_uv ).xyz;
	float3 z_normal = tMaterial2_n.Sample( sSampler, z_uv ).xyz;

	float3 weights = normalize( input.worldNormal );
	weights = weights * weights;



	x_normal = NormalColorToVector(x_normal); 
	y_normal = NormalColorToVector(y_normal);
	z_normal = NormalColorToVector(z_normal);

	float3x3 xtbn = float3x3(
		float3( 0.f, 0.f, -1.f ),
		float3( 0.f, 1.f, 0.f ),
		float3( 1.f, 0.f, 0.f )
		);

	float3x3 ytbn = float3x3(
		float3( 1.f, 0.f, 0.f ),
		float3( 0.f, 0.f, -1.f ),
		float3( 0.f, 1.f, 0.f )
		);

	z_normal.z *= sign( input.worldNormal.z );
	//z_color.z *= sign(input.worldNormal.z);

	x_normal = mul( x_normal, xtbn );
	x_normal.x *= sign( input.worldNormal.x );
	//x_color.x *= sign(input.worldNormal.x);

	y_normal = mul( y_normal, ytbn );
	y_normal.y *= sign( input.worldNormal.y );
	//y_color.y *= sign(input.worldNormal.y);


	float4 beforeLightingColor 
		= weights.x * x_color
		+ weights.y * y_color
		+ weights.z * z_color;

	float3 worldNormal 
		= float3(
		  weights.x * x_normal
		+ weights.y * y_normal
		+ weights.z * z_normal
		);

	worldNormal = normalize( worldNormal );
	//Calculate lighting with normal and color

	float3 diffuse = AMBIENT.xyz * AMBIENT.w;
	//return finalColor;

	for( int lightIndex = 0; lightIndex < MAX_LIGHTS; lightIndex++ )
	{
		float3 lightPosition = LIGHT[lightIndex].worldPosition;
		float3 dirToLight = normalize( lightPosition - input.worldPosition );
		float3 lightDirection = LIGHT[lightIndex].direction;
		float cosThetaInner = LIGHT[lightIndex].cosInnerAngle;
		float cosThetaOuter = LIGHT[lightIndex].cosOuterAngle;

		//Check for directional light
		dirToLight = lerp( dirToLight, -lightDirection, LIGHT[lightIndex].isDirectional );
		float3 dot3 = max( 0.f, dot( dirToLight, worldNormal ) );
		dot3 *= (LIGHT[lightIndex].color * LIGHT[lightIndex].intensity);

		float distanceToLight = length( lightPosition - input.worldPosition );

		float3 attenuation = LIGHT[lightIndex].attenuation;
		float att3 = min( 1.f, 1.f / (attenuation.x + attenuation.y*distanceToLight + attenuation.z*distanceToLight*distanceToLight ) );
		//spot light calculation
		float cosTheta = dot( normalize( lightDirection ), normalize(input.worldPosition - lightPosition) );
		float spotLightAttenuation = RangeMap( cosTheta, cosThetaOuter, cosThetaInner, 0.f, 1.f );
		spotLightAttenuation = saturate( spotLightAttenuation );
		att3 *= spotLightAttenuation;
		dot3 *= att3;

		diffuse += dot3;

		//specular
		float3 incidentVector = -dirToLight;
		float3 incidentReflect = reflect( incidentVector, worldNormal );
		float3 directionToEye = normalize(CAMERAPOSITION - input.worldPosition);

		//Used to solve bug where low specular power could cause the light to be seen from opposite side of sphere
		float incidentCosAngle = dot( worldNormal, dirToLight );
		//-.25f and 0.1 are arbitrary values
		float specularValue = smoothstep( -.25f, 0.1f, incidentCosAngle ) * dot(incidentReflect, directionToEye);

		float specular = SPECULARFACTOR * pow( max( 0.f,  specularValue ), SPECULARPOWER);
		specular *= att3;
		specular *= LIGHT[lightIndex].intensity;


		diffuse.xyz += specular;

	}
	float distanceToEye = length( CAMERAPOSITION - input.worldPosition );
	float fogMix = saturate( RangeMap( distanceToEye, FOGNEAR, FOGFAR, 0.f, 1.f ) );


	diffuse = min( float3( 1, 1, 1 ), diffuse );
	diffuse = saturate( diffuse ); //Saturate clamps to 1 (ask about?)

	float3 litColor = diffuse * beforeLightingColor.xyz;
	litColor = pow( max( 0.f, litColor ), 1/GAMMA );

	float3 finalColor = lerp( litColor, FOGCOLOR, fogMix );
	//return float4( distanceToEye.xxx, surfaceAlpha );
	return float4( finalColor.xyz, beforeLightingColor.w );
	//return float4( worldNormal.xyz, 1.f );
}