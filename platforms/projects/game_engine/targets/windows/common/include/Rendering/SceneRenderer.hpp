#pragma once

#include <Graphics/ConstantBuffer.hpp>
#include <Graphics/Graphics.hpp>
#include <Graphics/RasterizerState.hpp>
#include <Graphics/SamplerState.hpp>
#include <Rendering/MaterialBackend.hpp>
#include <Rendering/MeshBackend.hpp>
#include <Rendering/ShaderBackend.hpp>
#include <Rendering/TextureBackend.hpp>

#include <nodec_rendering/components/camera.hpp>
#include <nodec_rendering/components/image_renderer.hpp>
#include <nodec_rendering/components/light.hpp>
#include <nodec_rendering/components/mesh_renderer.hpp>
#include <nodec_scene/components/basic.hpp>
#include <nodec_scene/scene.hpp>

#include <nodec/resource_management/resource_registry.hpp>
#include <nodec/vector4.hpp>

#include <DirectXMath.h>

class SceneRenderer {
    using TextureEntry = nodec_rendering::resources::Material::TextureEntry;

public:
    struct DirectionalLight {
        nodec::Vector3f direction;
        float intensity;
        nodec::Vector4f color;
        uint32_t enabled{0x00};
        uint32_t padding[3];
    };

    struct SceneLighting {
        DirectionalLight directional;
    };

    struct SceneProperties {
        nodec::Vector4f cameraPos;
        SceneLighting lights;
    };

    struct ModelProperties {
        DirectX::XMFLOAT4X4 matrixMVP;
        DirectX::XMFLOAT4X4 matrixM;
        DirectX::XMFLOAT4X4 matrixMInverse;
    };

    struct TextureConfig {
        uint32_t texHasFlag;
        uint32_t padding[3];
    };

public:
    SceneRenderer(Graphics *pGfx, nodec::resource_management::ResourceRegistry &resourceRegistry)
        : mpGfx(pGfx),
          mScenePropertiesCB(pGfx, sizeof(SceneProperties), &mSceneProperties),
          mModelPropertiesCB(pGfx, sizeof(ModelProperties), &mModelProperties),
          mTextureConfigCB(pGfx, sizeof(TextureConfig), &mTextureConfig),
          mSamplerAnisotropic(pGfx, SamplerState::Type::Anisotropic),
          mSamplerBilinear(pGfx, SamplerState::Type::Bilinear),
          mSamplerPoint(pGfx, SamplerState::Type::Point),
          mRSCullBack{pGfx, D3D11_CULL_BACK},
          mRSCullNone{pGfx, D3D11_CULL_NONE} {
        using namespace nodec_rendering::resources;
        using namespace nodec::resource_management;
        using namespace nodec;

        auto planeMesh = resourceRegistry.get_resource<Mesh>("org.nodec-rendering.essentials/meshes/plane.mesh", LoadPolicy::Direct).get();

        if (!planeMesh) {
            logging::WarnStream(__FILE__, __LINE__) << "[SceneRenderer] >>> Cannot load the essential resource 'plane.mesh'.\n"
                                                       "Make sure the 'org.nodec-rendering.essentials' resource-package is installed.";
        }

        mPlaneMesh = std::static_pointer_cast<MeshBackend>(planeMesh);
    }

    void Render(nodec_scene::Scene &scene) {
        using namespace nodec;
        using namespace nodec_scene::components;
        using namespace nodec_rendering::components;
        using namespace nodec_rendering::resources;
        using namespace DirectX;

        auto aspect = static_cast<float>(mpGfx->GetWidth()) / mpGfx->GetHeight();

        mpGfx->GetContext().IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        mScenePropertiesCB.BindVS(mpGfx, 0);
        mScenePropertiesCB.BindPS(mpGfx, 0);

        mModelPropertiesCB.BindVS(mpGfx, 1);
        mModelPropertiesCB.BindPS(mpGfx, 1);

        mTextureConfigCB.BindVS(mpGfx, 2);
        mTextureConfigCB.BindPS(mpGfx, 2);

        // --- lights ---
        mSceneProperties.lights.directional.enabled = 0x00;
        scene.registry().view<const Light, const Transform>().each([&](auto entt, const Light &light, const Transform &trfm) {
            switch (light.type) {
            case LightType::Directional: {
                auto &directional = mSceneProperties.lights.directional;
                directional.enabled = 0x01;
                directional.color = light.color;
                directional.intensity = light.intensity;

                auto direction = trfm.local2world * Vector4f{0.0f, 0.0f, 1.0f, 0.0f};
                directional.direction.set(direction.x, direction.y, direction.z);
                break;
            }

            default:
                break;
            }
        });

        scene.registry().view<const Camera, const Transform>().each([&](auto entt, const Camera &camera, const Transform &cameraTrfm) {
            auto matrixP = XMMatrixPerspectiveFovLH(
                XMConvertToRadians(camera.fovAngle),
                aspect,
                camera.nearClipPlane, camera.farClipPlane);

            XMMATRIX cameraLocal2Wrold{cameraTrfm.local2world.m};
            XMVECTOR scale, rotQuat, trans;
            XMMatrixDecompose(&scale, &rotQuat, &trans, cameraLocal2Wrold);

            mSceneProperties.cameraPos.set(
                XMVectorGetByIndex(trans, 0),
                XMVectorGetByIndex(trans, 1),
                XMVectorGetByIndex(trans, 2),
                XMVectorGetByIndex(trans, 3));

            auto matrixV = XMMatrixInverse(nullptr, cameraLocal2Wrold);

            mScenePropertiesCB.Update(mpGfx, &mSceneProperties);

            mRSCullBack.Bind(mpGfx);
            scene.registry().view<const Transform, const MeshRenderer>().each([&](auto entt, const Transform &trfm, const MeshRenderer &meshRenderer) {
                if (meshRenderer.meshes.size() == 0) return;

                if (meshRenderer.meshes.size() != meshRenderer.materials.size()) return;

                XMMATRIX matrixM{trfm.local2world.m};
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

                    if (!(mesh && material)) continue;

                    auto *meshBackend = static_cast<MeshBackend *>(mesh.get());
                    auto *materialBackend = static_cast<MaterialBackend *>(material.get());
                    auto shader = materialBackend->shader();

                    if (!shader) continue;

                    auto *shaderBackend = static_cast<ShaderBackend *>(shader.get());
                    shaderBackend->bind(mpGfx);

                    materialBackend->bind_constant_buffer(mpGfx, 3);
                    SetCullMode(materialBackend->cull_mode());

                    mTextureConfig.texHasFlag = 0x00;
                    BindTextureEntries(materialBackend->texture_entries(), mTextureConfig.texHasFlag);
                    mTextureConfigCB.Update(mpGfx, &mTextureConfig);

                    meshBackend->bind(mpGfx);

                    mpGfx->DrawIndexed(meshBackend->triangles.size());
                } // End foreach mesh
            });   // End foreach mesh renderer

            if (mPlaneMesh) {
                scene.registry().view<const Transform, const ImageRenderer>().each([&](auto entt, const Transform &trfm, const ImageRenderer &renderer) {
                    if (!renderer.image || !renderer.material) return;

                    auto *imageBackend = static_cast<TextureBackend *>(renderer.image.get());
                    auto *materialBackend = static_cast<MaterialBackend *>(renderer.material.get());
                    auto shader = materialBackend->shader();
                    if (!shader) return;

                    auto *shaderBackend = static_cast<ShaderBackend *>(shader.get());

                    shaderBackend->bind(mpGfx);

                    materialBackend->set_texture_entry("albedo", {renderer.image, Sampler::Bilinear});

                    materialBackend->bind_constant_buffer(mpGfx, 3);
                    SetCullMode(materialBackend->cull_mode());

                    mTextureConfig.texHasFlag = 0x00;
                    BindTextureEntries(materialBackend->texture_entries(), mTextureConfig.texHasFlag);
                    mTextureConfigCB.Update(mpGfx, &mTextureConfig);

                    XMMATRIX matrixM{trfm.local2world.m};
                    const auto width = (imageBackend->width() / 2.0f) / renderer.pixelsPerUnit;
                    const auto height = (imageBackend->height() / 2.0f) / renderer.pixelsPerUnit;
                    matrixM = XMMatrixScaling(width, height, 1.0f) * matrixM;

                    // matrixM
                    auto matrixMInverse = XMMatrixInverse(nullptr, matrixM);

                    // DirectX Math using row-major representation
                    // HLSL using column-major representation
                    auto matrixMVP = matrixM * matrixV * matrixP;

                    XMStoreFloat4x4(&mModelProperties.matrixM, matrixM);
                    XMStoreFloat4x4(&mModelProperties.matrixMInverse, matrixMInverse);
                    XMStoreFloat4x4(&mModelProperties.matrixMVP, matrixMVP);

                    mModelPropertiesCB.Update(mpGfx, &mModelProperties);

                    mPlaneMesh->bind(mpGfx);

                    mpGfx->DrawIndexed(mPlaneMesh->triangles.size());
                });
            }
        }); // End foreach camera
    }

private:
    void SetCullMode(const nodec_rendering::CullMode &mode) {
        using namespace nodec_rendering;
        switch (mode) {
        default:
        case CullMode::Back:
            mRSCullBack.Bind(mpGfx);
            break;
        case CullMode::Front:
            // TODO: impl
        case CullMode::Off:
            mRSCullNone.Bind(mpGfx);
            break;
        }
    }
    void BindTextureEntries(const std::vector<TextureEntry> &textureEntries, uint32_t &texHasFlag) {
        UINT slot = 0;

        for (auto &entry : textureEntries) {
            if (!entry.texture) {
                // texture not setted.
                // skip bind texture,
                // but bind sampler because the The Pixel Shader unit expects a Sampler to be set at Slot 0.
                mSamplerBilinear.BindPS(mpGfx, slot);
                mSamplerBilinear.BindVS(mpGfx, slot);

                slot++;
                continue;
            }

            auto *textureBackend = static_cast<TextureBackend *>(entry.texture.get());

            if (textureBackend->texture_view()) {
                textureBackend->texture_view()->BindVS(mpGfx, slot);
                textureBackend->texture_view()->BindPS(mpGfx, slot);
            }

            switch (entry.sampler) {
            case nodec_rendering::resources::Sampler::Anisotropic:
                mSamplerAnisotropic.BindVS(mpGfx, slot);
                mSamplerAnisotropic.BindPS(mpGfx, slot);
                break;

            case nodec_rendering::resources::Sampler::Point:
                mSamplerPoint.BindVS(mpGfx, slot);
                mSamplerPoint.BindPS(mpGfx, slot);
                break;

            case nodec_rendering::resources::Sampler::Bilinear:
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

    RasterizerState mRSCullNone;
    RasterizerState mRSCullBack;

    Graphics *mpGfx;

    std::shared_ptr<MeshBackend> mPlaneMesh;
};