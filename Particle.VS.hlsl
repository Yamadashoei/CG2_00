#include "Particle.hlsli"

struct Transformationmatrix
{
    float32_t4x4 WVP;
    float32_t4x4 World;
};
ConstantBuffer<Transformationmatrix> gTransformationMatrices : register(b0);

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input, uint32_t instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    // ƒ[ƒ‹ƒhÀ•W•ÏŠ·‚ÆË‰e
    output.position = mul(input.position, gTransformationMatrices[instanceId].WVP);
    output.texcoord = input.texcoord;
    
    return output;
};