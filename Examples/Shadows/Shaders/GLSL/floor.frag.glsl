#version 460
#include "resources.h.glsl"

layout(location = 0) in vec4 outPosW;
layout(location = 1) in vec4 outNormal;
layout(location = 2) in vec4 outTangent;
layout(location = 3) in vec4 outBiTangent;
layout(location = 4) in mat4 outTBN;
layout(location = 8) in vec2 outTexCoords;

layout(location = 0) out vec4 finalColor;

void main() 
{
	finalColor = texture(sampler2D(gFloor, gSampler), outTexCoords);
}