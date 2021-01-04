
// === Parallax ===

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

// end Parallax ===

// === BRDF ===

inline half D_GGX(half perceptualRoughness, half ndoth, half3 normal, half3 halfDir)
{
    half3 ncrossh = cross(normal, halfDir);
    half a = ndoth * perceptualRoughness;
    half k = perceptualRoughness / (dot(ncrossh, ncrossh) + a * a);
    half d = k * k / PI;
    return min(d, 65504.0);
}
inline half3 F_Schlick(half3 f0, half cos)
{
    return f0 + (1 - f0) * pow(1 - cos, 5);
}

inline half Fd_Burley(half ndotv, half ndotl, half ldoth, half roughness)
{
    half fd90 = 0.5 + 2 * ldoth * ldoth * roughness;
    half lightScatter = (1 + (fd90 - 1) * pow(1 - ndotl, 5));
    half viewScatter = (1 + (fd90 - 1) * pow(1 - ndotv, 5));

    half diffuse = lightScatter * viewScatter / PI;
    return diffuse;
}

// <https://light11.hatenadiary.com/entry/2020/03/05/220957>
inline float V_SmithGGXCorrelated(float ndotl, float ndotv, float alpha)
{
    float lambdaV = ndotl * (ndotv * (1 - alpha) + alpha);
    float lambdaL = ndotv * (ndotl * (1 - alpha) + alpha);
    return 0.5f / (lambdaV + lambdaL + 0.0001);
}

float DisneyDiffuse(float albedo, float3 N, float3 L, float3 V, float roughness)
{
    float3 H = normalize(L + V);
    float dotLH = saturate(dot(L, H));
    float dotNL = saturate(dot(N, L));
    float dotNV = saturate(dot(N, V));
    float Fd90 = 0.5 + 2.0 * dotLH * dotLH * roughness;
    float FL = SchlickFresnel(1.0, Fd90, dotNL);
    float FV = SchlickFresnel(1.0, Fd90, dotNV);
    return (albedo * FL * FV) / PI;
}

// <https://qiita.com/mebiusbox2/items/1cd65993ffb546822213#brdf-%E3%81%AE%E6%80%A7%E8%B3%AA>
float SchlickFresnel(float u, float f0, float f90)
{
    return f0 + (f90 - f0) * pow(1.0 - u, 5.0);
}

// @note
// <https://light11.hatenadiary.com/entry/2020/03/05/220957>
float3 BRDF_2(BRDFSurface surface, float3 lightDir, float3 viewDir, float3 lightColor, float3 irradience, float3 envSpecular)
{
    const float3 normal = surface.normal;
    const float3 albedo = surface.albedo;
    const float metallic = surface.metallic;
    const float roughness = surface.roughness;
    
    float3 halfDir = normalize(lightDir + viewDir);
    half ndotv = abs(dot(normal, viewDir));
    float ndotl = max(0, dot(normal, lightDir));
    float ndoth = max(0, dot(normal, halfDir));
    half ldoth = max(0, dot(lightDir, halfDir));
    half reflectivity = lerp(0.04f.xxx, 1, metallic);
    half3 f0 = lerp(0.04f.xxx, albedo, metallic);
    
    // Diffuse
    half diffuseTerm = Fd_Burley(ndotv, ndotl, ldoth, roughness) * ndotl;
    half3 diffuse = albedo * (1 - reflectivity) * diffuseTerm * lightColor;
    
    // Indirect Diffuse
    diffuse += albedo * (1 - reflectivity) * irradience;
    
    // Specular
    float alpha = roughness * roughness;
    float V = V_SmithGGXCorrelated(ndotl, ndotv, alpha);
    float D = D_GGX(roughness, ndotv, normal, halfDir);
    float3 F = F_Schlick(f0, ldoth);
    float3 specular = V * D * F * ndotl * lightColor;

    specular = max(0, specular);
    
    // Indirect Specular
    half surfaceReduction = 1.0 / (alpha * alpha + 1.0);
    half f90 = saturate((1 - roughness) + reflectivity);
    specular += surfaceReduction * envSpecular * lerp(f0, f90, pow(1 - ndotv, 5));
                

    half3 color = diffuse + specular;
    return half4(color, 1);
}

// end BRDF === 