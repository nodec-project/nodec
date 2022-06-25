#pragma once

#include "AudioClipBackend.hpp"

#include <Audio/SouceVoice.hpp>

#include <memory>


/**
* ECS Component
*/
class AudioSourceActivity {
public:

    bool loop{ false };

    enum class State {
        Inactive,
        Stopped,
        Playing
    };

    State state{ State::Inactive };
    std::shared_ptr<AudioClipBackend> pClip;
    std::unique_ptr<SourceVoice> pVoice;

};