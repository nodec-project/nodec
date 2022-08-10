#pragma once

#include "AudioClipBackend.hpp"

#include <Audio/SouceVoice.hpp>

#include <memory>
#include <chrono>


/**
 * ECS Component
 */
class AudioSourceActivity {
public:

    enum class State {
        Stopped,
        Playing
    };

    State state{State::Stopped};
    std::shared_ptr<AudioClipBackend> pClip;
    std::unique_ptr<SourceVoice> pVoice;
    std::chrono::milliseconds playBeginTime;


};