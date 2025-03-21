struct Vertex
{
    float4 position;
    float4 normal;
    float4 tangent;
    float2 texCoords;
    float2 pad0;
};

cbuffer ObjectUniformBuffer : register(b0)
{
    float4x4 objectModel;
    float4x4 objectInverseModel;
};

cbuffer LightSourceUniformBuffer : register(b1)
{
    float4x4 lightSourceModel;
    float4x4 lightSourceView;
    float4x4 lightSourceProjection;
};

struct RootConstants
{
    uint numVertices;
};

ConstantBuffer<RootConstants> constants : register(b2);

RWStructuredBuffer<Vertex> vertexBuffer : register(u0);
RWTexture2D<float> shadowMap : register(u1);

[numthreads(32, 32, 1)]
void csMain(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    uint2 threadID = dispatchThreadId.xy;
    shadowMap[threadID] = 1.0f;

}