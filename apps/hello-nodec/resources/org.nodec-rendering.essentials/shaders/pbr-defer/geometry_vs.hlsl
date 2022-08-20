/**
 * geometry
 */

#include "geometry_interface.hlsl"

V2P VSMain(VSIn input) {
    V2P output;
    const float4 pos = float4(input.position, 1);
    output.position = mul(modelProperties.matrixMVP, pos);
    output.worldPos = mul(modelProperties.matrixM, pos).xyz;

    output.worldNormal = ModelToWorldNormal(input.normal);

    output.worldTangent = normalize(mul(modelProperties.matrixM, float4(input.tangent, 0)).xyz);
    
    output.texcoord = input.texcoord;
    
    return output;
}