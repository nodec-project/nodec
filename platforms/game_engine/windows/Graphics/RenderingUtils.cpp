#include "RenderingUtils.hpp"

using namespace nodec_modules;
using namespace nodec;

namespace RenderingUtils
{

void InitRenderingHandlers(
    std::shared_ptr<RenderingHandlers> handlers,
    rendering::RenderingModule& rendering_module)
{
    auto meshPublishingHandler
        = nodec::event::MemberCallback<RenderingHandlers, rendering::interfaces::Mesh*>::make_shared(
            handlers, &RenderingHandlers::HandleMeshBinding
        );

    rendering_module.on_bind_mesh += meshPublishingHandler;

    auto rendererRegistingHandler
        = nodec::event::MemberCallback<RenderingHandlers, NodecObject::Reference<rendering::interfaces::Renderer>>::make_shared(
            handlers, &RenderingHandlers::HandleRendererRegisting
        );
    rendering_module.on_regist_renderer += rendererRegistingHandler;
}

void BindMesh(nodec_modules::rendering::interfaces::Mesh* mesh, Graphics* graphics, GraphicsResources* graphicsResources)
{


    if (mesh->vertices.size() > 0
        && graphicsResources->vertexBufferMap.find(mesh->id()) == graphicsResources->vertexBufferMap.end())
    {
        auto vertexBuffer = std::make_shared<VertexBuffer>(
            graphics,
            mesh->vertices.size() * sizeof(nodec::Vector3f),
            sizeof(nodec::Vector3f),
            mesh->vertices.data()
            );
        graphicsResources->vertexBufferMap.emplace(mesh->id(), vertexBuffer);
    }

    if (mesh->triangles.size() > 0
        && graphicsResources->indexBufferMap.find(mesh->id()) == graphicsResources->indexBufferMap.end())
    {
        auto indexBuffer = std::make_shared<IndexBuffer>(
            graphics,
            mesh->triangles.size(),
            mesh->triangles.data()
            );
        graphicsResources->indexBufferMap.emplace(mesh->id(), indexBuffer);
    }
}
}