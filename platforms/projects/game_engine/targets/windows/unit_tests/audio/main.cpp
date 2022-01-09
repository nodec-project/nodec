#include <Audio/AudioIntegration.hpp>

#include <nodec/logging.hpp>
#include <nodec/audio/wave_format.hpp>

#include <fstream>
#include <thread>


struct Test {
    int field = 100;
};

inline std::ostream& operator<<(std::ostream& stream, const Test& test) {
    return stream << test.field;
}

int main() {
    using namespace nodec;
    using namespace nodec::audio::wave_format;
    using namespace Exceptions;

    logging::record_handlers().connect(logging::record_to_stdout_handler);
    try {
        ThrowIfFailed(CoInitializeEx(nullptr, COINIT_MULTITHREADED), __FILE__, __LINE__);

        AudioIntegration audioIntegration;

        std::ifstream audio_file;
        audio_file.open("08-Isabella's Song-s16.wav", std::ios::binary);
        if (!audio_file) {
            throw std::runtime_error("open failed");
        }

        RIFFChunkHeader riff_header;

        audio_file.read(reinterpret_cast<char*>(&riff_header), sizeof(RIFFChunkHeader));
        if (!audio_file) {
            throw std::runtime_error("Stream Error");
        }
        if (riff_header.descriptor.id != FOURCC_RIFF_TAG) {
            throw std::runtime_error("Not RIFF");
        }
        if (riff_header.type != FOURCC_WAVE_FILE_TAG) {
            throw std::runtime_error("Not WAVE File");
        }

        RIFFChunk riff_chunk;
        audio_file.read(reinterpret_cast<char*>(&riff_chunk), sizeof(RIFFChunk));
        if (!audio_file) {
            throw std::runtime_error("Stream Error");
        }
        if (riff_chunk.id != FOURCC_FORMAT_TAG) {
            throw std::runtime_error("Format Error. not found fmt chunk");
        }

        auto pos = audio_file.tellg();
        WAVEFormat wave_format;
        audio_file.read(reinterpret_cast<char*>(&wave_format), sizeof(WAVEFormat));
        if (!audio_file) {
            throw std::runtime_error("Stream Error");
        }

        logging::InfoStream(__FILE__, __LINE__) << "\n"
            << "sample_format  : " << static_cast<uint16_t>(wave_format.sample_format) << "\n"
            << "n_channels     : " << wave_format.n_channels << "\n"
            << "sample_rate    : " << wave_format.sample_rate << "\n"
            << "byte_rate      : " << wave_format.byte_rate << "\n"
            << "block_align    : " << wave_format.block_align << "\n"
            << "bits_per_sample: " << wave_format.bits_per_sample;

        audio_file.seekg(pos + static_cast<std::streampos>(riff_chunk.size));
        if (!audio_file) {
            throw std::runtime_error("Stream Error");
        }

        audio_file.read(reinterpret_cast<char*>(&riff_chunk), sizeof(RIFFChunk));
        if (!audio_file) {
            throw std::runtime_error("Stream Error");
        }
        if (riff_chunk.id != FOURCC_DATA_TAG) {
            throw std::runtime_error("Format Error. not found data chunk");
        }
        logging::InfoStream(__FILE__, __LINE__) << "\n"
            << "id:   " << riff_chunk.id << "\n"
            << "size: " << riff_chunk.size;

        std::vector<uint8_t> sample_data;
        sample_data.resize(riff_chunk.size);
        audio_file.read(reinterpret_cast<char*>(sample_data.data()), riff_chunk.size);
        if (!audio_file) {
            throw std::runtime_error("Stream Error");
        }
        //audio_file.read(reinterpret_cast<char*>())

        //Test test;
        //logging::InfoStream(__FILE__, __LINE__) << "\n"
        //    << "id:   " << riff_header.descriptor.id << "\n"
        //    << "size: " << riff_header.descriptor.size << "\n"
        //    << "type: " << riff_header.type << test;


        IXAudio2SourceVoice* pSourceVoice;

        WAVEFORMATEX wfx;
        wfx.wFormatTag = static_cast<WORD>(wave_format.sample_format);
        wfx.nChannels = static_cast<WORD>(wave_format.n_channels);
        wfx.nSamplesPerSec = static_cast<DWORD>(wave_format.sample_rate);
        wfx.nAvgBytesPerSec = static_cast<DWORD>(wave_format.byte_rate);
        wfx.nBlockAlign = static_cast<WORD>(wave_format.block_align);
        wfx.wBitsPerSample = static_cast<WORD>(wave_format.bits_per_sample);
        wfx.cbSize = 0;

        ThrowIfFailed(audioIntegration.GetXAudio().CreateSourceVoice(&pSourceVoice, &wfx), __FILE__, __LINE__);


        XAUDIO2_BUFFER buffer = {};
        buffer.pAudioData = sample_data.data();
        buffer.Flags = XAUDIO2_END_OF_STREAM;
        buffer.AudioBytes = sample_data.size();

        ThrowIfFailed(pSourceVoice->SubmitSourceBuffer(&buffer), __FILE__, __LINE__);

        ThrowIfFailed(pSourceVoice->Start(), __FILE__, __LINE__);
        
        
        //std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        std::cin.get();

    }
    catch (std::exception& e) {
        logging::ErrorStream(__FILE__, __LINE__) << e.what();
    }

    CoUninitialize();
    return 0;
}