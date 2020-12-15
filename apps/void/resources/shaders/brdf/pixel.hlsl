struct MaterialProperties
{
    float3 diffuse;
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
	
    //const float3 illumination = float3(0.0f, 0.0f, 1.0f);
    const float3 illumination = input.worldNormal;
	return float4(illumination, 1.0f);
}