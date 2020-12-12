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
            /**
            * @param [out] out_position
            */
            NodecObject::Reference<SceneObject>
                get_world_transform(Vector3f& out_position, Quaternionf& out_rotation, Vector3f& out_scale);

            NodecObject::Reference<SceneObject>
                set_world_transform(const Vector3f& position, const Quaternionf& rotation, const Vector3f& scale);


        };
    }
}
#endif