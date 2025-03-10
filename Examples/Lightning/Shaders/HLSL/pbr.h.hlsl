#ifndef PBR_H
#define PBR_H

#include "lightSource.h.hlsl"

#define PI 3.14159f

struct PBRMaterial
{
    float4 albedo;
    float metallic;
    float roughness;
    float ao;
};

struct PBRPixelDesc
{
    float4 normal;
    float4 posW;
    float4 posC;
    PBRMaterial material;
};

float3 FrenselSchlick(float3 halfwayDir, float3 viewDir, float3 f0)
{
    float hDotV = dot(halfwayDir, viewDir);
    return f0 + (1.0f - f0) * pow(clamp(1.0f - hDotV, 0.0f, 1.0f), 5.0f);
}

float DistributionGGX(float3 normal, float3 halfwayDir, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float ndotH = max(dot(normal, halfwayDir), 0.0f);
    
    float denom = (ndotH * ndotH * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;
	
    return a2 / denom;
}

float GeometrySchlickGGX(float3 normal, float3 dir, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;
    float nDotV = max(dot(normal, dir), 0.0f);

    float denom = nDotV * (1.0f - k) + k;
	
    return nDotV / denom;
}

float GeometrySmith(float3 normal, float3 viewDir, float3 lightDir, float roughness)
{
    float ggxV = GeometrySchlickGGX(normal, viewDir, roughness);
    float ggxL = GeometrySchlickGGX(normal, lightDir, roughness);
	
    return ggxV * ggxL;
}

float3 ComputePointLight(PointLight light, PBRPixelDesc desc)
{
    float3 f0 = float3(0.04f, 0.04f, 0.04f);
    f0 = lerp(f0, desc.material.albedo.rgb, desc.material.metallic);
    float3 normal = normalize(desc.normal.xyz);
    float3 lightDir = normalize(light.position.xyz - desc.posW.xyz);
    float3 viewDir = normalize(desc.posC.xyz - desc.posW.xyz);
    float3 halfwayDir = normalize(lightDir + viewDir);
    
    float distance = length(light.position.xyz - desc.posW.xyz);
    float attenuation = 1.0f / (distance * distance);
    float3 radiance = light.color.rgb * attenuation;
    float nDotL = max(dot(normal, lightDir), 0.0f);
    
    float d = DistributionGGX(normal, halfwayDir, desc.material.roughness);
    float g = GeometrySmith(normal, viewDir, lightDir, desc.material.roughness);
    float3 f = FrenselSchlick(halfwayDir, viewDir, f0);
    
    float3 kd = float3(1.0f, 1.0f, 1.0f) - f;
    kd = kd * (1.0f - desc.material.metallic);
    
    float3 numerator = d * f * g;
    float denominator = 4.0f * max(dot(normal, halfwayDir), 0.0f) * nDotL + 0.0001f;
    float3 specular = numerator / denominator;
    
    return (kd * (desc.material.albedo.rgb / PI) + specular) * radiance * nDotL;
}

float3 ComputeDirectionalLight(DirectionalLight light, PBRPixelDesc desc)
{
    float3 f0 = float3(0.04f, 0.04f, 0.04f);
    f0 = lerp(f0, desc.material.albedo.rgb, desc.material.metallic);
    float3 normal = normalize(desc.normal.xyz);
    float3 lightDir = normalize(-light.direction.xyz);
    float3 viewDir = normalize(desc.posC.xyz - desc.posW.xyz);
    float3 halfwayDir = normalize(lightDir + viewDir);
   
    float3 radiance = light.color.rgb;
    float nDotL = max(dot(normal, lightDir), 0.0f);
    
    float d = DistributionGGX(normal, halfwayDir, desc.material.roughness);
    float g = GeometrySmith(normal, viewDir, lightDir, desc.material.roughness);
    float3 f = FrenselSchlick(halfwayDir, viewDir, f0);
    
    float3 kd = float3(1.0f, 1.0f, 1.0f) - f;
    kd = kd * (1.0f - desc.material.metallic);
    
    float3 numerator = d * f * g;
    float denominator = 4.0f * max(dot(normal, halfwayDir), 0.0f) * nDotL + 0.0001f;
    float3 specular = numerator / denominator;
    
    return (kd * desc.material.albedo.rgb / PI + specular) * radiance * nDotL;
}

float3 ComputeSpotlight(Spotlight light, PBRPixelDesc desc)
{
    float3 f0 = float3(0.04f, 0.04f, 0.04f);
    f0 = lerp(f0, desc.material.albedo.rgb, desc.material.metallic);
    float3 normal = normalize(desc.normal.xyz);
    float3 lightDir = normalize(light.position.xyz - desc.posW.xyz);
    float3 viewDir = normalize(desc.posC.xyz - desc.posW.xyz);
    float3 halfwayDir = normalize(lightDir + viewDir);
    
    float distance = length(light.position.xyz - desc.posW.xyz);
    float attenuation = 1.0f / (distance * distance);
    float3 radiance = light.color.rgb * attenuation;
    float nDotL = max(dot(normal, lightDir), 0.0f);
    
    float d = DistributionGGX(normal, halfwayDir, desc.material.roughness);
    float g = GeometrySmith(normal, viewDir, lightDir, desc.material.roughness);
    float3 f = FrenselSchlick(halfwayDir, viewDir, f0);
   
    float3 kd = float3(1.0f, 1.0f, 1.0f) - f;
    kd = kd * (1.0f - desc.material.metallic);
    
     //spotlight intensity = (theta - outerCutoff) / (innerCutoff - outerCutoff)
    float theta = dot(lightDir, normalize(-light.direction.xyz));
    float epsilon = (light.innerCutoff - light.outerCutoff);
    float spotLightIntensity = clamp((theta - light.outerCutoff) / epsilon, 0.0f, 1.0f);
    
    f *= spotLightIntensity;
    kd *= spotLightIntensity;
    
    float3 numerator = d * f * g;
    float denominator = 4.0f * max(dot(normal, halfwayDir), 0.0f) * nDotL + 0.0001f;
    float3 specular = numerator / denominator;
    
    return (kd * desc.material.albedo.rgb / PI + specular) * radiance * nDotL;
}

#endif