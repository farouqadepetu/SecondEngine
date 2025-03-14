#version 460
#include "resources.h.glsl"

layout(location = 0) in vec4 outPosW;
layout(location = 1) in vec4 outNormal;
layout(location = 2) in vec4 outTangent;
layout(location = 3) in vec4 outBiTangent;
layout(location = 4) in mat4 outTBN;
layout(location = 8) in vec2 outTexCoords;

layout(location = 0) out vec4 finalColor;

void main() 
{
    float gamma = 2.2f;
    vec4 sampledDiffuse = texture(sampler2D(gBricksColor, gSampler), outTexCoords);
    sampledDiffuse.r = pow(sampledDiffuse.r, gamma);
    sampledDiffuse.g = pow(sampledDiffuse.g, gamma);
    sampledDiffuse.b = pow(sampledDiffuse.b, gamma);
    vec4 sampledNormal = texture(sampler2D(gBricksNormal, gSampler), outTexCoords);
    float sampledAmbient = texture(sampler2D(gBricksAO, gSampler), outTexCoords).r;
    float sampledRoughness = texture(sampler2D(gBricksRoughness, gSampler), outTexCoords).r;

    //Change range from [0,1] -> [-1,1]
    sampledNormal = sampledNormal * 2.0f - 1.0f;
    sampledNormal.xy *= 2.0f;
    sampledNormal = outTBN * normalize(sampledNormal);
    
    PhongPixelDesc desc;
    desc.normal = sampledNormal;
    desc.posW = outPosW;
    desc.posC = cameraBuffer.cameraPos;
    desc.material.ambientIntensity = sampledAmbient;
    desc.material.diffuse = sampledDiffuse;
    desc.material.specular = vec4(sampledRoughness, sampledRoughness, sampledRoughness, 1.0f);
    desc.material.shininess = 256.0f;
    
    vec3 color = vec3(0.0f, 0.0f, 0.0f);
    for (uint pLight = 0; pLight < NUM_POINT_LIGHTS; ++pLight)
    {
        color += ComputePointLight(pointLightBuffer.pointLight, desc) * 
        float(constants.currentLightSource == POINT || constants.currentLightSource == ALL);
    }
    
    for (uint dLight = 0; dLight < NUM_DIRECTIONAL_LIGHTS; ++dLight)
    {
        color += ComputeDirectionalLight(directionalLightBuffer.directionalLight, desc) * 
        float(constants.currentLightSource == DIRECTIONAL || constants.currentLightSource == ALL);
    }
    
    for (uint sLight = 0; sLight < NUM_SPOTLIGHTS; ++sLight)
    {
        color += ComputeSpotlight(spotlightBuffer.spotlight, desc) * 
        float(constants.currentLightSource == SPOTLIGHT || constants.currentLightSource == ALL);
    }

    vec3 ambient = vec3(0.05f, 0.05f, 0.05f) * sampledAmbient * sampledDiffuse.rgb;
    color += ambient;

    //tonemapping
    color = color / (color + vec3(1.0f, 1.0f, 1.0f));
    
    float gammaCorrection = 1.0f / gamma;
    color = pow(color, vec3(gammaCorrection, gammaCorrection, gammaCorrection));
    
    //Final color
    finalColor = vec4(color, 1.0f);
}