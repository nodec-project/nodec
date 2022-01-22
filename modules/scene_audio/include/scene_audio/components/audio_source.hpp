#ifndef SCENE_AUDIO__COMPONENTS__AUDIO_SOURCE_HPP_
#define SCENE_AUDIO__COMPONENTS__AUDIO_SOURCE_HPP_

#include "../resources/audio_clip.hpp"

#include <memory>


namespace scene_audio {
namespace components {

class AudioSource {

public:
    using AudioClipPtr = std::shared_ptr<resources::AudioClip>;

    AudioClipPtr clip;

    bool is_playing{ false };

    bool loop{ false };

    float volume{ 1.0f };

    int position{ 0 };
};

}
}

#endif
