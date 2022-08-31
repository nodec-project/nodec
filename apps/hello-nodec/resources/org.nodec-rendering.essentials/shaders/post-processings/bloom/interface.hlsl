#include "../../common/interface_scene.hlsl"

// --- Material properties ---
struct MaterialProperties {
    float threshold;
};

cbuffer cbMaterialProperties : register(b3) {
    MaterialProperties materialProperties;
};