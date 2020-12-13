#include "RenderingHandlers.hpp"
#include "RenderingUtils.hpp"

RenderingHandlers::RenderingHandlers(
    Graphics* graphics, 
    GraphicsResources* graphicsResources,
    GraphicsRenderer* graphicsRenderer):
    graphics(graphics),
    graphicsResources(graphicsResources),
    graphicsRenderer(graphicsRenderer)
{
    
}


void RenderingHandlers::HandleMeshBinding(const nodec_modules::rendering::interfaces::Mesh* mesh)
{
    RenderingUtils::BindMesh(mesh, graphics, graphicsResources);
}

void RenderingHandlers::HandleRendererRegisting(
    nodec::NodecObject::Reference<nodec_modules::rendering::interfaces::Renderer> renderer)
{
    if (auto renderer_locked = renderer.lock())
    {
        graphicsRenderer->renderers.emplace(renderer_locked->id(), renderer);
    }
}