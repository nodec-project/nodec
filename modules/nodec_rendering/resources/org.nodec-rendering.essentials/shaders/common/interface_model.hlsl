// --- HLSL Shader Common Interface ---
// All shaders must be included this interface.
//
// This interface provides the follwing features...
//  * Common input layout
//  * Accessing scene properties (camera position, lighting information...)
//  * Accessing model properties (model matrix, mvp-matrix, ...)
//  * Flag to validate texture slot

#include "interface_scene.hlsl"

struct ModelProperties
{
    matrix matrixMVP;
    matrix matrixM;
    matrix matrixMInverse;
};

cbuffer cbModelProperties : register(b2)
{
    ModelProperties modelProperties;
}

// @note
// <https://forum.unity.com/threads/world-space-normal.58810/>
float3 ModelToWorldNormal(float3 normal)
{
    return normalize(
        modelProperties.matrixMInverse[0].xyz * normal.x
        + modelProperties.matrixMInverse[1].xyz * normal.y 
        + modelProperties.matrixMInverse[2].xyz * normal.z);
}
