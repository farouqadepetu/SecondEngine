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
    float4 outPosW : POSITION; //world position
    float4 outNormal : NORMAL;
    float4 outTangent : TANGENT0;
    float4 outBiTangent : BINORMAL;
    float4x4 outTBN : TANGENT1;
    float2 outTexCoords : TEXCOORD;
};

VertexOutput vsMain(VertexInput vin)
{
    VertexOutput vout;
    
    float4x4 mvp = mul(model, mul(view, projection));
    float4 posH = mul(vin.inPos, mvp);
    float4 posW = mul(vin.inPos, model);
    float4 normal = normalize(mul(vin.inNormal, transposeInverseModel));
    float4 tangent = normalize(mul(vin.inTangent, transposeInverseModel));
    
    //re-orthogonalize using the gram-schmdit method.
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    float4 bitangent = normalize(float4(cross(tangent.xyz, normal.xyz), 0.0f));
    
    vout.outPosH = posH;
    vout.outPosW = posW;
    vout.outNormal = normal;
    vout.outTangent = tangent;
    vout.outBiTangent = bitangent;
    vout.outTBN = float4x4(tangent, bitangent, normal, float4(0.0f, 0.0f, 0.0f, 1.0f));
    vout.outTexCoords = vin.inTexCoords;
    
    return vout;
}