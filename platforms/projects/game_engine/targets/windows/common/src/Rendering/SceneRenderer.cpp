#include <Rendering/SceneRenderer.hpp>
#include <Font/FontBackend.hpp>

#include <nodec/unicode.hpp>

void SceneRenderer::RenderModel(nodec_scene::Scene &scene, ShaderBackend *activeShader,
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

    scene.registry().view<const Transform, const TextRenderer>().each([&](auto entt, const Transform &trfm, const TextRenderer &renderer) {
        const auto fontBackend = std::static_pointer_cast<FontBackend>(renderer.font);
        if (!fontBackend) return;

        const auto materialBackend = std::static_pointer_cast<MaterialBackend>(renderer.material);
        if (!materialBackend) return;

        const auto shaderBackend = std::static_pointer_cast<ShaderBackend>(materialBackend->shader());
        if (shaderBackend.get() != activeShader) return;
        
        const auto u32Text = nodec::unicode::utf8to32<std::u32string>(renderer.text);

        float offsetX = 0.0f;
        float offsetY = 0.0f;
        for (const auto& chCode : u32Text) {
            const auto& character = mFontCharacterDatabase.Get(fontBackend->GetFace(), renderer.pixel_size, chCode);

            // materialBackend->set_texture_entry("albedo", {character.})
        }
    });
}