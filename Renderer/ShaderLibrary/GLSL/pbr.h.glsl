#ifndef PBR_H
#define PBR_H

#define GLSL_SHADER
#include "../lightSource.h"

#define PI 3.14159f

struct PBRMaterial
{
    vec4 albedo;
    float metallic;
    float roughness;
    float ao;
};

vec3 FrenselSchlick(vec3 halfwayDir, vec3 viewDir, vec3 f0)
{
    float hDotV = dot(halfwayDir, viewDir);
    return f0 + (1.0f - f0) * pow(clamp(1.0f - hDotV, 0.0f, 1.0f), 5.0f);
}

float DistributionGGX(vec3 normal, vec3 halfwayDir, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float ndotH = max(dot(normal, halfwayDir), 0.0f);
    
    float denom = (ndotH * ndotH * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;
	
    return a2 / denom;
}

float GeometrySchlickGGX(vec3 normal, vec3 dir, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;
    float nDotV = max(dot(normal, dir), 0.0f);

    float denom = nDotV * (1.0f - k) + k;
	
    return nDotV / denom;
}

float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness)
{
    float ggxV = GeometrySchlickGGX(normal, viewDir, roughness);
    float ggxL = GeometrySchlickGGX(normal, lightDir, roughness);
	
    return ggxV * ggxL;
}

vec3 ComputePointLight(PointLight light, PBRMaterial material, PixelDesc desc)
{
    vec3 f0 = vec3(0.04f, 0.04f, 0.04f);
    f0 = mix(f0, material.albedo.rgb, material.metallic);
    
    float attenuation = 1.0f / (desc.distance * desc.distance);
    vec3 radiance = light.color.rgb * attenuation;
    float nDotL = max(dot(desc.normal, desc.lightDir), 0.0f);
    
    float d = DistributionGGX(desc.normal, desc.halfwayDir, material.roughness);
    float g = GeometrySmith(desc.normal, desc.viewDir, desc.lightDir, material.roughness);
    vec3 f = FrenselSchlick(desc.halfwayDir, desc.viewDir, f0);
    
    vec3 kd = vec3(1.0f, 1.0f, 1.0f) - f;
    kd = kd * (1.0f - material.metallic);
    
    vec3 numerator = d * f * g;
    float denominator = 4.0f * max(dot(desc.normal, desc.halfwayDir), 0.0f) * nDotL + 0.0001f;
    vec3 specular = numerator / denominator;
    
    return (kd * (material.albedo.rgb / PI) + specular) * radiance * nDotL;
}

vec3 ComputeDirectionalLight(DirectionalLight light, PBRMaterial material, PixelDesc desc)
{
    vec3 f0 = vec3(0.04f, 0.04f, 0.04f);
    f0 = mix(f0, material.albedo.rgb, material.metallic);
   
    vec3 radiance = light.color.rgb;
    float nDotL = max(dot(desc.normal, desc.lightDir), 0.0f);
    
    float d = DistributionGGX(desc.normal, desc.halfwayDir, material.roughness);
    float g = GeometrySmith(desc.normal, desc.viewDir, desc.lightDir, material.roughness);
    vec3 f = FrenselSchlick(desc.halfwayDir, desc.viewDir, f0);
    
    vec3 kd = vec3(1.0f, 1.0f, 1.0f) - f;
    kd = kd * (1.0f - material.metallic);
    
    vec3 numerator = d * f * g;
    float denominator = 4.0f * max(dot(desc.normal, desc.halfwayDir), 0.0f) * nDotL + 0.0001f;
    vec3 specular = numerator / denominator;
    
    return (kd * material.albedo.rgb / PI + specular) * radiance * nDotL;
}

vec3 ComputeSpotlight(Spotlight light, PBRMaterial material, PixelDesc desc)
{
    vec3 f0 = vec3(0.04f, 0.04f, 0.04f);
    f0 = mix(f0, material.albedo.rgb, material.metallic);

    float attenuation = 1.0f / (desc.distance * desc.distance);
    vec3 radiance = light.color.rgb * attenuation;
    float nDotL = max(dot(desc.normal, desc.lightDir), 0.0f);
    
    float d = DistributionGGX(desc.normal, desc.halfwayDir, material.roughness);
    float g = GeometrySmith(desc.normal, desc.viewDir, desc.lightDir, material.roughness);
    vec3 f = FrenselSchlick(desc.halfwayDir, desc.viewDir, f0);
   
    vec3 kd = vec3(1.0f, 1.0f, 1.0f) - f;
    kd = kd * (1.0f - material.metallic);
    
    //spotlight intensity = (theta - outerCutoff) / (innerCutoff - outerCutoff)
    float theta = dot(desc.lightDir, normalize(-light.direction.xyz));
    float epsilon = (light.innerCutoff - light.outerCutoff);
    float spotLightIntensity = clamp((theta - light.outerCutoff) / epsilon, 0.0f, 1.0f);
    
    f *= spotLightIntensity;
    kd *= spotLightIntensity;
    
    vec3 numerator = d * f * g;
    float denominator = 4.0f * max(dot(desc.normal, desc.halfwayDir), 0.0f) * nDotL + 0.0001f;
    vec3 specular = numerator / denominator;
    
    return (kd * material.albedo.rgb / PI + specular) * radiance * nDotL;
}

#endif