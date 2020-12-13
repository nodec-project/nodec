#pragma once

#include "Graphics.hpp"
#include "GraphicsResources.hpp"
#include "RenderingHandlers.hpp"

#include <nodec_modules/rendering/rendering_module.hpp>
#include <nodec_modules/rendering/interfaces/mesh.hpp>

namespace RenderingUtils
{
void InitRenderingHandlers(
    std::shared_ptr<RenderingHandlers> handlers,
    nodec_modules::rendering::RenderingModule& rendering_module);

void BindMesh(
    const nodec_modules::rendering::interfaces::Mesh* mesh, 
    Graphics* graphics, 
    GraphicsResources* graphicsResources);

}
