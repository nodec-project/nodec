#include <nodec/scene_set/component.hpp>

namespace nodec
{
namespace scene_set
{

Component::Component(SceneObject* owner) :
    // name will be overwritten after constructor in add_component(). see SceneObject::add_component().
    NodecObject("Unexpected Component Object. Component must be instanceated by add_component()."),
    p_scene_object(owner)
{

}

Component::~Component() {}

void Component::on_awake() {}

SceneObject& Component::scene_object() const noexcept
{
    return (*p_scene_object);
}

} // namespace scene_set
}