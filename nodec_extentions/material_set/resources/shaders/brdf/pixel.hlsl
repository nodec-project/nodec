/**
* @require Shader Model 4 (/4_0)
*/

#include "interface.hlsl"

#include "brdf.hlsl"

// <https://learnopengl.com/Advanced-Lighting/Parallax-Mapping>
// @note viewing at very near plane, it looks bad.
float2 ParallaxMapping(float2 texcoord, float3 viewDirInTangentSpace)
{
    const float heightScale = 0.05f;
    float height = texHeight.Sample(sampler_texHeight, texcoord).r;
    float2 p = viewDirInTangentSpace.xy / viewDirInTangentSpace.z * (height * heightScale);
    return texcoord - p;
}

float2 ParallaxUVs_Occl(float2 uv, float3 ViewVectorInTangentSpace, Texture2D HeightMap, SamplerState Sampler)
{
	// second iteration: Parallax Occlusion Mapping
    const float PARALLAX_HEIGHT_INTENSITY = 0.05;
    const int INVERSE_HEIGHT_MAP = 1;

    const float minLayers = 8;
    const float maxLayers = 32;
    const float numLayers = lerp(maxLayers, minLayers, dot(float3(0, 0, 1), ViewVectorInTangentSpace));

    float layerDepthIncrement = 1.0f / numLayers;
    float currLayerDepth = 0.0f;

	// the amount to shift the texture coordinates per layer (from vector P)
    //float2 P = normalize(ViewVectorInTangentSpace.xy) * heightIntensity;
    float2 P = ViewVectorInTangentSpace.xy / ViewVectorInTangentSpace.z * PARALLAX_HEIGHT_INTENSITY;
    float2 uv_delta = P / numLayers;

    float2 currUVs = uv;
    float currHeightMapValue = INVERSE_HEIGHT_MAP > 0
		? HeightMap.Sample(Sampler, uv).r
		: (1.0f - HeightMap.Sample(Sampler, uv).r);

	[loop]
    while (currLayerDepth < currHeightMapValue)
    {
        currUVs -= uv_delta; // shift uv's along the direction of P
        currHeightMapValue = INVERSE_HEIGHT_MAP > 0
			? HeightMap.Sample(Sampler, uv).r
			: (1.0f - HeightMap.Sample(Sampler, uv).r);
        currLayerDepth += layerDepthIncrement;
    }

	// get texture coordinates before collision (reverse operations)
    float2 prevUVs = currUVs + uv_delta;

	// get depth after and before collision for linear interpolation
    float afterDepth = currHeightMapValue - currLayerDepth;
    float beforeDepth = (INVERSE_HEIGHT_MAP > 0
		? HeightMap.Sample(Sampler, uv).r
		: (1.0f - HeightMap.Sample(Sampler, uv).r)) - currLayerDepth + layerDepthIncrement;
 
	// interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    float2 finalTexCoords = prevUVs * weight + currUVs * (1.0 - weight);

    return finalTexCoords;
}

float2 ApplyParallaxOffset(float2 texcoord, float3 vdir, float2 scale, Texture2D heightMap, SamplerState samplerHeightMap)
{
    float2 pdir = vdir.xy * scale;
    for (int i = 0; i < 4; i++)
    {
        float parallax = heightMap.Sample(samplerHeightMap, texcoord).x;
        texcoord += pdir * parallax;
    }
    return texcoord;

}
float2 ParallaxUVs_Occl2(float2 uv, float3 ViewVectorInTangentSpace, Texture2D HeightMap, SamplerState Sampler, float NdotV)
{
	//src: https://www.gamedev.net/articles/programming/graphics/a-closer-look-at-parallax-occlusion-mapping-r3262/

    const float fHeigtMapScale = 0.1f;
    const float nMaxSamples = 32.0f;
    const float nMinSamples = 8.0f;

    float fParallaxLimit = -length(ViewVectorInTangentSpace.xy) / ViewVectorInTangentSpace.z;
    fParallaxLimit *= fHeigtMapScale;

    float2 vOffsetDir = normalize(ViewVectorInTangentSpace.xy);
    float2 vMaxOffset = vOffsetDir * fParallaxLimit;

    int nNumSamples = (int) lerp(nMaxSamples, nMinSamples, NdotV);

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
        fCurrSampledHeight = HeightMap.SampleGrad(Sampler, uv + vCurrOffset, dx, dy).r;
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
    float3 cameraPos = float3(0.0f, 0.0f, 0.0f);
    
    
    const float3 viewDir = normalize(cameraPos - input.worldPos); // from obj to camera
    
    BRDFSurface surface;
    
    // re-orthogonalize T with respect to N
    float3 worldTangent = normalize(input.worldTangent - dot(input.worldTangent, input.worldNormal) * input.worldNormal);
    //float3 worldTangent = input.worldTangent;
    // then retrieve perpendicular vector B with the cross product of T and N
    //float3 worldBitangent = cross(input.worldNormal, worldTangent);
    float3 worldBitangent = cross(input.worldNormal, worldTangent); // !! MULTIPLY ORDAR IMPORTANT !!
    
    float3x3 matrixTBN = float3x3(worldTangent, worldBitangent, input.worldNormal); // TBN (row-major representation)
    //float3x3 matrixTBN = float3x3(worldBitangent, worldTangent, input.worldNormal); // TBN (row-major representation)
    //matrix matrixTBN = matrix(float4(worldTangent, 1), float4(worldBitangent, 1), float4(input.worldNormal, 1), float4(0, 0, 0, 1));
    
    
    // t2: height map
    if (textureConfig.texHasFlag & 0x02)
    {
        //float3 viewDirInTangentSpace = mul(float4(viewDir, 1), matrixTBN).xyz;
        //float3 viewDirInTangentSpace = float3(viewDir.x * worldTangent.x + viewDir.y * worldBitangent.x + viewDir.z * input.worldNormal.x,
        //                                      viewDir.x * worldTangent.y + viewDir.y * worldBitangent.y + viewDir.z * input.worldNormal.y,
        //                                      viewDir.x * worldTangent.z + viewDir.y * worldBitangent.z + viewDir.z * input.worldNormal.z);
        //float3 viewDirInTangentSpace = mul(-viewDir, transpose(matrixTBN));
        float3 viewDirInTangentSpace = mul(matrixTBN, -viewDir);
        //float3 normalInTangentSpace = mul(matrixTBN, input.worldNormal);
        //float3 viewDirInTangentSpace = mul(viewDir, matrixTBN);
        //input.texcoord = ParallaxMapping(input.texcoord, -viewDirInTangentSpace);
        input.texcoord = ParallaxUVs_Occl2(input.texcoord, viewDirInTangentSpace, texHeight, sampler_texHeight, dot(-viewDir, input.worldNormal));
        //input.texcoord = ParallaxUVs_Occl(input.texcoord, viewDirInTangentSpace, texHeight, sampler_texHeight);
        //input.texcoord = ApplyParallaxOffset(input.texcoord, viewDirInTangentSpace, 0.01, texHeight, sampler_texHeight);
        if (input.texcoord.x > 1.0 || input.texcoord.y > 1.0 || input.texcoord.x < 0.0 || input.texcoord.y < 0.0)
        {
            discard;
        }
    }
    
    // t1
    surface.albedo = (textureConfig.texHasFlag & 0x01) 
                        ? texAlbedo.Sample(sampler_texAlbedo, input.texcoord).xyz 
                        : materialProperties.albedo.xyz;

    // t3
    surface.metallic = (textureConfig.texHasFlag & 0x04)
                        ? texMetallic.Sample(sampler_texMetallic, input.texcoord).x
                        : materialProperties.metallic;
    
    // t4
    if (textureConfig.texHasFlag & 0x08)
    {
        float3 normal = texNormal.Sample(sampler_texNormal, input.texcoord).rgb;
        normal = normalize(normal * 2.0 - 1.0);
        //surface.normal = mul(float4(normal, 1), matrixTBN).xyz;
        surface.normal = mul(normal, matrixTBN);
    }
    else
    {
        surface.normal = input.worldNormal;
    }
    
    // t5
    surface.roughness = (textureConfig.texHasFlag & 0x10)
                        ? texRoughtness.Sample(sampler_texRoughtness, input.texcoord).x
                        : materialProperties.roughness;
    
    //const float ambient
    
    // illumination
    //const float3 iAmbient = surface.diffuseColor * 0.0f; // ambient
    float3 iDiffuseSpecular = float3(0.0f, 0.0f, 0.0f); // diffuse & specular
    float3 iEnv = 0.0f.xxx; // environment lighting
    
    
    const float3 lightDirection = normalize(float3(0.0f, 0.0f, 1.0f));
    const float3 lightColor = float3(1.0f, 1.0f, 1.0f);
    
    const float3 lightInSolidAngle = -lightDirection;
    
    const float3 environmentIrradience = float3(0.1f, 0.1f, 0.1f);
    const float3 environmentSpecular = float3(1.0f, 1.0f, 1.0f);
    
    //float3 indirectDiffuse = half3(0.1f, 0.1f, 0.1f);
    //float3 indirectSpecular = half3(1.0f, 1.0f, 1.0f);
    
    iDiffuseSpecular += BRDF(surface, lightInSolidAngle, viewDir) * lightColor;
    //iDiffuseSpecular += BRDF_2(surface, lightInSolidAngle, viewDir, lightColor, environmentIrradience, environmentSpecular);
    
    iEnv = EnvironmentBRDF(surface, viewDir, environmentIrradience, environmentSpecular);
    
    //const float3 illumination = iAmbient + iDiffuseSpecular + iEnv;
    const float3 illumination = iDiffuseSpecular + iEnv;
    //const float3 illumination = iDiffuseSpecular;
	return float4(illumination, 1.0f);
}