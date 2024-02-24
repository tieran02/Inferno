struct PixelOutput
{
    float4 attachment0 : SV_Target0;
};

PixelOutput main()
{
    PixelOutput output;
    output.attachment0 = float4(1.0,0.0,0.0,1.0);
    return output;
}