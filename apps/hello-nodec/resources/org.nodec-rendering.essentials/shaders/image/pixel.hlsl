#include "interface.hlsl"

float4 PSMain(V2P input) : SV_Target {
    // default is bright pink.
    float3 color = float3(1, 0, 0.5);

    if (textureConfig.texHasFlag & 0x01) {
        color = texAlbedo.Sample(sampler_texAlbedo, input.texcoord);
    }

    float mask = 1.0f;
    if (textureConfig.texHasFlag & 0x02) {
        mask = texMask.Sample(sampler_texMask, input.texcoord);
    }

    return float4(color, mask);
}