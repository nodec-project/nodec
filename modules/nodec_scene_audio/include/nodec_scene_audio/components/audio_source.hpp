#ifndef NODEC_SCENE_AUDIO__COMPONENTS__AUDIO_SOURCE_HPP_
#define NODEC_SCENE_AUDIO__COMPONENTS__AUDIO_SOURCE_HPP_

#include "../resources/audio_clip.hpp"

#include <memory>
#include <chrono>

namespace nodec_scene_audio {
namespace components {

struct AudioSource {
    using AudioClipPtr = std::shared_ptr<resources::AudioClip>;

    AudioClipPtr clip;

    bool is_playing{false};

    bool loop{false};

    float volume{1.0f};

    std::chrono::milliseconds position{0};
};

} // namespace components
} // namespace scene_audio

#endif
