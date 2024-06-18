#include "object3d.hlsli"

//03_00 p42
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);


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


PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    ////03_00 p43 p49
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    output.color = gMaterial.color * textureColor;//p43
    return output;
}