#version 460
#include "shapesResources.h.glsl"

layout(row_major, set = 1, binding = 0) uniform PerFrameUniformBuffer
{
    mat4 view;
    mat4 projection;
} perFrameBuffer;

layout(row_major, set = 1, binding = 1) uniform PerObjectUniformBuffer
{
    mat4 model;
} perObjectBuffer;

void main() 
{
    gl_Position = perFrameBuffer.projection * perFrameBuffer.view * perObjectBuffer.model * inputPosition;
    gl_Position.y = -gl_Position.y;
    outNormal = inputNormal;
    outTexCoords = inputTexCoords;
}