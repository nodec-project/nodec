#include "brightness_interface.hlsl"

V2P VSMain (VSIn input) {
    V2P output;
    output.position = float4(input.position, 1);
    output.texcoord = input.texcoord;
    return output;
}