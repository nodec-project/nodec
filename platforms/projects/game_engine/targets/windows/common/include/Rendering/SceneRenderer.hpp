#pragma once

#include <Graphics/ConstantBuffer.hpp>
#include <Graphics/GeometryBuffer.hpp>
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

#include <unordered_set>

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

        // Get quad mesh from resource registry.
        {
            auto quadMesh = resourceRegistry.get_resource<Mesh>("org.nodec-rendering.essentials/meshes/quad.mesh", LoadPolicy::Direct).get();

            if (!quadMesh) {
                logging::WarnStream(__FILE__, __LINE__) << "[SceneRenderer] >>> Cannot load the essential resource 'quad.mesh'.\n"
                                                           "Make sure the 'org.nodec-rendering.essentials' resource-package is installed.";
            }

            mQuadMesh = std::static_pointer_cast<MeshBackend>(quadMesh);
        }

        // Make screen quad mesh in NDC space which is not depend on target view size.
        {
            mScreenQuadMesh.reset(new MeshBackend());

            mScreenQuadMesh->vertices.resize(4);
            mScreenQuadMesh->triangles.resize(6);

            mScreenQuadMesh->vertices[0] = {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}};
            mScreenQuadMesh->vertices[1] = {{-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}};
            mScreenQuadMesh->vertices[2] = {{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}};
            mScreenQuadMesh->vertices[3] = {{1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}};

            mScreenQuadMesh->triangles[0] = 0;
            mScreenQuadMesh->triangles[1] = 1;
            mScreenQuadMesh->triangles[2] = 2;

            mScreenQuadMesh->triangles[3] = 0;
            mScreenQuadMesh->triangles[4] = 2;
            mScreenQuadMesh->triangles[5] = 3;
            mScreenQuadMesh->update_device_memory(mpGfx);
        }

        {
            for (auto &pBuffer : mGeometryBuffers) {
                pBuffer.reset(new GeometryBuffer(pGfx, pGfx->GetWidth(), pGfx->GetHeight()));
            }

            // Generate the depth stencil buffer texture.
            Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilTexture;
            D3D11_TEXTURE2D_DESC depthStencilBufferDesc{};
            depthStencilBufferDesc.Width = pGfx->GetWidth();
            depthStencilBufferDesc.Height = mpGfx->GetHeight();
            depthStencilBufferDesc.MipLevels = 1;
            depthStencilBufferDesc.ArraySize = 1;
            depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            depthStencilBufferDesc.SampleDesc.Count = 1;
            depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
            depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            ThrowIfFailedGfx(
                pGfx->GetDevice().CreateTexture2D(&depthStencilBufferDesc, nullptr, &depthStencilTexture),
                pGfx, __FILE__, __LINE__);

            D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
            depthStencilViewDesc.Format = depthStencilBufferDesc.Format;
            depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

            ThrowIfFailedGfx(
                pGfx->GetDevice().CreateDepthStencilView(depthStencilTexture.Get(), &depthStencilViewDesc, &mpDepthStencilView),
                pGfx, __FILE__, __LINE__);
        }
    }

    void Render(nodec_scene::Scene &scene) {
        using namespace nodec;
        using namespace nodec_scene::components;
        using namespace nodec_rendering::components;
        using namespace nodec_rendering::resources;
        using namespace DirectX;

        mScenePropertiesCB.BindVS(mpGfx, 0);
        mScenePropertiesCB.BindPS(mpGfx, 0);

        // --- lights ---
        {
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
        }

        // At first, we get the active shaders to render for each shader.
        std::unordered_set<ShaderBackend *> activeShaders;
        {
            scene.registry().view<const MeshRenderer, const Transform>().each([&](auto entt, const MeshRenderer &renderer, const Transform &trfm) {
                for (auto &material : renderer.materials) {
                    if (!material) continue;
                    auto *backend = static_cast<const MaterialBackend *>(material.get());
                    auto *shader = static_cast<ShaderBackend *>(backend->shader().get());
                    if (!shader) continue;
                    activeShaders.emplace(shader);
                }
            });

            scene.registry().view<const ImageRenderer, const Transform>().each([&](auto entt, const ImageRenderer &renderer, const Transform &trfm) {
                auto *material = static_cast<const MaterialBackend *>(renderer.material.get());
                if (!material) return;
                auto *shader = static_cast<ShaderBackend *>(material->shader().get());
                if (!shader) return;
                activeShaders.emplace(shader);
            });
        }

        if (activeShaders.size() == 0) return;

        const auto aspect = static_cast<float>(mpGfx->GetWidth()) / mpGfx->GetHeight();

        // --- per camera ---
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
            mScenePropertiesCB.Update(mpGfx, &mSceneProperties);

            auto matrixV = XMMatrixInverse(nullptr, cameraLocal2Wrold);

            for (auto *activeShader : activeShaders) {
                if (activeShader->pass_count() == 1) {
                    auto *target = &mpGfx->GetRenderTargetView();
                    mpGfx->GetContext().OMSetRenderTargets(1, &target, mpDepthStencilView.Get());
                    mpGfx->GetContext().ClearDepthStencilView(mpDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
                    mpGfx->GetContext().IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                    activeShader->bind(mpGfx);

                    // now lets draw the mesh.
                    RenderModel(scene, activeShader, matrixV, matrixP);
                } else {
                    const auto passCount = activeShader->pass_count();
                    // first pass
                    {
                        const auto &targets = activeShader->render_targerts(0);
                        std::vector<ID3D11RenderTargetView *> renderTargerts(targets.size());
                        for (size_t i = 0; i < targets.size(); ++i) {
                            const auto &name = targets[i];
                            renderTargerts[i] = &mGeometryBuffers[i]->GetRenderTargetView();
                            mpGfx->GetContext().ClearRenderTargetView(renderTargerts[i], Vector4f::zero.v);
                            mGeometryBufferNameMap[name] = i;
                        }

                        mpGfx->GetContext().OMSetRenderTargets(renderTargerts.size(), renderTargerts.data(), mpDepthStencilView.Get());
                        mpGfx->GetContext().ClearDepthStencilView(mpDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

                        mpGfx->GetContext().IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                        activeShader->bind(mpGfx, 0);

                        RenderModel(scene, activeShader, matrixV, matrixP);
                    }

                    // medium pass
                    {

                    }

                    // final pass
                    {
                        const auto passNum = passCount - 1;
                        const auto &textureResources = activeShader->texture_resources(passNum);
                        for (size_t i = 0; i < textureResources.size(); ++i) {
                            const auto &name = textureResources[i];
                            const auto index = mGeometryBufferNameMap[name];
                            auto* view = &mGeometryBuffers[index]->GetShaderResourceView();
                            mpGfx->GetContext().PSSetShaderResources(i, 1u, &view);
                        }

                        auto *target = &mpGfx->GetRenderTargetView();
                        mpGfx->GetContext().OMSetRenderTargets(1, &target, nullptr);
                        mpGfx->GetContext().IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                        activeShader->bind(mpGfx, passNum);

                        mSamplerBilinear.BindPS(mpGfx, 0);

                        mScreenQuadMesh->bind(mpGfx);
                        mpGfx->DrawIndexed(mScreenQuadMesh->triangles.size());
                    }
                }
            }

            // TODO: post processing
        }); // End foreach camera
    }

private:
    void RenderModel(nodec_scene::Scene &scene, ShaderBackend *activeShader, 
        const DirectX::XMMATRIX &matrixV, const DirectX::XMMATRIX &matrixP) {
        if (activeShader == nullptr) return;

        using namespace nodec;
        using namespace nodec_scene::components;
        using namespace nodec_rendering::components;
        using namespace nodec_rendering::resources;
        using namespace DirectX;

        mModelPropertiesCB.BindVS(mpGfx, 1);
        mModelPropertiesCB.BindPS(mpGfx, 1);

        mTextureConfigCB.BindVS(mpGfx, 2);
        mTextureConfigCB.BindPS(mpGfx, 2);

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
                auto &mesh = meshRenderer.meshes[i];
                auto &material = meshRenderer.materials[i];
                if (!mesh || !material) continue;

                auto *meshBackend = static_cast<MeshBackend *>(mesh.get());
                auto *materialBackend = static_cast<MaterialBackend *>(material.get());
                auto *shaderBackend = static_cast<ShaderBackend *>(material->shader().get());

                if (shaderBackend != activeShader) continue;

                materialBackend->bind_constant_buffer(mpGfx, 3);
                SetCullMode(materialBackend->cull_mode());

                mTextureConfig.texHasFlag = 0x00;
                BindTextureEntries(materialBackend->texture_entries(), mTextureConfig.texHasFlag);
                mTextureConfigCB.Update(mpGfx, &mTextureConfig);

                meshBackend->bind(mpGfx);
                mpGfx->DrawIndexed(meshBackend->triangles.size());
            } // End foreach mesh
        });   // End foreach mesh renderer

        if (mQuadMesh) {
            scene.registry().view<const Transform, const ImageRenderer>().each([&](auto entt, const Transform &trfm, const ImageRenderer &renderer) {
                auto &image = renderer.image;
                auto &material = renderer.material;
                if (!image || !material) return;

                auto *imageBackend = static_cast<TextureBackend *>(image.get());
                auto *materialBackend = static_cast<MaterialBackend *>(material.get());
                auto *shaderBackend = static_cast<ShaderBackend *>(material->shader().get());

                if (shaderBackend != activeShader) return;

                materialBackend->set_texture_entry("albedo", {image, Sampler::Bilinear});

                materialBackend->bind_constant_buffer(mpGfx, 3);
                SetCullMode(materialBackend->cull_mode());

                mTextureConfig.texHasFlag = 0x00;
                BindTextureEntries(materialBackend->texture_entries(), mTextureConfig.texHasFlag);
                mTextureConfigCB.Update(mpGfx, &mTextureConfig);

                XMMATRIX matrixM{trfm.local2world.m};
                const auto width = imageBackend->width() / (renderer.pixelsPerUnit + std::numeric_limits<float>::epsilon());
                const auto height = imageBackend->height() / (renderer.pixelsPerUnit + std::numeric_limits<float>::epsilon());
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

                mQuadMesh->bind(mpGfx);
                mpGfx->DrawIndexed(mQuadMesh->triangles.size());
            });
        }
    }

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

    std::shared_ptr<MeshBackend> mQuadMesh;
    std::unique_ptr<MeshBackend> mScreenQuadMesh;
    std::array<std::unique_ptr<GeometryBuffer>, 3> mGeometryBuffers;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mpDepthStencilView;

    std::unordered_map<std::string, int> mGeometryBufferNameMap;
};