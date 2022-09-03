#include "../../common/interface_scene.hlsl"


// --- shader.meta properties ---
struct MaterialProperties {
    float gamma;
    float exposure;
};

cbuffer cbMaterialProperties : register(b3) {
    MaterialProperties materialProperties;
};

// ---

// --- each pass properties ---
Texture2D texScreen : register(t0);
SamplerState sampler_tex : register(s0);
// ---

struct V2P {
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD0;
};