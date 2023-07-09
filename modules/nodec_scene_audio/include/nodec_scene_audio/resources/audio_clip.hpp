#ifndef NODEC_SCENE_AUDIO__RESOURCES__AUDIO_CLIP_HPP_
#define NODEC_SCENE_AUDIO__RESOURCES__AUDIO_CLIP_HPP_

namespace nodec_scene_audio {
namespace resources {

class AudioClip {
public:
    virtual ~AudioClip() {}

    // TODO: ステレオ/モノラル設定？に関するパラメータを持たせる
    int channelCount{2};
};

} // namespace resources
} // namespace nodec_scene_audio

#endif
