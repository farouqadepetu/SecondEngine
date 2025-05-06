layout(row_major, set = 1, binding = 0) uniform PerFrameUniformBuffer
{
    mat4 view;
    mat4 projection;
} perFrameBuffer;

layout(row_major, set = 1, binding = 1) uniform PerObjectUniformBuffer
{
    mat4 model[2];
} perObjectBuffer;

layout(set = 0, binding = 0) uniform texture2D gTexture2D;
layout(set = 0, binding = 1) uniform textureCube gTextureCube;
layout(set = 0, binding = 2) uniform sampler gSampler;
