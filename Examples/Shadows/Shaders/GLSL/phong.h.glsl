#ifndef PHONG_H
#define PHONG_H

#include "lightSource.h.glsl"

struct PhongMaterial
{
    vec4 diffuse;
    vec4 specular;
    float ambientIntensity;
    float shininess;
};

struct PhongPixelDesc
{
    vec4 normal;
    vec4 posW;
    vec4 posC;
    PhongMaterial material;
};

vec3 ComputePointLight(PointLight light, PhongPixelDesc desc)
{
    vec3 normal = normalize(desc.normal.xyz);
    vec3 lightDir = normalize(light.position.xyz - desc.posW.xyz);
    vec3 viewDir = normalize(desc.posC.xyz - desc.posW.xyz);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    
    //Ambient
    //vec3 ambient = light.color.rgb * desc.material.ambient.rgb;
        
    //Diffuse
    float diffuseIntensity = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = diffuseIntensity * light.color.rgb * desc.material.diffuse.rgb;
    
    //Specular
    float specularIntensity = pow(max(dot(normal, halfwayDir), 0.0f), desc.material.shininess);
    vec3 specular = (specularIntensity * light.color.rgb * desc.material.specular.rgb) * float(diffuseIntensity > 0.0f);
    
    float distance = length(light.position.xyz - desc.posW.xyz);
    float attenuation = 1.0f / (distance * distance);
    
    //ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    //Final color
    //return ambient + diffuse + specular;
    return diffuse + specular;
}

vec3 ComputeDirectionalLight(DirectionalLight light, PhongPixelDesc desc)
{
    vec3 normal = normalize(desc.normal.xyz);
    vec3 lightDir = normalize(-light.direction.xyz);
    vec3 viewDir = normalize(desc.posC.xyz - desc.posW.xyz);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    
     //Ambient
    //vec3 ambient = light.color.rgb * desc.material.ambient.rgb;
        
    //Diffuse
    float diffuseIntensity = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = diffuseIntensity * light.color.rgb * desc.material.diffuse.rgb;
    
    //Specular
    float specularIntensity = pow(max(dot(normal, halfwayDir), 0.0f), desc.material.shininess);
    vec3 specular = (specularIntensity * light.color.rgb * desc.material.specular.rgb) * float(diffuseIntensity > 0.0f);
    
    //Final color
    return  diffuse + specular;
}

vec3 ComputeSpotlight(Spotlight light, PhongPixelDesc desc)
{
    vec3 normal = normalize(desc.normal.xyz);
    vec3 lightDir = normalize(light.position.xyz - desc.posW.xyz);
    vec3 viewDir = normalize(desc.posC.xyz - desc.posW.xyz);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    
    //Ambient
    //vec3 ambient = light.color.rgb * desc.material.ambient.rgb;
        
    //Diffuse
    float diffuseIntensity = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = diffuseIntensity * light.color.rgb * desc.material.diffuse.rgb;
    
    //Specular
    float specularIntensity = pow(max(dot(normal, halfwayDir), 0.0f), desc.material.shininess);
    vec3 specular = (specularIntensity * light.color.rgb * desc.material.specular.rgb) * float(diffuseIntensity > 0.0f);
    
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