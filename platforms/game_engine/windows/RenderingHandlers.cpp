#include "RenderingHandlers.hpp"

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
        = nodec::event::MemberCallback<RenderingHandlers, nodec_modules::rendering::interfaces::Mesh&>::make_shared(
            thisShared, &RenderingHandlers::HandleMeshPublishing);

    rendering_module.on_publish_mesh += meshPublishingHandler;
}

void RenderingHandlers::HandleMeshPublishing(nodec_modules::rendering::interfaces::Mesh& mesh)
{

    if (pGraphicsResources->vertexBufferMap.find(mesh.id()) != pGraphicsResources->vertexBufferMap.end())
    {
        // Already exists.
        return;
    }

    if (mesh.vertices.size() > 0)
    {
        //auto vertexBuffer = std::shared_ptr<VertexBuffer>(
        //    new VertexBuffer(
        //        pGraphics,
        //        static_cast<UINT>(mesh.vertices.size() * sizeof(nodec::Vector3f)),
        //        static_cast<UINT>(sizeof(nodec::Vector3f)),
        //        mesh.vertices.data()
        //    )
        //    );
        auto vertexBuffer = std::make_shared<VertexBuffer>(
            pGraphics,
            mesh.vertices.size() * sizeof(nodec::Vector3f),
            sizeof(nodec::Vector3f),
            mesh.vertices.data()
            );
    }

}