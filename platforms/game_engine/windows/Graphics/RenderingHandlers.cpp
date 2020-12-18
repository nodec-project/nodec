#include "RenderingHandlers.hpp"
#include "RenderingUtils.hpp"

using namespace nodec;
using namespace nodec_modules;

void RenderingHandlers::SetupHandlers(std::shared_ptr<RenderingHandlers> handlers,
                                      nodec_modules::rendering::RenderingModule& rendering_module)
{
    auto meshBindingHandler
        = event::MemberCallback<RenderingHandlers, const rendering::interfaces::Mesh*>::make_shared(
            handlers, &RenderingHandlers::HandleMeshBinding
        );
    rendering_module.on_bind_mesh += meshBindingHandler;

    auto meshUnbindingHandler
        = event::MemberCallback<RenderingHandlers, const rendering::interfaces::Mesh*>::make_shared(
            handlers, &RenderingHandlers::HandleMeshUnbinding
        );
    rendering_module.on_unbind_mesh += meshUnbindingHandler;

    auto shaderBindingHandler
        = event::MemberCallback<RenderingHandlers, const rendering::interfaces::Shader*>::make_shared(
            handlers, &RenderingHandlers::HandleShaderBinding
        );
    rendering_module.on_bind_shader += shaderBindingHandler;

    auto rendererRegistingHandler
        = event::MemberCallback<RenderingHandlers, NodecObject::Reference<rendering::interfaces::Renderer>>::make_shared(
            handlers, &RenderingHandlers::HandleRendererRegisting
        );
    rendering_module.on_regist_renderer += rendererRegistingHandler;
}

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

void RenderingHandlers::HandleMeshUnbinding(const nodec_modules::rendering::interfaces::Mesh* mesh)
{
    logging::debug("hjhj", __FILE__, __LINE__);
    RenderingUtils::UnbindMesh(mesh, graphicsResources);
}

void RenderingHandlers::HandleShaderBinding(const nodec_modules::rendering::interfaces::Shader* shader)
{

}

void RenderingHandlers::HandleRendererRegisting(
    nodec::NodecObject::Reference<nodec_modules::rendering::interfaces::Renderer> renderer)
{
    if (auto renderer_locked = renderer.lock())
    {
        graphicsRenderer->renderers.emplace(renderer_locked->id(), renderer);
    }
}