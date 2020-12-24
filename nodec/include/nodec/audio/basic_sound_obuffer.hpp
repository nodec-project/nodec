#ifndef NODEC__AUDIO__BASIC_SOUND_BUFFER_HPP_
#define NODEC__AUDIO__BASIC_SOUND_BUFFER_HPP_

#include <nodec/macros.hpp>

#include <vector>

namespace nodec
{
namespace audio
{

template<typename FloatT>
class BasicSoundBuffer
{
public:
    NODEC_SHARED_PTR_DEFINITIONS(BasicSoundBuffer);

public:

    virtual size_t read(FloatT* samples, size_t max_length, bool loop) = 0;

protected:
    unsigned int sampling_rate;
    unsigned int n_channels;

};

}
}

#endif // !NODEC__AUDIO__BASIC_SOUND_BUFFER_HPP_
