#include "RenderingUtils.hpp"

using namespace nodec_modules;
using namespace nodec;

namespace RenderingUtils
{

void BindMesh(const nodec_modules::rendering::interfaces::Mesh* mesh, Graphics* graphics, GraphicsResources* graphicsResources)
{

    if (mesh->vertices.size() > 0
        && graphicsResources->vertexBufferMap.find(mesh->id()) == graphicsResources->vertexBufferMap.end())
    {
        auto vertexBuffer = std::make_shared<VertexBuffer>(
            graphics,
            mesh->vertices.size() * sizeof(rendering::interfaces::Mesh::Vertex),
            sizeof(rendering::interfaces::Mesh::Vertex),
            mesh->vertices.data()
            );
        graphicsResources->vertexBufferMap.emplace(mesh->id(), vertexBuffer);
        //logging::DebugStream(__FILE__, __LINE__) << sizeof(rendering::interfaces::Mesh::Vertex);
        //logging::DebugStream(__FILE__, __LINE__) << sizeof(Vector3f);
        //logging::DebugStream(__FILE__, __LINE__) << sizeof(float);
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