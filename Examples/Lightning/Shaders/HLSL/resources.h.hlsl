#include "phong.h.hlsl"
#include "pbr.h.hlsl"

#define MATERIAL 0
#define TEXTURE 1

cbuffer PerFrameUniformBuffer : register(b0)
{
    float4x4 view;
    float4x4 projection;
    float4 cameraPos;
};

cbuffer PerObjectUniformBuffer : register(b1)
{
    float4x4 model;
    float4x4 transposeInverseModel;
};

cbuffer PointLightUniformBuffer : register(b2)
{
    PointLight pointLight;
};

cbuffer DirectionalLightUniformBuffer : register(b3)
{
    DirectionalLight directionalLight;
};

cbuffer SpotLightUniformBuffer : register(b4)
{
    Spotlight spotLight;
};

cbuffer PhongMaterialUniformBuffer : register(b5)
{
    PhongMaterial phongMaterial;
};

cbuffer PBRMaterialUniformBuffer : register(b6)
{
    PBRMaterial pbrMaterial;
};

struct RootConstants
{
    uint currentMapping;
    uint currentLightSource;
    float normalScale;
};

ConstantBuffer<RootConstants> constants : register(b7);

Texture2D gBricksColor : register(t0);
Texture2D gBricksDisplacement : register(t1);
Texture2D gBricksAO : register(t2);
Texture2D gBricksRoughness : register(t3);
Texture2D gBricksNormal : register(t4);
TextureCube gTextureCube : register(t5);
SamplerState gSampler : register(s0);