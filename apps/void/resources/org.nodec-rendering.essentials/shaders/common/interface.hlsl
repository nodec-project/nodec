// --- HLSL Shader Common Interface ---
// All shaders must be included this interface.
//
// This interface provides the follwing features...
//  * Common input layout
//  * Accessing scene properties (camera position, lighting information...)
//  * Accessing model properties (model matrix, mvp-matrix, ...)
//  * Flag to validate texture slot

struct DirectionalLight
{
    float3 direction;
    float intensity;
    float4 color;
    int enabled;
};

struct SceneLighting
{
    DirectionalLight directional;
};

struct SceneProperties
{
    float4 cameraPos;
    SceneLighting lights;
};

cbuffer cbSceneProperties : register(b0)
{
    SceneProperties sceneProperties;
};

struct ModelProperties
{
    matrix matrixMVP;
    matrix matrixM;
    matrix matrixMInverse;
};

cbuffer cbModelProperties : register(b1)
{
    ModelProperties modelProperties;
}

struct TextureConfig
{
    uint texHasFlag;
};

cbuffer cbTextureConfig : register(b2)
{
    TextureConfig textureConfig;
}

struct VSIn
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD0;
    float3 tangent : TANGENT0;
};
