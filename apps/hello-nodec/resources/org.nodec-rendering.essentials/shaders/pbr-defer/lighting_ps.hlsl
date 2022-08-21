
#include "lighting_interface.hlsl"

#include "../common/brdf.hlsl"


float4 PSMain(V2P input) : SV_TARGET {
    if (texAlbedo.Sample(sampler_tex, input.texcoord).a < 0.01f) discard;

    const float3 cameraPos = sceneProperties.cameraPos.xyz;
    
    BRDFSurface surface;

    surface.albedo = texAlbedo.Sample(sampler_tex, input.texcoord);
    surface.normal = texNormal.Sample(sampler_tex, input.texcoord);
    const float4 matProps = texMatProps.Sample(sampler_tex, input.texcoord);
    surface.roughness = matProps.r;
    surface.metallic = matProps.g;

    // Reconstruct the world position from depth.
    const float nonLinearDepth = texDepth.Sample(sampler_tex, input.texcoord).r;
    const float3 viewSpacePosition = ViewSpacePosition(nonLinearDepth, input.texcoord, sceneProperties.matrixPInverse);
    const float3 worldPosition = mul(sceneProperties.matrixVInverse, float4(viewSpacePosition, 1)).xyz;

    const float3 viewDir = normalize(worldPosition - cameraPos); // camera to obj 

    // float4 color;
    // // First 10% of the depth buffer color red.
    // if(depth < 0.9f)
    // {
    //     color = float4(1.0, 0.0f, 0.0f, 1.0f);
    // }
	
    // // The next 0.025% portion of the depth buffer color green.
    // if(depth > 0.9f)
    // {
    //     color = float4(0.0, 1.0f, 0.0f, 1.0f);
    // }

    // // The remainder of the depth buffer color blue.
    // if(depth > 0.925f)
    // {
    //     color = float4(0.0, 0.0f, 1.0f, 1.0f);
    // }

    // return color;

    // return float4(worldPosition, 1);

    float3 outDiffuseSpecular = float3(0.0f, 0.0f, 0.0f);
    
    if (sceneProperties.lights.directional.enabled != 0) {
        const float3 wi = normalize(-sceneProperties.lights.directional.direction);
        const float3 radiance = sceneProperties.lights.directional.color * sceneProperties.lights.directional.intensity;
    
        outDiffuseSpecular += BRDF(surface, wi, -viewDir) * radiance * saturate(dot(surface.normal, wi));
    }

    float3 illumination = outDiffuseSpecular;
    return float4(illumination, 1.0f);
    
    // return float4(1, 0, 0.5, 1);
}