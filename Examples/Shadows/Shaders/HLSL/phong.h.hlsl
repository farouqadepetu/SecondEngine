#ifndef PHONG_H
#define PHONG_H

#include "lightSource.h.hlsl"

struct PhongMaterial
{
    float4 diffuse;
    float4 specular;
    float ambientIntensity;
    float shininess;
};

struct PhongPixelDesc
{
    float4 normal;
    float4 posW;
    float4 posC;
    PhongMaterial material;
};

float3 ComputePointLight(PointLight light, PhongPixelDesc desc)
{
    float3 normal = normalize(desc.normal.xyz);
    float3 lightDir = normalize(light.position.xyz - desc.posW.xyz);
    float3 viewDir = normalize(desc.posC.xyz - desc.posW.xyz);
    float3 halfwayDir = normalize(lightDir + viewDir);
    
    //Ambient
    //float3 ambient = light.color.rgb * desc.material.ambient.rgb;
        
    //Diffuse
    float diffuseIntensity = max(dot(normal, lightDir), 0.0f);
    float3 diffuse = diffuseIntensity * light.color.rgb * desc.material.diffuse.rgb;
    
    //Specular
    float specularIntensity = pow(max(dot(normal, halfwayDir), 0.0f), desc.material.shininess);
    float3 specular = (specularIntensity * light.color.rgb * desc.material.specular.rgb) * (diffuseIntensity > 0.0f);
    
    float distance = length(light.position.xyz - desc.posW.xyz);
    float attenuation = 1.0f / (distance * distance);
    
    //ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    //Final color
    //return ambient + diffuse + specular;
    return diffuse + specular;
}

float3 ComputeDirectionalLight(DirectionalLight light, PhongPixelDesc desc)
{
    float3 normal = normalize(desc.normal.xyz);
    float3 lightDir = normalize(-light.direction.xyz);
    float3 viewDir = normalize(desc.posC.xyz - desc.posW.xyz);
    float3 halfwayDir = normalize(lightDir + viewDir);
    
     //Ambient
    //float3 ambient = light.color.rgb * desc.material.ambient.rgb;
        
    //Diffuse
    float diffuseIntensity = max(dot(normal, lightDir), 0.0f);
    float3 diffuse = diffuseIntensity * light.color.rgb * desc.material.diffuse.rgb;
    
    //Specular
    float specularIntensity = pow(max(dot(normal, halfwayDir), 0.0f), desc.material.shininess);
    float3 specular = (specularIntensity * light.color.rgb * desc.material.specular.rgb) * (diffuseIntensity > 0.0f);
    
    //Final color
    return  diffuse + specular;
}

float3 ComputeSpotlight(Spotlight light, PhongPixelDesc desc)
{
    float3 normal = normalize(desc.normal.xyz);
    float3 lightDir = normalize(light.position.xyz - desc.posW.xyz);
    float3 viewDir = normalize(desc.posC.xyz - desc.posW.xyz);
    float3 halfwayDir = normalize(lightDir + viewDir);
    
    //Ambient
    //float3 ambient = light.color.rgb * desc.material.ambient.rgb;
        
    //Diffuse
    float diffuseIntensity = max(dot(normal, lightDir), 0.0f);
    float3 diffuse = diffuseIntensity * light.color.rgb * desc.material.diffuse.rgb;
    
    //Specular
    float specularIntensity = pow(max(dot(normal, halfwayDir), 0.0f), desc.material.shininess);
    float3 specular = (specularIntensity * light.color.rgb * desc.material.specular.rgb) * (diffuseIntensity > 0.0f);
    
    //spotlight intensity = (theta - outerCutoff) / (innerCutoff - outerCutoff)
    float theta = dot(lightDir, normalize(-light.direction.xyz));
    float epsilon = (light.innerCutoff - light.outerCutoff);
    float spotLightIntensity = clamp((theta - light.outerCutoff) / epsilon, 0.0f, 1.0f);
    
    diffuse *= spotLightIntensity;
    specular *= spotLightIntensity;
    
    float distance = length(light.position.xyz - desc.posW.xyz);
    float attenuation = 1.0f / (distance * distance);
    
    //ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    //Final color
    //return ambient + diffuse + specular;

    return diffuse + specular;
}

#endif