#pragma once

#include <Audio/SouceVoice.hpp>

#include <memory>


/**
* ECS Component
*/
class AudioSourceActivity {
public:

    std::unique_ptr<SourceVoice> mpSourceVoice;

};