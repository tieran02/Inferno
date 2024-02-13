struct PixelInput
{
    float4 color : COLOR;
};

struct PixelOutput
{
    float4 attachment0 : SV_Target0;
};

PixelOutput main(PixelInput pixelInput)
{
    PixelOutput output;
    output.attachment0 = pixelInput.color;
    return output;
}