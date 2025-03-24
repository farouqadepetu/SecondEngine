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
    float2 outTexCoords : TEXCOORD;
};

VertexOutput vsMain(VertexInput vin, uint id : SV_VertexID)
{
    VertexOutput vout; 
    
    float2 uv = float2((id << 1) & 2, id & 2);
    float4 posH = float4(uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
    
    vout.outPosH = posH;
    vout.outTexCoords = uv;
    
    return vout;
}