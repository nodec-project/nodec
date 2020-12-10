struct ObjectMatrices
{
	matrix modelViewProj;
	matrix model;
	//matrix normal;
};


cbuffer perModel
{
	ObjectMatrices ObjMatricies;
}


struct VSIn
{
	float3 position : POSITION;
	//float3 normal : NORMAL;
	//float3 tangent : TANGENT0;
	//float2 texCoord : TEXCOORD0;
};


struct PSIn
{
	float4 position : SV_Position;
	float3 worldPos : POSITION;
	//float3 normal : NORMAL;
	//float3 tangent : TANGENT;
	//float2 texCoord : TEXCOORD4;
};

PSIn VSMain(VSIn In)
{
	const float4 pos = float4(In.position, 1);
	
	PSIn Out;
	Out.position = mul(ObjMatricies.modelViewProj, pos);
	Out.worldPos = mul(ObjMatricies.model, pos).xyz;
	//Out.normal = normalize(mul(ObjMatricies.normal, In.normal));
	//Out.tangent = normalize(mul(ObjMatricies.normal, In.tangent));
	//Out.texCoord = In.texCoord;

	
	return Out;

}