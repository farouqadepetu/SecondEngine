#version 460
#include "resources.h.glsl"

layout(location = 0) in vec4 outPosW;

void main() 
{
	float lightDistance = length(outPosW.xyz - lightSourceBuffer.lightPosition[constants.lightIndex].xyz);
    
    lightDistance = lightDistance / 10.0f;
    
    gl_FragDepth = lightDistance;
}