#version 460
#include "resources.h.glsl"

layout(location = 0) in vec4 inPos;
layout(location = 1) in vec4 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec2 inTexCoords;

layout(location = 0) out vec4 outPosW;
layout(location = 1) out vec4 outPosL;
layout(location = 2) out vec4 outNormal;
layout(location = 3) out vec4 outTangent;
layout(location = 4) out vec4 outBiTangent;
layout(location = 5) out mat4 outTBN;
layout(location = 9) out vec2 outTexCoords;

vec2 SampleCube(const vec3 v, out uint faceIndex)
{
    vec3 vAbs = abs(v);
    float ma;
    vec2 uv;
    if (vAbs.z >= vAbs.x && vAbs.z >= vAbs.y)
    {
        faceIndex = v.z < 0.0 ? 5 : 4;
        ma = 0.5 / vAbs.z;
        uv = vec2(v.z < 0.0 ? -v.x : v.x, -v.y);
    }
    else if (vAbs.y >= vAbs.x)
    {
        faceIndex = v.y < 0.0 ? 3 : 2;
        ma = 0.5 / vAbs.y;
        uv = vec2(v.x, v.y < 0.0 ? -v.z : v.z);
    }
    else
    {
        faceIndex = v.x < 0.0 ? 1 : 0;
        ma = 0.5 / vAbs.x;
        uv = vec2(v.x < 0.0 ? v.z : -v.z, -v.y);
    }
    return uv * ma + 0.5;
}

void main()
{
    uint objectIndex = constants.objectIndex;
    uint lightIndex = 0;

    vec4 posW = objectBuffer.objectModel[objectIndex] * inPos;
    if(constants.currentLightSource == POINT_LIGHT)
    {
        vec3 lightToFrag = posW.xyz - pointLightBuffer.pointLight.position.xyz;
        vec2 uv;
        
        uv = SampleCube(lightToFrag, lightIndex);

    }
    else if (constants.currentLightSource == SPOTLIGHT)
    {
        lightIndex = 7;
    }

    mat4 mvp = cameraBuffer.cameraProjection * cameraBuffer.cameraView * objectBuffer.objectModel[objectIndex];
    vec4 posH = mvp * inPos;

    mat4 shadowMatrix = lightSourceBuffer.lightSourceProjection[constants.lightIndex] * 
    lightSourceBuffer.lightSourceView[constants.lightIndex] * objectBuffer.objectModel[objectIndex];

    vec4 posL = shadowMatrix * inPos;
    vec4 normal = normalize(objectBuffer.objectInverseModel[objectIndex] * inNormal);
    vec4 tangent = normalize(objectBuffer.objectInverseModel[objectIndex] * inTangent);

    //re-orthogonalize using the gram-schmdit method.
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    vec4 bitangent = vec4(cross(tangent.xyz, normal.xyz), 0.0f);
    
    gl_Position = posH;
    gl_Position.y = -gl_Position.y;
    outPosW = posW;
    outPosL = posL;
    outNormal = normal;
    outTangent = tangent;
    outBiTangent = bitangent;
    outTBN = mat4(tangent, bitangent, normal, vec4(0.0f, 0.0f, 0.0f, 1.0f));
    outTexCoords = inTexCoords;
}