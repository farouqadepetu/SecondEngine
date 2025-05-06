#version 460
#include "resources.h.glsl"

layout(location = 0) in vec4 inputPosition;
layout(location = 1) in vec4 inputNormal;
layout(location = 2) in vec4 inputTangent;
layout(location = 3) in vec2 inputTexCoords;

layout(location = 0) out vec4 outNormal;
layout(location = 1) out vec3 outTexCoords;

void main() 
{
    vec4 posH = perFrameBuffer.projection * perFrameBuffer.view * perObjectBuffer.model[1] * inputPosition;
    gl_Position = posH.xyww; //make z = w so that when perspective divide happens z/w = 1 which makes the skybox always on the far plane.
    gl_Position.y = -gl_Position.y;
    outNormal = inputNormal;
    outTexCoords = vec3(inputPosition.x, inputPosition.y, inputPosition.z);
}