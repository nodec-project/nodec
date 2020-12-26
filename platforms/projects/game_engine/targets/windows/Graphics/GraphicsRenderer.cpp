#include "GraphicsRenderer.hpp"
#include "RenderingUtils.hpp"
#include "VertexShader.hpp"
#include "PixelShader.hpp"

#include <nodec_modules/rendering/interfaces/mesh.hpp>
#include <nodec_modules/rendering/interfaces/shader.hpp>
#include <nodec_modules/rendering/interfaces/material.hpp>
#include <nodec/scene_set/scene_object.hpp>


using namespace nodec;

GraphicsRenderer::GraphicsRenderer(Graphics* graphics) :
    cbModelProperties(graphics, sizeof(ModelProperties), &modelProperties),
    samplerAnisotropic(graphics, Sampler::Type::Anisotropic),
    samplerBilinear(graphics, Sampler::Type::Bilinear),
    samplerPoint(graphics, Sampler::Type::Point)
{

}

void GraphicsRenderer::Render(Graphics* graphics, GraphicsResources* resources)
{

    graphics->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    cbModelProperties.BindVS(graphics, 0);
    cbModelProperties.BindPS(graphics, 0);


    for (auto iter = renderers.begin(); iter != renderers.end();)
    {
        if (auto renderer = iter->second.lock())
        {
            auto mesh = renderer->mesh;
            auto material = renderer->material;

            if (!mesh || !material)
            {
                ++iter;
                continue;
            }

            // Model ConstantBuffer
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

                auto aspect = static_cast<float>(graphics->GetWidth()) / static_cast<float>(graphics->GetHeight());
                auto matrixP = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45.0f),
                                                                 aspect,
                                                                 0.1f,
                                                                 100.0f);
                // DirectX Math using row-major representation
                // HLSL using column-major representation
                auto matrixMVP = matrixM * matrixP;
                //auto matrixMVP = matrixM * matrixV * matrixP;

                //DirectX::XMStoreFloat4x4(&(modelConstants.matrixM), DirectX::XMMatrixTranspose(matrixM));
                //DirectX::XMStoreFloat4x4(&(modelConstants.matrixMVP), DirectX::XMMatrixTranspose(matrixMVP));
                //DirectX::XMStoreFloat4x4(&(modelConstants.matrixMInverse), DirectX::XMMatrixTranspose(matrixMInverse));

                DirectX::XMStoreFloat4x4(&(modelProperties.matrixM), matrixM);
                DirectX::XMStoreFloat4x4(&(modelProperties.matrixMVP), matrixMVP);
                DirectX::XMStoreFloat4x4(&(modelProperties.matrixMInverse), matrixMInverse);

                cbModelProperties.Update(graphics, &modelProperties);
            } // Model ConstantBuffer

            // Material Properties Primitive
            {
                auto iter = resources->constantBufferMap.find(material->id());
                if (iter != resources->constantBufferMap.end())
                {
                    auto constantBuffer = iter->second;
                    constantBuffer->BindVS(graphics, 1);
                    constantBuffer->BindPS(graphics, 1);

                    std::vector<uint8_t> cbuffer;
                    RenderingUtils::CreateMaterialCBuffer(material.get(), cbuffer);
                    constantBuffer->Update(graphics, cbuffer.data());
                }
            }

            BindMaterialTextures(material.get(), graphics, resources);

            BindShader(&material->shader(), graphics, resources);
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

void GraphicsRenderer::BindMesh(const nodec_modules::rendering::interfaces::Mesh* mesh,
                                Graphics* graphics, GraphicsResources* resources)
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

void GraphicsRenderer::BindShader(const nodec_modules::rendering::interfaces::Shader* shader,
                                  Graphics* graphics, GraphicsResources* resources)
{
    {
        auto iter = resources->vertexShaderMap.find(shader->id());
        if (iter != resources->vertexShaderMap.end())
        {
            iter->second->Bind(graphics);
        }
    }
    {
        auto iter = resources->pixelShaderMap.find(shader->id());
        if (iter != resources->pixelShaderMap.end())
        {
            iter->second->Bind(graphics);
        }
    }
}

void GraphicsRenderer::BindMaterialTextures(const nodec_modules::rendering::interfaces::Material* material,
                                            Graphics* graphics, GraphicsResources* resources)
{
    UINT slot = 0;
    for (auto& pair : material->texture_properties())
    {
        if (!pair.second.texture)
        {
            slot++;
            continue;
        }
        auto iter = resources->textureMap.find(pair.second.texture->id());
        if (iter != resources->textureMap.end())
        {
            iter->second->BindVS(graphics, slot);
            iter->second->BindPS(graphics, slot);

            switch (pair.second.sampler)
            {
            case nodec_modules::rendering::interfaces::Sampler::Anisotropic:
                samplerAnisotropic.BindPS(graphics, slot);
                samplerAnisotropic.BindVS(graphics, slot);
                break;
            case nodec_modules::rendering::interfaces::Sampler::Point:
                samplerPoint.BindPS(graphics, slot);
                samplerPoint.BindVS(graphics, slot);
                break;
            default:
            case nodec_modules::rendering::interfaces::Sampler::Bilinear:
                samplerBilinear.BindPS(graphics, slot);
                samplerBilinear.BindVS(graphics, slot);
                break;
            }
        }

        slot++;
    }

}
