#version 460
#include "resources.h.glsl"

layout(location = 0) in vec4 outPosW;
layout(location = 1) in vec4 outPosL;
layout(location = 2) in vec4 outNormal;
layout(location = 3) in vec4 outTangent;
layout(location = 4) in vec4 outBiTangent;
layout(location = 5) in mat4 outTBN;
layout(location = 9) in vec2 outTexCoords;

layout(location = 0) out vec4 finalColor;

float ComputeShadow(vec4 posL)
{
    //perspective divide
    vec3 projCoords = posL.xyz / posL.w;
    
    //transform from [-1, 1] -> [0, 1]
    projCoords.x = projCoords.x * 0.5f + 0.5f;
    projCoords.y = -projCoords.y * 0.5f + 0.5f;
    
    //get the closest depth value from light perspective
    float closestDepth = texture(sampler2D(gShadowMap, gSampler), projCoords.xy).r;
    
    float currentDepth = projCoords.z;
    
    //check if the pixel is in shadow or not
    //1.0f if true (not in shadow), 0.0f if false (in shadow)
    float shadow = float((currentDepth - constants.shadowBias) < closestDepth);
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if (projCoords.z > 1.0)
        shadow = 0.0;
    
    return shadow;
}

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

float ComputeShadowPointLight(vec3 posW, vec3 lightPosition, float nDotL, float farPlane)
{
    vec3 lightToFrag = posW - lightPosition;
    
    uint faceIndex;
    vec2 texCoords;
    float closestDepth;
    float currentDepth;
    
    texCoords = SampleCube(lightToFrag, faceIndex);
    
    closestDepth = texture(sampler2D(gShadowMapPL[faceIndex], gSampler), texCoords).r;
    
    currentDepth = length(lightToFrag) / farPlane;
    
    float bias = constants.shadowBias;
    
    float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
    
    return shadow;
}

void main()
{
    uint objectIndex = constants.objectIndex;
    
    PixelDesc desc;
    desc.normal = normalize(outNormal.xyz);
    desc.viewDir = normalize(cameraBuffer.cameraPos.xyz - outPosW.xyz);
    
    vec3 color = vec3(0.0f, 0.0f, 0.0f);
    
    if(constants.currentLightSource == DIRECTIONAL_LIGHT)
    {
        desc.lightDir = normalize(-directionalLightBuffer.directionalLight.direction.xyz);
        desc.halfwayDir = normalize(desc.lightDir + desc.viewDir);
        color += ComputeDirectionalLight(directionalLightBuffer.directionalLight, objectBuffer.material[objectIndex], desc);
        color *= ComputeShadow(outPosL);
    }
    else if (constants.currentLightSource == POINT_LIGHT)
    {
        desc.lightDir = normalize(pointLightBuffer.pointLight.position.xyz - outPosW.xyz);
        desc.halfwayDir = normalize(desc.lightDir + desc.viewDir);
        desc.distance = length(pointLightBuffer.pointLight.position.xyz - outPosW.xyz);
        color += ComputePointLight(pointLightBuffer.pointLight, objectBuffer.material[objectIndex], desc);
        float shadow = ComputeShadowPointLight(outPosW.xyz, pointLightBuffer.pointLight.position.xyz, dot(desc.normal, desc.lightDir), 10.0f);
        color = color * (1.0f - shadow);
    }
    else //SPOTLIGHT
    {
        
    }
    
    vec3 ambient = objectBuffer.material[objectIndex].albedo.rgb * objectBuffer.material[objectIndex].ao;
    color += ambient;
    
    //tonemapping
    color = color / (color + vec3(1.0f, 1.0f, 1.0f));
    
    float gammaCorrection = 1.0f / 2.2f;
    color = pow(color, vec3(gammaCorrection, gammaCorrection, gammaCorrection));
    
    finalColor = vec4(color, 1.0f);
}