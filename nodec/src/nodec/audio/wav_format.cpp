#include <nodec/audio/wav_format.hpp>
#include <nodec/logging.hpp>

namespace nodec
{
namespace audio
{


namespace wav_format
{


namespace
{
constexpr uint64_t main_chunk_size = 12;
const char* sub_format_guid_pcm_integer = "\x01\x00\x00\x00\x00\x00\x10\x00\x80\x00\x00\xAA\x00\x38\x9B\x71";
const char* sub_format_guid_pcm_float = "\x03\x00\x00\x00\x00\x00\x10\x00\x80\x00\x00\xAA\x00\x38\x9B\x71";

}

bool decode8(std::istream& stream, uint8_t& value)
{
    stream.read(reinterpret_cast<char*>(value), sizeof(value));
    return !stream.fail();
}

bool decode16(std::istream& stream, uint16_t& value)
{
    uint8_t bytes[sizeof(value)];
    stream.read(reinterpret_cast<char*>(bytes), sizeof(bytes));
    if (stream.fail())
    {
        return false;
    }

    value = bytes[0] | (bytes[1] << 8);
    return true;
}

bool decode24(std::istream& stream, uint32_t& value)
{
    uint8_t bytes[3];
    stream.read(reinterpret_cast<char*>(bytes), sizeof(bytes));
    if (stream.fail())
    {
        return false;
    }

    value = bytes[0] | (bytes[1] << 8) | (bytes[2] << 16);
    return true;
}

bool decode32(std::istream& stream, uint32_t& value)
{
    uint8_t bytes[sizeof(value)];
    stream.read(reinterpret_cast<char*>(bytes), sizeof(bytes));
    if (stream.fail())
    {
        return false;
    }
    value = bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);

    return true;
}


bool decode32float(std::istream& stream, float& value)
{
    uint8_t bytes[sizeof(value)];
    stream.read(reinterpret_cast<char*>(bytes), sizeof(bytes));
    if (stream.fail())
    {
        return false;
    }
    uint32_t value_bytes = bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
    value = *reinterpret_cast<float*>(&value_bytes);

    return true;
}



bool parse_header(std::istream& stream, HeaderInfo& info,
                  std::streampos& data_start, std::streampos& data_end)
{

    char main_chunk[main_chunk_size];
    stream.read(main_chunk, main_chunk_size);
    if (stream.fail())
    {
        return false;
    }


    bool data_chunk_found = false;
    while (!data_chunk_found)
    {
        // parse the sub-chunk id and size
        char sub_chunk_id[4];
        stream.read(sub_chunk_id, sizeof(sub_chunk_id));
        if (stream.fail())
        {
            return false;
        }

        uint32_t sub_chunk_size = 0;
        if (!decode32(stream, sub_chunk_size))
        {
            return false;
        }

        auto sub_chunk_start = stream.tellg();
        if (sub_chunk_start == -1)
        {
            return false;
        }

        // check which chunk it is
        if (sub_chunk_id[0] == 'f'
            && sub_chunk_id[1] == 'm'
            && sub_chunk_id[2] == 't'
            && sub_chunk_id[3] == ' ')
        {
            // "fmt" chunk

            // Audio format
            uint16_t format = 0;
            if (!decode16(stream, format))
            {
                return false;
            }
            info.format = static_cast<Format>(format);

            // chennel count
            uint16_t n_channels = 0;
            if (!decode16(stream, n_channels))
            {
                return false;
            }
            info.n_channels = n_channels;

            // sampling rate
            uint32_t n_samples_per_sec = 0;
            if (!decode32(stream, n_samples_per_sec))
            {
                return false;
            }
            info.n_samples_per_sec = n_samples_per_sec;

            // byte rate
            uint32_t n_avg_bytes_per_sec = 0;
            if (!decode32(stream, n_avg_bytes_per_sec))
            {
                return false;
            }

            // block align
            uint16_t n_block_align = 0;
            if (!decode16(stream, n_block_align))
            {
                return false;
            }

            // bits per sample
            uint16_t bits_per_sample = 0;
            if (!decode16(stream, bits_per_sample))
            {
                return false;
            }
            info.bits_per_sample = bits_per_sample;

            if (info.format == Format::Extensible)
            {
                // extention size
                uint16_t extention_size = 0;
                if (!decode16(stream, extention_size))
                {
                    return false;
                }

                // valid bits per sample
                uint16_t valid_bits_per_sample = 0;
                if (!decode16(stream, valid_bits_per_sample))
                {
                    return false;
                }
                info.valid_bits_per_sample = valid_bits_per_sample;

                // channel mask
                uint32_t channel_mask = 0;
                if (!decode32(stream, channel_mask))
                {
                    return false;
                }
                info.channel_mask = channel_mask;

                // sub_format
                char sub_format[16];
                stream.read(sub_format, sizeof(sub_format));
                if (stream.fail())
                {
                    return false;
                }
                if (std::memcmp(sub_format, sub_format_guid_pcm_integer, sizeof(sub_format)) == 0)
                {
                    info.sub_format = SubFormat::PCM_Integer;
                }
                else if (std::memcmp(sub_format, sub_format_guid_pcm_float, sizeof(sub_format)) == 0)
                {
                    info.sub_format = SubFormat::PCM_Float;
                }


            }

            // skip potential extra information
            // <https://stackoverflow.com/questions/5501924/incrementing-a-streampos-object>
            stream.seekg(sub_chunk_start + static_cast<std::streamoff>(sub_chunk_size));
            if (stream.fail())
            {
                return false;
            }

        } // endif "fmt" chunk
        else if (sub_chunk_id[0] == 'd'
                 && sub_chunk_id[1] == 'a'
                 && sub_chunk_id[2] == 't'
                 && sub_chunk_id[3] == 'a')
        {
            // "data" chunk

            // compute the total number of samples
            auto bytes_per_sample = info.bits_per_sample / 8;
            info.n_samples = sub_chunk_size / bytes_per_sample;

            // store the start and end position of samples in the file
            data_start = sub_chunk_start;
            data_end = data_start + static_cast<std::streamoff>(sub_chunk_size);

            data_chunk_found = true;
        }
        else
        {
            // unknown chunk skip it
            stream.seekg(static_cast<std::streamoff>(sub_chunk_size), std::ios_base::cur);
            if (stream.fail())
            {
                return false;
            }
        }
    }
    return true;
}

template<typename FloatT>
bool decode32float_stereo(std::istream& stream, FloatT* const* const samples, uint64_t index)
{
    float value;
    if (!decode32float(stream, value))
    {
        return false;
    }
    samples[0][index] = static_cast<FloatT>(value);

    if (!decode32float(stream, value))
    {
        return false;
    }
    samples[1][index] = static_cast<FloatT>(value);

    return true;
}

template<typename FloatT, typename Function>
size_t read_data_generic(FloatT* const* const  samples, std::istream& stream, const std::streamoff& offset, const std::streamsize& max_length,
                         const std::streampos& data_start, const std::streampos& data_end, bool loop, Function decoder)
{
    stream.seekg(data_start + offset);
    size_t index = 0;

    while (true)
    {
        if (stream.tellg() >= data_end)
        {
            if (stream.fail())
            {
                // if prev operation 'tellg()' setted fail bit.
                break;
            }

            // reach the end of stream.
            if (!loop)
            {
                break;
            }

            // seek to data start for loop.
            stream.seekg(data_start);
        }

        if (!decoder(stream, samples, index))
        {
            return false;
        }

        index++;

        if (index >= max_length)
        {
            break;
        }
    }

    return index;
}


template<typename FloatT>
size_t read_data_float32_stereo(FloatT* const* const samples, std::istream& stream, const std::streamoff& offset, const std::streamsize& max_length,
                                const std::streampos& data_start, const std::streampos& data_end, bool loop)
{
    using Function = decltype(decode32float_stereo<FloatT>);
    return read_data_generic<FloatT, Function>(samples, stream, offset, max_length, data_start, data_end, loop, decode32float_stereo);
}

template
size_t read_data_float32_stereo(float* const* const samples, std::istream& stream, const std::streamoff& offset, const std::streamsize& max_length,
                                const std::streampos& data_start, const std::streampos& data_end, bool loop);

template
size_t read_data_float32_stereo(double* const* const samples, std::istream& stream, const std::streamoff& offset, const std::streamsize& max_length,
                                const std::streampos& data_start, const std::streampos& data_end, bool loop);

} // namespace wav_format
} // namespace audio
} // namespace nodec