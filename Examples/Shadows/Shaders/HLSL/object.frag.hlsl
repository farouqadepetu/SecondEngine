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

float ComputeShadow(float4 posL)
{
    //perspective divide
    float3 projCoords = posL.xyz / posL.w;
    
    //transform from [-1, 1] -> [0, 1]
    projCoords.x = projCoords.x * 0.5f + 0.5f;
    projCoords.y = -projCoords.y * 0.5f + 0.5f;
    
    //get the closest depth value from light perspective
    float closestDepth = gShadowMap.Sample(gSampler, projCoords.xy).r;
    
    float currentDepth = projCoords.z;
    
    //check if the pixel is in shadow or not
    //1.0f if true (in shadow), 0.0f if false (not in shadow)
    float shadow = float((currentDepth - constants.shadowBias) < closestDepth);
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if (projCoords.z > 1.0)
        shadow = 0.0;
    
    return shadow;
}

float2 SampleCube(const float3 v, out uint faceIndex)
{
    float3 vAbs = abs(v);
    float ma;
    float2 uv;
    if (vAbs.z >= vAbs.x && vAbs.z >= vAbs.y)
    {
        faceIndex = v.z < 0.0 ? 5 : 4;
        ma = 0.5 / vAbs.z;
        uv = float2(v.z < 0.0 ? -v.x : v.x, -v.y);
    }
    else if (vAbs.y >= vAbs.x)
    {
        faceIndex = v.y < 0.0 ? 3 : 2;
        ma = 0.5 / vAbs.y;
        uv = float2(v.x, v.y < 0.0 ? -v.z : v.z);
    }
    else
    {
        faceIndex = v.x < 0.0 ? 1 : 0;
        ma = 0.5 / vAbs.x;
        uv = float2(v.x < 0.0 ? v.z : -v.z, -v.y);
    }
    
    return uv * ma + 0.5;
}

float ComputeShadowPointLight(float3 posW, float3 lightPosition, float nDotL, float farPlane)
{
    float3 lightToFrag = posW - lightPosition;
    
    uint faceIndex;
    float2 texCoords;
    float closestDepth;
    float currentDepth;
    
    texCoords = SampleCube(lightToFrag, faceIndex);
    
    closestDepth = gShadowMapPL[faceIndex].Sample(gSampler, texCoords).r;
    
    currentDepth = length(lightToFrag) / farPlane;
    
    float bias = constants.shadowBias;
    
    float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
    
    return shadow;
}

float4 psMain(VertexOutput vout) : SV_Target
{
    uint objectIndex = constants.objectIndex;
    
    PixelDesc desc;
    desc.normal = normalize(vout.outNormal.xyz);
    desc.viewDir = normalize(cameraPos.xyz - vout.outPosW.xyz);
    
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    
    if(constants.currentLightSource == DIRECTIONAL_LIGHT)
    {
        desc.lightDir = normalize(-directionalLight.direction.xyz);
        desc.halfwayDir = normalize(desc.lightDir + desc.viewDir);
        finalColor += ComputeDirectionalLight(directionalLight, material[objectIndex], desc);
        finalColor *= ComputeShadow(vout.outPosL);
    }
    else if (constants.currentLightSource == POINT_LIGHT)
    {
        desc.lightDir = normalize(pointLight.position.xyz - vout.outPosW.xyz);
        desc.halfwayDir = normalize(desc.lightDir + desc.viewDir);
        desc.distance = length(pointLight.position.xyz - vout.outPosW.xyz);
        finalColor += ComputePointLight(pointLight, material[objectIndex], desc);
        float shadow = ComputeShadowPointLight(vout.outPosW.xyz, pointLight.position.xyz, dot(desc.normal, desc.lightDir), 10.0f);
        finalColor = finalColor * (1.0f - shadow);
    }
    else //SPOTLIGHT
    {
        
    }
    
    float3 ambient = material[objectIndex].albedo.rgb * material[objectIndex].ao;
    finalColor += ambient;
    
    //tonemapping
    finalColor = finalColor / (finalColor + float3(1.0f, 1.0f, 1.0f));
    
    float gammaCorrection = 1.0f / 2.2f;
    finalColor = pow(finalColor, float3(gammaCorrection, gammaCorrection, gammaCorrection));
    
    return float4(finalColor, 1.0f);
}