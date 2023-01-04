#include "../common/interface_scene.hlsl"

Texture2D texAlbedo : register(t0);
Texture2D texNormal : register(t1);
Texture2D texMatProps : register(t2);
Texture2D texDepth : register(t3);

SamplerState sampler_tex : register(s0);

struct V2P {
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD0;
};