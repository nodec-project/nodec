#include <nodec/audio/wav_file_sound_ibuffer.hpp>
#include <nodec/logging.hpp>
#include <nodec/error_formatter.hpp>

#include <sstream>
#include <stdexcept>

namespace nodec {
namespace audio {

template class WavFileSoundIBuffer<float>;
template class WavFileSoundIBuffer<double>;


template<typename FloatT>
WavFileSoundIBuffer<FloatT>::WavFileSoundIBuffer() :
    bytes_per_sample(0),
    format(wav_format::Format::PCM_Integer) {

}

template<typename FloatT>
void WavFileSoundIBuffer<FloatT>::open(const std::string& path) {
    file.open(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error(error_fomatter::with_type_file_line<std::runtime_error>(
            Formatter() << "Cannot open sound file. path: " << path,
            __FILE__, __LINE__
            ));
    }

    wav_format::HeaderInfo header_info;

    if (!wav_format::parse_header(file, header_info, data_start, data_end)) {
        throw std::runtime_error(error_fomatter::with_type_file_line<std::runtime_error>(
            Formatter()
            << "Failed to read header (invalid or unsupported file). path: "
            << path,
            __FILE__, __LINE__
            ));
    }

    if ((header_info.format != wav_format::Format::PCM_Integer)
        && (header_info.format != wav_format::Format::PCM_Float)
        && (header_info.format != wav_format::Format::Extensible)) {
        throw std::runtime_error(error_fomatter::with_type_file_line<std::runtime_error>(
            Formatter()
            << "Unsupported format: format tag (" << std::hex << std::uppercase
            << static_cast<int>(header_info.format) << std::dec << ") is not supported.",
            __FILE__, __LINE__
            ));
    }
    format = header_info.format;

    if (header_info.bits_per_sample != 8
        && header_info.bits_per_sample != 16
        && header_info.bits_per_sample != 24
        && header_info.bits_per_sample != 32) {
        throw std::runtime_error(error_fomatter::with_type_file_line<std::runtime_error>(
            Formatter()
            << "Unsupported sample size: " << header_info.bits_per_sample
            << " bit (Supported sample size are 8/16/24/32 bit)",
            __FILE__, __LINE__
            ));
    }

    bytes_per_sample = header_info.bits_per_sample / 8;

    if (header_info.format == wav_format::Format::Extensible) {
        if (header_info.sub_format == wav_format::SubFormat::None) {
            throw std::runtime_error(error_fomatter::with_type_file_line<std::runtime_error>(
                "Unsupported format: extensible format with unkown sub format.",
                __FILE__, __LINE__
                ));
        }

        if (header_info.valid_bits_per_sample != header_info.bits_per_sample) {
            throw std::runtime_error(error_fomatter::with_type_file_line<std::runtime_error>(
                Formatter()
                << "Unsupported format: sample size ("
                << header_info.valid_bits_per_sample << " bits) and "
                << "sample container size (" << header_info.bits_per_sample
                << " bits) differ",
                __FILE__, __LINE__
                ));
        }

        format = static_cast<wav_format::Format>(header_info.sub_format);
    }

    if (format == wav_format::Format::PCM_Float
        && bytes_per_sample != 4) {
        throw std::runtime_error(error_fomatter::with_type_file_line<std::runtime_error>(
            Formatter()
            << "Unsupported format: float " << header_info.bits_per_sample << " bit not supported. "
            << "(supported format is 32bit float)",
            __FILE__, __LINE__
            ));
    }

    this->sample_rate = header_info.n_samples_per_sec;
    this->n_channels = header_info.n_channels;

    logging::DebugStream(__FILE__, __LINE__) << header_info.n_channels;
    logging::DebugStream(__FILE__, __LINE__) << header_info.bits_per_sample;
    //logging::DebugStream(__FILE__, __LINE__) << header_info.bits_per_sample;

}

template<typename FloatT>
size_t WavFileSoundIBuffer<FloatT>::read(FloatT* const* const samples, std::streamoff offset, std::streamsize max_length, bool loop) {
    switch (format) {
    case nodec::audio::wav_format::Format::PCM_Integer:
        break;

    case nodec::audio::wav_format::Format::PCM_Float:
        switch (bytes_per_sample) {
        case 4:
            switch (this->n_channels) {
            case 2:
                return wav_format::read_data_float32_stereo(samples, file, offset, max_length, data_start, data_end, loop);

            default:
                break;
            }
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    return 0;
}


}
}