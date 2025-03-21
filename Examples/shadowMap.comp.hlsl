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
    uint offset;
};

ConstantBuffer<RootConstants> constants : register(b2);

RWStructuredBuffer<Vertex> vertexBuffer : register(u0);
RWTexture2D<float> shadowMap : register(u1);

[numthreads(64, 1, 1)]
void csMain(uint3 dispatchThreadId : SV_DispatchThreadID)
{
	uint index = dispatchThreadId.x + constants.offset;
    
    if (index < constants.numVertices + constants.offset)
	{
        float4x4 mvp = mul(objectModel, mul(lightSourceView, lightSourceProjection));
        float4 posH = mul(vertexBuffer[index].position, mvp);
        
        float3 ndc = posH.xyz / posH.w;
        
        //change range from [-1, 1] -> [0, 1]
        ndc.x = ndc.x * 0.5f + 0.5f;
        ndc.y = -ndc.y * 0.5f + 0.5f;
        
        //Shadow map indices
        ndc.x = ndc.x * 1024;
        ndc.y = ndc.y * 1024;
        
        float currentDepth;
        InterlockedExchange(shadowMap[ndc.xy], shadowMap[ndc.xy], currentDepth);
        
        if (ndc.z < currentDepth)
        {
            InterlockedExchange(shadowMap[ndc.xy], ndc.z, currentDepth);
        }
    }
}