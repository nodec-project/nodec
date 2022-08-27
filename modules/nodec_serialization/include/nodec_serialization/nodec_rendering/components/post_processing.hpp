#ifndef NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__POST_PROCESSING_HPP_
#define NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__POST_PROCESSING_HPP_

#include <nodec_rendering/components/post_processing.hpp>
#include <nodec_scene_serialization/scene_serialization.hpp>

#include <cereal/types/polymorphic.hpp>
#include <cereal/types/vector.hpp>

namespace nodec_rendering {
namespace components {

class SerializablePostProcessing : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializablePostProcessing()
        : BaseSerializableComponent(this) {
    }

    struct Effect {
        bool enabled;
        std::string material;

        template<class Archive>
        void serialize(Archive &archive) {
            archive(cereal::make_nvp("enabled", enabled));
            archive(cereal::make_nvp("material", material));
        }
    };
    
    std::vector<Effect> effects;

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("effects", effects));
    }
};

} // namespace components
} // namespace nodec_rendering

CEREAL_REGISTER_TYPE(nodec_rendering::components::SerializablePostProcessing);
CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec_scene_serialization::BaseSerializableComponent, nodec_rendering::components::SerializablePostProcessing);

#endif