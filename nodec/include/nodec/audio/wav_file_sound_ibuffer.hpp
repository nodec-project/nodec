#ifndef NODEC__AUDIO__WAV_FILE_SOUND_BUFFER_HPP_
#define NODEC__AUDIO__WAV_FILE_SOUND_BUFFER_HPP_

#include <nodec/audio/basic_sound_ibuffer.hpp>
#include <nodec/macros.hpp>
#include <nodec/nodec_exception.hpp>

#include <fstream>

namespace nodec
{
namespace audio
{

template<typename FloatT>
class WavFileSoundIBuffer : public BasicSoundIBuffer<FloatT>
{
public:
    class Exception : public NodecException
    {
        using NodecException::NodecException;
    };

public:
    WavFileSoundIBuffer();

public:
    size_t read(FloatT* samples, size_t max_length, bool loop) override;

    void open(const std::string& path);

private:
    std::ifstream file;
    std::streampos data_start;
    std::streampos data_end;

private:
    NODEC_DISABLE_COPY(WavFileSoundIBuffer);
};

}
}

#endif