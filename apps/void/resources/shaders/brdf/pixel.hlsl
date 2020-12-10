
struct PSIn
{
	float4 position : SV_POSITION;
	float3 worldPos : POSTION;
	//float3 normal : NORMAL;
	//float3 tangent : TANGENT;
	//float2 texCoord : TEXCOORD4;
};

float4 PSMain(PSIn In) : SV_Target
{
	
	const float3 illumination = float3(0.5f, 0.5f, 0.5f);
	return float4(illumination, 1.0f);

}