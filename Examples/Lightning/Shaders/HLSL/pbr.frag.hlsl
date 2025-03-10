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
    float4 sampledAlbedo = pow(gBricksColor.Sample(gSampler, vout.outTexCoords), 2.2f);
    float4 sampledNormal = gBricksNormal.Sample(gSampler, vout.outTexCoords);
    float sampledAO = gBricksAO.Sample(gSampler, vout.outTexCoords).r;
    float sampledRoughness = gBricksRoughness.Sample(gSampler, vout.outTexCoords).r;
    float displacement = gBricksDisplacement.Sample(gSampler, vout.outTexCoords).r;

    //Change range from [0,1] -> [-1,1]
    sampledNormal = sampledNormal * 2.0f - 1.0f;
    sampledNormal.xy *= constants.normalScale;
    sampledNormal = mul(normalize(sampledNormal), vout.outTBN);
    
    float4 albedo = pbrMaterial.albedo * (constants.currentMapping == MATERIAL) +
    sampledAlbedo * (constants.currentMapping == TEXTURE);
    
    float4 normal = vout.outNormal * (constants.currentMapping == MATERIAL) +
    sampledNormal * (constants.currentMapping == TEXTURE);
    
    float roughness = pbrMaterial.roughness * (constants.currentMapping == MATERIAL) +
    sampledRoughness * (constants.currentMapping == TEXTURE);
    
    float ao = pbrMaterial.ao * (constants.currentMapping == MATERIAL) + 
    sampledAO * (constants.currentMapping == TEXTURE);

    PBRPixelDesc desc;
    desc.normal = normal;
    desc.posW = vout.outPosW;
    desc.posC = cameraPos;
    desc.material.albedo = albedo;
    desc.material.roughness = roughness;
    desc.material.metallic = pbrMaterial.metallic;
    desc.material.ao = ao;
    
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    for (uint pLight = 0; pLight < NUM_POINT_LIGHTS; ++pLight)
    {
        finalColor += ComputePointLight(pointLight, desc) *
        (constants.currentLightSource == POINT || constants.currentLightSource == ALL);
    }
    
    for (uint dLight = 0; dLight < NUM_DIRECTIONAL_LIGHTS; ++dLight)
    {
        finalColor += ComputeDirectionalLight(directionalLight, desc) * 
        (constants.currentLightSource == DIRECTIONAL || constants.currentLightSource == ALL);
    }
    
    for (uint sLight = 0; sLight < NUM_SPOTLIGHTS; ++sLight)
    {
        finalColor += ComputeSpotlight(spotLight, desc) * 
        (constants.currentLightSource == SPOTLIGHT || constants.currentLightSource == ALL);
    }
    
    float3 ambient = float3(0.1f, 0.1f, 0.1f) * albedo.rgb * ao;
    finalColor += ambient;
    
    //tonemapping
    finalColor = finalColor / (finalColor + float3(1.0f, 1.0f, 1.0f));
    
    float gammaCorrection = 1.0f / 2.2f;
    finalColor = pow(finalColor, float3(gammaCorrection, gammaCorrection, gammaCorrection));
    
    return float4(finalColor, 1.0f);
}