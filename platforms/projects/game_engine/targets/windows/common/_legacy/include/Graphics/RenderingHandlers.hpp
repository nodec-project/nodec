#pragma once

#include "Graphics.hpp"
#include "GraphicsResources.hpp"
#include "GraphicsRenderer.hpp"

#include <nodec_modules/rendering/rendering_module.hpp>


class RenderingHandlers
{
public:
    static void SetupHandlers(std::shared_ptr<RenderingHandlers> handlers,
                              nodec_modules::rendering::RenderingModule& rendering_module);

public:
    RenderingHandlers(Graphics* graphics, GraphicsResources* graphicsResources, GraphicsRenderer* graphicsRenderer);

    void HandleMeshBinding(const nodec_modules::rendering::interfaces::Mesh* mesh);
    void HandleMeshUnbinding(const nodec_modules::rendering::interfaces::Mesh* mesh);

    void HandleShaderBinding(const nodec_modules::rendering::interfaces::Shader* shader);
    void HandleShaderUnbinding(const nodec_modules::rendering::interfaces::Shader* shader);

    void HandleMaterialBinding(const nodec_modules::rendering::interfaces::Material* material);
    void HandleMaterialUnbinding(const nodec_modules::rendering::interfaces::Material* material);

    void HandleTextureBinding(const nodec_modules::rendering::interfaces::Texture* texture);
    void HandleTextureUnbinding(const nodec_modules::rendering::interfaces::Texture* texture);

    void HandleRendererRegistering(nodec::NodecObject::Reference<nodec_modules::rendering::interfaces::Renderer> renderer);

    void HandleCameraRegistering(nodec::NodecObject::Reference<nodec_modules::rendering::interfaces::Camera> camera);

private:
    Graphics* graphics;
    GraphicsResources* graphicsResources;
    GraphicsRenderer* graphicsRenderer;


private:
    NODEC_DISABLE_COPY(RenderingHandlers)
};

