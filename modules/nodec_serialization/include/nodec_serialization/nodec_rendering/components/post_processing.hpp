#ifndef NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__POST_PROCESSING_HPP_
#define NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__POST_PROCESSING_HPP_

#include <nodec_rendering/components/post_processing.hpp>
#include <nodec_scene_serialization/serializable_component.hpp>

#include <cereal/types/vector.hpp>

namespace nodec_rendering {
namespace components {

class SerializablePostProcessing : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializablePostProcessing()
        : BaseSerializableComponent(this) {
    }

    SerializablePostProcessing(const PostProcessing &other)
        : BaseSerializableComponent(this),
          effects(other.effects) {}

    operator PostProcessing() const noexcept {
        PostProcessing value;
        value.effects = effects;
        return value;
    }

    std::vector<PostProcessing::Effect> effects;

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("effects", effects));
    }
};

template<class Archive>
void save(Archive &archive, const PostProcessing::Effect &effect) {
    using namespace nodec_scene_serialization;
    ArchiveContext &context = cereal::get_user_data<ArchiveContext>(archive);

    archive(cereal::make_nvp("enabled", effect.enabled));
    archive(cereal::make_nvp("material", context.resource_registry().lookup_name<resources::Material>(effect.material).first));
}

template<class Archive>
void load(Archive &archive, PostProcessing::Effect &effect) {
    using namespace nodec_scene_serialization;
    ArchiveContext &context = cereal::get_user_data<ArchiveContext>(archive);
    archive(cereal::make_nvp("enabled", effect.enabled));

    std::string name;
    archive(cereal::make_nvp("material", name));
    effect.material = context.resource_registry().get_resource_direct<resources::Material>(name);
}

} // namespace components
} // namespace nodec_rendering

NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(nodec_rendering::components::SerializablePostProcessing)

#endif