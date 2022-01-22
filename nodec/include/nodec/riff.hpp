#ifndef NODEC__RIFF_HPP_
#define NODEC__RIFF_HPP_

#include <nodec/logging.hpp>
#include <nodec/endian.hpp>

#include <cstdint>
#include <iostream>
#include <algorithm>


namespace nodec {

namespace riff {

// about FOURCC:
//  * <https://en.wikipedia.org/wiki/FourCC>
//  * <https://docs.microsoft.com/ja-jp/windows/win32/directshow/fourcc-codes>
//

struct FOURCC {
    char bytes[4]{ 0x00 };

    constexpr bool operator==(const FOURCC& other) const noexcept {
        return bytes[0] == other.bytes[0] && bytes[1] == other.bytes[1]
            && bytes[2] == other.bytes[2] && bytes[3] == other.bytes[3];
    }

    constexpr bool operator!=(const FOURCC& other) const noexcept {
        return !(*this == other);
    }
};


constexpr FOURCC FOURCC_RIFF_TAG        = { 'R', 'I', 'F', 'F' };
constexpr FOURCC FOURCC_LIST_TAG        = { 'L', 'I', 'S', 'T' };
constexpr FOURCC FOURCC_FORMAT_TAG      = { 'f', 'm', 't', ' ' };
constexpr FOURCC FOURCC_DATA_TAG        = { 'd', 'a', 't', 'a' };
constexpr FOURCC FOURCC_WAVE_FILE_TAG   = { 'W', 'A', 'V', 'E' };



// about RIFF:
//  * <https://en.wikipedia.org/wiki/Resource_Interchange_File_Format>
//
struct RIFFChunk {
    FOURCC id;
    uint32_t size{ 0 };
};


inline std::pair<RIFFChunk, bool> find_riff_chunk(const FOURCC& target, std::istream& stream, std::streampos end = -1) {

    RIFFChunk chunk;

    stream.read(reinterpret_cast<char*>(&chunk), sizeof(RIFFChunk));

    while (stream && (end < 0 || stream.tellg() < end)) {

        chunk.size = endian::little_to_native(chunk.size);

        if (chunk.id == target) {
            // found
            return { chunk, true };
        }

        stream.seekg(chunk.size, std::ios_base::cur);
        stream.read(reinterpret_cast<char*>(&chunk), sizeof(RIFFChunk));
    }

    return { {}, false };
}

inline std::ostream& operator<<(std::ostream& stream, const nodec::riff::FOURCC& code) {
    return stream << code.bytes[0] << code.bytes[1] << code.bytes[2] << code.bytes[3];
}

}
}

#endif