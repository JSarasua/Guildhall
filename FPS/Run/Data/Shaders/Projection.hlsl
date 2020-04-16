#include "ShaderUtils.hlsl"

struct projection_t
{
	float4x4 projection;

	float3 position;
	//float pad00;

	//float3 normal;
	float strength;
};

cbuffer materialData : register(b5)
{
	projection_t PROJECTIONDATA;
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
Texture2D <float4> tProject : register(t14);
//tProject
//borderSampler

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
	float4 clipPos = mul( PROJECTIONDATA.projection, float4( input.worldPosition, 1.f ) );
	float local_z = clipPos.w;
	float3 ndc = clipPos.xyz / local_z;
	float2 uv = (ndc.xy + float2(1.f, 1.f)) * 0.5f;

	float4 textureColor = tProject.Sample( sSampler, uv );
	

	//branchless way to only work on things inside box
	float ublend = step( 0, uv.x ) * (1.f - step(1.f, uv.x ));
	float vblend = step( 0, uv.y ) * (1.f - step( 1.f, uv.y ));
	float blend = ublend * vblend;


	float3 dirToProjector = normalize( PROJECTIONDATA.position - input.worldPosition );
	float3 normal = normalize( input.worldNormal );
	float facing = step( 0, dot( dirToProjector, normal ) );
	blend *= facing;


	blend *= step( 0.f, 1.f - ndc.z );
	
	float4 finalColor = lerp( 0.f.xxxx, textureColor, blend );
	return finalColor;
}