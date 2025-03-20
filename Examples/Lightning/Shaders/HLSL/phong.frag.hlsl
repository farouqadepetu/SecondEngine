#include "resources.h.hlsl"

struct VertexOutput
{
    float4 outPosH : SV_Position;
    float4 outPosW : POSITION; //world position
    float4 outNormal : NORMAL;
    float4 outTangent : TANGENT0;
    float4 outBiTangent : BINORMAL;
    float4x4 outTBN : TANGENT1;
    float2 outTexCoords : TEXCOORD;
};

float4 psMain(VertexOutput vout) : SV_Target
{
    float4 normal;
    PhongMaterial material;
    
    if (constants.currentMapping == TEXTURE)
    {
        material.diffuse = pow(gBricksColor.Sample(gSampler, vout.outTexCoords), 2.2f);
        material.ambientIntensity = gBricksAO.Sample(gSampler, vout.outTexCoords).r * 0.1f;
        
        float roughness = gBricksRoughness.Sample(gSampler, vout.outTexCoords).r;
        material.specular = float4(roughness, roughness, roughness, 1.0f);
        
        material.shininess = 256.0f;
        
        normal = gBricksNormal.Sample(gSampler, vout.outTexCoords);
        normal = normal * 2.0f - 1.0f;
        normal.xy *= constants.normalScale;
        normal = mul(normalize(normal), vout.outTBN);
    }
    else //MATERIAL
    {
        material.diffuse = phongMaterial.diffuse;
        material.specular = phongMaterial.specular;
        material.ambientIntensity = phongMaterial.ambientIntensity;
        material.shininess = phongMaterial.shininess;
        normal = vout.outNormal;
    }
   
    PixelDesc desc;
    desc.normal = normalize(normal.xyz);
    desc.viewDir = normalize(cameraPos.xyz - vout.outPosW.xyz);
    
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    for (uint pLight = 0; pLight < constants.numPointLights; ++pLight)
    {
        desc.lightDir = normalize(pointLight.position.xyz - vout.outPosW.xyz);
        desc.halfwayDir = normalize(desc.lightDir + desc.viewDir);
        desc.distance = length(pointLight.position.xyz - vout.outPosW.xyz);
        finalColor += ComputePointLight(pointLight, material, desc);
    }
  
    for (uint dLight = 0; dLight < constants.numDirectionalLights; ++dLight)
    {
        desc.lightDir = normalize(-directionalLight.direction.xyz);
        desc.halfwayDir = normalize(desc.lightDir + desc.viewDir);
        finalColor += ComputeDirectionalLight(directionalLight, material, desc);
    }
    
    for (uint sLight = 0; sLight < constants.numSpotlights; ++sLight)
    {
        desc.lightDir = normalize(spotLight.position.xyz - vout.outPosW.xyz);
        desc.halfwayDir = normalize(desc.lightDir + desc.viewDir);
        desc.distance = length(spotLight.position.xyz - vout.outPosW.xyz);
        finalColor += ComputeSpotlight(spotLight, material, desc);
    }
    
    float3 ambient = material.ambientIntensity * material.diffuse.rgb;
    finalColor += ambient;
    
    //tonemapping
    finalColor = finalColor / (finalColor + float3(1.0f, 1.0f, 1.0f));
    
    float gammaCorrection = 1.0f / 2.2f;
    finalColor = pow(finalColor, float3(gammaCorrection, gammaCorrection, gammaCorrection));
    
    return float4(finalColor, 1.0f);
}