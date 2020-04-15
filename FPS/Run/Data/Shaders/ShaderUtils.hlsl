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

	float2 pad01;

	float3 attenuation;
	
	float1 pad02; //not required
};

cbuffer lightConstants : register(b3)
{
	float4 AMBIENT;
	light_t LIGHT[MAX_LIGHTS];
	//float3 ATTENUATION;

	//float pad01;
}


// Textures & Samplers are also a form of constant
// data - unform/constant across the entire call
// tSlots are for big things
Texture2D <float4> tDiffuse	: register(t0);	// color of the surface
Texture2D <float4> tNormal : register(t1);
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

float3 NormalColorToVector( float3 normalColor )
{
	normalColor.x = RangeMap( normalColor.x, 0.f, 1.f, -1.f, 1.f );
	normalColor.y = RangeMap( normalColor.y, 0.f, 1.f, -1.f, 1.f );

	return normalColor;
}