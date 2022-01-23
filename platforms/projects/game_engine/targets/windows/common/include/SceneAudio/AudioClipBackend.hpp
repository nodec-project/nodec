#pragma once

#include <Audio/AudioIntegration.hpp>
#include <Exceptions.hpp>

#include <scene_audio/resources/audio_clip.hpp>

#include <nodec/riff.hpp>
#include <nodec/formatter.hpp>

#include <fstream>


class AudioClipBackend : public scene_audio::resources::AudioClip {

public:
    AudioClipBackend(const std::string& path) {
        using namespace nodec::riff;
        using namespace nodec;

        std::ifstream audio_file;
        audio_file.open(path, std::ios::binary);
        if (!audio_file) {
            throw std::runtime_error(ErrorFormatter<std::runtime_error>(__FILE__, __LINE__)
                << "Open file failed. path: " << path
            );
        }


    }


private:
    WAVEFORMATEX wfx{ 0x00 };

};
