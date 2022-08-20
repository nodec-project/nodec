#include "../common/interface.hlsl"

struct V2P {
    float4 position : SV_Position;
    float3 worldPos : POSITION;
    float3 worldNormal : NORMAL;
    float2 texcoord : TEXCOORD0;
    float3 worldTangent : TANGENT;
};

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
