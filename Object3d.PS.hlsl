#include "object3d.hlsli"

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    
    float shininess;
};

struct DirectionalLight
{
    float32_t4 color; //ライトの色
    float32_t3 direction; //ライトの向き
    float intensity; //輝度
};

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

struct Camera
{
    float32_t3 worldPosition;
};

Texture2D<float32_t4> gTexture : register(t0);
ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<Camera> gCamera : register(b2);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

SamplerState gSampler : register(s0);


PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
 
    if (gMaterial.enableLighting != 0)
    {
        float NdotL = dot(normalize(input.normal), normalize(-gDirectionalLight.direction));
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);

        float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
        float3 reflectLight = reflect(normalize(gDirectionalLight.direction), normalize(input.normal));

         // HalfVectorを求めて計算する
        float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);
        float NDotH = dot(normalize(input.normal), halfVector);
        
        float RdotE = dot(reflectLight, toEye);
        float specularPow = pow(saturate(RdotE), gMaterial.shininess); //shines値変更

     // 拡散反射
        float3 diffuse =
     gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
     // 鏡面反射
        float3 specular =
     gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
     // 拡散反射+鏡面反射
        output.color.rgb = diffuse + specular;
     // アルファは今まで通り
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else
    {
        output.color = gMaterial.color * textureColor;

    }
 
    return output;
}