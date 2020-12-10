#include "RenderingUtils.hpp"



namespace RenderingUtils
{

    void InitRenderingHandlers(std::shared_ptr<RenderingHandlers> handlers, nodec_modules::rendering::RenderingModule& rendering_module)
    {
        auto meshPublishingHandler
            = nodec::event::MemberCallback<RenderingHandlers, nodec_modules::rendering::interfaces::Mesh*>::make_shared(
                handlers, &RenderingHandlers::HandleMeshBinding);

        rendering_module.on_bind_mesh += meshPublishingHandler;
    }

    void BindMesh(nodec_modules::rendering::interfaces::Mesh* mesh, Graphics* graphics, GraphicsResources* graphicsResources)
    {
        if (graphicsResources->vertexBufferMap.find(mesh->id()) != graphicsResources->vertexBufferMap.end())
        {
            // Already exists.
            return;
        }

        if (mesh->vertices.size() > 0)
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
                graphics,
                mesh->vertices.size() * sizeof(nodec::Vector3f),
                sizeof(nodec::Vector3f),
                mesh->vertices.data()
                );
        }
    }
}