#include "ShaderUtils.hlsl"

struct fresnel_t
{
	float3 color;
	float power;
	float factor;

	float3 pad00;

};

cbuffer materialConstants : register(b5)
{
	fresnel_t FRESNEL;
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
	//float4 localTangent = float4( input.tangent, 0.f );
	//float4 localBitangent = float4( input.bitangent, 0.f );
	float4 worldNormal = mul( MODEL, localNormal );
	//float4 worldTangent = mul( MODEL, localTangent );
	//float4 worldBitangent = mul( MODEL, localBitangent );


	// forward vertex input onto the next stage
	v2f.position = clipPos;
	//v2f.color = input.color * TINT; 
	//v2f.uv = input.uv; 
	v2f.worldPosition = worldPos.xyz;
	v2f.worldNormal = worldNormal.xyz;
	//v2f.worldTangent = worldTangent.xyz;
	//v2f.worldBitangent = worldBitangent.xyz;

	return v2f;
}

//--------------------------------------------------------------------------------------
// Fragment Shader
// 
// SV_Target0 at the end means the float4 being returned
// is being drawn to the first bound color target.
float4 FragmentFunction( v2f_t input ) : SV_Target0
{
	float3 directionToEye = normalize(CAMERAPOSITION - input.worldPosition);
	float fresnel = length( cross( directionToEye, input.worldNormal ) );

	fresnel = pow( fresnel , FRESNEL.power );
	return float4( FRESNEL.color , fresnel );
}