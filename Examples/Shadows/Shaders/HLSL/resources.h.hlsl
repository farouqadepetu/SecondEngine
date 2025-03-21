#include "../ShaderLibrary/HLSL/pbr.h.hlsl"


cbuffer CameraUniformBuffer : register(b0)
{
    float4x4 cameraView;
    float4x4 cameraProjection;
    float4 cameraPos;
};

cbuffer ObjectUniformBuffer : register(b1)
{
    float4x4 objectModel;
    float4x4 objectInverseModel;
    uint materialIndex;
};

cbuffer LightSourceUniformBuffer : register(b2)
{
    float4x4 lightSourceModel;
    float4x4 lightSourceView;
    float4x4 lightSourceProjection;
};

cbuffer PointLightUniformBuffer : register(b3)
{
    PointLight pointLight;
};

cbuffer DirectionalLightUniformBuffer : register(b4)
{
    DirectionalLight directionalLight;
};

cbuffer SpotLightUniformBuffer : register(b5)
{
    Spotlight spotLight;
};

cbuffer PBRMaterialUniformBuffer : register(b6)
{
    PBRMaterial material[7];
}

struct RootConstants
{
    uint currentLightSource;
    float shadowBias;
};

ConstantBuffer<RootConstants> constants : register(b7);

Texture2D gShadowMap : register(t0);
Texture2D gShadowMapPL[6] : register(t1);
//SamplerState gSampler : register(s0);
SamplerComparisonState gSampler : register(s0);