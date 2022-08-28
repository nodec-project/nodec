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
#include <nodec_rendering/components/directional_light.hpp>
#include <nodec_rendering/components/image_renderer.hpp>
#include <nodec_rendering/components/mesh_renderer.hpp>
#include <nodec_rendering/components/point_light.hpp>
#include <nodec_rendering/components/post_processing.hpp>
#include <nodec_rendering/components/scene_lighting.hpp>
#include <nodec_scene/components/basic.hpp>
#include <nodec_scene/scene.hpp>

#include <nodec/resource_management/resource_registry.hpp>
#include <nodec/vector4.hpp>

#include <DirectXMath.h>

#include <algorithm>
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

    struct PointLight {
        nodec::Vector3f position;
        float range;
        nodec::Vector3f color;
        float intensity;
    };

    static constexpr size_t MAX_NUM_OF_POINT_LIGHTS = 100;

    struct SceneLighting {
        nodec::Vector4f ambientColor;
        uint32_t numOfPointLights;
        uint32_t padding[3];
        DirectionalLight directional;
        PointLight pointLights[MAX_NUM_OF_POINT_LIGHTS];
    };

    struct SceneProperties {
        nodec::Vector4f cameraPos;
        DirectX::XMFLOAT4X4 matrixP;
        DirectX::XMFLOAT4X4 matrixPInverse;
        DirectX::XMFLOAT4X4 matrixV;
        DirectX::XMFLOAT4X4 matrixVInverse;
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

        mTextureConfigCB.BindVS(mpGfx, 1);
        mTextureConfigCB.BindPS(mpGfx, 1);

        // --- lights ---
        {
            mSceneProperties.lights.directional.enabled = 0x00;
            scene.registry().view<const nodec_rendering::components::DirectionalLight, const Transform>().each([&](auto entt, const nodec_rendering::components::DirectionalLight &light, const Transform &trfm) {
                auto &directional = mSceneProperties.lights.directional;
                directional.enabled = 0x01;
                directional.color = light.color;
                directional.intensity = light.intensity;

                auto direction = trfm.local2world * Vector4f{0.0f, 0.0f, 1.0f, 0.0f};
                directional.direction.set(direction.x, direction.y, direction.z);
            });
            scene.registry().view<const nodec_rendering::components::SceneLighting>().each([&](auto entt, const nodec_rendering::components::SceneLighting &lighting) {
                mSceneProperties.lights.ambientColor = lighting.ambient_color;
            });
        }

        // At first, we get the active shaders to render for each shader.
        std::vector<ShaderBackend *> activeShaders;
        {
            std::unordered_set<ShaderBackend *> shaders;
            scene.registry().view<const MeshRenderer, const Transform>().each([&](auto entt, const MeshRenderer &renderer, const Transform &trfm) {
                for (auto &material : renderer.materials) {
                    if (!material) continue;
                    auto *backend = static_cast<const MaterialBackend *>(material.get());
                    auto *shader = static_cast<ShaderBackend *>(backend->shader().get());
                    if (!shader) continue;
                    shaders.emplace(shader);
                }
            });

            scene.registry().view<const ImageRenderer, const Transform>().each([&](auto entt, const ImageRenderer &renderer, const Transform &trfm) {
                auto *material = static_cast<const MaterialBackend *>(renderer.material.get());
                if (!material) return;
                auto *shader = static_cast<ShaderBackend *>(material->shader().get());
                if (!shader) return;
                shaders.emplace(shader);
            });

            // now let's sort by priority.
            activeShaders.resize(shaders.size());
            std::copy(shaders.begin(), shaders.end(), activeShaders.begin());
            std::sort(activeShaders.begin(), activeShaders.end(),
                      [](const auto &a, const auto &b) { return a->rendering_priority() < b->rendering_priority(); });
        }

        if (activeShaders.size() == 0) return;

        const auto aspect = static_cast<float>(mpGfx->GetWidth()) / mpGfx->GetHeight();

        // Render the scene per each camera.
        scene.registry().view<const Camera, const Transform>().each([&](auto cameraEntt, const Camera &camera, const Transform &cameraTrfm) {
            ID3D11RenderTargetView *pCameraRenderTargetView = &mpGfx->GetRenderTargetView();

            // --- Get active post process effects. ---
            std::vector<const PostProcessing::Effect *> activePostProcessEffects;
            {
                const PostProcessing *postProcessing = scene.registry().try_get_component<const PostProcessing>(cameraEntt);

                if (postProcessing) {
                    for (const auto &effect : postProcessing->effects) {
                        if (effect.enabled && effect.material && effect.material->shader()) {
                            activePostProcessEffects.push_back(&effect);
                        }
                    }
                }

                // If some effects, the off-screen buffer is neeeded.
                if (activePostProcessEffects.size() > 0) {
                    auto &buffer = mGeometryBuffers["screen"];
                    if (!buffer) {
                        buffer.reset(new GeometryBuffer(mpGfx, mpGfx->GetWidth(), mpGfx->GetHeight()));
                    }

                    // Set the render target.
                    pCameraRenderTargetView = &buffer->GetRenderTargetView();
                }
            }

            // Clear render target view with solid color.
            // TODO: Consider skybox.
            {
                const float color[] = {0.1f, 0.1f, 0.1f, 1.0f};
                mpGfx->GetContext().ClearRenderTargetView(pCameraRenderTargetView, color);
            }

            const auto matrixP = XMMatrixPerspectiveFovLH(
                XMConvertToRadians(camera.fovAngle),
                aspect,
                camera.nearClipPlane, camera.farClipPlane);
            const auto matrixPInverse = XMMatrixInverse(nullptr, matrixP);

            XMStoreFloat4x4(&mSceneProperties.matrixP, matrixP);
            XMStoreFloat4x4(&mSceneProperties.matrixPInverse, matrixPInverse);

            XMMATRIX cameraLocal2Wrold{cameraTrfm.local2world.m};
            XMVECTOR scale, rotQuat, trans;
            XMMatrixDecompose(&scale, &rotQuat, &trans, cameraLocal2Wrold);

            auto matrixV = XMMatrixInverse(nullptr, cameraLocal2Wrold);
            XMStoreFloat4x4(&mSceneProperties.matrixV, matrixV);
            XMStoreFloat4x4(&mSceneProperties.matrixVInverse, cameraLocal2Wrold);

            mSceneProperties.cameraPos.set(
                XMVectorGetByIndex(trans, 0),
                XMVectorGetByIndex(trans, 1),
                XMVectorGetByIndex(trans, 2),
                XMVectorGetByIndex(trans, 3));

            // Update active point lights.
            {
                mSceneProperties.lights.numOfPointLights = 0;
                auto view = scene.registry().view<const Transform, const nodec_rendering::components::PointLight>();
                for (const auto &entt : view) {
                    // TODO: Light culling.
                    const auto index = mSceneProperties.lights.numOfPointLights;
                    if (index >= MAX_NUM_OF_POINT_LIGHTS) break;

                    const auto &trfm = view.get<const Transform>(entt);
                    const auto &light = view.get<const nodec_rendering::components::PointLight>(entt);
                    const auto worldPosition = trfm.local2world * Vector4f(0, 0, 0, 1.0f);

                    mSceneProperties.lights.pointLights[index].position.set(worldPosition.x, worldPosition.y, worldPosition.z);
                    mSceneProperties.lights.pointLights[index].color.set(light.color.x, light.color.y, light.color.z);
                    mSceneProperties.lights.pointLights[index].intensity = light.intensity;
                    mSceneProperties.lights.pointLights[index].range = light.range;

                    ++mSceneProperties.lights.numOfPointLights;
                }
            }

            mScenePropertiesCB.Update(mpGfx, &mSceneProperties);

            // Reset depth buffer.
            mpGfx->GetContext().ClearDepthStencilView(mpDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

            // Executes the shader programs.
            for (auto *activeShader : activeShaders) {
                if (activeShader->pass_count() == 1) {
                    // One pass shader.
                    mpGfx->GetContext().OMSetRenderTargets(1, &pCameraRenderTargetView, mpDepthStencilView.Get());
                    mpGfx->GetContext().IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                    activeShader->bind(mpGfx);

                    // now lets draw the mesh.
                    RenderModel(scene, activeShader, matrixV, matrixP);
                } else {
                    // Multi pass shader.
                    const auto passCount = activeShader->pass_count();
                    // first pass
                    {
                        const auto &targets = activeShader->render_targerts(0);
                        std::vector<ID3D11RenderTargetView *> renderTargerts(targets.size());
                        for (size_t i = 0; i < targets.size(); ++i) {
                            const auto &name = targets[i];
                            auto &buffer = mGeometryBuffers[name];
                            if (!buffer) {
                                buffer.reset(new GeometryBuffer(mpGfx, mpGfx->GetWidth(), mpGfx->GetHeight()));
                            }
                            renderTargerts[i] = &buffer->GetRenderTargetView();
                            mpGfx->GetContext().ClearRenderTargetView(renderTargerts[i], Vector4f::zero.v);
                        }

                        mpGfx->GetContext().OMSetRenderTargets(renderTargerts.size(), renderTargerts.data(), mpDepthStencilView.Get());
                        mpGfx->GetContext().ClearDepthStencilView(mpDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

                        mpGfx->GetContext().IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                        activeShader->bind(mpGfx, 0);

                        RenderModel(scene, activeShader, matrixV, matrixP);
                    }

                    // medium pass
                    // NOTE: This pass maybe not necessary.
                    //  Alternatively, we can use the another shader pass.
                    {

                    }

                    // final pass
                    {
                        const auto passNum = passCount - 1;

                        mpGfx->GetContext().OMSetRenderTargets(1, &pCameraRenderTargetView, nullptr);
                        mpGfx->GetContext().IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

                        const auto &textureResources = activeShader->texture_resources(passNum);
                        for (size_t i = 0; i < textureResources.size(); ++i) {
                            const auto &name = textureResources[i];
                            auto &buffer = mGeometryBuffers[name];
                            if (!buffer) continue;
                            auto *view = &buffer->GetShaderResourceView();
                            mpGfx->GetContext().PSSetShaderResources(i, 1u, &view);
                        }

                        activeShader->bind(mpGfx, passNum);

                        mSamplerBilinear.BindPS(mpGfx, 0);

                        mScreenQuadMesh->bind(mpGfx);
                        mpGfx->DrawIndexed(mScreenQuadMesh->triangles.size());
                    }
                }
            }

            // --- Post Processing ---
            if (activePostProcessEffects.size() > 0) {
                mpGfx->GetContext().IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

                if (activePostProcessEffects.size() > 1) {
                    // if multiple effects, needs the back buffer.
                    auto &backBuffer = mGeometryBuffers["screen_back"];
                    if (!backBuffer) {
                        backBuffer.reset(new GeometryBuffer(mpGfx, mpGfx->GetWidth(), mpGfx->GetHeight()));
                    }
                }

                for (std::size_t i = 0; i < activePostProcessEffects.size(); ++i) {
                    if (i != activePostProcessEffects.size() - 1) {
                        pCameraRenderTargetView = &mGeometryBuffers["screen_back"]->GetRenderTargetView();
                    } else {
                        // if last, render target is frame buffer.
                        pCameraRenderTargetView = &mpGfx->GetRenderTargetView();
                    }

                    mpGfx->GetContext().OMSetRenderTargets(1, &pCameraRenderTargetView, nullptr);

                    // It is assured that material and shader are exists.
                    // It is checked at the begining of rendering pass of camera.
                    auto materialBackend = std::static_pointer_cast<MaterialBackend>(activePostProcessEffects[i]->material);
                    auto shaderBackend = std::static_pointer_cast<ShaderBackend>(materialBackend->shader());

                    materialBackend->bind_constant_buffer(mpGfx, 3);
                    SetCullMode(materialBackend->cull_mode());

                    mTextureConfig.texHasFlag = 0x00;
                    auto slot = BindTextureEntries(materialBackend->texture_entries(), mTextureConfig.texHasFlag);
                    mTextureConfigCB.Update(mpGfx, &mTextureConfig);

                    const auto &textureResources = shaderBackend->texture_resources(0);
                    for (std::size_t i = 0; i < textureResources.size(); ++i, ++slot) {
                        const auto &name = textureResources[i];
                        auto &buffer = mGeometryBuffers[name];
                        if (!buffer) continue;
                        auto *view = &buffer->GetShaderResourceView();
                        mpGfx->GetContext().PSSetShaderResources(slot, 1u, &view);
                    }
                    shaderBackend->bind(mpGfx, 0);

                    mSamplerBilinear.BindPS(mpGfx, 0);

                    mScreenQuadMesh->bind(mpGfx);
                    mpGfx->DrawIndexed(mScreenQuadMesh->triangles.size());

                    std::swap(mGeometryBuffers["screen"], mGeometryBuffers["screen_back"]);
                }
            }
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

        mModelPropertiesCB.BindVS(mpGfx, 2);
        mModelPropertiesCB.BindPS(mpGfx, 2);

        scene.registry().view<const Transform, const MeshRenderer>().each([&](auto entt, const Transform &trfm, const MeshRenderer &meshRenderer) {
            if (meshRenderer.meshes.size() == 0) return;
            if (meshRenderer.meshes.size() != meshRenderer.materials.size()) return;

            // DirectX Math using row-major representation, row-major memory order.
            // nodec using column-major representation, column-major memory order.
            // HLSL using column-major representation, row-major memory order.
            //
            // nodec -> DirectX Math
            //  Mathematically, when a matrix is converted from column-major representation to row-major representation,
            //  it needs to be transposed.
            //  However, memory ordering of nodec and DirectX Math is different.
            //  Therefore, the matrix is automatically transposed when it is assigned to each other.
            XMMATRIX matrixM{trfm.local2world.m};
            auto matrixMInverse = XMMatrixInverse(nullptr, matrixM);

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

    /**
     * @brief
     *
     * @param textureEntries
     * @param texHasFlag
     * @return UINT next slot number.
     */
    UINT BindTextureEntries(const std::vector<TextureEntry> &textureEntries, uint32_t &texHasFlag) {
        UINT slot = 0;

        for (auto &entry : textureEntries) {
            if (!entry.texture) {
                // texture not setted.
                // skip bind texture,
                // but bind sampler because the The Pixel Shader unit expects a Sampler to be set at Slot 0.
                mSamplerBilinear.BindPS(mpGfx, slot);
                mSamplerBilinear.BindVS(mpGfx, slot);

                ++slot;
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
        return slot;
    }

private:
    // slot 0
    SceneProperties mSceneProperties;
    ConstantBuffer mScenePropertiesCB;

    // slot 1
    TextureConfig mTextureConfig;
    ConstantBuffer mTextureConfigCB;

    // slot 2
    ModelProperties mModelProperties;
    ConstantBuffer mModelPropertiesCB;

    SamplerState mSamplerAnisotropic;
    SamplerState mSamplerPoint;
    SamplerState mSamplerBilinear;

    RasterizerState mRSCullNone;
    RasterizerState mRSCullBack;

    Graphics *mpGfx;

    std::shared_ptr<MeshBackend> mQuadMesh;
    std::unique_ptr<MeshBackend> mScreenQuadMesh;
    std::unordered_map<std::string, std::unique_ptr<GeometryBuffer>> mGeometryBuffers;

    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mpDepthStencilView;
};