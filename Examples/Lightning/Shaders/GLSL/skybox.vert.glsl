#version 460
#include "resources.h.glsl"

layout(location = 0) in vec4 inPos;
layout(location = 1) in vec4 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec2 inTexCoords;

layout(location = 0) out vec4 outNormal;
layout(location = 1) out vec3 outTexCoords;

void main() 
{
    vec4 posH = perFrameBuffer.projection * perFrameBuffer.view * perObjectBuffer.model * inPos;
    gl_Position = posH.xyww; //make z = w so that when perspective divide happens z/w = 1 which makes the skybox always on the far plane.
    gl_Position.y = -gl_Position.y;
    outNormal = inNormal;
    outTexCoords = vec3(inPos.x, inPos.y, inPos.z);
}