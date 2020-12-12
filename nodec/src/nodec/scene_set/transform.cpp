#include <nodec/scene_set/transform.hpp>
#include <nodec/scene_set/scene_object.hpp>


namespace nodec
{
namespace scene_set
{

NodecObject::Reference<SceneObject>
Transform::get_world_transform(Vector3f& out_position, Quaternionf& out_rotation, Vector3f& out_scale)
{
    out_position = local_position;
    out_rotation = local_rotation;
    out_scale = local_scale;

    NodecObject::Reference<SceneObject> root;
    auto parent = scene_object().parent().lock();
    while (parent)
    {
        out_position += parent->transform().local_position;
        out_scale += parent->transform().local_scale;
        out_rotation = out_rotation * parent->transform().local_rotation;
        root = parent;
        parent = parent->parent().lock();
    }

    return root;
}

NodecObject::Reference<SceneObject>
Transform::set_world_transform(const Vector3f& position, const Quaternionf& rotation, const Vector3f& scale)
{
    NodecObject::Reference<SceneObject> root;

    return root;
}

}
}