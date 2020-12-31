#include <openal_extention/audio_device.hpp>

#include <nodec/math/math.hpp>
#include <nodec/logging.hpp>

#include <limits>
#include <array>
#include <thread>
#include <chrono>

using namespace nodec;
using namespace openal_extention;

int main()
{
    logging::record_handlers += logging::StaticRecordHandler::make_shared(&logging::record_to_stdout_handler);

    constexpr int sampling_rate = 44100;
    constexpr int tone = 440;
    constexpr int duration = 3;
    constexpr size_t data_size = sampling_rate * duration;

    std::unique_ptr<int16_t[]> data(new int16_t[data_size]);

    // radian per sampling rate
    double delta = (2 * math::pi<double> *tone) / sampling_rate;

    // generate sin wave
    for (size_t i = 0; i < data_size; i++)
    {
        data[i] = std::numeric_limits<uint16_t>::max() * std::sin(delta * i);
    }

    // init device and create context
    AudioDevice audio_device;

    // make buffer and source
    ALuint buffer;
    ALuint source;
    alGenBuffers(1, &buffer);
    alGenSources(1, &source);

    alBufferData(buffer, AL_FORMAT_MONO16, data.get(), data_size, sampling_rate);
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

    // clean up
    alDeleteBuffers(1, &buffer);
    alDeleteSources(1, &source);


    return 0;
}