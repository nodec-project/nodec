
struct ModelProperties
{
    matrix matrixMVP;
    matrix matrixM;
};

cbuffer cbModelProperties : register(b0)
{
    ModelProperties modelProperties;
}


struct VSIn
{
	float3 position : POSITION;
	//float3 normal : NORMAL;
	//float3 tangent : TANGENT0;
	//float2 texCoord : TEXCOORD0;
};


struct V2P
{
	float4 position : SV_Position;
	//float3 worldPos : POSITION;
	//float3 normal : NORMAL;
	//float3 tangent : TANGENT;
	//float2 texCoord : TEXCOORD4;
};

V2P VSMain(VSIn input)
{
	const float4 pos = float4(input.position, 1);
	
	V2P output;
    output.position = mul(pos, modelProperties.matrixMVP);
    //output.position = pos;
	//Out.normal = normalize(mul(ObjMatricies.normal, In.normal));
	//Out.tangent = normalize(mul(ObjMatricies.normal, In.tangent));
	//Out.texCoord = In.texCoord;

	
	return output;

}