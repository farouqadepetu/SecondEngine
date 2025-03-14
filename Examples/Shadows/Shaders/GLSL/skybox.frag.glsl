#version 460
#include "resources.h.glsl"

layout(location = 1) in vec3 outTexCoords;

layout(location = 0) out vec4 outColor;

void main() 
{
	outColor = texture(samplerCube(gTextureCube, gSampler), outTexCoords);
}