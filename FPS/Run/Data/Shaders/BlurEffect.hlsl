cbuffer frame_constants : register(b0)	//Index 0 is time
{
    float SYSTEM_TIME_SECONDS;
    float SYSTEM_TIME_DELTA_SECONDS;
    float GAMMA;
    float FOGNEAR;

    float3 FOGCOLOR;
    float FOGFAR;
};


// input to the vertex shader - for now, a special input that is the index of the vertex we're drawing
struct vs_input_t
{
    uint vidx : SV_VERTEXID; // SV_* stands for System Variable (ie, built-in by D3D11 and has special meaning)
                             // in this case, is the the index of the vertex coming in.
};

// Textures & Samplers are also a form of constant
// data - unform/constant across the entire call
// tSlots are for big things
Texture2D <float4> tDiffuse   : register(t0);   // color of the surface
SamplerState sSampler : register(s0);     // sampler are rules on how to sample color per pixel


                                          // Output from Vertex Shader, and Input to Fragment Shader
                                          // For now, just position
struct VertexToFragment_t
{
    // SV_POSITION is a semantic - or a name identifying this variable. 
    // Usually a semantic can be any name we want - but the SV_* family
    // usually denote special meaning (SV = System Variable I believe)
    // SV_POSITION denotes that this is output in clip space, and will be 
    // use for rasterization.  When it is the input (pixel shader stage), it will
    // actually hold the pixel coordinates.
    float4 position : SV_POSITION;
    float2 uv : UV; 
};

//--------------------------------------------------------------------------------------
// constants
//--------------------------------------------------------------------------------------
// The term 'static' refers to this an built into the shader, and not coming
// from a contsant buffer - which we'll get into later (if you remove static, you'll notice
// this stops working). 
static float3 POSITIONS[3] = {
    float3(-1.0f, -1.0f, 0.0f), 
    float3( 3.0f, -1.0f, 0.0f), 
    float3(-1.0f,  3.0f, 0.0f)
};

static float2 UVS[3] = {
    float2( 0.0f,  1.0f ), 
    float2( 2.0f,  1.0f ), 
    float2( 0.0f,  -1.0f )
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VertexToFragment_t VertexFunction( vs_input_t input )
{
    VertexToFragment_t v2f = (VertexToFragment_t)0;

    // The output of a vertex shader is in clip-space, which is a 4D vector
    // so we need to convert out input to a 4D vector.
    v2f.position = float4( POSITIONS[input.vidx], 1.0f );
    v2f.uv = UVS[input.vidx]; 

    // And return - this will pass it on to the next stage in the pipeline;
    return v2f;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
// If I'm only returning one value, I can optionally just mark the return value with
// a SEMANTIC - in this case, SV_TARGET0, which means it is outputting to the first colour 
// target.
float4 FragmentFunction( VertexToFragment_t input ) : SV_Target0 // semeantic of what I'm returning
{
    float blurSize = 0.05f;
    float blurIncrement = blurSize * 0.1f;
    float4 blurColor = float4(0.f,0.f,0.f,0.f);
    float2 currentUV = input.uv;

    float2 startOfBlurUV = input.uv - ( blurSize * 0.5f );
    for( float yIndex = 0; yIndex < 11; yIndex++ )
    {
        currentUV.y = (blurIncrement * yIndex) + startOfBlurUV.y;
        for( float xindex = 0; xindex < 11; xindex++ )
        {
            currentUV.x = (blurIncrement * xindex) + startOfBlurUV.x;
            blurColor += tDiffuse.Sample( sSampler, currentUV );
        }
    }

    blurColor /= 121.f;
    return blurColor;

    //float4 color = tDiffuse.Sample( sSampler, input.uv );
    //return color; 

}