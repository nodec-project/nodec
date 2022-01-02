#ifndef SERIALIZATION__RENDERING__COMPONENTS__CAMERA_HPP_
#define SERIALIZATION__RENDERING__COMPONENTS__CAMERA_HPP_

#include <scene_serialization/scene_serialization.hpp>

#include <cereal/types/polymorphic.hpp>
#include <cereal/types/vector.hpp>

namespace rendering {
namespace components {

class SerializableCamera : public scene_serialization::BaseSerializableComponent {
public:
    SerializableCamera()
        : BaseSerializableComponent(this) {

    }
    
    float farClipPlane{ 100.0f };
    float nearClipPlane{ 0.01f };
    float fovAngle{ 45.0f };


    template<class Archive>
    void serialize(Archive& archive) {

        archive(cereal::make_nvp("farClipPlane", farClipPlane));
        archive(cereal::make_nvp("nearClipPlane", nearClipPlane));
        archive(cereal::make_nvp("fovAngle", fovAngle));

    }

};

}
}

#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>

CEREAL_REGISTER_TYPE(rendering::components::SerializableCamera);
CEREAL_REGISTER_POLYMORPHIC_RELATION(scene_serialization::BaseSerializableComponent, rendering::components::SerializableCamera);


#endif