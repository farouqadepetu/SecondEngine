#include "resources.h.hlsl"

struct VertexOutput
{
    float4 outPosH : SV_Position;
    float4 outPosW : POSITION;
};

float psMain(VertexOutput vout) : SV_Depth
{
    float lightDistance = length(vout.outPosW.xyz - lightPosition[constants.lightIndex].xyz);
    
    lightDistance = lightDistance / 10.0f;
    
    return lightDistance;
}