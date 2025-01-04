#version 450


layout(location = 0) in vec4 inPos;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 fragColor;

layout(binding = 0) uniform UniformData
{
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

void main() 
{
    gl_Position =  ubo.proj * ubo.view * ubo.model * inPos;
    fragColor = inColor;
}

