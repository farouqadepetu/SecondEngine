#version 460

layout(location = 0) in vec4 outNormal;
layout(location = 1) in vec2 outTexCoords;

layout(set = 0, binding = 0) uniform texture2D gTexture;
layout(set = 2, binding = 0) uniform sampler gSampler;

layout(location = 0) out vec4 outColor;

void main() 
{
    outColor = texture(sampler2D(gTexture, gSampler), outTexCoords);
}