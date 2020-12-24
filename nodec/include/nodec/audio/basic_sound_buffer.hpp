#ifndef NODEC__AUDIO__BASIC_SOUND_BUFFER_HPP_
#define NODEC__AUDIO__BASIC_SOUND_BUFFER_HPP_

#include <nodec/macros.hpp>

#include <vector>

namespace nodec
{
namespace audio
{

template<typename T>
class BasicSoundBuffer
{
public:
    NODEC_SHARED_PTR_DEFINITIONS(BasicSoundBuffer);

public:

    size_t read(T* samples, size_t max_length, bool loop);

protected:
    unsigned int sampling_rate;
    unsigned int n_channels;
    std::vector<T> samples;

};

}
}

#endif // !NODEC__AUDIO__BASIC_SOUND_BUFFER_HPP_
