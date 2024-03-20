cbuffer viewCB : register(b0)
{
    row_major float4x4 viewMatrix : packoffset(c0);
    row_major float4x4 projectionMatrix : packoffset(c4);
    float4 ambientColor;
    float4 lightColor;
    float4 lightDir;
};

cbuffer transformCB : register(b1)
{
    row_major float4x4 modelMatrix;
};

struct VertexInput
{
    float3 inPos : POSITION;
    float3 inNormal : NORMAL;
    float2 inuv : TEXCOORD;
};

struct VertexOutput
{
    float3 normal : COLOR;
    float2 uv : TEXCOORD;
    float4 position : SV_Position;
};

VertexOutput main(VertexInput vertexInput)
{
    VertexOutput output;
    output.position = mul(float4(vertexInput.inPos, 1.0f), mul(modelMatrix, mul(viewMatrix, projectionMatrix)));
    output.normal = mul(float4(vertexInput.inNormal, 1.0f), transpose(modelMatrix)).xyz;

    // float3x3 normalMatrix = transpose((float3x3)modelMatrix);
    // float3 Normal = mul(vertexInput.inNormal, normalMatrix);
    // output.normal = Normal;

    output.uv = vertexInput.inuv;
    return output;
}