#ifndef NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__NON_VISIBLE_HPP_
#define NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__NON_VISIBLE_HPP_

#include <nodec_rendering/components/non_visible.hpp>
#include <nodec_scene_serialization/serializable_component.hpp>

namespace nodec_rendering {
namespace components {

class SerializableNonVisible : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializableNonVisible()
        : BaseSerializableComponent(this) {}

    SerializableNonVisible(const NonVisible &other)
        : BaseSerializableComponent(this),
          self(other.self) {}

    operator NonVisible() const noexcept {
        NonVisible value;
        value.self = self;
        return value;
    }

    bool self{false};

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("self", self));
    }
};
} // namespace components
} // namespace nodec_rendering

NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(nodec_rendering::components::SerializableNonVisible)

#endif