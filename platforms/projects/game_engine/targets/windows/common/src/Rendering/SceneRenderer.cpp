#include <Font/FontBackend.hpp>
#include <Rendering/SceneRenderer.hpp>

#include <nodec/unicode.hpp>

void SceneRenderer::Render(nodec_scene::Scene &scene) {
    using namespace nodec;
    using namespace nodec_scene::components;
    using namespace nodec_rendering;
    using namespace nodec_rendering::components;
    using namespace nodec_rendering::resources;
    using namespace DirectX;

    mScenePropertiesCB.BindVS(mpGfx, SCENE_PROPERTIES_CB_SLOT);
    mScenePropertiesCB.BindPS(mpGfx, SCENE_PROPERTIES_CB_SLOT);

    mTextureConfigCB.BindVS(mpGfx, TEXTURE_CONFIG_CB_SLOT);
    mTextureConfigCB.BindPS(mpGfx, TEXTURE_CONFIG_CB_SLOT);

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

        scene.registry().view<const TextRenderer, const Transform>().each([&](auto entt, const TextRenderer &renderer, const Transform &transform) {
            if (!renderer.material) return;
            auto *shader = renderer.material->shader().get();
            if (shader == nullptr) return;
            shaders.emplace(static_cast<ShaderBackend *>(shader));
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

            // If some effects, the off-screen buffer is needed.
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

        auto matrixP = XMMatrixIdentity();

        switch (camera.projection) {
        case Camera::Projection::Perspective:
            matrixP = XMMatrixPerspectiveFovLH(
                XMConvertToRadians(camera.fov_angle),
                aspect,
                camera.near_clip_plane, camera.far_clip_plane);
            break;
        case Camera::Projection::Orthographic:
            matrixP = XMMatrixOrthographicLH(
                camera.ortho_width, camera.ortho_width / aspect,
                camera.near_clip_plane, camera.far_clip_plane);
            break;
        default:
            break;
        }

        const auto matrixPInverse = XMMatrixInverse(nullptr, matrixP);

        XMStoreFloat4x4(&mSceneProperties.matrixP, matrixP);
        XMStoreFloat4x4(&mSceneProperties.matrixPInverse, matrixPInverse);

        XMMATRIX cameraLocal2World{cameraTrfm.local2world.m};
        XMVECTOR scale, rotQuat, trans;
        XMMatrixDecompose(&scale, &rotQuat, &trans, cameraLocal2World);

        auto matrixV = XMMatrixInverse(nullptr, cameraLocal2World);
        XMStoreFloat4x4(&mSceneProperties.matrixV, matrixV);
        XMStoreFloat4x4(&mSceneProperties.matrixVInverse, cameraLocal2World);

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
                    const auto &targets = activeShader->render_targets(0);
                    std::vector<ID3D11RenderTargetView *> renderTargets(targets.size());
                    for (size_t i = 0; i < targets.size(); ++i) {
                        const auto &name = targets[i];
                        auto &buffer = mGeometryBuffers[name];
                        if (!buffer) {
                            buffer.reset(new GeometryBuffer(mpGfx, mpGfx->GetWidth(), mpGfx->GetHeight()));
                        }
                        renderTargets[i] = &buffer->GetRenderTargetView();
                        mpGfx->GetContext().ClearRenderTargetView(renderTargets[i], Vector4f::zero.v);
                    }

                    mpGfx->GetContext().OMSetRenderTargets(renderTargets.size(), renderTargets.data(), mpDepthStencilView.Get());
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

                    GetSamplerState({Sampler::FilterMode::Bilinear, Sampler::WrapMode::Clamp}).BindPS(mpGfx, 0);

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

                // It is assured that material and shader are exists.
                // It is checked at the beginning of rendering pass of camera.
                auto materialBackend = std::static_pointer_cast<MaterialBackend>(activePostProcessEffects[i]->material);
                auto shaderBackend = std::static_pointer_cast<ShaderBackend>(materialBackend->shader());

                SetCullMode(materialBackend->cull_mode());
                materialBackend->bind_constant_buffer(mpGfx, MATERIAL_PROPERTIES_CB_SLOT);

                mTextureConfig.texHasFlag = 0x00;
                const auto slotOffset = BindTextureEntries(materialBackend->texture_entries(), mTextureConfig.texHasFlag);
                mTextureConfigCB.Update(mpGfx, &mTextureConfig);

                for (int passNum = 0; passNum < shaderBackend->pass_count(); ++passNum) {
                    if (passNum == shaderBackend->pass_count() - 1) {
                        // If last pass.
                        // The render target must be one final target.

                        mpGfx->GetContext().OMSetRenderTargets(1, &pCameraRenderTargetView, nullptr);

                    } else {
                        // If halfway pass.
                        // Support the multiple render targets.

                        const auto &targets = shaderBackend->render_targets(passNum);
                        std::vector<ID3D11RenderTargetView *> renderTargets(targets.size());
                        for (size_t i = 0; i < targets.size(); ++i) {
                            const auto &name = targets[i];
                            auto &buffer = mGeometryBuffers[name];
                            if (!buffer) {
                                buffer.reset(new GeometryBuffer(mpGfx, mpGfx->GetWidth(), mpGfx->GetHeight()));
                            }
                            renderTargets[i] = &buffer->GetRenderTargetView();
                            mpGfx->GetContext().ClearRenderTargetView(renderTargets[i], Vector4f::zero.v);
                        }

                        mpGfx->GetContext().OMSetRenderTargets(renderTargets.size(), renderTargets.data(), nullptr);
                    }

                    // --- Bind texture resources.
                    // Bind sampler for textures.

                    GetSamplerState({Sampler::FilterMode::Bilinear, Sampler::WrapMode::Clamp}).BindPS(mpGfx, slotOffset);
                    const auto &textureResources = shaderBackend->texture_resources(passNum);
                    for (std::size_t i = 0; i < textureResources.size(); ++i) {
                        const auto &name = textureResources[i];
                        auto &buffer = mGeometryBuffers[name];
                        if (!buffer) continue;
                        auto *view = &buffer->GetShaderResourceView();
                        mpGfx->GetContext().PSSetShaderResources(slotOffset + i, 1u, &view);
                    }
                    shaderBackend->bind(mpGfx, passNum);

                    mScreenQuadMesh->bind(mpGfx);
                    mpGfx->DrawIndexed(mScreenQuadMesh->triangles.size());
                } // End foreach pass.

                std::swap(mGeometryBuffers["screen"], mGeometryBuffers["screen_back"]);
            } // End foreach effect.
        }
    }); // End foreach camera
}

void SceneRenderer::RenderModel(nodec_scene::Scene &scene, ShaderBackend *activeShader,
                                const DirectX::XMMATRIX &matrixV, const DirectX::XMMATRIX &matrixP) {
    if (activeShader == nullptr) return;

    using namespace nodec;
    using namespace nodec_scene::components;
    using namespace nodec_rendering;
    using namespace nodec_rendering::components;
    using namespace nodec_rendering::resources;
    using namespace DirectX;

    mModelPropertiesCB.BindVS(mpGfx, MODEL_PROPERTIES_CB_SLOT);
    mModelPropertiesCB.BindPS(mpGfx, MODEL_PROPERTIES_CB_SLOT);

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

            materialBackend->bind_constant_buffer(mpGfx, MATERIAL_PROPERTIES_CB_SLOT);
            SetCullMode(materialBackend->cull_mode());

            mTextureConfig.texHasFlag = 0x00;
            BindTextureEntries(materialBackend->texture_entries(), mTextureConfig.texHasFlag);
            mTextureConfigCB.Update(mpGfx, &mTextureConfig);

            meshBackend->bind(mpGfx);
            mpGfx->DrawIndexed(meshBackend->triangles.size());
        } // End foreach mesh
    });   // End foreach mesh renderer

    [&]() {
        if (!mQuadMesh) return;

        mBSAlphaBlend.Bind(mpGfx);

        scene.registry().view<const Transform, const ImageRenderer>().each([&](auto entt, const Transform &trfm, const ImageRenderer &renderer) {
            auto &image = renderer.image;
            auto &material = renderer.material;
            if (!image || !material) return;

            auto *imageBackend = static_cast<TextureBackend *>(image.get());
            auto *materialBackend = static_cast<MaterialBackend *>(material.get());
            auto *shaderBackend = static_cast<ShaderBackend *>(material->shader().get());

            if (shaderBackend != activeShader) return;

            auto savedAlbedo = materialBackend->get_texture_entry("albedo");
            materialBackend->set_texture_entry("albedo", {image, {Sampler::FilterMode::Bilinear, Sampler::WrapMode::Clamp}});

            materialBackend->bind_constant_buffer(mpGfx, MATERIAL_PROPERTIES_CB_SLOT);
            SetCullMode(materialBackend->cull_mode());

            mTextureConfig.texHasFlag = 0x00;
            BindTextureEntries(materialBackend->texture_entries(), mTextureConfig.texHasFlag);
            mTextureConfigCB.Update(mpGfx, &mTextureConfig);

            XMMATRIX matrixM{trfm.local2world.m};
            const auto width = imageBackend->width() / (renderer.pixels_per_unit + std::numeric_limits<float>::epsilon());
            const auto height = imageBackend->height() / (renderer.pixels_per_unit + std::numeric_limits<float>::epsilon());
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

            if (savedAlbedo) {
                materialBackend->set_texture_entry("albedo", *savedAlbedo);
            }
        });

        mBSDefault.Bind(mpGfx);
    }();

    [&]() {
        mBSAlphaBlend.Bind(mpGfx);
        scene.registry().view<const Transform, const TextRenderer>().each([&](auto entt, const Transform &trfm, const TextRenderer &renderer) {
            auto fontBackend = std::static_pointer_cast<FontBackend>(renderer.font);
            if (!fontBackend) return;

            auto materialBackend = std::static_pointer_cast<MaterialBackend>(renderer.material);
            if (!materialBackend) return;

            auto shaderBackend = std::static_pointer_cast<ShaderBackend>(materialBackend->shader());
            if (shaderBackend.get() != activeShader) return;

            auto savedMask = materialBackend->get_texture_entry("mask");
            auto savedAlbedo = materialBackend->get_vector4_property("albedo");

            const auto u32Text = nodec::unicode::utf8to32<std::u32string>(renderer.text);
            const float pixelsPerUnit = renderer.pixels_per_unit;

            float offsetX = 0.0f;
            float offsetY = 0.0f;
            for (const auto &chCode : u32Text) {
                if (chCode == '\n') {
                    offsetY -= renderer.pixel_size / pixelsPerUnit;
                    offsetX = 0.0f;
                    continue;
                }

                const auto &character = mFontCharacterDatabase.Get(fontBackend->GetFace(), renderer.pixel_size, chCode);

                float posX = offsetX + character.bearing.x / pixelsPerUnit;
                float posY = offsetY - (character.size.y - character.bearing.y) / pixelsPerUnit;
                float w = character.size.x / pixelsPerUnit;
                float h = character.size.y / pixelsPerUnit;

                offsetX += (character.advance >> 6) / pixelsPerUnit;

                if (!character.pFontTexture) {
                    continue;
                }

                materialBackend->set_texture_entry("mask", {character.pFontTexture, {Sampler::FilterMode::Bilinear, Sampler::WrapMode::Clamp}});
                materialBackend->set_vector4_property("albedo", renderer.color);

                materialBackend->bind_constant_buffer(mpGfx, 3);
                SetCullMode(materialBackend->cull_mode());

                mTextureConfig.texHasFlag = 0x00;
                BindTextureEntries(materialBackend->texture_entries(), mTextureConfig.texHasFlag);
                mTextureConfigCB.Update(mpGfx, &mTextureConfig);

                XMMATRIX matrixM{trfm.local2world.m};
                matrixM = XMMatrixScaling(w / 2, h / 2, 1.0f) * XMMatrixTranslation(posX + w / 2, posY + h / 2, 0.0f) * matrixM;

                // matrixM
                auto matrixMInverse = XMMatrixInverse(nullptr, matrixM);

                // DirectX Math using row-major representation
                // HLSL using column-major representation
                auto matrixMVP = matrixM * matrixV * matrixP;

                XMStoreFloat4x4(&mModelProperties.matrixM, matrixM);
                XMStoreFloat4x4(&mModelProperties.matrixMInverse, matrixMInverse);
                XMStoreFloat4x4(&mModelProperties.matrixMVP, matrixMVP);

                mModelPropertiesCB.Update(mpGfx, &mModelProperties);
                mScreenQuadMesh->bind(mpGfx);
                mpGfx->DrawIndexed(mScreenQuadMesh->triangles.size());
            }

            if (savedMask) {
                materialBackend->set_texture_entry("mask", *savedMask);
            }

            if (savedAlbedo) {
                materialBackend->set_vector4_property("albedo", *savedAlbedo);
            }
        });

        mBSDefault.Bind(mpGfx);
    }();
}