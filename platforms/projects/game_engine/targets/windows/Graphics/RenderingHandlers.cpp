#include "RenderingHandlers.hpp"
#include "RenderingUtils.hpp"

using namespace nodec;
using namespace nodec_modules;

void RenderingHandlers::SetupHandlers(std::shared_ptr<RenderingHandlers> handlers,
                                      nodec_modules::rendering::RenderingModule& rendering_module)
{
    // --- Mesh ---
    auto meshBindingHandler
        = event::MemberCallback<RenderingHandlers, const rendering::interfaces::Mesh*>::make_shared(
            handlers, &RenderingHandlers::HandleMeshBinding
        );
    rendering_module.on_bind_mesh += meshBindingHandler;

    auto meshUnbindingHandler
        = event::MemberCallback<RenderingHandlers, const rendering::interfaces::Mesh*>::make_shared(
            handlers, &RenderingHandlers::HandleMeshUnbinding
        );
    rendering_module.on_unbind_mesh += meshUnbindingHandler;
    // End Mesh ---

    // --- Shader ---
    auto shaderBindingHandler
        = event::MemberCallback<RenderingHandlers, const rendering::interfaces::Shader*>::make_shared(
            handlers, &RenderingHandlers::HandleShaderBinding
        );
    rendering_module.on_bind_shader += shaderBindingHandler;

    auto shaderUnbindingHandler
        = event::MemberCallback<RenderingHandlers, const rendering::interfaces::Shader*>::make_shared(
            handlers, &RenderingHandlers::HandleShaderUnbinding
        );
    rendering_module.on_unbind_shader += shaderUnbindingHandler;
    // End Shader ---

    // --- Material ---
    auto materialBindingHandler
        = event::MemberCallback<RenderingHandlers, const rendering::interfaces::Material*>::make_shared(
            handlers, &RenderingHandlers::HandleMaterialBinding
        );
    rendering_module.on_bind_material += materialBindingHandler;

    auto materialUnbindingHandler
        = event::MemberCallback<RenderingHandlers, const rendering::interfaces::Material*>::make_shared(
            handlers, &RenderingHandlers::HandleMaterialUnbinding
        );
    rendering_module.on_unbind_material += materialUnbindingHandler;
    // End Material ---

    // --- Texture ---
    auto textureBindingHandler
        = event::MemberCallback<RenderingHandlers, const rendering::interfaces::Texture*>::make_shared(
            handlers, &RenderingHandlers::HandleTextureBinding
        );
    rendering_module.on_bind_texture += textureBindingHandler;

    auto textureUnbindingHandler
        = event::MemberCallback<RenderingHandlers, const rendering::interfaces::Texture*>::make_shared(
            handlers, &RenderingHandlers::HandleTextureUnbinding
        );
    rendering_module.on_unbind_texture += textureUnbindingHandler;
    // End Texture ---

    auto rendererRegistingHandler
        = event::MemberCallback<RenderingHandlers, NodecObject::Reference<rendering::interfaces::Renderer>>::make_shared(
            handlers, &RenderingHandlers::HandleRendererRegisting
        );
    rendering_module.on_regist_renderer += rendererRegistingHandler;
}

RenderingHandlers::RenderingHandlers(
    Graphics* graphics, 
    GraphicsResources* graphicsResources,
    GraphicsRenderer* graphicsRenderer):
    graphics(graphics),
    graphicsResources(graphicsResources),
    graphicsRenderer(graphicsRenderer)
{
    
}

// --- Mesh ---
void RenderingHandlers::HandleMeshBinding(const nodec_modules::rendering::interfaces::Mesh* mesh)
{
    RenderingUtils::BindMesh(mesh, graphics, graphicsResources);
}

void RenderingHandlers::HandleMeshUnbinding(const nodec_modules::rendering::interfaces::Mesh* mesh)
{
    RenderingUtils::UnbindMesh(mesh, graphicsResources);
}
// End Mesh ---

// --- Shader ---
void RenderingHandlers::HandleShaderBinding(const nodec_modules::rendering::interfaces::Shader* shader)
{
    RenderingUtils::BindShader(shader, graphics, graphicsResources);
}

void RenderingHandlers::HandleShaderUnbinding(const nodec_modules::rendering::interfaces::Shader* shader)
{
    RenderingUtils::UnbindShader(shader, graphicsResources);
}
// End Shader ---

// --- Material ---
void RenderingHandlers::HandleMaterialBinding(const nodec_modules::rendering::interfaces::Material* material)
{
    RenderingUtils::BindMaterial(material, graphics, graphicsResources);
}

void RenderingHandlers::HandleMaterialUnbinding(const nodec_modules::rendering::interfaces::Material* material)
{
    RenderingUtils::UnbindMaterial(material, graphicsResources);
}
// End Material ---

// --- Texture ---
void RenderingHandlers::HandleTextureBinding(const nodec_modules::rendering::interfaces::Texture* texture)
{
    RenderingUtils::BindTexture(texture, graphics, graphicsResources);
}
void RenderingHandlers::HandleTextureUnbinding(const nodec_modules::rendering::interfaces::Texture* texture)
{
    RenderingUtils::UnbindTexture(texture, graphicsResources);
}
// End Texture ---

void RenderingHandlers::HandleRendererRegisting(
    nodec::NodecObject::Reference<nodec_modules::rendering::interfaces::Renderer> renderer)
{
    if (auto renderer_locked = renderer.lock())
    {
        graphicsRenderer->renderers.emplace(renderer_locked->id(), renderer);
    }
}