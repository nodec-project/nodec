#include "GraphicsRenderer.hpp"

#include <nodec_modules/rendering/interfaces/mesh.hpp>
#include <nodec/scene_set/scene_object.hpp>

#include "VertexShader.hpp"
#include "PixelShader.hpp"

using namespace nodec;

GraphicsRenderer::GraphicsRenderer(Graphics* graphics) :
    modelConstantBuffer(graphics, sizeof(ModelConstants), &modelConstants)
{

}

void GraphicsRenderer::Render(Graphics* graphics, GraphicsResources* resources)
{

    graphics->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


    const D3D11_INPUT_ELEMENT_DESC ied[] ={
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    VertexShader vertexShader(graphics, "resources/shaders/brdf/vertex.cso", ied, 1);
    PixelShader pixelShader(graphics, "resources/shaders/brdf/pixel.cso");

    if (vertexShader.Fail())
    {
        //logging::debug("ffgh", __FILE__, __LINE__);

    }

    vertexShader.Bind(graphics);
    pixelShader.Bind(graphics);

    modelConstantBuffer.BindVS(graphics, 0);


    for (auto iter = renderers.begin(); iter != renderers.end();)
    {
        if (auto renderer = iter->second.lock())
        {
            auto mesh = renderer->mesh;

            if (!mesh)
            {
                ++iter;
                continue;
            }

            {
                Vector3f position, scale;
                Quaternionf rotation;
                renderer->scene_object().transform().get_world_transform(position, rotation, scale);

                auto object2world = DirectX::XMMatrixTransformation(DirectX::XMVECTOR{ 0.0f, 0.0f,0.0f },
                                                                    DirectX::XMVECTOR{ 0.0f, 0.0f, 0.0f, 1.0f },
                                                                    DirectX::XMVECTOR{ scale.x, scale.y, scale.z },
                                                                    DirectX::XMVECTOR{ 0.0f, 0.0f, 0.0f },
                                                                    DirectX::XMVECTOR{ rotation.x, rotation.y, rotation.z, rotation.w },
                                                                    DirectX::XMVECTOR{ position.x, position.y, position.z });

                DirectX::XMStoreFloat4x4(&(modelConstants.object2world), DirectX::XMMatrixTranspose(object2world));
                modelConstantBuffer.Update(graphics, &modelConstants);
            }

            BindMesh(mesh.get(), graphics, resources);


            graphics->DrawIndexed(mesh->triangles.size());


            ++iter;
        }
        else
        {
            // renderer might be deleted.
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