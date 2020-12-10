#ifndef NODEC__SCENE_SET__COMPONENT_HPP_
#define NODEC__SCENE_SET__COMPONENT_HPP_


#include <nodec/nodec_object.hpp>

namespace nodec
{
namespace scene_set
{

// Prevent include circular reference
// Do not like this
//  #include <nodec/scene_set/scene_object.hpp>
class SceneObject;

class Component : public NodecObject
{
public:
    Component(SceneObject* owner);

    virtual ~Component();

    virtual void on_awake();

public:
    SceneObject& scene_object() const noexcept;

private:
    SceneObject* p_scene_object;
};

}
}

#endif