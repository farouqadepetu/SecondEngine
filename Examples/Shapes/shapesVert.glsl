#version 450

layout(location = 0) in vec4 inputPosition;
layout(location = 1) in vec4 inputNormal;
layout(location = 2) in vec2 inputTexCoords;

layout(location = 0) out vec4 outNormal;
layout(location = 1) out vec2 outTexCoords;

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
    uint index = gl_InstanceIndex;
    gl_Position = perObjectBuffer.model * perFrameBuffer.view * perFrameBuffer.projection * inputPosition;
    gl_Position.y = -gl_Position.y;
    outNormal = inputNormal;
    outTexCoords = inputTextCoords;
}