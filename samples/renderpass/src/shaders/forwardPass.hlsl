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

VertexOutput VSmain(VertexInput vertexInput)
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
    //float3 dir = normalize(lightDir).xyz;

    float3 finalColor;

    finalColor = diffuse * ambientColor.rgb;
    finalColor += saturate(dot(lightDir.xyz, normal) * lightColor.rgb * diffuse);

    PixelOutput output;
    output.attachment0 = float4(finalColor, 1.0);
    return output;
}