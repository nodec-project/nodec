#pragma once

#include "Graphics.hpp"
#include "GraphicsResources.hpp"
#include "ConstantBuffer.hpp"
#include "Sampler.hpp"

//#include <nodec_modules/rendering/interfaces/rendering.hpp>

#include <DirectXMath.h>
#include <unordered_map>

class GraphicsRenderer
{
public:
    struct SceneProperties
    {
        nodec::Vector4f cameraPos;
    };

    struct ModelProperties
    {
        DirectX::XMFLOAT4X4 matrixMVP;
        DirectX::XMFLOAT4X4 matrixM;
        DirectX::XMFLOAT4X4 matrixMInverse;
    };

    struct TextureConfig
    {
        uint32_t texHasFlag;
        uint32_t padding[3];
    };

public:
    GraphicsRenderer(Graphics* graphics);
    void Render(Graphics* graphics, GraphicsResources* resources);

public:
    std::unordered_map<
        nodec::NodecObject::ID,
        nodec::NodecObject::Reference<nodec_modules::rendering::interfaces::Renderer>>
        renderers;

    nodec::NodecObject::Reference<nodec_modules::rendering::interfaces::Camera> currentCamera;

private:
    void BindMesh(const nodec_modules::rendering::interfaces::Mesh* mesh,
                  Graphics* graphics, GraphicsResources* resources);

    void BindShader(const nodec_modules::rendering::interfaces::Shader* shader,
                    Graphics* graphics, GraphicsResources* resources);

    void BindMaterialTextures(const nodec_modules::rendering::interfaces::Material* material,
                              Graphics* graphics, GraphicsResources* resources, uint32_t& texhasFlag);

private:
    SceneProperties sceneProperties;
    ConstantBuffer cbSceneProperties;

    ModelProperties modelProperties;
    ConstantBuffer cbModelProperties;

    TextureConfig textureConfig;
    ConstantBuffer cbTextureConfig;

    Sampler samplerAnisotropic;
    Sampler samplerPoint;
    Sampler samplerBilinear;

private:
    NODEC_DISABLE_COPY(GraphicsRenderer);

};