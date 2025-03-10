#include "resources.h.hlsl"

struct VertexInput
{
    float4 inPos : POSITION;
};

struct VertexOutput
{
    float4 outPosH : SV_Position;
};

VertexOutput vsMain(VertexInput vin)
{
    VertexOutput vout;
    float4x4 model = pointLight.model * (constants.currentLightSource == POINT)+ 
    spotLight.model * (constants.currentLightSource == SPOTLIGHT);
    
    float4x4 mvp = mul(model, mul(view, projection));
    float4 posH = mul(vin.inPos, mvp);
    
    vout.outPosH = posH;
    
    return vout;
}