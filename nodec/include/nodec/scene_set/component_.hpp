#ifndef NODEC__SCENE_SET__COMPONENT_HPP_
#define NODEC__SCENE_SET__COMPONENT_HPP_

#include <nodec/nodec_exception.hpp>
#include <nodec/nodec_object.hpp>

namespace nodec
{
namespace scene_set
{

class SceneObject;

class Component : public NodecObject
{
public:
    Component(SceneObject* owner);

    virtual ~Component();

    virtual void on_awake();

public:
    void attach_to(SceneObject* scene_object);
    bool has_owner() const noexcept;
    SceneObject& scene_object() const;

private:
    /**
    * @brief pointer to scene object (owner). 
    */
    SceneObject* p_scene_object;
};

}
}

#endif