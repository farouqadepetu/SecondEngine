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
    return float4(0.0f, 0.5f, 0.5f, 1.0f);

}