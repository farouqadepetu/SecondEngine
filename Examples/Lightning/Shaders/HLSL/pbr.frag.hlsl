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
    PBRMaterial material;
    
    if (constants.currentMapping == TEXTURE)
    {
        material.albedo = pow(gBricksColor.Sample(gSampler, vout.outTexCoords), 2.2f);
        material.ao = gBricksAO.Sample(gSampler, vout.outTexCoords).r * 0.1f;
        material.roughness = gBricksRoughness.Sample(gSampler, vout.outTexCoords).r;
        material.metallic = 0.0f;
        
        normal = gBricksNormal.Sample(gSampler, vout.outTexCoords);
        normal = normal * 2.0f - 1.0f;
        normal.xy *= constants.normalScale;
        normal = mul(normalize(normal), vout.outTBN);
    }
    else //MATERIAL
    {
        material.albedo = pbrMaterial.albedo;
        normal = vout.outNormal;
        material.roughness = pbrMaterial.roughness;
        material.ao = pbrMaterial.ao;
        material.metallic = pbrMaterial.metallic;
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
    
    float3 ambient = material.albedo.rgb * material.ao;
    finalColor += ambient;
    
    //tonemapping
    finalColor = finalColor / (finalColor + float3(1.0f, 1.0f, 1.0f));
    
    float gammaCorrection = 1.0f / 2.2f;
    finalColor = pow(finalColor, float3(gammaCorrection, gammaCorrection, gammaCorrection));
    
    return float4(finalColor, 1.0f);
}