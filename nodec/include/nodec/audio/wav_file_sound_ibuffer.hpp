#ifndef NODEC__AUDIO__WAV_FILE_SOUND_IBUFFER_HPP_
#define NODEC__AUDIO__WAV_FILE_SOUND_IBUFFER_HPP_

#include <nodec/audio/basic_sound_ibuffer.hpp>
#include <nodec/macros.hpp>
#include <nodec/exception.hpp>
#include <nodec/audio/wav_format.hpp>

#include <fstream>

namespace nodec {
namespace audio {

template<typename FloatT>
class WavFileSoundIBuffer : public BasicSoundIBuffer<FloatT> {
public:
    WavFileSoundIBuffer();

public:
    void open(const std::string& path);

    size_t read(FloatT* const* const  samples, std::streamoff offset, std::streamsize max_length, bool loop) override;


private:
    std::ifstream file;
    std::streampos data_start;
    std::streampos data_end;

    wav_format::Format format;
    uint16_t bytes_per_sample;



private:
    NODEC_DISABLE_COPY(WavFileSoundIBuffer);
};

}
}

#endif