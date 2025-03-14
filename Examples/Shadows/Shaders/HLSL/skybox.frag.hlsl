#include "resources.h.hlsl"

struct VertexOutput
{
    float4 outPosH : SV_Position;
    float3 outTexCoords : TEXCOORD;
};

float4 psMain(VertexOutput vout) : SV_Target
{
    return gTextureCube.Sample(gSampler, vout.outTexCoords);
}