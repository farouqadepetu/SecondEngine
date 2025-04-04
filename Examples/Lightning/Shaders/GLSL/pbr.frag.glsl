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
    vec4 normal;
    PBRMaterial material;

    if (constants.currentMapping == TEXTURE)
    {
        material.albedo = texture(sampler2D(gBricksColor, gSampler), outTexCoords);
        material.albedo.r = pow(material.albedo.r, gamma);
        material.albedo.g = pow(material.albedo.g, gamma);
        material.albedo.b = pow(material.albedo.b, gamma);

        material.ao = texture(sampler2D(gBricksAO, gSampler), outTexCoords).r * 0.1f;
        material.roughness = texture(sampler2D(gBricksRoughness, gSampler), outTexCoords).r;
        material.metallic = 0.0f;
        
        //Change range from [0,1] -> [-1,1]
        normal = texture(sampler2D(gBricksNormal, gSampler), outTexCoords);
        normal = normal * 2.0f - 1.0f;
        normal.xy *= constants.normalScale;
        normal = outTBN * normalize(normal);
    }
    else //MATERIAL
    {
        material.albedo = pbrMaterialBuffer.material.albedo;
        material.roughness = pbrMaterialBuffer.material.roughness;
        material.ao = pbrMaterialBuffer.material.ao;
        material.metallic = pbrMaterialBuffer.material.metallic;
        normal = outNormal;
    }

    PixelDesc desc;
    desc.normal = normalize(normal.xyz);
    desc.viewDir = normalize(perFrameBuffer.cameraPos.xyz - outPosW.xyz);
    
    vec3 color = vec3(0.0f, 0.0f, 0.0f);
    for (uint pLight = 0; pLight < constants.numPointLights; ++pLight)
    {
        desc.lightDir = normalize(pointLightBuffer.pointLight.position.xyz - outPosW.xyz);
        desc.halfwayDir = normalize(desc.lightDir + desc.viewDir);
        desc.distance = length(pointLightBuffer.pointLight.position.xyz - outPosW.xyz);
        color += ComputePointLight(pointLightBuffer.pointLight, material, desc);
    }
  
    for (uint dLight = 0; dLight < constants.numDirectionalLights; ++dLight)
    {
        desc.lightDir = normalize(-directionalLightBuffer.directionalLight.direction.xyz);
        desc.halfwayDir = normalize(desc.lightDir + desc.viewDir);
        color += ComputeDirectionalLight(directionalLightBuffer.directionalLight, material, desc);
    }
    
    for (uint sLight = 0; sLight < constants.numSpotlights; ++sLight)
    {
        desc.lightDir = normalize(spotlightBuffer.spotlight.position.xyz - outPosW.xyz);
        desc.halfwayDir = normalize(desc.lightDir + desc.viewDir);
        desc.distance = length(spotlightBuffer.spotlight.position.xyz - outPosW.xyz);
        color += ComputeSpotlight(spotlightBuffer.spotlight, material, desc);
    }
    
    vec3 ambient = material.albedo.rgb * material.ao;
    color += ambient;
    
    //tonemapping
    color = color / (color + vec3(1.0f, 1.0f, 1.0f));
    
   float gammaCorrection = 1.0f / gamma;
   color = pow(color, vec3(gammaCorrection, gammaCorrection, gammaCorrection));

   //Final color
   finalColor = vec4(color, 1.0f);
}