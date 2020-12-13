#include <nodec/scene_set/scene_object.hpp>
#include <nodec/scene_set/transform.hpp>

#include <nodec/nodec_exception.hpp>

namespace nodec
{
namespace scene_set
{

SceneObject::SceneObject(const std::string& name) :
    NodecObject(name),
    p_transform(nullptr)
{
    auto tf_ref = add_component<Transform>();
    if (auto tf = tf_ref.lock())
    {
        p_transform = tf.get();
    }
    else
    {
        throw NodecException("Failed to regist transform component.", __FILE__, __LINE__);
    }
}

Transform&
SceneObject::transform() const noexcept
{
    return (*p_transform);
}


NodecObject::Reference<SceneObject>
SceneObject::parent()
{
    return parent_;
}

NodecObject::Reference<SceneObject>
SceneObject::append_child(NodecObject::Holder<SceneObject> child)
{
    if (!child)
    {
        throw NodecException("Holder doesn't have instanciated SceneObject.", __FILE__, __LINE__);
    }

    if (auto parent = child->parent_.lock())
    {
        // child has parent.
        throw NodecException("This child already has a parent.", __FILE__, __LINE__);
    }


    auto result = children.emplace(child->id(), child);
    if (!result.second)
    {
        throw NodecException("Cannot append child.", __FILE__, __LINE__);
    }

    child->parent_ = shared_from(this);
    return child;
}

NodecObject::Holder<SceneObject>
SceneObject::remove_child(SceneObject& child)
{
    auto iter = children.find(child.id());
    if (iter == children.end())
    {
        throw NodecException("Not found the specified child.", __FILE__, __LINE__);
    }

    auto removed = iter->second;
    children.erase(iter);
    //return NodecObject::instanciate<SceneObject>("dummy");

    child.parent_.reset();
    return removed;
    //return iter->second;
}

}
}