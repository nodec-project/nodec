/**
* pbr
* @require Shader Model 4 (/4_0)
*/

// about implementation:
//  * <https://github.com/vilbeyli/VQEngine-Legacy/blob/master/Source/Shaders/forward_brdf_ps.hlsl>

#include "interface.hlsl"

#include "../common/brdf.hlsl"

float2 ApplyParallaxOffset(float2 uv, float3 viewDirInTangentSpace, Texture2D heightMap, SamplerState samplerHeightMap, float dotVN)
{
    //src: https://www.gamedev.net/articles/programming/graphics/a-closer-look-at-parallax-occlusion-mapping-r3262/

    const float fHeigtMapScale = 0.1f;
    const float nMaxSamples = 32.0f;
    const float nMinSamples = 8.0f;

    float fParallaxLimit = -length(viewDirInTangentSpace.xy) / viewDirInTangentSpace.z;
    fParallaxLimit *= fHeigtMapScale;

    float2 vOffsetDir = normalize(viewDirInTangentSpace.xy);
    float2 vMaxOffset = vOffsetDir * fParallaxLimit;

    int nNumSamples = (int) lerp(nMaxSamples, nMinSamples, dotVN);

    float fStepSize = 1.0 / (float) nNumSamples;

    float2 dx = ddx(uv);
    float2 dy = ddy(uv);

    float fCurrRayHeight = 1.0;
    float2 vCurrOffset = float2(0, 0);
    float2 vLastOffset = float2(0, 0);

    float fLastSampledHeight = 1;
    float fCurrSampledHeight = 1;

    int nCurrSample = 0;

    while (nCurrSample < nNumSamples)
    {
        fCurrSampledHeight = heightMap.SampleGrad(samplerHeightMap, uv + vCurrOffset, dx, dy).r;
        if (fCurrSampledHeight > fCurrRayHeight)
        {
            float delta1 = fCurrSampledHeight - fCurrRayHeight;
            float delta2 = (fCurrRayHeight + fStepSize) - fLastSampledHeight;

            float ratio = delta1 / (delta1 + delta2);

            vCurrOffset = (ratio) * vLastOffset + (1.0 - ratio) * vCurrOffset;

            nCurrSample = nNumSamples + 1;
        }
        else
        {
            nCurrSample++;

            fCurrRayHeight -= fStepSize;

            vLastOffset = vCurrOffset;
            vCurrOffset += fStepSize * vMaxOffset;

            fLastSampledHeight = fCurrSampledHeight;
        }
    }

    return uv + vCurrOffset;
}



float4 PSMain(V2P input) : SV_Target
{
    // normalize !!MUST DO
    // why?
    //  * <https://forum.unity.com/threads/normalizing-a-vector-in-the-vertex-or-fragment-shader-is-not-the-same.1120411/>
    input.worldNormal = normalize(input.worldNormal);
    input.worldTangent = normalize(input.worldTangent);
    
    const float3 cameraPos = sceneProperties.cameraPos.xyz;
    
    const float3 viewDir = normalize(input.worldPos - cameraPos); // camera to obj 
    
    BRDFSurface surface;
    
    // re-orthogonalize T with respect to N
    float3 worldTangent = normalize(input.worldTangent - dot(input.worldTangent, input.worldNormal) * input.worldNormal);
    // then retrieve perpendicular vector B with the cross product of T and N
    float3 worldBitangent = cross(input.worldNormal, worldTangent); // !! MULTIPLY ORDAR IMPORTANT !!
    
    float3x3 matrixTBN = float3x3(worldTangent, worldBitangent, input.worldNormal); // TBN (row-major representation)
    
    
    // t2: height
    if (textureConfig.texHasFlag & 0x04)
    {
        float3 viewDirInTangentSpace = mul(matrixTBN, viewDir);

        input.texcoord = ApplyParallaxOffset(input.texcoord, viewDirInTangentSpace, texHeight, sampler_texHeight, dot(viewDir, input.worldNormal));

        if (input.texcoord.x > 1.0 || input.texcoord.y > 1.0 || input.texcoord.x < 0.0 || input.texcoord.y < 0.0)
        {
            discard;
        }
    }
    
    // t0: albedo
    surface.albedo = (textureConfig.texHasFlag & 0x01) 
                        ? texAlbedo.Sample(sampler_texAlbedo, input.texcoord).xyz 
                        : materialProperties.albedo.xyz;
    
    // t1: normal
    if (textureConfig.texHasFlag & 0x02)
    {
        float3 normal = texNormal.Sample(sampler_texNormal, input.texcoord).rgb;
        normal = normalize(normal * 2.0 - 1.0);
        surface.normal = mul(normal, matrixTBN);
    }
    else
    {
        surface.normal = input.worldNormal;
    }
    
    // t3: metallic
    surface.metallic = (textureConfig.texHasFlag & 0x08)
                        ? texMetallic.Sample(sampler_texMetallic, input.texcoord).x
                        : materialProperties.metallic;
    
    
    // t4: roughness
    surface.roughness = (textureConfig.texHasFlag & 0x10)
                        ? texRoughtness.Sample(sampler_texRoughtness, input.texcoord).x
                        : materialProperties.roughness;
    
    // illumination
    float3 oDiffuseSpecular = float3(0.0f, 0.0f, 0.0f); // diffuse & specular
    
    if (sceneProperties.lights.directional.enabled != 0)
    {
        const float3 wi = normalize(-sceneProperties.lights.directional.direction);
        const float3 radiance = sceneProperties.lights.directional.color * sceneProperties.lights.directional.intensity;
    
        oDiffuseSpecular += BRDF(surface, wi, -viewDir) * radiance * saturate(dot(surface.normal, wi));
    }
    
    
    //const float3 environmentIrradience = float3(0.1f, 0.1f, 0.1f);
    //const float3 environmentSpecular = float3(1.0f, 1.0f, 1.0f);
    // environment lighting
    //float3 oEnv = EnvironmentBRDF(surface, -viewDir, environmentIrradience, environmentSpecular);
    
    //float3 illumination = oDiffuseSpecular + oEnv;
    float3 illumination = oDiffuseSpecular;
    
    // t5: ambient_occlusion
    if (textureConfig.texHasFlag & 0x20)
    {
        illumination *= texAmbientOcclusion.Sample(sampler_texAmbientOcclusion, input.texcoord).r;
    }
    
    return float4(illumination, 1.0f);
}