cbuffer viewCB : register(b0)
{
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float4 ambientColor;
    float4 lightColor;
    float4 lightDir;
};

cbuffer transformCB : register(b1)
{
    float4x4 modelMatrix;
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

VertexOutput VSmain(VertexInput vertexInput)
{
    VertexOutput output;
    output.position = mul(mul(projectionMatrix, mul(viewMatrix, modelMatrix)), float4(vertexInput.inPos, 1.0f));
    output.normal = mul((float3x3)modelMatrix, vertexInput.inNormal);

    output.uv = vertexInput.inuv;
    return output;
}


struct PixelInput
{
    float3 normal : COLOR;
    float2 uv : TEXCOORD;
};

struct PixelOutput
{
    float4 attachment0 : SV_Target0;
};

Texture2D g_texture : register(t0);
SamplerState samp : register(s0);

PixelOutput PSmain(PixelInput pixelInput)
{
    float3 diffuse = g_texture.Sample(samp, pixelInput.uv).rgb;
    float3 normal = normalize(pixelInput.normal).xyz;
    float3 dir = normalize(lightDir).xyz;

    float3 finalColor;

    finalColor = diffuse * ambientColor.rgb;
    finalColor += saturate(dot(dir, normal) * lightColor.rgb * diffuse);

    PixelOutput output;
    output.attachment0 = float4(finalColor, 1.0);
    return output;
}