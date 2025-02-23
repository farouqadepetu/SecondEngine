#include "resources.h.hlsl"

struct VertexInput
{
    float4 inputPosition : POSITION;
    float4 inputNormal : NORMAL;
    float2 inputTexCoords : TEXCOORD;
};

struct VertexOutput
{
    float4 outputPosition : SV_Position;
    float4 outputNormal : NORMAL;
    float3 outputTexCoords : TEXCOORD;
};

VertexOutput vsMain(VertexInput vin)
{
    VertexOutput vout;
    float4x4 mvp = mul(model, mul(view, projection));
    float4 posH = mul(vin.inputPosition, mvp);
    vout.outputPosition = posH.xyww; //make z = w so that when perspective divide happens z/w = 1 which makes the skybox always on the far plane.
    vout.outputNormal = vin.inputNormal;
    vout.outputTexCoords = float3(vin.inputPosition.x, vin.inputPosition.y, vin.inputPosition.z);
    
    return vout;
}