#version 450

layout(location = 0) in vec4 inPos;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 texCoords;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoords;

layout(binding = 0) uniform UniformData
{
    mat4 model[12];
    mat4 view[12];
    mat4 proj[12];
} ubo;

void main() 
{
    uint index = gl_InstanceIndex;
    gl_Position =  ubo.proj[index] * ubo.view[index] * ubo.model[index] * inPos;
    fragColor = inColor;
    fragTexCoords = texCoords;
}

