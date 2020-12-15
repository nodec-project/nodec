#include "brdf.hlsl"

struct MaterialProperties
{
    float3 diffuse;
    float roughness;
    float metalness;
    float3 specular;
};

cbuffer cbMaterialProperties
{
    MaterialProperties materialProperties;
};


struct V2P
{
    float4 position : SV_Position;
    float3 worldPos : POSITION;
    float3 worldNormal : NORMAL;
	//float3 tangent : TANGENT;
	//float2 texCoord : TEXCOORD4;
};

float4 PSMain(V2P input) : SV_Target
{
    float3 cameraPos = float3(0.0f, 0.0f, 0.0f);
    
    
    const float3 worldPos = input.worldPos;
    const float3 worldNormal = input.worldNormal;
    const float3 viewDir = normalize(cameraPos - worldPos); // from obj to camera
    
    BRDFSurface surface;
    
    surface.normal = input.worldNormal;
    surface.diffuseColor = float3(1.0f, 0.0f, 0.0f);
    surface.specularColor = float3(1.0f, 1.0f, 1.0f);
    
    surface.metalness = 0.5;
    surface.roughness = 0.2;
    
    //const float ambient
    
    // illumination
    const float3 iAmbient = surface.diffuseColor * 0.0f; // ambient
    float3 iDiffuseSpecular = float3(0.0f, 0.0f, 0.0f); // diffuse & specular
    float3 iEnv = 0.0f.xxx; // environment lighting
    
    
    const float3 lightDirection = normalize(float3(0.0f, 0.0f, 1.0f));
    const float3 lightColor = float3(1.0f, 1.0f, 1.0f);
    
    const float3 lightInSolidAngle = -lightDirection;
    
    iDiffuseSpecular += BRDF(surface, lightInSolidAngle, viewDir) * lightColor;
    
    const float3 illumination = iAmbient + iDiffuseSpecular + iEnv;
	return float4(illumination, 1.0f);
}