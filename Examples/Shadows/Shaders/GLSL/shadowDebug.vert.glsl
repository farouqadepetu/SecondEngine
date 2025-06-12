#version 460
#include "resources.h.glsl"

layout(location = 0) in vec4 inPos;
layout(location = 1) in vec4 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec2 inTexCoords;

layout(location = 0) out vec2 outTexCoords;

void main()
{
    uint id = gl_VertexIndex;
    vec2 uv = vec2((id << 1) & 2, id & 2);
    vec4 posH = vec4(uv * vec2(2.0f, -2.0f) + vec2(-1.0f, 1.0f), 0.0f, 1.0f);
    
    gl_Position = posH;
    gl_Position.y = -gl_Position.y;
    outTexCoords = uv;
}