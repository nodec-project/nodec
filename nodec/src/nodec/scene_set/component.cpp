#include <nodec/scene_set/component.hpp>
#include <nodec/scene_set/scene_object.hpp>

#include <sstream>

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

void Component::attach_to(SceneObject* scene_object)
{
    if (p_scene_object != nullptr)
    {
        std::ostringstream oss;
        oss << "This component has already been attached to a scene object.\n"
            << "component.name   : " << name << "\n"
            << "scene_object.name: " << p_scene_object->name;
        throw NodecException(oss.str(), __FILE__, __LINE__);
    }
    p_scene_object = scene_object;

    try
    {
        scene_object->add_component(shared_from(this));
    }
    catch (...)
    {
        p_scene_object = nullptr;
    }
}

bool Component::has_owner() const noexcept
{
    return p_scene_object != nullptr;
}

SceneObject& Component::scene_object() const
{
    if (p_scene_object == nullptr)
    {
        std::ostringstream oss;

        oss << "This component has not been attached to any SceneObject. ";
        oss << "component.name: ";
        oss << name;
        throw NodecException(oss.str(), __FILE__, __LINE__);
    }
    return (*p_scene_object);
}

} // namespace scene_set
} // namespace nodec