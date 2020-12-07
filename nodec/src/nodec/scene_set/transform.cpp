#include <nodec/scene_set/transform.hpp>

namespace nodec
{
    namespace scene_set
    {
        Vector3f Transform::get_world_position()
        {
            return Vector3f::zero;
        }

        Vector3f Transform::get_world_scale()
        {
            return Vector3f::zero;

        }

        Quaternionf Transform::get_world_rotation()
        {
            return Quaternionf::identity;
        }

        void Transform::set_world_position(const Vector3f& position)
        {

        }
        void Transform::set_world_scale(const Vector3f& scale)
        {

        }
        void Transform::set_world_rotation(const Quaternionf& rotation)
        {

        }
    }
}