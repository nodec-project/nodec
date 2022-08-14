#include "interface.hlsl"

V2P VSMain(VSIn input) {
    V2P output;

    const float4 pos = float4(input.position, 1);
    output.position = mul(modelProperties.matrixMVP, pos);
    
    output.texcoord = input.texcoord;

    return output;
}