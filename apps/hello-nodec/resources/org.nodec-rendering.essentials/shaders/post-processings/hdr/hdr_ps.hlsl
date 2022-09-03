#include "hdr_interface.hlsl"

float4 PSMain(V2P input) : SV_TARGET {

    float3 hdrColor = texScreen.Sample(sampler_tex, input.texcoord);

    // Exposure tone mapping
    //  https://learnopengl.com/Advanced-Lighting/HDR

    hdrColor = float3(1.0, 1.0, 1.0) - exp(-hdrColor * materialProperties.exposure);
    
    hdrColor = pow(hdrColor, 1.0 / materialProperties.gamma);

    return float4(hdrColor, 1.0f);
}