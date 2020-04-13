//--------------------------------------------------------------------------------------
// Stream Input
// ------
// Stream Input is input that is walked by the vertex shader.  
// If you say "Draw(3,0)", you are telling to the GPU to expect '3' sets, or 
// elements, of input data.  IE, 3 vertices.  Each call of the VertxShader
// we be processing a different element. 
//--------------------------------------------------------------------------------------

// inputs are made up of internal names (ie: uv) and semantic names
// (ie: TEXCOORD).  "uv" would be used in the shader file, where
// "TEXCOORD" is used from the client-side (cpp code) to attach ot. 
// The semantic and internal names can be whatever you want, 
// but know that semantics starting with SV_* usually denote special 
// inputs/outputs, so probably best to avoid that naming.
struct vs_input_t 
{
	// we are not defining our own input data; 
	float3 position     : POSITION; 
	float4 color        : COLOR; 
	float2 uv           : TEXCOORD; 

	float3 tangent		: TANGENT;
	float3 bitangent	: BITANGENT;
	float3 normal		: NORMAL;
}; 

static float SHIFT = 0.75f;
cbuffer time_constants : register(b0)	//Index 0 is time
{
	float SYSTEM_TIME_SECONDS;
	float SYSTEM_TIME_DELTA_SECONDS;
	float GAMMA;
};

cbuffer camera_constants : register(b1)
{
//	float2 orthoMin;
//	float2 orthoMax;
	float4x4 PROJECTION; // CAMERA_TO_CLIP_TRANSFORM in the model view projection
	float4x4 VIEW;
	float3 CAMERAPOSITION;

	float pad00;
};

cbuffer model : register(b2)
{
	float4x4 MODEL;
	float4 TINT;

	float SPECULARFACTOR;
	float SPECULARPOWER;
};


static const int MAX_LIGHTS = 8;
struct light_t
{
	float3 worldPosition;
	float pad00; //not required

	float3 color;
	float intensity;

	float3 direction;
	float isDirectional; //Check if its a point light

	//Spot light
	float cosInnerAngle;
	float cosOuterAngle;
	float2 pad01; //not required
};

cbuffer lightConstants : register(b3)
{
	float4 AMBIENT;
	light_t LIGHT[MAX_LIGHTS];
	float3 ATTENUATION;

	float pad01;
}

struct dissolve_t
{
	float amount; //(0 to 1)
	float edgeWidth;
	float3 edgeColor;

	float3 pad00;
};

cbuffer materialConstants : register(b5)
{
	dissolve_t DISSOLVE;
}


// Textures & Samplers are also a form of constant
// data - unform/constant across the entire call
// tSlots are for big things
Texture2D <float4> tDiffuse	: register(t0);	// color of the surface
Texture2D <float4> tNormal : register(t1);

//User defined
Texture2D <float> tPattern : register(t8);
SamplerState sSampler : register(s0);		// sampler are rules on how to sample color per pixel

//--------------------------------------------------------------------------------------
// Programmable Shader Stages
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// for passing data from vertex to fragment (v-2-f)
struct v2f_t 
{
	float4 position			: SV_POSITION; 
	float4 color			: COLOR; 
	float2 uv				: UV; 


	float3 worldPosition	: WORLDPOS;
	float3 worldNormal		: WORLD_NORMAL;
	float3 worldTangent		: WORLD_TANGENT;
	float3 worldBitangent	: WORLD_BITANGENT;
	float3 cameraPosition	: CAMERAPOS;
}; 

float RangeMap( float val, float inMin, float inMax, float outMin, float outMax )
{
	float inRange = inMax - inMin;
	float outRange = outMax - outMin;
	return ((val - inMin) / inRange) * outRange + outMin;
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

//--------------------------------------------------------------------------------------
// Fragment Shader
// 
// SV_Target0 at the end means the float4 being returned
// is being drawn to the first bound color target.
float4 FragmentFunction( v2f_t input ) : SV_Target0
{
	float edgeWidth = DISSOLVE.edgeWidth;
	float dissolveAmount = DISSOLVE.amount;
	float3 dissolveEdgeColor = DISSOLVE.edgeColor;

	float range = 1.f + edgeWidth;
	float minDissolve = -edgeWidth + range * dissolveAmount;
	float maxDissolve = minDissolve + edgeWidth;
	float height = tPattern.Sample( sSampler, input.uv );
	clip( height - minDissolve );
	float dissolveLerpValue = (height - minDissolve)/(maxDissolve - minDissolve);
	dissolveLerpValue = clamp( dissolveLerpValue, 0.f, 1.f );


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
		float3 dot3 = max( 0.f, dot( dirToLight, worldNormal ) );
		dot3 *= (LIGHT[lightIndex].color * LIGHT[lightIndex].intensity);

		float distanceToLight = length( lightPosition - input.worldPosition );
		float att3 = min( 1.f, 1.f / (ATTENUATION.x + ATTENUATION.y*distanceToLight + ATTENUATION.z*distanceToLight*distanceToLight ) );
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
	finalColor = lerp( dissolveEdgeColor, finalColor, dissolveLerpValue );
	//normalize(incidentReflect);
	//float3 finalColor = directionToEye.xyz;
	finalColor = pow( max( 0.f, finalColor ), 1/GAMMA );

	return float4( finalColor.xyz, surfaceAlpha );
}