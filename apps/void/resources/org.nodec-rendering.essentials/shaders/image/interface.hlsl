// --- Image Shader Interface ---

#include "../common/interface.hlsl"

struct V2P {
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD0;
};

Texture2D texAlbedo : register(t0);

SamplerState sampler_texAlbedo : register(s0);
