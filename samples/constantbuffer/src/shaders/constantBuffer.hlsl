cbuffer transformCB : register(b0)
{
    float4x4 modelMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};
struct VertexInput
{
    float2 inPos : POSITION;
    float2 inuv : TEXCOORD;
    float3 inColor : COLOR;
};

struct VertexOutput
{
    float2 uv : TEXCOORD;
    float3 color : COLOR;
    float4 position : SV_Position;
};

VertexOutput VSmain(VertexInput vertexInput)
{
    float3 inColor = vertexInput.inColor;
    float4 position = mul(mul(projectionMatrix, mul(viewMatrix, modelMatrix)), float4(vertexInput.inPos, 0.0, 1.0f));

    VertexOutput output;
    output.position = position;
    output.uv = vertexInput.inuv;
    output.color = inColor;
    return output;
}

struct PixelInput
{
    float2 uv : TEXCOORD;
    float3 color : COLOR;
};

struct PixelOutput
{
    float4 attachment0 : SV_Target0;
};

Texture2D g_texture : register(t0);
SamplerState samp : register(s0);

PixelOutput PSmain(PixelInput pixelInput)
{
    float3 textureColour = g_texture.Sample(samp, pixelInput.uv).rgb;

    PixelOutput output;
    output.attachment0 = float4(textureColour, 1.0);
    return output;
}