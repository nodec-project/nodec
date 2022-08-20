/**
* geometry
 */
#include "geometry_interface.hlsl"

 struct PSOut {
    float4 albedo : SV_TARGET0;
    float4 normal : SV_TARGET1;

    //! r: roughness, a: metallic
    float4 matProps : SV_TARGET2;
 };

 PSOut PSMain(V2P input) {
    PSOut output;

    output.normal = float4(input.worldNormal, 1);

    return output;
 }