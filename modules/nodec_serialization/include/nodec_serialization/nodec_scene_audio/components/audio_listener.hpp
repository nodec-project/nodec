#ifndef NODEC_SERIALIZATION__NODEC_SCENE_AUDIO__COMPONENTS__AUDIO_LISTENER_HPP_
#define NODEC_SERIALIZATION__NODEC_SCENE_AUDIO__COMPONENTS__AUDIO_LISTENER_HPP_

#include <nodec_scene_audio/components/audio_listener.hpp>
#include <nodec_scene_serialization/serializable_component.hpp>

namespace nodec_scene_audio {
namespace components {

// sceneファイルへの書き込みを行う際に必要なクラス
class SerializableAudioListener : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializableAudioListener()
        : BaseSerializableComponent{this} {}

    SerializableAudioListener(const AudioListener &other)
        : BaseSerializableComponent(this) {}
    
    operator AudioListener() const noexcept {
        AudioListener value;
        return value;
    }

    template<class Archive>
    void save(Archive &archive) const {
        using namespace nodec_scene_serialization;
        auto &context = cereal::get_user_data<ArchiveContext>(archive);

        // TODO: save property
    }

    template<class Archive>
    void load(Archive &archive) {
        using namespace nodec_scene_serialization;
        using namespace nodec::resource_management;
        auto &context = cereal::get_user_data<ArchiveContext>(archive);

        // TODO: load property
    }
};

} // namespace components
} // namespace nodec_scene_audio

NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(nodec_scene_audio::components::SerializableAudioListener)

#endif