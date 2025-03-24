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
    float4 outPosW : POSITION;
};

VertexOutput vsMain(VertexInput vin)
{
    VertexOutput vout;
    
    uint objectIndex = constants.objectIndex;
    uint lightIndex = constants.lightIndex;
    
    float4x4 shadowMatrix = mul(objectModel[objectIndex], mul(lightSourceView[lightIndex], lightSourceProjection[lightIndex]));
    float4 posH = mul(vin.inPos, shadowMatrix);
    float4 posW = mul(vin.inPos, objectModel[objectIndex]);
    
    vout.outPosH = posH;
    vout.outPosW = posW;
    
    return vout;
}