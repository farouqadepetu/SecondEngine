#include "resources.h.hlsl"

struct VertexInput
{
    float4 inPos : POSITION;
    float4 inNormal : NORMAL;
    float4 inTangent : TANGENT;
    float2 inTexCoords : TEXCOORD;
};

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

VertexOutput vsMain(VertexInput vin)
{
    VertexOutput vout;
    
    uint objectIndex = constants.objectIndex;
    uint lightIndex = 0;

    float4 posW = mul(vin.inPos, objectModel[objectIndex]);
    if(constants.currentLightSource == POINT_LIGHT)
    {
        float3 lightToFrag = posW.xyz - pointLight.position.xyz;
        float2 uv;
        
        uv = SampleCube(lightToFrag, lightIndex);

    }
    else if (constants.currentLightSource == SPOTLIGHT)
    {
        lightIndex = 7;
    }
    
    float4x4 mvp = mul(objectModel[objectIndex], mul(cameraView, cameraProjection));
    float4 posH = mul(vin.inPos, mvp);
    float4x4 shadowMatrix = mul(objectModel[objectIndex], mul(lightSourceView[lightIndex], lightSourceProjection[lightIndex]));
    float4 posL = mul(vin.inPos, shadowMatrix);
    float4 normal = mul(vin.inNormal, objectInverseModel[objectIndex]);
    float4 tangent = normalize(mul(vin.inTangent, objectInverseModel[objectIndex]));
    
    //re-orthogonalize using the gram-schmdit method.
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    float4 bitangent = float4(cross(tangent.xyz, normal.xyz), 0.0f);
    
    vout.outPosH = posH;
    vout.outPosW = posW;
    vout.outPosL = posL;
    vout.outNormal = normal;
    vout.outTangent = tangent;
    vout.outBiTangent = bitangent;
    vout.outTBN = float4x4(tangent, bitangent, normal, float4(0.0f, 0.0f, 0.0f, 1.0f));
    vout.outTexCoords = vin.inTexCoords;
    
    return vout;
}