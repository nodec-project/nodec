
#include "lighting_interface.hlsl"

float4 PSMain(V2P input) : SV_TARGET {
    if (texMatProps.Sample(sampler_tex, input.texcoord).a < 0.01f) discard;
    return texNormal.Sample(sampler_tex, input.texcoord);
    return float4(1, 0, 0.5, 1);
}