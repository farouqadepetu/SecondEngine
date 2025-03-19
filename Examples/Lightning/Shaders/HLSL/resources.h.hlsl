#include "../ShaderLibrary/HLSL/phong.h.hlsl"
#include "../ShaderLibrary/HLSL/pbr.h.hlsl"

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
    uint numPointLights;
    uint numDirectionalLights;
    uint numSpotlights;
    float normalScale;
};

ConstantBuffer<RootConstants> constants : register(b7);

Texture2D gBricksColor : register(t0);
Texture2D gBricksAO : register(t1);
Texture2D gBricksRoughness : register(t2);
Texture2D gBricksNormal : register(t3);
TextureCube gTextureCube : register(t4);
SamplerState gSampler : register(s0);