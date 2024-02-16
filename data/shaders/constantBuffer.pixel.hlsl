struct PixelInput
{
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
    PixelOutput output;
    output.attachment0 = float4(pixelInput.color, 1.0);
    return output;
}