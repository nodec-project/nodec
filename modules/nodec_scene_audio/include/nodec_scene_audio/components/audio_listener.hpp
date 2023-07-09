#ifndef NODEC_SCENE_AUDIO__COMPONENTS__AUDIO_LISTENER_HPP_
#define NODEC_SCENE_AUDIO__COMPONENTS__AUDIO_LISTENER_HPP_

namespace nodec_scene_audio {
namespace components {

struct AudioListener {
    // TODO: Emitter構造体をprivateなコンポーネントとして作って持たせる
    // AudioSourceに対するAudioSourceActivity的な奴をつくる
    // NOTE: Cone用パラメータ,このパラメータ群自体をprivateコンポーネント化した方が良さそう
    float InnerAngle; 
    float OuterAngle; 
    float InnerVolume;
    float OuterVolume;
    float InnerLPF;   
    float OuterLPF;   
    float InnerReverb;
    float OuterReverb;
};

} // namespace components
} // namespace scene_audio

#endif
