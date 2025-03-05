#include "resources.h.hlsl"

struct VertexOutput
{
    float4 outputPosition : SV_Position;
    float4 outputNormal : NORMAL;
    float2 outputTexCoords : TEXCOORD;
    float3 outputTexCoords3D : TEXCOORD1;
};

float4 psMain(VertexOutput vout) : SV_Target
{
    if(isShape2D)
        return gTexture2D.Sample(gSampler, vout.outputTexCoords);
    else
        return gTextureCube.Sample(gSampler, vout.outputTexCoords3D);
}