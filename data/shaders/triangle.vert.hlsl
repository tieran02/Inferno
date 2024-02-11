#define LocalRootSignature "RootFlags( ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | DENY_VERTEX_SHADER_ROOT_ACCESS)" \
              

struct VertexInput
{
    float3 inPos : POSITION;
};

struct VertexOutput
{
    float4 position : SV_Position;
};

VertexOutput main(VertexInput vertexInput)
{
    float3 inPos = vertexInput.inPos;
    float4 position = float4(inPos, 1.0f);

    VertexOutput output;
    output.position = position;
    return output;
}