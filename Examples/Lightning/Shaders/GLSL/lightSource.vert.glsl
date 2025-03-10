#version 460
#include "resources.h.glsl"

layout(location = 0) in vec4 inPos;
layout(location = 1) in vec4 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec2 inTexCoords;


void main() 
{
    mat4 model = pointLightBuffer.pointLight.model * float(constants.currentLightSource == POINT) +
    spotlightBuffer.spotlight.model * float(constants.currentLightSource == SPOTLIGHT);

    mat4 mvp = perFrameBuffer.projection * perFrameBuffer.view * model;
    vec4 posH = mvp * inPos;

    gl_Position = posH;
    gl_Position.y = -gl_Position.y;
}