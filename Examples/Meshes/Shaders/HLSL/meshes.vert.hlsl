#include "resources.h.hlsl"

struct VertexInput
{
    float4 inputPosition : POSITION;
    float4 inputNormal : NORMAL;
    float4 inputTangent : TANGENT;
    float2 inputTexCoords : TEXCOORD;
};

struct VertexOutput
{
    float4 outputPosition : SV_Position;
    float4 outputNormal : NORMAL;
    float2 outputTexCoords : TEXCOORD;
    float3 outputTexCoords3D : TEXCOORD1;
};

VertexOutput vsMain(VertexInput vin)
{
    VertexOutput vout;
    float4x4 mvp = mul(model[0], mul(view, projection));
    vout.outputPosition = mul(vin.inputPosition, mvp);
    vout.outputNormal = vin.inputNormal;
    vout.outputTexCoords = vin.inputTexCoords;
    vout.outputTexCoords3D = float3(vin.inputPosition.x, vin.inputPosition.y, vin.inputPosition.z);
    
    return vout;
}