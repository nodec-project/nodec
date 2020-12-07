#ifndef NODEC__SCENE_SET__TRANSFORM_HPP_
#define NODEC__SCENE_SET__TRANSFORM_HPP_

#include <nodec/scene_set/component.hpp>
#include <nodec/vector3.hpp>
#include <nodec/quaternion.hpp>

namespace nodec
{
    namespace scene_set
    {

        class Transform : public Component
        {
        public:
            Vector3f local_position;
            Vector3f local_scale;
            Quaternionf local_rotation;
            
        public:
            using Component::Component;

        public:
            Vector3f get_world_position();
            Vector3f get_world_scale();
            Quaternionf get_world_rotation();

            void set_world_position(const Vector3f& position);
            void set_world_scale(const Vector3f& scale);
            void set_world_rotation(const Quaternionf& rotation);

        };
    }
}
#endif