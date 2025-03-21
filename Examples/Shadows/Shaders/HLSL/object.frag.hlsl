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

/*float ComputeShadow(float4 lightSpacePos)
{
    //perspective divide
    float3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    
    //transform from [-1, 1] -> [0, 1]
    projCoords.x = projCoords.x * 0.5f + 0.5f;
    projCoords.y = -projCoords.y * 0.5f + 0.5f;
    
    //get the closest depth value from light perspective
    float closestDepth = gShadowMap.Sample(gSampler, projCoords.xy).r;
    
    float currentDepth = projCoords.z;
    
    //check if the pixel is in shadow or not
    //1.0f if true (in shadow), 0.0f if false (not in shadow)
    float shadow = float((currentDepth - constants.shadowBias) > closestDepth);
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if (projCoords.z > 1.0)
        shadow = 0.0;
    
    return shadow;
}*/

float ComputeShadow2(float4 lightSpacePos, float3 color)
{
    //perspective divide
    float3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    
    //transform from [-1, 1] -> [0, 1]
    projCoords.x = projCoords.x * 0.5f + 0.5f;
    projCoords.y = -projCoords.y * 0.5f + 0.5f;
    
    //get the closest depth value from light perspective compared with the current depth - bias
    //if sampled depth <= current depth then 1 is returned (pixel is in shadow)
    //else 0 is returned (pixel is not in shadow)
    float closestDepth = gShadowMap.SampleCmp(gSampler, projCoords.xy, projCoords.z - constants.shadowBias);
    
    return closestDepth;
}

float4 psMain(VertexOutput vout) : SV_Target
{
    PixelDesc desc;
    desc.normal = normalize(vout.outNormal.xyz);
    desc.viewDir = normalize(cameraPos.xyz - vout.outPosW.xyz);
    
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    
    desc.lightDir = normalize(-directionalLight.direction.xyz);
    desc.halfwayDir = normalize(desc.lightDir + desc.viewDir);
    finalColor += ComputeDirectionalLight(directionalLight, material[materialIndex], desc);
    
    /*for (uint pLight = 0; pLight < 0; ++pLight)
    {
        desc.lightDir = normalize(pointLight.position.xyz - vout.outPosW.xyz);
        desc.halfwayDir = normalize(desc.lightDir + desc.viewDir);
        desc.distance = length(pointLight.position.xyz - vout.outPosW.xyz);
        finalColor += ComputePointLight(pointLight, pbrMaterial, desc);
    }
  
    for (uint dLight = 0; dLight < 1; ++dLight)
    {
        desc.lightDir = normalize(-directionalLight.direction.xyz);
        desc.halfwayDir = normalize(desc.lightDir + desc.viewDir);
        finalColor += ComputeDirectionalLight(directionalLight, pbrMaterial, desc);
    }
    
    for (uint sLight = 0; sLight < 0; ++sLight)
    {
        desc.lightDir = normalize(spotLight.position.xyz - vout.outPosW.xyz);
        desc.halfwayDir = normalize(desc.lightDir + desc.viewDir);
        desc.distance = length(spotLight.position.xyz - vout.outPosW.xyz);
        finalColor += ComputeSpotlight(spotLight, pbrMaterial, desc);
    }*/
    
    //float shadow = ComputeShadow(vout.outPosL);
    //finalColor = finalColor * (1.0f - shadow);
    
    finalColor *= ComputeShadow2(vout.outPosL, finalColor);
    
    float3 ambient = material[materialIndex].albedo.rgb * material[materialIndex].ao;
    finalColor += ambient;
    
    //tonemapping
    finalColor = finalColor / (finalColor + float3(1.0f, 1.0f, 1.0f));
    
    float gammaCorrection = 1.0f / 2.2f;
    finalColor = pow(finalColor, float3(gammaCorrection, gammaCorrection, gammaCorrection));
    
    return float4(finalColor, 1.0f);
}