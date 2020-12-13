
struct V2P
{
    float4 position : SV_Position;
	//float3 worldPos : POSITION;
	//float3 normal : NORMAL;
	//float3 tangent : TANGENT;
	//float2 texCoord : TEXCOORD4;
};

float4 PSMain(V2P input) : SV_Target
{
	
	const float3 illumination = float3(0.5f, 0.5f, 0.5f);
	return float4(illumination, 1.0f);

}