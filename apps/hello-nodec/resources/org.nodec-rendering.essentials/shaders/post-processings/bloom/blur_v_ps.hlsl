#include "blur_interface.hlsl"

float4 PSMain(V2P input) : SV_TARGET0 {
    float textureWidth;
    float textureHeight;
    texBrightness.GetDimensions(textureWidth, textureHeight);

    // gets size of single texel
    float2 texOffset = float2(1.0, 1.0) / float2(textureWidth, textureHeight);

    const half WEIGHTS[5] = {0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216};
    // current fragment's contribution
    float3 result = texBrightness.Sample(sampler_tex, input.texcoord).rgb * WEIGHTS[0];
    // return float4(result, 1.0);
    for(int i = 1; i < 5; ++i) {
        const float2 weightedOffset = float2(0.0f, texOffset.y * i);
        result += texBrightness.Sample(sampler_tex, input.texcoord + weightedOffset).rgb * WEIGHTS[i];
        result += texBrightness.Sample(sampler_tex, input.texcoord - weightedOffset).rgb * WEIGHTS[i];
    }
    return float4(result, 1.0);
}