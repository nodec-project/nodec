#ifndef NODEC__AUDIO__SOUND_STREAM_HPP_
#define NODEC__AUDIO__SOUND_STREAM_HPP_

#include <nodec/audio/basic_sound_buffer.hpp>
#include <nodec/macros.hpp>

#include <memory>


namespace nodec
{
namespace audio
{

template<typename FloatT>
class SoundStream
{
public:
    SoundStream(BasicSoundBuffer<FloatT>::SharedPtr buffer);

private:
    NODEC_DISABLE_COPY(SoundStream);
};

}
}
#endif