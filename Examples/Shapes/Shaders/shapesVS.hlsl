#include "shapesResources.h.hlsl"

cbuffer PerFrameUniformBuffer : register(b0)
{
    float4x4 view;
    float4x4 projection;
};


cbuffer PerObjectUniformBuffer : register(b1)
{
    float4x4 model;
};

VertexOutput vsMain(VertexInput vin)
{
    VertexOutput vout;
    float4x4 mvp = mul(model, mul(view, projection));
    vout.outputPosition = mul(vin.inputPosition, mvp);
    vout.outputNormal = vin.inputNormal;
    vout.outputTexCoords = vin.inputTexCoords;
    
    return vout;
}