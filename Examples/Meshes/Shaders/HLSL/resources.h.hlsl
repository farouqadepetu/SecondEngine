cbuffer PerFrameUniformBuffer : register(b0)
{
    float4x4 view;
    float4x4 projection;
};

cbuffer PerObjectUniformBuffer : register(b1)
{
    float4x4 model[2];
};

Texture2D gTexture2D : register(t0);
TextureCube gTextureCube : register(t1);
SamplerState gSampler : register(s0);