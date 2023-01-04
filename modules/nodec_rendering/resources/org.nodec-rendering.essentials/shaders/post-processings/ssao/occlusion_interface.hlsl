#include "../../common/interface_scene.hlsl"

// --- shader.meta properties ---
struct MaterialProperties
{
    float radius;
    float bias;
};

cbuffer cbMaterialProperties : register(b3)
{
    MaterialProperties materialProperties;
};

Texture2D texNoise : register(t0);
SamplerState sampler_texNoise : register(s0);

Texture2D texSamples : register(t1);
SamplerState sampler_texSamples : register(s1);

// ---

// --- each pass properties ---
Texture2D texDepth : register(t2);
Texture2D texNormal : register(t3);
Texture2D texScreen : register(t4);
SamplerState sampler_tex : register(s2);
// ---

struct V2P {
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD0;
};