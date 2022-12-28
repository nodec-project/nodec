#ifndef NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__MAIN_CAMERA_HPP_
#define NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__MAIN_CAMERA_HPP_

#include <nodec_rendering/components/main_camera.hpp>
#include <nodec_scene_serialization/base_serializable_component.hpp>

namespace nodec_rendering {
namespace components {

class SerializableMainCamera : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializableMainCamera()
        : BaseSerializableComponent(this) {
    }

    template<class Archive>
    void serialize(Archive &archive) {
    }
};

} // namespace components
} // namespace nodec_rendering

CEREAL_REGISTER_TYPE(nodec_rendering::components::SerializableMainCamera)
CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec_scene_serialization::BaseSerializableComponent, nodec_rendering::components::SerializableMainCamera)

#endif