#version 460
#include "resources.h.glsl"

layout(location = 0) out vec4 finalColor;

void main() 
{
    finalColor = pointLightBuffer.pointLight.color;
}