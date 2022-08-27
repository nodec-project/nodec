#include "process_interface.hlsl"

float4 PSMain(V2P input) : SV_TARGET {

    float3 illumination = texScreen.Sample(sampler_tex, input.texcoord); 
    // we need to gamma correct at the end of the shader.
    // details: https://learnopengl.com/PBR/Lighting
    // TODO: Move to postprocess.
    // illumination = illumination / (illumination + float3(1.0, 1.0, 1.0));
    illumination = pow(illumination, 1.0/2.2);  

    return float4(illumination, 1.0f);
}