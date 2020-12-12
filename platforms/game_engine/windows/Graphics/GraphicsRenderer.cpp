#include "GraphicsRenderer.hpp"

#include <nodec_modules/rendering/interfaces/mesh.hpp>
#include <nodec/scene_set/scene_object.hpp>

using namespace nodec;

void GraphicsRenderer::Render(Graphics* graphics, GraphicsResources* resources)
{
    for (auto iter = renderers.begin(); iter != renderers.end();)
    {
        if (auto renderer = iter->second.lock())
        {
            Vector3f position, scale;
            Quaternionf rotation;
            renderer->scene_object().transform().get_world_transform(position, rotation, scale);

            if (auto mesh = renderer->mesh)
            {
                BindMesh(mesh.get(), graphics, resources);


            }

            logging::debug("asas", __FILE__, __LINE__);
            ++iter;
        }
        else
        {
            iter = renderers.erase(iter);
        }
    }
}

void GraphicsRenderer::BindMesh(
    nodec_modules::rendering::interfaces::Mesh* mesh, Graphics* graphics, GraphicsResources* resources)
{
    {
        auto iter = resources->vertexBufferMap.find(mesh->id());
        if (iter != resources->vertexBufferMap.end())
        {
            iter->second->Bind(graphics);
        }
    }
    {
        auto iter = resources->indexBufferMap.find(mesh->id());
        if (iter != resources->indexBufferMap.end())
        {
            iter->second->Bind(graphics);
        }
    }
    
}