
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
    float4x4 matrixPInverse;
    float4x4 matrixVInverse;
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


float3 ViewSpacePosition(in const float nonLinearDepth, const in float2 uv, const in matrix invProjection)
{
	// src: 
	// https://mynameismjp.wordpress.com/2009/03/10/reconstructing-position-from-depth/
	// http://www.derschmale.com/2014/01/26/reconstructing-positions-from-the-depth-buffer/

	const float x = uv.x * 2 - 1;			// [-1, 1]
    const float y = (1.0f - uv.y) * 2 - 1;	// [-1, 1]
	const float z = nonLinearDepth;			// [ 0, 1]

	float4 projectedPosition = float4(x, y, z, 1);
	float4 viewSpacePosition = mul(invProjection, projectedPosition);
	return viewSpacePosition.xyz / viewSpacePosition.w;
}