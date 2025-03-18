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
    float4 outPosW : POSITION0; //world position
    float4 outPosL : POSITION1; //light space position
    float4 outNormal : NORMAL;
    float4 outTangent : TANGENT0;
    float4 outBiTangent : BINORMAL;
    float4x4 outTBN : TANGENT1;
    float2 outTexCoords : TEXCOORD;
};

VertexOutput vsMain(VertexInput vin)
{
    VertexOutput vout;
    
    uint index = constants.shape;
    
    float4x4 mvp = mul(objectModel[index], mul(cameraView, cameraProjection));
    float4x4 lvp = mul(objectModel[index], mul(lightSourceView, lightSourceProjection));
    float4 posH = mul(vin.inPos, mvp);
    float4 posW = mul(vin.inPos, objectModel[index]);
    float4 posL = mul(vin.inPos, lvp);
    float4 normal = mul(vin.inNormal, objectInverseModel[index]);
    float4 tangent = normalize(mul(vin.inTangent, objectInverseModel[index]));
    
    //re-orthogonalize using the gram-schmdit method.
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    float4 bitangent = float4(cross(tangent.xyz, normal.xyz), 0.0f);
    
    vout.outPosH = posH;
    vout.outPosW = posW;
    vout.outPosL = posL;
    vout.outNormal = normal;
    vout.outTangent = tangent;
    vout.outBiTangent = bitangent;
    vout.outTBN = float4x4(tangent, bitangent, normal, float4(0.0f, 0.0f, 0.0f, 1.0f));
    vout.outTexCoords = vin.inTexCoords;
    
    return vout;
}