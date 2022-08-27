#ifndef NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__POINT_LIGHT_HPP_
#define NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__POINT_LIGHT_HPP_

#include <nodec_rendering/components/directional_light.hpp>
#include <nodec_scene_serialization/scene_serialization.hpp>

#include <cereal/types/polymorphic.hpp>
#include <cereal/types/vector.hpp>

namespace nodec_rendering {
namespace components {

class SerializablePointLight : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializablePointLight()
        : BaseSerializableComponent(this) {
    }

    nodec::Vector4f color{1.0f, 1.0f, 1.0f, 1.0f};
    float intensity{1.0f};
    float range{10.0f};

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("color", color));
        archive(cereal::make_nvp("intensity", intensity));
        archive(cereal::make_nvp("range", range));
    }
};

} // namespace components
} // namespace nodec_rendering

CEREAL_REGISTER_TYPE(nodec_rendering::components::SerializablePointLight);
CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec_scene_serialization::BaseSerializableComponent, nodec_rendering::components::SerializablePointLight);

#endif