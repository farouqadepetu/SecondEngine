#include "resources.h.hlsl"

struct VertexInput
{
    float4 inPos : POSITION;
    float4 inNormal : NORMAL;
    float4 inTangent : TANGENT;
    float2 inTexCoords : TEXCOORD;
};

struct VertexOutput
{
    float4 outPosH : SV_Position;
    float3 outTexCoords : TEXCOORD;
};

VertexOutput vsMain(VertexInput vin)
{
    VertexOutput vout;
    float4x4 mvp = mul(objectModel, mul(cameraView, cameraProjection));
    float4 posH = mul(vin.inPos, mvp);
    
    //make z = w so that when perspective divide happens z/w = 1 which makes the skybox always on the far plane.
    vout.outPosH = posH.xyww;
    vout.outTexCoords = float3(vin.inPos.x, vin.inPos.y, vin.inPos.z);
    
    return vout;
}