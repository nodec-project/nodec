#ifndef NODEC_MODULES__RENDERING__RENDERING_MODULE_HPP_
#define NODEC_MODULES__RENDERING__RENDERING_MODULE_HPP_

#include "interfaces/rendering.hpp"

namespace nodec_modules
{
namespace rendering
{
class RenderingModule : public interfaces::Rendering
{
public:
    RenderingModule();

public:
    float frame_delta_time() const noexcept override;

    void bind_mesh(const interfaces::Mesh* mesh) override;
    void unbind_mesh(const interfaces::Mesh* mesh) override;

    void bind_shader(const interfaces::Shader* shader) override;
    void unbind_shader(const interfaces::Shader* shader) override;

    void bind_material(const interfaces::Material* material) override;
    void unbind_material(const interfaces::Material* material) override;

    void bind_texture(const interfaces::Texture* texture) override;
    void unbind_texture(const interfaces::Texture* texture) override;

    void regist_renderer(nodec::NodecObject::Reference<interfaces::Renderer> renderer) override;

public:
    float frame_delta_time_;

    nodec::event::Event<const interfaces::Mesh*> on_bind_mesh;
    nodec::event::Event<const interfaces::Mesh*> on_unbind_mesh;
    nodec::event::Event<const interfaces::Shader*> on_bind_shader;
    nodec::event::Event<const interfaces::Shader*> on_unbind_shader;
    nodec::event::Event<const interfaces::Material*> on_bind_material;
    nodec::event::Event<const interfaces::Material*> on_unbind_material;
    nodec::event::Event<const interfaces::Texture*> on_bind_texture;
    nodec::event::Event<const interfaces::Texture*> on_unbind_texture;

    nodec::event::Event<nodec::NodecObject::Reference<interfaces::Renderer>> on_regist_renderer;

};
}
}

#endif