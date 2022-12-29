#ifndef NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__NON_VISIBLE_HPP_
#define NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__NON_VISIBLE_HPP_

#include <nodec_rendering/components/non_visible.hpp>
#include <nodec_scene_serialization/base_serializable_component.hpp>

namespace nodec_rendering {
namespace components {

class SerializableNonVisible : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializableNonVisible()
        : BaseSerializableComponent(this) {}

    bool self{false};

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("self", self));
    }
};
} // namespace components
} // namespace nodec_rendering

CEREAL_REGISTER_TYPE(nodec_rendering::components::SerializableNonVisible)
CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec_scene_serialization::BaseSerializableComponent, nodec_rendering::components::SerializableNonVisible)

#endif