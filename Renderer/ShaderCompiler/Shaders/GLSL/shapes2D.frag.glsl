#version 460
#include "resources.h.glsl"

layout(location = 0) in vec4 outNormal;
layout(location = 1) in vec2 outTexCoords;
layout(location = 2) in vec3 outTexCoords3D;

layout(location = 0) out vec4 outColor;

void main() 
{
	outColor = texture(sampler2D(gTexture2D, gSampler), outTexCoords);
}