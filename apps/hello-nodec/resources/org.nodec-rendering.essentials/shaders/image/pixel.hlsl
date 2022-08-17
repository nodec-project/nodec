#include "interface.hlsl"

float4 PSMain(V2P input) : SV_Target {
    // default is bright pink.
    float4 color = float4(1, 0, 0.5, 1);

    if (textureConfig.texHasFlag & 0x01) {
        color = texAlbedo.Sample(sampler_texAlbedo, input.texcoord);
    }
    return color;
}