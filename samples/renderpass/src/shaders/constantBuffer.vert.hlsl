cbuffer viewCB : register(b0)
{
    row_major float4x4 viewMatrix : packoffset(c0);
    row_major float4x4 projectionMatrix : packoffset(c4);
};

cbuffer transformCB : register(b1)
{
    row_major float4x4 modelMatrix : packoffset(c0);
};

struct VertexInput
{
    float3 inPos : POSITION;
    float3 inColor : COLOR;
    float2 inuv : TEXCOORD;
};

struct VertexOutput
{
    float2 uv : TEXCOORD;
    float3 color : COLOR;
    float4 position : SV_Position;
};

VertexOutput main(VertexInput vertexInput)
{
    float3 inColor = vertexInput.inColor;
    float4 position = mul(float4(vertexInput.inPos, 1.0f), mul(modelMatrix, mul(viewMatrix, projectionMatrix)));

    VertexOutput output;
    output.position = position;
    output.uv = vertexInput.inuv;
    output.color = inColor;
    return output;
}