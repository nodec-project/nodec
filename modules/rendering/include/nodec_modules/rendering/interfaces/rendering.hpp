#ifndef NODEC_MODULES__RENDERING__INTERFACES__RENDERING_HPP_
#define NODEC_MODULES__RENDERING__INTERFACES__RENDERING_HPP_


#include <nodec/module_interface.hpp>
#include <nodec/event.hpp>


namespace nodec_modules
{
namespace rendering
{
namespace interfaces
{

class Mesh;
class Shader;
class Renderer;
class Material;

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

    virtual void regist_renderer(nodec::NodecObject::Reference<Renderer> renderer) = 0;

public:
    nodec::event::Event<Rendering&> on_frame_update;
};
}
}
}
#endif