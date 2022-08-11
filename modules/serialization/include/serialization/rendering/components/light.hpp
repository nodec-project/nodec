#ifndef SERIALIZATION__NODEC_RENDERING__COMPONENTS__LIGHT_HPP_
#define SERIALIZATION__NODEC_RENDERING__COMPONENTS__LIGHT_HPP_

#include <nodec_rendering/components/light.hpp>

#include <scene_serialization/scene_serialization.hpp>

#include <cereal/types/polymorphic.hpp>
#include <cereal/types/vector.hpp>

namespace nodec_rendering {
namespace components {

class SerializableLight : public scene_serialization::BaseSerializableComponent {
public:
    SerializableLight()
        : BaseSerializableComponent(this) {
    }

    LightType type{LightType::Directional};
    nodec::Vector4f color{1.0f, 1.0f, 1.0f, 1.0f};
    float intensity{1.0f};

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("type", type));
        archive(cereal::make_nvp("color", color));
        archive(cereal::make_nvp("intensity", intensity));
    }
};

} // namespace components
} // namespace nodec_rendering

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/xml.hpp>

CEREAL_REGISTER_TYPE(nodec_rendering::components::SerializableLight);
CEREAL_REGISTER_POLYMORPHIC_RELATION(scene_serialization::BaseSerializableComponent, nodec_rendering::components::SerializableLight);

#endif