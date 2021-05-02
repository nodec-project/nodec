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
        throw NodecException("Failed to add transform component.", __FILE__, __LINE__);
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

const std::unordered_map<NodecObject::ID, NodecObject::Holder<SceneObject>>&
SceneObject::children() const noexcept
{
    return children_;
}

const std::unordered_map<size_t, NodecObject::Holder<Component>>& 
SceneObject::components() const noexcept
{
    return components_;
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


    auto result = children_.emplace(child->id(), child);
    if (!result.second)
    {
        throw NodecException("Failed to append child.", __FILE__, __LINE__);
    }

    child->parent_ = shared_from(this);
    return child;
}

NodecObject::Holder<SceneObject>
SceneObject::remove_child(SceneObject& child)
{
    auto iter = children_.find(child.id());
    if (iter == children_.end())
    {
        throw NodecException("Not found the specified child.", __FILE__, __LINE__);
    }

    auto removed = iter->second;
    children_.erase(iter);

    child.parent_.reset();
    return removed;
}

} // namespace scene_set
} // namespace nodec