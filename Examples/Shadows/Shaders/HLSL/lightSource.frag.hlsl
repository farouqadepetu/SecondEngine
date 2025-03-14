#include "resources.h.hlsl"

struct VertexOutput
{
    float4 outPosH : SV_Position;
};

float4 psMain(VertexOutput vout) : SV_Target
{
    return pointLight.color;
}