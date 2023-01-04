#include "brightness_interface.hlsl"

struct PSOut {
    float4 brightColor : SV_TARGET0;
};

PSOut PSMain(V2P input) {
    PSOut output;
    float3 color = texScreen.Sample(sampler_tex, input.texcoord);

    // check whether fragment output is higher than threshold, if so output as brightness color
    float brightness = dot(color, float3(0.2126, 0.7152, 0.0722));

    if (brightness > materialProperties.threshold) {
        output.brightColor = float4(color, 1.0);
    } else {
        output.brightColor = float4(0.0, 0.0, 0.0, 0.0);
    }
    return output;
}