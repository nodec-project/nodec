#ifndef NODEC__RIFF_HPP_
#define NODEC__RIFF_HPP_

#include <cstdint>
#include <iostream>


namespace nodec {

// about FOURCC:
//  * <https://en.wikipedia.org/wiki/FourCC>
//  * <https://docs.microsoft.com/ja-jp/windows/win32/directshow/fourcc-codes>
//

struct FOURCC {
    char bytes[4];

    constexpr bool operator==(const FOURCC& other) const noexcept {
        return bytes[0] == other.bytes[0] && bytes[1] == other.bytes[1]
            && bytes[2] == other.bytes[2] && bytes[3] == other.bytes[3];
    }

    constexpr bool operator!=(const FOURCC& other) const noexcept {
        return !(*this == other);
    }
};



constexpr FOURCC FOURCC_RIFF_TAG = { 'R', 'I', 'F', 'F' };
constexpr FOURCC FOURCC_FORMAT_TAG = { 'f', 'm', 't', ' ' };
constexpr FOURCC FOURCC_DATA_TAG = { 'd', 'a', 't', 'a' };
constexpr FOURCC FOURCC_WAVE_FILE_TAG = { 'W', 'A', 'V', 'E' };



// about RIFF:
//  * <https://en.wikipedia.org/wiki/Resource_Interchange_File_Format>
//
struct RIFFChunk {
    FOURCC id;
    uint32_t size;
};

struct RIFFChunkHeader {
    RIFFChunk descriptor;
    FOURCC type;
};

inline std::ostream& operator<<(std::ostream& stream, const nodec::FOURCC& code) {
    return stream << code.bytes[0] << code.bytes[1] << code.bytes[2] << code.bytes[3];
}

}

#endif