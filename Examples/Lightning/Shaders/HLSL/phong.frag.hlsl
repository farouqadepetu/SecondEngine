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
    float sampledAmbient = gBricksAO.Sample(gSampler, vout.outTexCoords).r;
    float4 sampledDiffuse = pow(gBricksColor.Sample(gSampler, vout.outTexCoords), 2.2f);
    float roughness = gBricksRoughness.Sample(gSampler, vout.outTexCoords).r;
    float4 sampledSpecular = float4(roughness, roughness, roughness, 1.0f);
    float4 sampledNormal = gBricksNormal.Sample(gSampler, vout.outTexCoords);
    float displacement = gBricksDisplacement.Sample(gSampler, vout.outTexCoords).r;

    //Change range from [0,1] -> [-1,1]
    sampledNormal = sampledNormal * 2.0f - 1.0f;
    sampledNormal.xy *= constants.normalScale;
    sampledNormal = mul(sampledNormal, vout.outTBN);
    
    float ambientIntensity = phongMaterial.ambientIntensity * (constants.currentMapping == MATERIAL) +
    sampledAmbient * (constants.currentMapping == TEXTURE);
    
    float4 diffuseColor = phongMaterial.diffuse * (constants.currentMapping == MATERIAL) +
    sampledDiffuse * (constants.currentMapping == TEXTURE);
    
    float4 specularColor = phongMaterial.specular * (constants.currentMapping == MATERIAL) +
    sampledSpecular * (constants.currentMapping == TEXTURE);
    
    float4 normal = vout.outNormal * (constants.currentMapping == MATERIAL) +
    sampledNormal * (constants.currentMapping == TEXTURE);
    
    float shininessValue = phongMaterial.shininess * (constants.currentMapping == MATERIAL) +
    256.0f * (constants.currentMapping == TEXTURE);
   
    PhongPixelDesc desc;
    desc.normal = normal;
    desc.posW = vout.outPosW;
    desc.posC = cameraPos;
    desc.material.ambientIntensity = ambientIntensity;
    desc.material.diffuse = diffuseColor;
    desc.material.specular = specularColor;
    desc.material.shininess = shininessValue;
    
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    for (uint pLight = 0; pLight < NUM_POINT_LIGHTS; ++pLight)
    {
        finalColor += ComputePointLight(pointLight, desc) * (constants.currentLightSource == POINT || constants.currentLightSource == ALL);
    }
    
    for (uint dLight = 0; dLight < NUM_DIRECTIONAL_LIGHTS; ++dLight)
    {
        finalColor += ComputeDirectionalLight(directionalLight, desc) * 
        (constants.currentLightSource == DIRECTIONAL || constants.currentLightSource == ALL);
    }
    
    for (uint sLight = 0; sLight < NUM_SPOTLIGHTS; ++sLight)
    {
        finalColor += ComputeSpotlight(spotLight, desc) * (constants.currentLightSource == SPOTLIGHT || constants.currentLightSource == ALL);
    }
    
    float3 ambient = float3(0.05f, 0.05f, 0.05f) * ambientIntensity * diffuseColor.rgb;
    finalColor += ambient;
    
    //tonemapping
    finalColor = finalColor / (finalColor + float3(1.0f, 1.0f, 1.0f));
    
    float gammaCorrection = 1.0f / 2.2f;
    finalColor = pow(finalColor, float3(gammaCorrection, gammaCorrection, gammaCorrection));
    
    return float4(finalColor, 1.0f);
}