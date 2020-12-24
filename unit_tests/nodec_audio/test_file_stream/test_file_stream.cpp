#include <nodec/audio/wav_file_sound_ibuffer.hpp>
#include <nodec/logging.hpp>

#include <openal_extention/audio_device.hpp>

using namespace nodec;
using namespace openal_extention;
using namespace nodec::audio;

int main()
{

    logging::record_handlers += logging::StaticRecordHandler::make_shared(&logging::record_to_stdout_handler);
    logging::DebugStream(__FILE__, __LINE__) << "Debug start" << std::flush;

    try
    {
        AudioDevice audio_device;

        WavFileSoundIBuffer<float> sound_buffer;
        sound_buffer.open("miku-activated.wav");


    }
    catch (const std::exception& e)
    {
        logging::ErrorStream(__FILE__, __LINE__) << e.what();
    }

    

    return 0;
}