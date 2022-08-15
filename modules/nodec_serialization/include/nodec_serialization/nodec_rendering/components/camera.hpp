#ifndef NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__CAMERA_HPP_
#define NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__CAMERA_HPP_

#include <scene_serialization/scene_serialization.hpp>

#include <cereal/types/polymorphic.hpp>
#include <cereal/types/vector.hpp>

namespace nodec_rendering {
namespace components {

class SerializableCamera : public scene_serialization::BaseSerializableComponent {
public:
    SerializableCamera()
        : BaseSerializableComponent(this) {
    }

    float farClipPlane{100.0f};
    float nearClipPlane{0.01f};
    float fovAngle{45.0f};

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("farClipPlane", farClipPlane));
        archive(cereal::make_nvp("nearClipPlane", nearClipPlane));
        archive(cereal::make_nvp("fovAngle", fovAngle));
    }
};

} // namespace components
} // namespace nodec_rendering

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/xml.hpp>

CEREAL_REGISTER_TYPE(nodec_rendering::components::SerializableCamera);
CEREAL_REGISTER_POLYMORPHIC_RELATION(scene_serialization::BaseSerializableComponent, nodec_rendering::components::SerializableCamera);

#endif