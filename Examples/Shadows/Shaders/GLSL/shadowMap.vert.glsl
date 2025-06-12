#version 460
#include "resources.h.glsl"

layout(location = 0) in vec4 inPos;
layout(location = 1) in vec4 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec2 inTexCoords;

layout(location = 0) out vec4 outPosW;

void main()
{
    uint objectIndex = constants.objectIndex;
    uint lightIndex = constants.lightIndex;
    mat4 mvp = lightSourceBuffer.lightSourceProjection[lightIndex] * lightSourceBuffer.lightSourceView[lightIndex] * objectBuffer.objectModel[objectIndex];
    vec4 posH = mvp * inPos;
    vec4 posW = objectBuffer.objectModel[objectIndex] * inPos;
    
    outPosW = posW;
    gl_Position = posH;
    gl_Position.y = -gl_Position.y;
}