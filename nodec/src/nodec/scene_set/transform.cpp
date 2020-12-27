#include <nodec/scene_set/transform.hpp>
#include <nodec/scene_set/scene_object.hpp>
#include <nodec/math/gfx.hpp>


namespace nodec
{
namespace scene_set
{


Transform::Transform(SceneObject* owner) :
    Component(owner),
    local_position(0.0f, 0.0f, 0.0f),
    local_rotation(0.0f, 0.0f, 0.0f, 1.0f),
    local_scale(1.0f, 1.0f, 1.0f)
{

}


NodecObject::Reference<SceneObject>
Transform::get_world_transform(Vector3f& world_position, Quaternionf& world_rotation, Vector3f& world_scale)
{
    world_position = local_position;
    world_rotation = local_rotation;
    world_scale = local_scale;

    NodecObject::Reference<SceneObject> root;
    auto parent = scene_object().parent().lock();
    while (parent)
    {
        Transform& parent_transform = parent->transform();
        world_position *= parent_transform.local_scale;
        world_position = nodec::math::gfx::transform(world_position, parent_transform.local_rotation);
        world_position += parent_transform.local_position;

        world_scale *= parent_transform.local_scale;
        world_rotation = parent_transform.local_rotation * world_rotation;
        root = parent;
        parent = parent->parent().lock();
    }

    return root;
}


}
}