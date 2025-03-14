#include "resources.h.hlsl"

struct VertexOutput
{
    float4 outPosH : SV_Position;
    float4 outPosW : POSITION0; //world position
    float4 outPosL : POSITION1; //light space position
    float4 outNormal : NORMAL;
    float4 outTangent : TANGENT0;
    float4 outBiTangent : BINORMAL;
    float4x4 outTBN : TANGENT1;
    float2 outTexCoords : TEXCOORD;
};

float4 psMain(VertexOutput vout) : SV_Target
{
    float4 sampledAlbedo = pow(gWoodColor.Sample(gSampler, vout.outTexCoords), 2.2f);
    float4 sampledNormal = gWoodNormal.Sample(gSampler, vout.outTexCoords);
    float sampledRoughness = gWoodRoughness.Sample(gSampler, vout.outTexCoords).r;

    //Change range from [0,1] -> [-1,1]
    sampledNormal = sampledNormal * 2.0f - 1.0f;
    sampledNormal.xy *= 0.1f;
    sampledNormal = mul(sampledNormal, vout.outTBN);
    
    PBRPixelDesc desc;
    desc.posL = vout.outPosL;
    desc.normal = normalize(sampledNormal.xyz);
    desc.viewDir = normalize(cameraPos.xyz - vout.outPosW.xyz);
    desc.shadowMap = gShadowMap;
    desc.samplerState = gSampler;
    desc.material.albedo = sampledAlbedo;
    desc.material.roughness = sampledRoughness;
    desc.material.metallic = 0.0f;
    desc.material.ao = 1.0f;
    
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    for (uint pLight = 0; pLight < NUM_POINT_LIGHTS; ++pLight)
    {
        desc.lightDir = normalize(pointLight.position.xyz - vout.outPosW.xyz);
        desc.halfwayDir = normalize(desc.lightDir + desc.viewDir);
        desc.distance = length(pointLight.position.xyz - vout.outPosW.xyz);
        
        finalColor += ComputePointLight(pointLight, desc) *
        (constants.currentLightSource == POINT || constants.currentLightSource == ALL);
        
        //float shadow = ComputeShadow(desc);
        //finalColor = finalColor * (1.0f - shadow);
    }
    
    for (uint dLight = 0; dLight < NUM_DIRECTIONAL_LIGHTS; ++dLight)
    {
        desc.lightDir = normalize(-directionalLight.direction.xyz);
        desc.halfwayDir = normalize(desc.lightDir + desc.viewDir);
        
        finalColor += ComputeDirectionalLight(directionalLight, desc) * 
        (constants.currentLightSource == DIRECTIONAL || constants.currentLightSource == ALL);
        
        //float shadow = ComputeShadow(desc);
        //finalColor = finalColor * (1.0f - shadow);
    }
    
    for (uint sLight = 0; sLight < NUM_SPOTLIGHTS; ++sLight)
    {
        desc.lightDir = normalize(pointLight.position.xyz - vout.outPosW.xyz);
        desc.halfwayDir = normalize(desc.lightDir + desc.viewDir);
        desc.distance = length(pointLight.position.xyz - vout.outPosW.xyz);
        
        finalColor += ComputeSpotlight(spotLight, desc) * 
        (constants.currentLightSource == SPOTLIGHT || constants.currentLightSource == ALL);
        
        //float shadow = ComputeShadow(desc);
        //finalColor = finalColor * (1.0f - shadow);
    }
    
    float shadow = ComputeShadow(desc);
    finalColor = finalColor * (1.0f - shadow);
    
    float3 ambient = float3(0.1f, 0.1f, 0.1f) * sampledAlbedo.rgb * 1.0f;
    finalColor += ambient;
    
    //tonemapping
    finalColor = finalColor / (finalColor + float3(1.0f, 1.0f, 1.0f));
    
    float gammaCorrection = 1.0f / 2.2f;
    finalColor = pow(finalColor, float3(gammaCorrection, gammaCorrection, gammaCorrection));
    
    return float4(finalColor, 1.0f);
}