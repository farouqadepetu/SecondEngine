#include "phong.h.hlsl"
#include "pbr.h.hlsl"

#define NUM_SHAPES 6

cbuffer CameraUniformBuffer : register(b0)
{
    float4x4 cameraView;
    float4x4 cameraProjection;
    float4 cameraPos;
};

cbuffer ObjectUniformBuffer : register(b1)
{
    float4x4 objectModel[NUM_SHAPES];
    float4x4 objectInverseModel[NUM_SHAPES];
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

struct RootConstants
{
    uint currentLightSource;
    uint shape;
};

ConstantBuffer<RootConstants> constants : register(b6);

Texture2D gWoodColor : register(t0);
Texture2D gWoodRoughness : register(t1);
Texture2D gWoodNormal : register(t2);
Texture2D gShadowMap : register(t3);
TextureCube gTextureCube : register(t4);
SamplerState gSampler : register(s0);