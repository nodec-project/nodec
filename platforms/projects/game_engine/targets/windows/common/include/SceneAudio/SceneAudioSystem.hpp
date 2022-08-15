#pragma once

#include "AudioSourceActivity.hpp"

#include <nodec_scene_audio/components/audio_source.hpp>
#include <nodec_scene/components/basic.hpp>
#include <nodec_scene/scene_registry.hpp>

class SceneAudioSystem {
    using SceneRegistry = nodec_scene::SceneRegistry;
    using SceneEntity = nodec_scene::SceneEntity;

public:
    SceneAudioSystem(AudioPlatform *audioPlatform, SceneRegistry *pSceneRegistry)
        : mpAudioPlatform(audioPlatform) {
        using namespace nodec_scene_audio::components;
        pSceneRegistry->component_constructed<AudioSource>().connect(
            [&](auto &registry, auto entity) {
                registry.emplace_component<AudioSourceActivity>(entity);
            });
    }

public:
    void UpdateAudio(SceneRegistry &registry) {
        using namespace nodec;
        using namespace nodec_scene_audio::components;
        using namespace nodec_scene::components;
        using namespace Exceptions;

        registry.view<AudioSource, Transform, AudioSourceActivity>().each([&](auto entt, AudioSource &source, Transform &trfm, AudioSourceActivity &activity) {
            try {
                switch (activity.state) {
                case AudioSourceActivity::State::Stopped: {
                    if (!source.is_playing) break;

                    // -> Playing

                    // Casts the client clip to backend clip and the activity owns it.
                    activity.pClip = std::static_pointer_cast<AudioClipBackend>(source.clip);

                    if (!activity.pClip) break;

                    if (!activity.pVoice || activity.pVoice->GetWfx() != activity.pClip->wfx()) {
                        activity.pVoice.reset(new SourceVoice(mpAudioPlatform, activity.pClip->wfx()));
                    }

                    const auto &wfx = activity.pClip->wfx();
                    int playBegin = source.position.count() * wfx.nSamplesPerSec / 1000;
                    int totalSampleFrames = activity.pClip->samples().size() / wfx.nBlockAlign;

                    // logging::InfoStream(__FILE__, __LINE__) << "*** " << playBegin << " : " << totalSampleFrames << " , " << wfx.nSamplesPerSec
                    //                                         << ", " << wfx.wBitsPerSample;

                    if (playBegin < 0 || totalSampleFrames <= playBegin) break;

                    XAUDIO2_BUFFER buffer = {};
                    buffer.pAudioData = activity.pClip->samples().data();
                    buffer.Flags = XAUDIO2_END_OF_STREAM;
                    buffer.AudioBytes = activity.pClip->samples().size();
                    buffer.LoopCount = source.loop ? XAUDIO2_LOOP_INFINITE : 0;

                    buffer.PlayBegin = playBegin;
                    activity.playBeginTime = source.position;

                    ThrowIfFailed(activity.pVoice->GetVoice().FlushSourceBuffers(), __FILE__, __LINE__);
                    activity.pVoice->SubmitSourceBuffer(&buffer);
                    // ThrowIfFailed(activity.pVoice->GetVoice().SubmitSourceBuffer(&buffer), __FILE__, __LINE__);
                    ThrowIfFailed(activity.pVoice->GetVoice().Start(), __FILE__, __LINE__);

                    activity.state = AudioSourceActivity::State::Playing;

                } break;

                case AudioSourceActivity::State::Playing: {
                    using namespace std::chrono;

                    if (activity.pVoice->GetBufferState() == SourceVoice::BufferState::Submitting) break;

                    auto position = duration_cast<milliseconds>(activity.pVoice->GetTimeSinceBufferStart());
                    if (activity.pVoice->GetLoopCount() == 1) {
                        position += activity.playBeginTime;
                    }

                    source.position = position;

                    if (activity.pVoice->GetBufferState() == SourceVoice::BufferState::BufferEnd) {
                        // -> Stopped
                        source.is_playing = false;
                        activity.state = AudioSourceActivity::State::Stopped;
                        break;
                    }

                    if (!source.is_playing) {
                        // -> Stopped
                        ThrowIfFailed(activity.pVoice->GetVoice().Stop(), __FILE__, __LINE__);
                        activity.state = AudioSourceActivity::State::Stopped;
                        break;
                    }
                } break;
                }
            } catch (...) {
                [&]() {
                    logging::ErrorStream error(__FILE__, __LINE__);
                    error << "[SceneAudioSystem::UpdateAudio] >>> Audio Error. \n"
                             "entity: "
                          << std::hex << "0x" << entt << std::dec << "\n"
                                                                     "details:\n";
                    try {
                        throw;
                    } catch (std::exception &e) {
                        error << e.what();
                    } catch (...) {
                        error << "Unknown";
                    }
                }();
            }
        });
    }

private:
    AudioPlatform *mpAudioPlatform;
};