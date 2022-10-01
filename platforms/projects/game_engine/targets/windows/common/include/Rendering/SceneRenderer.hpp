#pragma once

#include <Font/FontCharacterDatabase.hpp>
#include <Graphics/BlendState.hpp>
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
#include <nodec_rendering/components/text_renderer.hpp>
#include <nodec_rendering/sampler.hpp>
#include <nodec_scene/components/transform.hpp>
#include <nodec_scene/scene.hpp>

#include <nodec/resource_management/resource_registry.hpp>
#include <nodec/vector4.hpp>

#include <DirectXMath.h>

#include <algorithm>
#include <unordered_set>

class SceneRenderer {
    using TextureEntry = nodec_rendering::resources::Material::TextureEntry;

    static constexpr UINT SCENE_PROPERTIES_CB_SLOT = 0;
    static constexpr UINT TEXTURE_CONFIG_CB_SLOT = 1;
    static constexpr UINT MODEL_PROPERTIES_CB_SLOT = 2;
    static constexpr UINT MATERIAL_PROPERTIES_CB_SLOT = 3;

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
          mBSDefault(BlendState::CreateDefaultBlend(pGfx)),
          mBSAlphaBlend(BlendState::CreateAlphaBlend(pGfx)),
          mRSCullBack{pGfx, D3D11_CULL_BACK},
          mRSCullNone{pGfx, D3D11_CULL_NONE},
          mFontCharacterDatabase{pGfx} {
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

    void Render(nodec_scene::Scene &scene);

private:
    void RenderModel(nodec_scene::Scene &scene, ShaderBackend *activeShader,
                     const DirectX::XMMATRIX &matrixV, const DirectX::XMMATRIX &matrixP);

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
                auto &defaultSamplerState = GetSamplerState({}); 
                defaultSamplerState.BindPS(mpGfx, slot);
                defaultSamplerState.BindVS(mpGfx, slot);

                ++slot;
                continue;
            }

            auto *textureBackend = static_cast<TextureBackend *>(entry.texture.get());
            {
                auto *view = &textureBackend->shader_resource_view();
                mpGfx->GetContext().VSSetShaderResources(slot, 1u, &view);
                mpGfx->GetContext().PSSetShaderResources(slot, 1u, &view);
            }

            auto &samplerState = GetSamplerState(entry.sampler);
            samplerState.BindVS(mpGfx, slot);
            samplerState.BindPS(mpGfx, slot);

            texHasFlag |= 0x01 << slot;

            ++slot;
        }
        return slot;
    }

    SamplerState& GetSamplerState(const nodec_rendering::Sampler& sampler) {
        auto &state = mSamplerStates[sampler];
        if (state) return *state;

        state = SamplerState::Create(mpGfx, sampler);
        return *state;
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

    std::unordered_map<nodec_rendering::Sampler, nodec::optional<SamplerState>> mSamplerStates;

    RasterizerState mRSCullNone;
    RasterizerState mRSCullBack;

    BlendState mBSDefault;
    BlendState mBSAlphaBlend;

    Graphics *mpGfx;

    std::shared_ptr<MeshBackend> mQuadMesh;
    std::unique_ptr<MeshBackend> mScreenQuadMesh;
    std::unordered_map<std::string, std::unique_ptr<GeometryBuffer>> mGeometryBuffers;

    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mpDepthStencilView;

    FontCharacterDatabase mFontCharacterDatabase;
};