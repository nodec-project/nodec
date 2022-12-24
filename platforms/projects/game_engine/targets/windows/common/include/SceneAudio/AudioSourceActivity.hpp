#pragma once

#include "AudioClipBackend.hpp"

#include <Audio/SouceVoice.hpp>

#include <chrono>
#include <memory>

/**
 * ECS Component
 */
struct AudioSourceActivity {
    enum class State {
        Stopped,
        Playing
    };

    State state{State::Stopped};
    std::shared_ptr<AudioClipBackend> pClip;
    std::unique_ptr<SourceVoice> pVoice;
    std::chrono::milliseconds playBeginTime;
};