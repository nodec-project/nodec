
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

struct VSIn
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD0;
    float3 tangent : TANGENT0;
};
