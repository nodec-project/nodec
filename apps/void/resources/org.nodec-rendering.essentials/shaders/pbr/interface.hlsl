
struct DirectionalLight
{
    float3 direction;
    float intensity;
    float4 color;
    int enabled;
};

struct SceneLighting
{
    DirectionalLight directional;
};

struct SceneProperties
{
    float4 cameraPos;
    SceneLighting lights;
};

cbuffer cbSceneProperties : register(b0)
{
    SceneProperties sceneProperties;
};

struct ModelProperties
{
    matrix matrixMVP;
    matrix matrixM;
    matrix matrixMInverse;
};

cbuffer cbModelProperties : register(b1)
{
    ModelProperties modelProperties;
}


struct MaterialProperties
{
    float metallic;
    float roughness;
    float4 albedo;
};

cbuffer cbMaterialProperties : register(b2)
{
    MaterialProperties materialProperties;
};

struct TextureConfig
{
    uint texHasFlag;
};

cbuffer cbTextureConfig : register(b3)
{
    TextureConfig textureConfig;
}

Texture2D texAlbedo           : register(t0);
Texture2D texNormal           : register(t1);
Texture2D texHeight           : register(t2);
Texture2D texMetallic         : register(t3);
Texture2D texRoughtness       : register(t4);
Texture2D texAmbientOcclusion : register(t5);

SamplerState sampler_texAlbedo           : register(s0);
SamplerState sampler_texNormal           : register(s1);
SamplerState sampler_texHeight           : register(s2);
SamplerState sampler_texMetallic         : register(s3);
SamplerState sampler_texRoughtness       : register(s4);
SamplerState sampler_texAmbientOcclusion : register(s5);


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