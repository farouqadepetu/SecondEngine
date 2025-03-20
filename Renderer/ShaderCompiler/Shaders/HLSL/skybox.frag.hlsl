#include "resources.h.hlsl"

struct VertexOutput
{
    float4 outputPosition : SV_Position;
    float4 outputNormal : NORMAL;
    float3 outputTexCoords : TEXCOORD;
};

float4 psMain(VertexOutput vout) : SV_Target
{
    return gTextureCube.Sample(gSampler, vout.outputTexCoords);
}