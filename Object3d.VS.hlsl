#include "object3d.hlsli"

struct Transformationmatrix
{
    float32_t4x4 WVP;
    float32_t4x4 World;
    float32_t4 color;
};
ConstantBuffer<Transformationmatrix> gTransformationMatrix : register(b0);

struct VertexShaderInput //input
{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal:NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gTransformationMatrix.WVP);
    output.normal = normalize(mul(input.normal, (float32_t3x3) gTransformationMatrix.World));
    output.texcoord = input.texcoord;
    output.worldPosition = mul(input.position, gTransformationMatrix.World).xyz;
   
    return output;
};