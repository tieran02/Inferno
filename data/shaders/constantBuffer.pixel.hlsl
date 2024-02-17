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

PixelOutput main(PixelInput pixelInput)
{
    float3 textureColour = g_texture.Sample(samp, pixelInput.uv).rgb;

    PixelOutput output;
    output.attachment0 = float4(textureColour, 1.0);
    return output;
}