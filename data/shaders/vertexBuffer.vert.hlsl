#define LocalRootSignature "RootFlags( ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | DENY_VERTEX_SHADER_ROOT_ACCESS)" \
              
struct VertexInput
{
    float4 inPos : POSITION;
    float4 inColor : COLOR;
};

struct VertexOutput
{
    float4 color : COLOR;
    float4 position : SV_Position;
};

VertexOutput main(VertexInput vertexInput)
{
    float3 inColor = vertexInput.inColor;
    float4 position = vertexInput.inPos;

    VertexOutput output;
    output.position = position;
    output.color = inColor;
    return output;
}