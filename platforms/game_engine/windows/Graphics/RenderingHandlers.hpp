#pragma once

#include "Graphics.hpp"
#include "GraphicsResources.hpp"
#include "GraphicsRenderer.hpp"

#include <nodec_modules/rendering/rendering_module.hpp>


class RenderingHandlers
{
public:
    RenderingHandlers(Graphics* graphics, GraphicsResources* graphicsResources, GraphicsRenderer* graphicsRenderer);

    void HandleMeshBinding(nodec_modules::rendering::interfaces::Mesh* mesh);
    void HandleRendererRegisting(
        nodec::NodecObject::Reference<nodec_modules::rendering::interfaces::Renderer> renderer
    );

private:
    Graphics* graphics;
    GraphicsResources* graphicsResources;
    GraphicsRenderer* graphicsRenderer;
    

private:
    NODEC_DISABLE_COPY(RenderingHandlers);
};

