struct VertexInput
{
    float2 inPos : POSITION;
    float3 inColor : COLOR;
};

struct VertexOutput
{
    float3 color : COLOR;
    float4 position : SV_Position;
};

VertexOutput main(VertexInput vertexInput)
{
    float3 inColor = vertexInput.inColor;
    float4 position = float4(vertexInput.inPos, 0.0,1.0);

    VertexOutput output;
    output.position = position;
    output.color = inColor;
    return output;
}