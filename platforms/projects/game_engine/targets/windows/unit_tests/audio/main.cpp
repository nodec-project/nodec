#include <Audio/AudioPlatform.hpp>
#include <SceneAudio/AudioClipBackend.hpp>

#include <nodec/logging.hpp>
#include <nodec/audio/wave_format.hpp>
#include <nodec/endian.hpp>

#include <fstream>
#include <thread>


struct Test {
    union
    {
        char bytes[2];
        uint16_t value;
    };

};
//
//inline std::ostream& operator<<(std::ostream& stream, const Test& test) {
//    return stream << test.field;
//}


int main() {
    using namespace nodec;
    using namespace nodec::riff;
    using namespace nodec::audio::wave_format;
    using namespace Exceptions;

    logging::record_handlers().connect(logging::record_to_stdout_handler);

    try {
        ThrowIfFailed(CoInitializeEx(nullptr, COINIT_MULTITHREADED), __FILE__, __LINE__);

        AudioPlatform audioPlatform;

        AudioClipBackend clip("08-Isabella's Song-f32.wav");

        //std::ifstream audio_file;
        //audio_file.open("08-Isabella's Song-s16.wav", std::ios::binary);
        //audio_file.open("08-Isabella's Song-f32.wav", std::ios::binary);
        //audio_file.open("dodon.wav", std::ios::binary);
        //audio_file.unsetf(std::ios::skipws);
        //audio_file.open("void.txt", std::ios::binary);

        //if (!audio_file) {
        //    throw std::runtime_error("open failed");
        //}

        //RIFFChunk riff_chunk; bool found;
        //RIFFChunk chunk;

        //std::tie(riff_chunk, found) = find_riff_chunk(FOURCC_RIFF_TAG, audio_file);
        //if (!found) {
        //    throw std::runtime_error("Not RIFF");
        //}
        //riff::FOURCC id;
        //audio_file.read(reinterpret_cast<char*>(&id), sizeof(riff::FOURCC));
        //if (id != FOURCC_WAVE_FILE_TAG) {
        //    throw std::runtime_error("Not WAVE File");
        //}

        //// Preserve start of subchunk in riff chunk, and end of riff chunk.
        //auto sub_chunk_start = audio_file.tellg();
        //auto riff_end = audio_file.tellg() + static_cast<std::streamsize>(riff_chunk.size);

        //std::tie(chunk, found) = find_riff_chunk(FOURCC_FORMAT_TAG, audio_file, riff_end);
        //if (!found) {
        //    throw std::runtime_error("Format Error. not found fmt chunk");
        //}

        //WAVEFormat wave_format;
        //audio_file.read(reinterpret_cast<char*>(&wave_format), sizeof(WAVEFormat));
        //if (!audio_file) {
        //    throw std::runtime_error("Stream Error");
        //}

        //logging::InfoStream(__FILE__, __LINE__) << "\n"
        //    << "sample_format  : " << static_cast<uint16_t>(wave_format.sample_format) << "\n"
        //    << "n_channels     : " << wave_format.n_channels << "\n"
        //    << "sample_rate    : " << wave_format.sample_rate << "\n"
        //    << "byte_rate      : " << wave_format.byte_rate << "\n"
        //    << "block_align    : " << wave_format.block_align << "\n"
        //    << "bits_per_sample: " << wave_format.bits_per_sample;

        //audio_file.seekg(sub_chunk_start);
        //std::tie(chunk, found) = find_riff_chunk(FOURCC_DATA_TAG, audio_file, riff_end);
        //if (!found) {
        //    throw std::runtime_error("Format Error. not found data chunk");
        //}

        //logging::InfoStream(__FILE__, __LINE__) << "\n"
        //    << "id:   " << chunk.id << "\n"
        //    << "size: " << chunk.size;

        //std::vector<uint8_t> sample_data;
        //sample_data.resize(chunk.size);
        //audio_file.read(reinterpret_cast<char*>(sample_data.data()), chunk.size);
        //if (!audio_file) {
        //    throw std::runtime_error("Stream Error");
        //}


        IXAudio2SourceVoice* pSourceVoice;

        ThrowIfFailed(audioPlatform.GetXAudio().CreateSourceVoice(&pSourceVoice, &clip.wfx()), __FILE__, __LINE__);


        XAUDIO2_BUFFER buffer = {};
        buffer.pAudioData = clip.samples().data();
        buffer.Flags = XAUDIO2_END_OF_STREAM;
        buffer.AudioBytes = clip.samples().size();

        ThrowIfFailed(pSourceVoice->SubmitSourceBuffer(&buffer), __FILE__, __LINE__);

        ThrowIfFailed(pSourceVoice->Start(), __FILE__, __LINE__);



        //std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        std::cin.get();
        pSourceVoice->Stop();

        buffer.PlayBegin = 0;
        //ThrowIfFailed(pSourceVoice->SubmitSourceBuffer(&buffer), __FILE__, __LINE__);
        //audioIntegration.GetXAudio().StopEngine();

        std::cin.get();
        pSourceVoice->Start();

        //audioIntegration.GetXAudio().StartEngine();

        std::cin.get();

        pSourceVoice->Stop();
        pSourceVoice->DestroyVoice();

    }
    catch (std::exception& e) {
        logging::ErrorStream(__FILE__, __LINE__) << e.what();
    }

    CoUninitialize();
    return 0;
}