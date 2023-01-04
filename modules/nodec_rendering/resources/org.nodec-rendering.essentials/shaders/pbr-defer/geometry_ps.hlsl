/**
* geometry
 */
#include "geometry_interface.hlsl"

 struct PSOut {
    float4 albedo : SV_TARGET0;
    float4 normal : SV_TARGET1;

    //! r: roughness; g: metallic; a: mask
    float4 matProps : SV_TARGET2;

    float4 depth : SV_TARGET3;
 };

 PSOut PSMain(V2P input) {
    PSOut output;

    float3 worldNormal = normalize(input.worldNormal);
    float3 worldTangent = normalize(input.worldTangent);

    // https://learnopengl.com/Advanced-Lighting/Normal-Mapping
    // re-orthogonalize T with respect to N
    worldTangent = normalize(worldTangent - dot(worldNormal, worldTangent) * worldNormal);
    float3 worldBitangent = cross(worldNormal, worldTangent);
    
    // TBN matrix maps the position vector in TBN space into world space.
    // NOTE: float3x3 matrix initialization is row major!
    //   float3x3(row1, row2, row3)
    //   * https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-per-component-math
    float3x3 matrixTBN = float3x3(worldTangent, worldBitangent, worldNormal);
    
    // NOTE: Depth value is not linear!
    //  http://marupeke296.com/DXG_No71_depthbuffer.html
    float depth = input.depth.z / input.depth.w;
    output.depth = float4(depth.xxxx);

    // t0: albedo
    float3 outAlbedo = (textureConfig.texHasFlag & 0x01) 
                        ? texAlbedo.Sample(sampler_texAlbedo, input.texcoord).xyz 
                        : materialProperties.albedo.xyz;
    
    float3 outNormal = worldNormal;

    // t1: normal
    if (textureConfig.texHasFlag & 0x02) {
        float3 normal = texNormal.Sample(sampler_texNormal, input.texcoord).rgb;

        // obtain normal from normal map in range [0,1].
        normal = normalize(normal * 2.0 - 1.0);
        outNormal = mul(normal, matrixTBN);
    }

    // t3: metallic
    float outMetallic = (textureConfig.texHasFlag & 0x08)
                        ? texMetallic.Sample(sampler_texMetallic, input.texcoord).x
                        : materialProperties.metallic;

    // t4: roughness
    float outRoughness = (textureConfig.texHasFlag & 0x10)
                        ? texRoughtness.Sample(sampler_texRoughtness, input.texcoord).x
                        : materialProperties.roughness;

    output.albedo = float4(outAlbedo, 1);
    output.normal = float4(outNormal, 1);
    output.matProps = float4(outRoughness, outMetallic, 0, 1);

    return output;
 }