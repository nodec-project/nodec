#include "RenderingHandlers.hpp"
#include "RenderingUtils.hpp"

RenderingHandlers::RenderingHandlers(Graphics* pGraphics, GraphicsResources* pGraphicsResources):
    pGraphics(pGraphics),
    pGraphicsResources(pGraphicsResources)
{
    
}

void RenderingHandlers::Init(
    nodec_modules::rendering::RenderingModule& rendering_module
)
{
    auto thisShared = shared_from_this();
    auto meshPublishingHandler
        = nodec::event::MemberCallback<RenderingHandlers, nodec_modules::rendering::interfaces::Mesh*>::make_shared(
            thisShared, &RenderingHandlers::HandleMeshPublishing);

    rendering_module.on_publish_mesh += meshPublishingHandler;
}

void RenderingHandlers::HandleMeshPublishing(nodec_modules::rendering::interfaces::Mesh* mesh)
{

    PublishMesh(mesh, pGraphics, pGraphicsResources);

}