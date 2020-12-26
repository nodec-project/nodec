
struct ModelProperties
{
    matrix matrixMVP;
    matrix matrixM;
    matrix matrixMInverse;
};

cbuffer cbModelProperties : register(b0)
{
    ModelProperties modelProperties;
}


struct VSIn
{
	float3 position : POSITION;
    float3 normal : NORMAL;
	//float3 tangent : TANGENT0;
    float2 texcoord : TEXCOORD0;
};


struct V2P
{
	float4 position : SV_Position;
    float3 worldPos : POSITION;
    float3 worldNormal : NORMAL;
    float2 texcoord : TEXCOORD0;
	//float3 tangent : TANGENT;
	//float2 texCoord : TEXCOORD4;
};

float3 ModelToWorldNormal(float3 normal)
{
  //  return normalize(
		//normal.x * modelProperties.matrixMInverse[0].xyz
		//+ normal.y * modelProperties.matrixMInverse[1].xyz 
		//+ normal.z * modelProperties.matrixMInverse[2].xyz);
    return normalize(
		modelProperties.matrixMInverse[0].xyz * normal.x
		+ modelProperties.matrixMInverse[1].xyz * normal.y 
		+ modelProperties.matrixMInverse[2].xyz * normal.z);
}

V2P VSMain(VSIn input)
{
	V2P output;
	
	const float4 pos = float4(input.position, 1);
    output.position = mul(modelProperties.matrixMVP, pos);
    output.worldPos = mul(modelProperties.matrixM, pos).xyz;
    //output.position = mul(pos, modelProperties.matrixMVP);
    //output.worldPos = mul(pos, modelProperties.matrixM).xyz;
	
    output.worldNormal = ModelToWorldNormal(input.normal);
    //output.worldNormal = float3(1, 0, 0);
    //output.worldNormal = input.normal;
    
    output.texcoord = input.texcoord;
	
	return output;

}