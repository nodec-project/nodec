#include "interface.hlsl"

Texture2D texBrightness : register(t0);
SamplerState sampler_tex : register(s0);

struct V2P {
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD0;
};
