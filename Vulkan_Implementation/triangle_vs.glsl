#version 450

layout(location = 0) in vec4 inPos;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec3 fragColor;

void main() 
{
    gl_Position = vec4(inPos, 0.0f, 1.0f);
    fragColor = inColor;
}
