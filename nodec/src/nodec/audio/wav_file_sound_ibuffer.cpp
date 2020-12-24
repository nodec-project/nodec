#include <nodec/audio/wav_file_sound_ibuffer.hpp>
#include <nodec/audio/wav_format.hpp>
#include <nodec/logging.hpp>

namespace nodec
{
namespace audio
{

template class WavFileSoundIBuffer<float>;
template class WavFileSoundIBuffer<double>;


template<typename FloatT>
WavFileSoundIBuffer<FloatT>::WavFileSoundIBuffer()
{

}

template<typename FloatT>
void WavFileSoundIBuffer<FloatT>::open(const std::string& path)
{
    file.open(path, std::ios::binary);
    if (!file)
    {
        throw Exception("Cannot open sound file. path: " + path, __FILE__, __LINE__);
    }

    wav_format::HeaderInfo header_info;

    if (!wav_format::parse_header(file, header_info, data_start, data_end))
    {
        throw Exception("Failed to read header (invalid or unsupported file). path: " + path, 
                        __FILE__, __LINE__);
    }

    logging::DebugStream(__FILE__, __LINE__) << header_info.n_channels;
    logging::DebugStream(__FILE__, __LINE__) << header_info.bits_per_sample;
    logging::DebugStream(__FILE__, __LINE__) << header_info.n_samples;
    logging::DebugStream(__FILE__, __LINE__) << (header_info.format == wav_format::Format::PCM_Integer);

}

template<typename FloatT>
size_t WavFileSoundIBuffer<FloatT>::read(FloatT* samples, size_t max_length, bool loop)
{
    return 0;
}


}
}