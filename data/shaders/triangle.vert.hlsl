static const float2 positions[3] =
{
    float2(0.0, 0.5),
    float2(0.5, -0.5),
    float2(-0.5, -0.5)
  };

struct VertexOutput
{
    float4 position : SV_Position;
};

VertexOutput main(uint vid : SV_VertexID)
{
    float4 position = float4(positions[vid], 0.0, 1.0);

    VertexOutput output;
    output.position = position;
    return output;
}