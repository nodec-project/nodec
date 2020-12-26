#include "interface.hlsl"

#include "brdf.hlsl"


float4 PSMain(V2P input) : SV_Target
{
    float3 cameraPos = float3(0.0f, 0.0f, 0.0f);
    
    
    const float3 worldPos = input.worldPos;
    const float3 worldNormal = input.worldNormal;
    const float3 viewDir = normalize(cameraPos - worldPos); // from obj to camera
    
    BRDFSurface surface;
    
    surface.normal = input.worldNormal;
    
    surface.albedo = materialProperties.albedo.xyz;
    surface.albedo = texAlbedo.Sample(sampler_texAlbedo, input.texcoord);
    surface.metallic = materialProperties.metallic;
    surface.roughness = materialProperties.roughness;
    
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