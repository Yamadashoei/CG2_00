
//02_01 p6
struct Material
{
    float32_t4 color;
};

ConstantBuffer<Material> gMaterial : register(b0);
//02_00 p16 //02_01_èCê≥ p6
struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};


PixelShaderOutput main()
{
    PixelShaderOutput output;
    output.color = gMaterial.color;
    return output;
}