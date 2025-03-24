#version 460
#include "resources.h.glsl"

layout(location = 0) in vec4 inputPosition;
layout(location = 1) in vec4 inputNormal;
layout(location = 2) in vec4 inputTangent;
layout(location = 3) in vec2 inputTexCoords;

layout(location = 0) out vec4 outNormal;
layout(location = 1) out vec2 outTexCoords;
layout(location = 2) out vec3 outTexCoords3D;

void main() 
{
    gl_Position = perFrameBuffer.projection * perFrameBuffer.view * perObjectBuffer.model[0] * inputPosition;
    gl_Position.y = -gl_Position.y;
    outNormal = inputNormal;
    outTexCoords = inputTexCoords;
    outTexCoords3D = vec3(inputPosition.x, inputPosition.y, inputPosition.z);
}