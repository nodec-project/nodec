#ifndef NODEC_SERIALIZATION__NODEC_SCENE_AUDIO__COMPONENTS__AUDIO_SOURCE_HPP_
#define NODEC_SERIALIZATION__NODEC_SCENE_AUDIO__COMPONENTS__AUDIO_SOURCE_HPP_

#include <nodec_scene_audio/resources/audio_clip.hpp>
#include <nodec_scene_serialization/base_serializable_component.hpp>

namespace nodec_scene_audio {
namespace components {

class SerializableAudioSource : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializableAudioSource()
        : BaseSerializableComponent{this} {}

    std::shared_ptr<resources::AudioClip> clip;

    bool is_playing{false};
    bool loop{false};
    float volume{1.0f};
    std::chrono::milliseconds position{0};

    template<class Archive>
    void save(Archive &archive) const {
        using namespace nodec_scene_serialization;
        auto &context = cereal::get_user_data<ArchiveContext>(archive);

        archive(cereal::make_nvp("clip", context.resource_registry().lookup_name<resources::AudioClip>(clip).first));
        archive(cereal::make_nvp("is_playing", is_playing));
        archive(cereal::make_nvp("loop", loop));
    }

    template<class Archive>
    void load(Archive &archive) {
        using namespace nodec_scene_serialization;
        using namespace nodec::resource_management;
        auto &context = cereal::get_user_data<ArchiveContext>(archive);

        {
            std::string name;
            archive(cereal::make_nvp("clip", name));
            clip = context.resource_registry().get_resource_direct<resources::AudioClip>(name);
        }
        archive(cereal::make_nvp("is_playing", is_playing));
        archive(cereal::make_nvp("loop", loop));
    }
};

} // namespace components
} // namespace nodec_scene_audio

CEREAL_REGISTER_TYPE(nodec_scene_audio::components::SerializableAudioSource)

CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec_scene_serialization::BaseSerializableComponent, nodec_scene_audio::components::SerializableAudioSource)

#endif