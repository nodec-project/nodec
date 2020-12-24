#include <openal_extention/audio_device.hpp>

#include <nodec/logging.hpp>

namespace openal_extention
{

AudioDevice::AudioDevice()
{
    p_device = alcOpenDevice(nullptr);
    if (!p_device)
    {
        throw Exception("Failed to open device", __FILE__, __LINE__);
    }

    p_context = alcCreateContext(p_device, nullptr);
    if (!p_context)
    {
        throw Exception("Failed to create context", __FILE__, __LINE__);
    }
    alcMakeContextCurrent(p_context);

    nodec::logging::InfoStream(__FILE__, __LINE__) 
        << "[AudioDevice] >>> Successfully initialized.\n"
        << "Device info: " << alcGetString(p_device, ALC_DEVICE_SPECIFIER);
}

AudioDevice::~AudioDevice()
{
    if (p_device)
    {
        alcCloseDevice(p_device);
    }

    if (p_context)
    {
        alcDestroyContext(p_context);
    }

    nodec::logging::InfoStream(__FILE__, __LINE__) << "[AudioDevice] >>> End AudioDevice.";
}

ALCdevice* AudioDevice::device() const noexcept
{
    return p_device;
}

ALCcontext* AudioDevice::context() const noexcept
{
    return p_context;
}


}