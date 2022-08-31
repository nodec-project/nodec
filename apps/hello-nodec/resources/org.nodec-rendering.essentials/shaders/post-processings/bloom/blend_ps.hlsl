#include "blend_interface.hlsl"

float4 PSMain(V2P input) : SV_TARGET0 {
    //// TODO: Move to material properties.
    //float exposure = 0.1;
    //const float gamma = 2.2;

    float3 hdrColor = texScreen.Sample(sampler_tex, input.texcoord).rgb;
    float3 bloomColor = texBrightness.Sample(sampler_tex, input.texcoord).rgb;

    // additive blending.
    hdrColor += bloomColor;
    return float4(hdrColor, 1.0f);

    //// tone mapping
    //float3 result = 1.0f - exp(-hdrColor * exposure);
    //result = pow(result, 1.0 / gamma);
    //return float4(result, 1.0f);
}