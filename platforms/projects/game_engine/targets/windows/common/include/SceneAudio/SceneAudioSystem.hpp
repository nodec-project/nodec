#pragma once

#include "AudioSourceActivity.hpp"

#include <scene_set/scene_registry.hpp>
#include <scene_set/components/basic.hpp>
#include <scene_audio/components/audio_source.hpp>

class SceneAudioSystem {
    using SceneRegistry = scene_set::SceneRegistry;
    using SceneEntity = scene_set::SceneEntity;


public:
    SceneAudioSystem(AudioPlatform* audioPlatform, SceneRegistry* pSceneRegistry)
        : mpAudioPlatform(audioPlatform)
    {
        using namespace scene_audio::components;
        pSceneRegistry->component_constructed<AudioSource>().connect(
            [&](auto& registry, auto entity) {
                OnAudioSourceCreated(registry, entity);
            }
        );

    }

public:
    void UpdateAudio(SceneRegistry& registry) {
        using namespace nodec;
        using namespace scene_audio::components;
        using namespace scene_set::components;
        using namespace Exceptions;

        for (auto entt : registry.view<AudioSource, Transform, AudioSourceActivity>()) {
            auto& source = registry.get_component<AudioSource>(entt);
            auto& trfm = registry.get_component<Transform>(entt);
            auto& activity = registry.get_component<AudioSourceActivity>(entt);

            try {

                switch (activity.state) {
                case AudioSourceActivity::State::Inactive:
                    activity.pClip = std::static_pointer_cast<AudioClipBackend>(source.clip);
                    if (activity.pClip) {
                        if (!activity.pVoice || activity.pVoice->GetWfx() != activity.pClip->wfx()) {
                            activity.pVoice.reset(new SourceVoice(mpAudioPlatform, activity.pClip->wfx()));
                        }

                        XAUDIO2_BUFFER buffer = {};
                        buffer.pAudioData = activity.pClip->samples().data();
                        buffer.Flags = XAUDIO2_END_OF_STREAM;
                        buffer.AudioBytes = activity.pClip->samples().size();

                        ThrowIfFailed(activity.pVoice->GetVoice().SubmitSourceBuffer(&buffer), __FILE__, __LINE__);

                        if (source.is_playing) {
                            // -> Playing

                            ThrowIfFailed(activity.pVoice->GetVoice().Start(), __FILE__, __LINE__);

                            activity.state = AudioSourceActivity::State::Playing;
                        }
                        else {
                            // -> Stopped
                            activity.state = AudioSourceActivity::State::Stopped;
                        }
                    }
                    break;

                case AudioSourceActivity::State::Playing:

                    break;

                case AudioSourceActivity::State::Stopped:
                    break;
                }
            }
            catch (...) {
                [&]() {
                    logging::ErrorStream error(__FILE__, __LINE__);
                    error << "[SceneAudioSystem::UpdateAudio] >>> Audio Error. \n"
                        "entity: " << std::hex << "0x" << entt << std::dec << "\n"
                        "details:\n";
                    try {
                        throw;
                    }
                    catch (std::exception& e) {
                        error << e.what();
                    }
                    catch (...) {
                        error << "Unknown";
                    }
                }();
            }
        }
    }

private:

    void OnAudioSourceCreated(SceneRegistry& registry, const SceneEntity entity) {
        registry.emplace_component<AudioSourceActivity>(entity);
    }

private:
    AudioPlatform* mpAudioPlatform;

};