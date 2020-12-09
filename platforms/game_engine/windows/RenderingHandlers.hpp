#pragma once

#include "Graphics.hpp"
#include "GraphicsResources.hpp"

#include <nodec_modules/rendering/rendering_module.hpp>


class RenderingHandlers : public std::enable_shared_from_this<RenderingHandlers>
{
public:
    RenderingHandlers(Graphics* pGraphics, GraphicsResources* pGraphicsResources);

    void Init(nodec_modules::rendering::RenderingModule& rendering_module);
    void HandleMeshPublishing(nodec_modules::rendering::interfaces::Mesh& mesh);

private:
    Graphics* pGraphics;
    GraphicsResources* pGraphicsResources;

private:
    NODEC_DISABLE_COPY(RenderingHandlers);
};

