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
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0                           , D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    VertexShader vertexShader(graphics, "resources/shaders/brdf/vertex.cso", ied, std::size(ied));
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

                //logging::DebugStream(__FILE__, __LINE__) << position << ", " << rotation << ", " << scale << std::flush;

                auto matrixM = DirectX::XMMatrixTransformation(DirectX::XMVECTOR{ 0.0f, 0.0f, 0.0f },
                                                               DirectX::XMVECTOR{ 0.0f, 0.0f, 0.0f, 1.0f },
                                                               DirectX::XMVECTOR{ scale.x, scale.y, scale.z },
                                                               DirectX::XMVECTOR{ 0.0f, 0.0f, 0.0f },
                                                               DirectX::XMVECTOR{ rotation.x, rotation.y, rotation.z, rotation.w },
                                                               DirectX::XMVECTOR{ position.x, position.y, position.z });

                auto matrixMInverse = DirectX::XMMatrixInverse(nullptr, matrixM);

                //auto matrixM = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

                //DirectX::XMVECTOR eye         = DirectX::XMVectorSet(0.0f, 0.0f, -2.0f, 0.0f);
                //DirectX::XMVECTOR focus       = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
                //DirectX::XMVECTOR up          = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
                //DirectX::XMMATRIX matrixV  = DirectX::XMMatrixLookAtLH(eye, focus, up);

                auto matrixP = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45.0f),
                                                                 1280.0f / 720.0f,
                                                                 0.1f,
                                                                 100.0f);

                auto matrixMVP =  matrixM * matrixP;
                //auto matrixMVP = matrixM * matrixV * matrixP;

                DirectX::XMStoreFloat4x4(&(modelConstants.matrixM), DirectX::XMMatrixTranspose(matrixM));
                DirectX::XMStoreFloat4x4(&(modelConstants.matrixMVP), DirectX::XMMatrixTranspose(matrixMVP));
                DirectX::XMStoreFloat4x4(&(modelConstants.matrixMInverse), DirectX::XMMatrixTranspose(matrixMInverse));

                //DirectX::XMStoreFloat4x4(&(modelConstants.matrixM), matrixM);
                //DirectX::XMStoreFloat4x4(&(modelConstants.matrixMVP), matrixMVP);
                //DirectX::XMStoreFloat4x4(&(modelConstants.matrixMInverse), matrixMInverse);

                modelConstantBuffer.Update(graphics, &modelConstants);
            }

            BindMesh(mesh.get(), graphics, resources);


            graphics->DrawIndexed(mesh->triangles.size());
            //logging::DebugStream(__FILE__, __LINE__) << mesh->triangles.size() << std::flush;

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