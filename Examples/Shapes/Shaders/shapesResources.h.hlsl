struct VertexInput
{
    float4 inputPosition : POSITION;
    float4 inputNormal : NORMAL;
    float2 inputTexCoords : TEXCOORD;
};

struct VertexOutput
{
    float4 outputPosition : SV_Position;
    float4 outputNormal : NORMAL;
    float2 outputTexCoords : TEXCOORD;
};