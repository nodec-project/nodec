/**
* @require Shader Model 4 (/4_0)
*/

#include "interface.hlsl"


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
    
    output.worldTangent = normalize(mul(modelProperties.matrixM, float4(input.tangent, 0)).xyz);
	
    output.texcoord = input.texcoord;
	
	return output;

}