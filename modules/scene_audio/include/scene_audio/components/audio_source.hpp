#ifndef SCENE_AUDIO__COMPONENTS__AUDIO_SOURCE_HPP_
#define SCENE_AUDIO__COMPONENTS__AUDIO_SOURCE_HPP_


namespace scene_audio {
namespace components {

class AudioSource {
public:
    virtual ~AudioSource() {}

public:
    virtual void start() = 0;
    virtual void stop() = 0;

};

}
}

#endif
