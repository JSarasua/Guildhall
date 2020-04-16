#include "ShaderUtils.hlsl"

cbuffer materialData : register(b5)
{
	projection
	position
	normal
	strength
}
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
tProject
borderSampler
//Requires all objects to be drawn again
//use exact equals for comparison

//in code (filter_mode_linear, filter mode nearest, border_color
//--------------------------------------------------------------------------------------
// Fragment Shader
// 
// SV_Target0 at the end means the float4 being returned
// is being drawn to the first bound color target.
float4 FragmentFunction( v2f_t input ) : SV_Target0
{
	float4 clipPos = float4( input.worldPosition, 1.f ) * PROJECTION_MATRIX:
	float local_z = clip_pos.w;
	float3 ndc = clip_pos.xyz / local_z;
	float2 uv = (ndc.xy + float2(1.f)) * 0.5f;
	//bug of backside is colored
	//bug of everything is white
	
	//branchless way to only work on things inside box
	float ublend = step( 0, uv.x ) * (1.f - step(1.f, uv.x ));
	float vblend = step( 0, uv.y ) * (1.f - step( 1.f, uv.y ));
	float blend = ublend * vblend;
	//branch way to fix backside
	float4 textureColor = Ssample( sample, tProject, uv );
	if( uv >= vec2( 0.f ) && uv <= vec2( 1.f, 1.f ) )
	{

	}
	else
	{
		textureColor = float4(0.f, 0.f, 0.f, 0.f);
	}


	float3 dirToProjector = normalize( PROJECTOR_POSITION - input.worldPosition );
	float3 normal = normalize( input.worldNormal );
	float facing = step( 0, dot( dirToProjector, normal ) );
	blend *= facing;

	//forseth flipped this
	blend *= step( 0.f, ndc.z );


	float4 finalColor = lerp( 0.f.xxxx, textureColor, blend );

	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	float4 textureColor = tDiffuse.Sample( sSampler, input.uv );
	textureColor = pow( max(0.f, textureColor), GAMMA );
	float4 textureNormal = tNormal.Sample( sSampler, input.uv );
	float3 surfaceColor = (input.color * textureColor).xyz; //tint our texture color
	float surfaceAlpha = (input.color.a * textureColor.a);

	float3 diffuse = AMBIENT.xyz * AMBIENT.w; // ambient color * ambient intensity
	
	
	float3 surfaceNormal = textureNormal.xyz;
	surfaceNormal.x = RangeMap( surfaceNormal.x, 0.f, 1.f, -1.f, 1.f );
	surfaceNormal.y = RangeMap( surfaceNormal.y, 0.f, 1.f, -1.f, 1.f );
	
	

	float3 normal = normalize( input.worldNormal );
	float3 tangent = normalize( input.worldTangent );
	float3 bitangent = normalize( input.worldBitangent );

	float3x3 tbn = float3x3( tangent, bitangent, normal );
	float3 worldNormal = mul( surfaceNormal, tbn );
	//Added in dot3 factor
	
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


	diffuse = min( float3( 1, 1, 1 ), diffuse );
	diffuse = saturate( diffuse ); //Saturate clamps to 1 (ask about?)
	
	float3 finalColor = diffuse * surfaceColor;
	finalColor = pow( max( 0.f, finalColor ), 1/GAMMA );

	return float4( finalColor.xyz, surfaceAlpha );
}