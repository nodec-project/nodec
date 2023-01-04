#include "occlusion_interface.hlsl"

float4 PSMain(V2P input) : SV_TARGET {
    // TODO: Calc from texture and screen metadata.
    const float2 noiseScale = float2(1280/4, 720/4);

    const float nonLinearDepth = texDepth.Sample(sampler_tex, input.texcoord).r;

    // Since SSAO is a screen-space technique where occlusion is calculated from the visible view, 
    // it makes sense to implement the algorithm in view-space.
    const float3 position = ViewSpacePosition(nonLinearDepth, input.texcoord, sceneProperties.matrixPInverse);
    const float3 normalInWorldSpace = texNormal.Sample(sampler_tex, input.texcoord);
    const float3 normal = normalize(mul(sceneProperties.matrixV, normalInWorldSpace));
    const float3 randomVec = texNoise.Sample(sampler_tex, input.texcoord * noiseScale).xyz * 2.0 - 1.0;

    const float3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    const float3 bitangent = cross(normal, tangent);
    const float3x3 matrixTBN = float3x3(tangent, bitangent, normal);
    float occlusion = 0.0;

    // TODO: Retrieve from texture properties.
    const int kernelSize = 8;

    const float radius = materialProperties.radius;
    const float bias = materialProperties.bias;

    for (int x = 0; x < kernelSize; ++x) {
        for (int y = 0; y < kernelSize; ++y) {
            float3 samplePos = texSamples.Sample(sampler_texSamples, float2(x / (float)kernelSize, y / (float)kernelSize));
            samplePos = samplePos * 2.0 - 1.0; // [-1, 1]
            samplePos = position + mul(samplePos, matrixTBN) * radius;
            float4 offset = float4(samplePos, 1.0f);
            offset      = mul(sceneProperties.matrixP, offset);    // from view to clip-space
            // Flip y axis?
            offset.xy = ((offset.xy / offset.w) * float2(1.0f, -1.0f)) * 0.5f + 0.5f; // [0, 1]

            // src:
            // https://developer.nvidia.com/content/depth-precision-visualized
            // http://dev.theomader.com/linear-depth/
            // https://www.mvps.org/directx/articles/linear_z/linearz.htm
            // http://www.humus.name/index.php?ID=255
            const float a = sceneProperties.matrixP[2][3];
            const float b = sceneProperties.matrixP[2][2];
            const float d = texDepth.Sample(sampler_tex, offset.xy).r;
            const float sampleDepth = a / (d - b);

            // const float sampleDepth = ViewSpacePosition(
            //     texDepth.Sample(sampler_tex, offset.xy).r, offset.xy, sceneProperties.matrixPInverse).z;

            // range check & accumulate
            //if (smoothstep(0.0f, 1.0f, radius / abs(position.z - sampleDepth)) > 0) {
            //    if (sampleDepth < samplePos.z - bias) {
            //        occlusion += 1.0f;
            //    }
            //}
            float rangeCheck = smoothstep(0.0, 1.0, radius / abs(position.z - sampleDepth));
            occlusion += (sampleDepth < samplePos.z - bias ? 1.0 : 0.0) * rangeCheck;
            //occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0);  
        }
    }

    occlusion = 1.0 - (occlusion / (kernelSize * kernelSize));

    float3 illumination = texScreen.Sample(sampler_tex, input.texcoord) * occlusion;
    return float4(illumination, 1.0f);
    //return float4(occlusion.xxx, 1.0f);
}