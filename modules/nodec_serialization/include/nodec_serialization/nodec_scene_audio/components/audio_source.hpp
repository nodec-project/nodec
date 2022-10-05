#ifndef NODEC_SERIALIZATION__NODEC_SCENE_AUDIO__COMPONENTS__AUDIO_SOURCE_HPP_
#define NODEC_SERIALIZATION__NODEC_SCENE_AUDIO__COMPONENTS__AUDIO_SOURCE_HPP_

#include <nodec_scene_serialization/serializable_component.hpp>

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
    }

    template<class Archive>
    void load(Archive &archive) {
    }
};

} // namespace components
} // namespace nodec_scene_audio

#endif