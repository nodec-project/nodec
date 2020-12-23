#include <al.h>
#include <alc.h>

#include <nodec/math/math.hpp>

#include <limits>
#include <array>
#include <thread>
#include <chrono>

using namespace nodec;

int main()
{

    constexpr int sampling_rate = 44100;
    constexpr int tone = 440;
    constexpr int duration = 3;
    constexpr size_t data_size = sampling_rate * duration;

    std::array<int16_t, data_size> data;

    // radian per sampling rate
    double delta = (2 * math::pi<double> *tone) / sampling_rate;

    // generate sin wave
    for (size_t i = 0; i < data_size; i++)
    {
        data[i] = std::numeric_limits<uint16_t>::max() * std::sin(delta * i);
    }

    // init device and create context
    ALCdevice* device = alcOpenDevice(nullptr);
    ALCcontext* context = alcCreateContext(device, nullptr);
    alcMakeContextCurrent(context);

    // make buffer and source
    ALuint buffer;
    ALuint source;
    alGenBuffers(1, &buffer);
    alGenSources(1, &source);

    alBufferData(buffer, AL_FORMAT_MONO16, data.data(), data.size(), sampling_rate);
    alSourcei(source, AL_BUFFER, buffer);
    alSourcePlay(source);

    ALint state;
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        alGetSourcei(source, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING)
        {
            break;
        }
    }

    return 0;
}