#include "shapesResources.h.hlsl"

Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

float4 psMain(VertexOutput vout) : SV_Target
{
    return gTexture.Sample(gSampler, vout.outputTexCoords);
}