#include <nodec/audio/wav_file_sound_ibuffer.hpp>
#include <nodec/logging.hpp>

#include <openal_extention/audio_device.hpp>

using namespace nodec;
using namespace openal_extention;
using namespace nodec::audio;

float samplesl[2048];
float samplesr[2048];
float* samples[2];

int main()
{

    logging::record_handlers += logging::StaticRecordHandler::make_shared(&logging::record_to_stdout_handler);
    logging::DebugStream(__FILE__, __LINE__) << "Debug start" << std::flush;

    try
    {
        AudioDevice audio_device;

        WavFileSoundIBuffer<float> sound_buffer;
        sound_buffer.open("miku-activated.wav");
        //sound_buffer.open("tone440_float32_stereo.wav");
        samples[0] = samplesl;
        samples[1] = samplesr;

        auto size = sound_buffer.read(samples, 0, 2048, false);

        logging::DebugStream debug_stream(__FILE__, __LINE__);

        for (auto i = 0; i < 2048; i++)
        {
            debug_stream << samples[0][i] << ", " << samples[1][i] << "\n";
        }
        

    }
    catch (const std::exception& e)
    {
        logging::ErrorStream(__FILE__, __LINE__) << e.what();
    }



    return 0;
}