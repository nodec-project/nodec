#pragma once

#include <Audio/AudioIntegration.hpp>
#include <Exceptions.hpp>

#include <scene_audio/resources/audio_clip.hpp>

#include <nodec/riff.hpp>
#include <nodec/audio/wave_format.hpp>
#include <nodec/formatter.hpp>

#include <fstream>


class AudioClipBackend : public scene_audio::resources::AudioClip {

public:
    AudioClipBackend(const std::string& path) {
        using namespace nodec::riff;
        using namespace nodec::audio::wave_format;
        using namespace nodec;

        std::ifstream audio_file;
        audio_file.open(path, std::ios::binary);
        if (!audio_file) {
            throw std::runtime_error(ErrorFormatter<std::runtime_error>(__FILE__, __LINE__)
                << "Open file failed. path: " << path
            );
        }


        RIFFChunk riff_chunk; bool found;
        RIFFChunk chunk;

        std::tie(riff_chunk, found) = find_riff_chunk(FOURCC_RIFF_TAG, audio_file);
        if (!found) {
            throw std::runtime_error(ErrorFormatter<std::runtime_error>(__FILE__, __LINE__)
                << "Not RIFF file. path: " << path
            );
        }

        riff::FOURCC id;
        audio_file.read(reinterpret_cast<char*>(&id), sizeof(riff::FOURCC));
        if (id != FOURCC_WAVE_FILE_TAG) {
            throw std::runtime_error(ErrorFormatter<std::runtime_error>(__FILE__, __LINE__)
                << "Not WAVE file. path: " << path
            );
        }

        // Preserve start of subchunk in riff chunk, and end of riff chunk.
        auto sub_chunk_start = audio_file.tellg();
        auto riff_end = audio_file.tellg() + static_cast<std::streamsize>(riff_chunk.size);

        std::tie(chunk, found) = find_riff_chunk(FOURCC_FORMAT_TAG, audio_file, riff_end);
        if (!found) {
            throw std::runtime_error(ErrorFormatter<std::runtime_error>(__FILE__, __LINE__)
                << "Format Error. not found fmt chunk. path: " << path
            );
        }

        WAVEFormat wave_format;
        audio_file.read(reinterpret_cast<char*>(&wave_format), sizeof(WAVEFormat));
        if (!audio_file) {
            throw std::runtime_error(ErrorFormatter<std::runtime_error>(__FILE__, __LINE__)
                << "Format Error. cannot read fmt chunk. path: " << path
            );
        }

        logging::InfoStream(__FILE__, __LINE__) << "\n"
            << "sample_format  : " << static_cast<uint16_t>(wave_format.sample_format) << "\n"
            << "n_channels     : " << wave_format.n_channels << "\n"
            << "sample_rate    : " << wave_format.sample_rate << "\n"
            << "byte_rate      : " << wave_format.byte_rate << "\n"
            << "block_align    : " << wave_format.block_align << "\n"
            << "bits_per_sample: " << wave_format.bits_per_sample;

        audio_file.seekg(sub_chunk_start);
        std::tie(chunk, found) = find_riff_chunk(FOURCC_DATA_TAG, audio_file, riff_end);
        if (!found) {
            throw std::runtime_error(ErrorFormatter<std::runtime_error>(__FILE__, __LINE__)
                << "Format Error. not found data chunk. path: " << path
            );
        }

        samples_.resize(chunk.size);
        audio_file.read(reinterpret_cast<char*>(samples_.data()), chunk.size);
        if (!audio_file) {
            throw std::runtime_error(ErrorFormatter<std::runtime_error>(__FILE__, __LINE__)
                << "Format Error. cannot read data chunk. path: " << path
            );
        }

        wfx_.wFormatTag = static_cast<WORD>(wave_format.sample_format);
        wfx_.nChannels = static_cast<WORD>(wave_format.n_channels);
        wfx_.nSamplesPerSec = static_cast<DWORD>(wave_format.sample_rate);
        wfx_.nAvgBytesPerSec = static_cast<DWORD>(wave_format.byte_rate);
        wfx_.nBlockAlign = static_cast<WORD>(wave_format.block_align);
        wfx_.wBitsPerSample = static_cast<WORD>(wave_format.bits_per_sample);
        wfx_.cbSize = 0;
    }

    const std::vector<uint8_t>& samples() const { return samples_; }
    const WAVEFORMATEX& wfx() const { return wfx_; }

private:
    WAVEFORMATEX wfx_{ 0x00 };
    std::vector<uint8_t> samples_;

};
