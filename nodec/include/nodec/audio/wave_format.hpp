#ifndef NODEC__AUDIO__WAVE_FORMAT_HPP_
#define NODEC__AUDIO__WAVE_FORMAT_HPP_

#include <nodec/riff.hpp>

#include <iostream>

namespace nodec {
namespace audio {

/**
* @detail
*   Wave Format:
*       <https://wavefilegem.com/how_wave_files_work.html>
*       <http://soundfile.sapp.org/doc/WaveFormat/>
*       <https://docs.microsoft.com/ja-jp/windows-hardware/drivers/audio/extensible-wave-format-descriptors>
*       <http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html>
*
*   WAVE-FORMAT-EXTENSIBLE:
*       <http://dream.cs.bath.ac.uk/researchdev/wave-ex/wave_ex.html>
*       <https://docs.microsoft.com/en-us/windows/win32/api/mmreg/ns-mmreg-waveformatextensible>
*/
namespace wave_format {

enum class SampleFormat : uint16_t {
    PCM = 0x01,
    IEEE_FLOAT = 0x03,
    EXTENSIBLE = 0xfffe
};

struct WAVEFormat {
    SampleFormat sample_format;
    uint16_t n_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
};

//
///**
//* @brief read integer as little endian from stream and
//*        return them in the host byte order.
//*/
//bool decode8(std::istream& stream, uint8_t& value);
//
//bool decode16(std::istream& stream, uint16_t& value);
//
//bool decode24(std::istream& stream, uint32_t& value);
//
//bool decode32(std::istream& stream, uint32_t& value);
//
//bool decode32float(std::istream& stream, float& value);
//
//
//
//enum class Format
//{
//    PCM_Integer = 0x01,
//    PCM_Float = 0x03,
//    Extensible = 0xfffe,
//};
//
//enum class SubFormat
//{
//    None = 0x00,
//    PCM_Integer = 0x01,
//    PCM_Float = 0x03
//};
//
//struct HeaderInfo
//{
//    Format format;
//    uint16_t n_channels;
//    uint32_t n_samples_per_sec;
//    uint16_t bits_per_sample;
//    uint16_t valid_bits_per_sample;
//    uint32_t channel_mask;
//    SubFormat sub_format;
//    size_t n_samples;
//};
//
//bool parse_header(std::istream& stream, HeaderInfo& info,
//                  std::streampos& data_start, std::streampos& data_end);
//
//template<typename FloatT>
//size_t read_data_float32_stereo(FloatT* const* const samples, std::istream& stream, const std::streamoff& offset, const std::streamsize& max_length,
//                                const std::streampos& data_start, const std::streampos& data_end, bool loop);

}
}

}


#endif