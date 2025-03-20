#ifndef PHONG_H
#define PHONG_H

#define HLSL_SHADER
#include "../lightSource.h"

struct PhongMaterial
{
    float4 diffuse;
    float4 specular;
    float ambientIntensity;
    float shininess;
};

float3 ComputePointLight(PointLight light, PhongMaterial material, PixelDesc desc)
{
    //Diffuse
    float diffuseIntensity = max(dot(desc.normal, desc.lightDir), 0.0f);
    float3 diffuse = diffuseIntensity * light.color.rgb * material.diffuse.rgb;
    
    //Specular
    float specularIntensity = pow(max(dot(desc.normal, desc.halfwayDir), 0.0f),material.shininess);
    float3 specular = (specularIntensity * light.color.rgb * material.specular.rgb) * (diffuseIntensity > 0.0f);
    
    float attenuation = 1.0f / (desc.distance * desc.distance);
    
    diffuse *= attenuation;
    specular *= attenuation;
    
    //Final color
    return diffuse + specular;
}

float3 ComputeDirectionalLight(DirectionalLight light, PhongMaterial material, PixelDesc desc)
{
    //Diffuse
    float diffuseIntensity = max(dot(desc.normal, desc.lightDir), 0.0f);
    float3 diffuse = diffuseIntensity * light.color.rgb * material.diffuse.rgb;
    
    //Specular
    float specularIntensity = pow(max(dot(desc.normal, desc.halfwayDir), 0.0f), material.shininess);
    float3 specular = (specularIntensity * light.color.rgb * material.specular.rgb) * (diffuseIntensity > 0.0f);
    
    //Final color
    return  diffuse + specular;
}

float3 ComputeSpotlight(Spotlight light, PhongMaterial material, PixelDesc desc)
{
    //Diffuse
    float diffuseIntensity = max(dot(desc.normal, desc.lightDir), 0.0f);
    float3 diffuse = diffuseIntensity * light.color.rgb * material.diffuse.rgb;
    
    //Specular
    float specularIntensity = pow(max(dot(desc.normal, desc.halfwayDir), 0.0f), material.shininess);
    float3 specular = (specularIntensity * light.color.rgb * material.specular.rgb) * (diffuseIntensity > 0.0f);
    
    //spotlight intensity = (theta - outerCutoff) / (innerCutoff - outerCutoff)
    float theta = dot(desc.lightDir, normalize(-light.direction.xyz));
    float epsilon = (light.innerCutoff - light.outerCutoff);
    float spotLightIntensity = clamp((theta - light.outerCutoff) / epsilon, 0.0f, 1.0f);
    
    diffuse *= spotLightIntensity;
    specular *= spotLightIntensity;
    
    float attenuation = 1.0f / (desc.distance * desc.distance);
    
    diffuse *= attenuation;
    specular *= attenuation;

    return diffuse + specular;
}

#endif