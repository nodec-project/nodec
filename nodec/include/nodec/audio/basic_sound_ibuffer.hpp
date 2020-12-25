#ifndef NODEC__AUDIO__BASIC_SOUND_IBUFFER_HPP_
#define NODEC__AUDIO__BASIC_SOUND_IBUFFER_HPP_

#include <nodec/macros.hpp>

#include <vector>

namespace nodec
{
namespace audio
{

template<typename FloatT>
class BasicSoundIBuffer
{
public:
    NODEC_SHARED_PTR_DEFINITIONS(BasicSoundIBuffer);

public:

    virtual size_t read(FloatT* const* const samples, std::streamoff offset, std::streamsize max_length, bool loop) = 0;

protected:
    uint32_t sample_rate;
    uint16_t n_channels;

};

}
}

#endif // !NODEC__AUDIO__BASIC_SOUND_BUFFER_HPP_
