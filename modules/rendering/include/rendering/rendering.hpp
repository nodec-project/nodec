#ifndef NODEC_MODULES__RENDERING__INTERFACES__RENDERING_HPP_
#define NODEC_MODULES__RENDERING__INTERFACES__RENDERING_HPP_

#include <nodec_modules/rendering/interfaces/mesh.hpp>
#include <nodec_modules/rendering/interfaces/shader.hpp>
#include <nodec_modules/rendering/interfaces/renderer.hpp>
#include <nodec_modules/rendering/interfaces/material.hpp>
#include <nodec_modules/rendering/interfaces/texture.hpp>
#include <nodec_modules/rendering/interfaces/camera.hpp>

#include <nodec/module_interface.hpp>
#include <nodec/event.hpp>


namespace nodec_modules
{
namespace rendering
{
namespace interfaces
{


class Rendering : public nodec::ModuleInterface
{
public:
    using ModuleInterface::ModuleInterface;

public:
    virtual float frame_delta_time() const noexcept = 0;

    virtual void bind_mesh(const Mesh* mesh) = 0;
    virtual void unbind_mesh(const Mesh* mesh) = 0;

    virtual void bind_shader(const Shader* shader) = 0;
    virtual void unbind_shader(const Shader* shader) = 0;

    virtual void bind_material(const Material* material) = 0;
    virtual void unbind_material(const Material* material) = 0;

    virtual void bind_texture(const Texture* texture) = 0;
    virtual void unbind_texture(const Texture* texture) = 0;

    virtual void register_renderer(nodec::NodecObject::Reference<Renderer> renderer) = 0;

    virtual void register_camera(nodec::NodecObject::Reference<Camera> camera) = 0;

public:
    nodec::event::Event<Rendering&> on_frame_update;
};
}
}
}
#endif