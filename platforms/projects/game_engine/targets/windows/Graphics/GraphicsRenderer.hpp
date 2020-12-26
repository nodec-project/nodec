#pragma once

#include "Graphics.hpp"
#include "GraphicsResources.hpp"
#include "ConstantBuffer.hpp"
#include "Sampler.hpp"

#include <nodec_modules/rendering/interfaces/renderer.hpp>

#include <DirectXMath.h>
#include <unordered_map>

class GraphicsRenderer
{
public:
    struct ModelProperties
    {
        DirectX::XMFLOAT4X4 matrixMVP;
        DirectX::XMFLOAT4X4 matrixM;
        DirectX::XMFLOAT4X4 matrixMInverse;
    };

public:
    GraphicsRenderer(Graphics* graphics);
    void Render(Graphics* graphics, GraphicsResources* resources);

public:
    std::unordered_map<
        nodec::NodecObject::ID,
        nodec::NodecObject::Reference<nodec_modules::rendering::interfaces::Renderer>>
        renderers;

private:
    void BindMesh(const nodec_modules::rendering::interfaces::Mesh* mesh, 
                  Graphics* graphics, GraphicsResources* resources);

    void BindShader(const nodec_modules::rendering::interfaces::Shader* shader,
                    Graphics* graphics, GraphicsResources* resources);

    

private:
    ModelProperties modelProperties;
    ConstantBuffer cbModelProperties;
    Sampler samplerAnisotropic;
    Sampler samplerPoint;
    Sampler samplerBilinear;

private:
    NODEC_DISABLE_COPY(GraphicsRenderer);

};