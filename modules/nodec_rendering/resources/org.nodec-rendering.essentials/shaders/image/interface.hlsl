// --- Image Shader Interface ---

#include "../common/interface_model.hlsl"

struct V2P {
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD0;
};

struct MaterialProperties {
    float4 albedo;
};

cbuffer cbMaterialProperties : register(b3)
{
    MaterialProperties materialProperties;
};

Texture2D texAlbedo : register(t0);
Texture2D texMask : register(t1);

SamplerState sampler_texAlbedo : register(s0);
SamplerState sampler_texMask : register(s1);
