#version 460
#include "resources.h.glsl"

layout(location = 0) in vec4 inPos;
layout(location = 1) in vec4 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec2 inTexCoords;

layout(location = 0) out vec4 outPosW;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outTangent;
layout(location = 3) out vec4 outBiTangent;
layout(location = 4) out mat4 outTBN;
layout(location = 8) out vec2 outTexCoords;

void main()
{
    mat4 mvp = cameraBuffer.cameraProjection * cameraBuffer.cameraView * objectBuffer.objectModel;
    vec4 posH = mvp * inPos;
    vec4 posW = objectBuffer.objectModel * inPos;
    vec4 normal = normalize(objectBuffer.objectInverseModel * inNormal);
    vec4 tangent = normalize(objectBuffer.objectInverseModel * inTangent);

    //re-orthogonalize using the gram-schmdit method.
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    vec4 bitangent = vec4(cross(tangent.xyz, normal.xyz), 0.0f);
    
    gl_Position = posH;
    gl_Position.y = -gl_Position.y;
    outPosW = posW;
    outNormal = normal;
    outTangent = tangent;
    outBiTangent = bitangent;
    outTBN = mat4(tangent, bitangent, normal, vec4(0.0f, 0.0f, 0.0f, 1.0f));
    outTexCoords = inTexCoords;
}