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

float ComputeShadow(float4 lightSpacePos, float2 texCoords)
{
    //perspective divide
    //range is [-1, 1] after the divide
    float3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    
    //transform range from [-1, 1] -> [0, 1]
    projCoords = projCoords * 0.5f + 0.5f;
    
    float closetDepth = gShadowMap.Sample(gSampler, texCoords).r;
    
    float currentDepth = projCoords.z;
    
    //if true, the pixel is in shadow
    //return 1.0 or 0.0
    return float(currentDepth > closetDepth);
}

float4 psMain(VertexOutput vout) : SV_Target
{
    float4 sampledDiffuse = pow(gWoodColor.Sample(gSampler, vout.outTexCoords), 2.2f);
    float sampledRoughness = gWoodRoughness.Sample(gSampler, vout.outTexCoords).r;
    float4 sampledNormal = gWoodNormal.Sample(gSampler, vout.outTexCoords);

    //Change range from [0,1] -> [-1,1]
    sampledNormal = sampledNormal * 2.0f - 1.0f;
    sampledNormal.xy *= 2.0f;
    sampledNormal = mul(sampledNormal, vout.outTBN);
   
    PhongPixelDesc desc;
    desc.normal = sampledNormal;
    desc.posW = vout.outPosW;
    desc.posC = cameraPos;
    desc.material.ambientIntensity = 1.0f;
    desc.material.diffuse = sampledDiffuse;
    desc.material.specular = float4(sampledRoughness, sampledRoughness, sampledRoughness, 1.0f);
    desc.material.shininess = 256.0f;
    
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
    
    float shadow = ComputeShadow(vout.outPosL, vout.outTexCoords);
    
    finalColor = finalColor * (1.0f - shadow);
    
    float3 ambient = float3(0.05f, 0.05f, 0.05f) * sampledDiffuse.rgb * 1.0f;
    finalColor += ambient;
    
    //tonemapping
    finalColor = finalColor / (finalColor + float3(1.0f, 1.0f, 1.0f));
    
    float gammaCorrection = 1.0f / 2.2f;
    finalColor = pow(finalColor, float3(gammaCorrection, gammaCorrection, gammaCorrection));
    
    return float4(finalColor, 1.0f);
}