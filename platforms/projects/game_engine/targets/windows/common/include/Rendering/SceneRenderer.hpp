#pragma once

#include <Graphics/Graphics.hpp>
#include <Graphics/ConstantBuffer.hpp>
#include <Graphics/SamplerState.hpp>
#include <Rendering/MeshBackend.hpp>
#include <Rendering/MaterialBackend.hpp>
#include <Rendering/ShaderBackend.hpp>
#include <Rendering/TextureBackend.hpp>

#include <scene_set/scene.hpp>
#include <scene_set/components/basic.hpp>
#include <rendering/components/mesh_renderer.hpp>
#include <rendering/components/camera.hpp>

#include <nodec/vector4.hpp>

#include <DirectXMath.h>


class SceneRenderer {
    using TextureEntry = rendering::resources::Material::TextureEntry;

public:
    struct SceneProperties
    {
        nodec::Vector4f cameraPos;
    };

    struct ModelProperties
    {
        DirectX::XMFLOAT4X4 matrixMVP;
        DirectX::XMFLOAT4X4 matrixM;
        DirectX::XMFLOAT4X4 matrixMInverse;
    };

    struct TextureConfig
    {
        uint32_t texHasFlag;
        uint32_t padding[3];
    };

public:

    SceneRenderer(Graphics* pGfx)
        : mpGfx(pGfx)
        , mScenePropertiesCB(pGfx, sizeof(SceneProperties), &mSceneProperties)
        , mModelPropertiesCB(pGfx, sizeof(ModelProperties), &mModelProperties)
        , mTextureConfigCB(pGfx, sizeof(TextureConfig), &mTextureConfig)
        , mSamplerAnisotropic(pGfx, SamplerState::Type::Anisotropic)
        , mSamplerBilinear(pGfx, SamplerState::Type::Bilinear)
        , mSamplerPoint(pGfx, SamplerState::Type::Point)
    {

    }

    void Render(scene_set::Scene& scene) {
        using namespace scene_set::components;
        using namespace rendering::components;
        using namespace DirectX;

        auto aspect = static_cast<float>(mpGfx->GetWidth()) / mpGfx->GetHeight();

        mpGfx->GetContext().IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        mScenePropertiesCB.BindVS(mpGfx, 0);
        mScenePropertiesCB.BindPS(mpGfx, 0);

        mModelPropertiesCB.BindVS(mpGfx, 1);
        mModelPropertiesCB.BindPS(mpGfx, 1);

        // slot 2 is reserved for material cb

        mTextureConfigCB.BindVS(mpGfx, 3);
        mTextureConfigCB.BindPS(mpGfx, 3);


        for (auto cameraEntt : scene.registry().view<const Camera, const Transform>()) {
            auto& camera = scene.registry().get_component<const Camera>(cameraEntt);
            auto& cameraTrfm = scene.registry().get_component<const Transform>(cameraEntt);

            auto matrixP = XMMatrixPerspectiveFovLH(
                XMConvertToRadians(camera.fovAngle),
                aspect,
                camera.nearClipPlane, camera.farClipPlane);


            XMMATRIX cameraLocal2Wrold{ cameraTrfm.local2world.m };
            XMVECTOR scale, rotQuat, trans;
            XMMatrixDecompose(&scale, &rotQuat, &trans, cameraLocal2Wrold);

            mSceneProperties.cameraPos.set(
                XMVectorGetByIndex(trans, 0),
                XMVectorGetByIndex(trans, 1),
                XMVectorGetByIndex(trans, 2),
                XMVectorGetByIndex(trans, 3)
            );

            auto matrixV = XMMatrixInverse(nullptr, cameraLocal2Wrold);

            mScenePropertiesCB.Update(mpGfx, &mSceneProperties);

            for (auto entt : scene.registry().view<const Transform, const MeshRenderer>()) {
                auto& trfm = scene.registry().get_component<const Transform>(entt);
                auto& meshRenderer = scene.registry().get_component<const MeshRenderer>(entt);

                if (meshRenderer.meshes.size() == 0) {
                    continue;
                }

                if (meshRenderer.meshes.size() != meshRenderer.materials.size()) {
                    continue;
                }

                XMMATRIX matrixM{ trfm.local2world.m };
                auto matrixMInverse = XMMatrixInverse(nullptr, matrixM);

                // DirectX Math using row-major representation
                // HLSL using column-major representation
                auto matrixMVP = matrixM * matrixV * matrixP;

                XMStoreFloat4x4(&mModelProperties.matrixM, matrixM);
                XMStoreFloat4x4(&mModelProperties.matrixMInverse, matrixMInverse);
                XMStoreFloat4x4(&mModelProperties.matrixMVP, matrixMVP);

                mModelPropertiesCB.Update(mpGfx, &mModelProperties);

                for (int i = 0; i < meshRenderer.meshes.size(); ++i) {
                    auto mesh = meshRenderer.meshes[i];
                    auto material = meshRenderer.materials[i];

                    if (!(mesh && material)) {
                        continue;
                    }

                    auto* meshBackend = static_cast<MeshBackend*>(mesh.get());
                    auto* materialBackend = static_cast<MaterialBackend*>(material.get());
                    auto shader = materialBackend->shader();

                    if (!shader) {
                        continue;
                    }

                    auto* shaderBackend = static_cast<ShaderBackend*>(shader.get());

                    if (shaderBackend->input_layout()) {
                        shaderBackend->input_layout()->Bind(mpGfx);
                    }

                    if (shaderBackend->pixel_shader()) {
                        shaderBackend->pixel_shader()->Bind(mpGfx);
                    }

                    if (shaderBackend->vertex_shader()) {
                        shaderBackend->vertex_shader()->Bind(mpGfx);
                    }

                    materialBackend->update_device_memory();

                    if (materialBackend->constant_buffer()) {
                        materialBackend->constant_buffer()->BindVS(mpGfx, 2);
                        materialBackend->constant_buffer()->BindPS(mpGfx, 2);
                    }

                    mTextureConfig.texHasFlag = 0x00;
                    BindTextureEntries(materialBackend->texture_entries(), mTextureConfig.texHasFlag);
                    mTextureConfigCB.Update(mpGfx, &mTextureConfig);

                    if (meshBackend->vertex_buffer()) {
                        meshBackend->vertex_buffer()->Bind(mpGfx);
                    }

                    if (meshBackend->index_buffer()) {
                        meshBackend->index_buffer()->Bind(mpGfx);
                    }

                    mpGfx->DrawIndexed(meshBackend->triangles.size());
                } // End foreach mesh
            } // End foreach mesh renderer

            break;
        } // End foreach camera

    }

private:
    void BindTextureEntries(const std::vector<TextureEntry>& textureEntries, uint32_t& texHasFlag) {
        UINT slot = 0;

        for (auto& entry : textureEntries) {
            if (!entry.texture) {
                // texture not setted.
                // skip bind texture, 
                // but bind sampler because the The Pixel Shader unit expects a Sampler to be set at Slot 0.
                mSamplerBilinear.BindPS(mpGfx, slot);
                mSamplerBilinear.BindVS(mpGfx, slot);

                slot++;
                continue;
            }

            auto* textureBackend = static_cast<TextureBackend*>(entry.texture.get());

            if (textureBackend->texture_view()) {
                textureBackend->texture_view()->BindVS(mpGfx, slot);
                textureBackend->texture_view()->BindPS(mpGfx, slot);
            }

            switch (entry.sampler)
            {
            case rendering::resources::Sampler::Anisotropic:
                mSamplerAnisotropic.BindVS(mpGfx, slot);
                mSamplerAnisotropic.BindPS(mpGfx, slot);
                break;
            case rendering::resources::Sampler::Point:
                mSamplerPoint.BindVS(mpGfx, slot);
                mSamplerPoint.BindPS(mpGfx, slot);
                break;

            case rendering::resources::Sampler::Bilinear:
            default:
                mSamplerBilinear.BindVS(mpGfx, slot);
                mSamplerBilinear.BindPS(mpGfx, slot);
                break;
            }
            texHasFlag |= 0x01 << slot;

            ++slot;
        }
    }

private:
    SceneProperties mSceneProperties;
    ConstantBuffer mScenePropertiesCB;

    ModelProperties mModelProperties;
    ConstantBuffer mModelPropertiesCB;

    TextureConfig mTextureConfig;
    ConstantBuffer mTextureConfigCB;

    SamplerState mSamplerAnisotropic;
    SamplerState mSamplerPoint;
    SamplerState mSamplerBilinear;

    Graphics* mpGfx;



};