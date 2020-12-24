#ifndef OPENAL_EXTENTION__AUDIO_DEVICE_HPP_
#define OPENAL_EXTENTION__AUDIO_DEVICE_HPP_

#include <al.h>
#include <alc.h>

#include <nodec/macros.hpp>
#include <nodec/nodec_exception.hpp>


namespace openal_extention
{

class AudioDevice
{
public:
    class Exception : nodec::NodecException
    {
    public:
        using NodecException::NodecException;
    };

public:
    AudioDevice();
    ~AudioDevice();

    ALCdevice* device() const noexcept;
    ALCcontext* context() const noexcept;

private:
    ALCdevice* p_device;
    ALCcontext* p_context;

private:
    NODEC_DISABLE_COPY(AudioDevice);
};

}

#endif