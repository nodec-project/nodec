
struct ModelProperties
{
    matrix matrixMVP;
    matrix matrixM;
    matrix matrixMInverse;
};

cbuffer cbModelProperties : register(b0)
{
    ModelProperties modelProperties;
}


struct MaterialProperties
{
    float metallic;
    float roughness;
    float4 albedo;
};

cbuffer cbMaterialProperties : register(b1)
{
    MaterialProperties materialProperties;
};

struct TextureConfig
{
    uint texHasFlag;
};

cbuffer cbTextureConfig : register(b2)
{
    TextureConfig textureConfig;
}

Texture2D texAlbedo : register(t0);
Texture2D texHeight : register(t1);
Texture2D texMetallic : register(t2);
Texture2D texNormal : register(t3);
Texture2D texRoughtness : register(t4);

SamplerState sampler_texAlbedo : register(s0);
SamplerState sampler_texHeight : register(s1);
SamplerState sampler_texMetallic : register(s2);
SamplerState sampler_texNormal : register(s3);
SamplerState sampler_texRoughtness : register(s4);


struct VSIn
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD0;
    float3 tangent : TANGENT0;
};


struct V2P
{
    float4 position : SV_Position;
    float3 worldPos : POSITION;
    float3 worldNormal : NORMAL;
    float2 texcoord : TEXCOORD0;
    float3 worldTangent : TANGENT;
};