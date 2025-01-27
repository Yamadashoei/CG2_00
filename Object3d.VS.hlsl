#include "object3d.hlsli"

struct Transformationmatrix
{
    float32_t4x4 WVP;
};
ConstantBuffer<Transformationmatrix> gTtansformationMatrix : register(b0);

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gTtansformationMatrix.WVP);
    
    output.texcoord = input.texcoord;
    return output;
};