#pragma once

#include "Graphics.hpp"
#include "GraphicsResources.hpp"

#include <nodec_modules/rendering/rendering_module.hpp>


class RenderingHandlers
{
public:
    RenderingHandlers(Graphics* pGraphics, GraphicsResources* pGraphicsResources);

    void HandleMeshBinding(nodec_modules::rendering::interfaces::Mesh* mesh);

private:
    Graphics* pGraphics;
    GraphicsResources* pGraphicsResources;

private:
    NODEC_DISABLE_COPY(RenderingHandlers);
};

