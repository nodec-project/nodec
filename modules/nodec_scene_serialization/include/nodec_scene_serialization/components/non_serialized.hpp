#ifndef NODEC_SCENE_SERIALIZATION__COMPONENTS__NON_SERIALIZED_HPP_
#define NODEC_SCENE_SERIALIZATION__COMPONENTS__NON_SERIALIZED_HPP_

#include "../base_serializable_component.hpp"

namespace nodec_scene_serialization {
namespace components {

class NonSerialized : public BaseSerializableComponent {
public:
    NonSerialized()
        : BaseSerializableComponent(this) {}

    template<class Archive>
    void serialize(Archive &archive) {}
};

} // namespace components
} // namespace nodec_scene_serialization

CEREAL_REGISTER_TYPE(nodec_scene_serialization::components::NonSerialized)
CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec_scene_serialization::BaseSerializableComponent, nodec_scene_serialization::components::NonSerialized)

#endif