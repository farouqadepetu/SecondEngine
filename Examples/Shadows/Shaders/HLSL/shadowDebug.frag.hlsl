#include "resources.h.hlsl"

struct VertexOutput
{
    float4 outPosH : SV_Position;
    float2 outTexCoords : TEXCOORD;
};

float4 psMain(VertexOutput vout) : SV_Target
{
    float depth;
    if (constants.currentLightSource == DIRECTIONAL_LIGHT)
    {
        depth = gShadowMap.Sample(gSampler, vout.outTexCoords).r;
    }
    else if (constants.currentLightSource == POINT_LIGHT)
    {
        depth = gShadowMapPL[constants.debugIndex].Sample(gSampler, vout.outTexCoords).r;
        depth = depth * 2.0f - 1.0f; // Back to NDC 
        depth = (2.0f * 1.0f * 10.0f) / (10.0f + 1.0f - depth * (10.0f - 1.0f));
        depth = depth / 10.0f;
    }

    return float4(depth, depth, depth, 1.0f);
}