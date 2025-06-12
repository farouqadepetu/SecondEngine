#include "../ShaderLibrary/HLSL/pbr.h.hlsl"

#define NUM_OBJECTS 7
#define NUM_LIGHT_DATA 8
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOTLIGHT 2


cbuffer CameraUniformBuffer : register(b0)
{
    float4x4 cameraView;
    float4x4 cameraProjection;
    float4 cameraPos;
};

cbuffer ObjectUniformBuffer : register(b1)
{
    float4x4 objectModel[NUM_OBJECTS];
    float4x4 objectInverseModel[NUM_OBJECTS];
    PBRMaterial material[NUM_OBJECTS];
};

cbuffer LightSourceUniformBuffer : register(b2)
{
    float4x4 lightSourceModel[NUM_LIGHT_DATA];
    float4x4 lightSourceView[NUM_LIGHT_DATA];
    float4x4 lightSourceProjection[NUM_LIGHT_DATA];
    float4 lightPosition[NUM_LIGHT_DATA];
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
    float shadowBias;
    uint objectIndex;
    uint lightIndex;
    uint debugIndex;
};

ConstantBuffer<RootConstants> constants : register(b6);

Texture2D gShadowMap : register(t0);
Texture2D gShadowMapPL[6] : register(t1);
SamplerState gSampler : register(s0);