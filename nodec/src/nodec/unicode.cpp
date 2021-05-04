#include <nodec/unicode.hpp>

#include <array>


namespace nodec {
/*
* Unicode
*   "
*       The Unicode Standard defines a codespace of numerical values
*       ranging from 0 through 10FFFF16, called code points and
*       denoted as U+0000 through U+10FFFF respectively.
*       <https://en.wikipedia.org/wiki/Unicode>
*   "
*
*/
namespace unicode {


namespace details {

/**
* UTF-8
*   The x characters are replaced by the bits of the code point.
*
*   |UTF-8 Number of bytes |  Byte 1   |  Byte 2   |  Byte 3   |  Byte 4   |
*   +----------------------+-----------+-----------+-----------+-----------+
*   |           1          | 0xxx xxxx |           |           |           |
*   |           2          | 110x xxxx | 10xx xxxx |           |           |
*   |           3          | 111x xxxx | 10xx xxxx | 10xx xxxx |           |
*   |           4          | 1111 0xxx | 10xx xxxx | 10xx xxxx | 10xx xxxx |
*
*   <https://en.wikipedia.org/wiki/UTF-8>
*/

/**
* UTF-16
*   U+0000 to U+D7FF and U+E000 to U+FFFF
*       Both UTF-16 and UCS-2 encode code points in this range as single 16-bit code units
*       that are numerically equal to the corresponding code points.
*   Code points from U+010000 to U+10FFFF
*       U' = yyyyyyyyyyxxxxxxxxxx  // U - 0x10000
*       W1 = 110110yyyyyyyyyy      // 0xD800 + yyyyyyyyyy
*       W2 = 110111xxxxxxxxxx      // 0xDC00 + xxxxxxxxxx
*   U+D800 to U+DFFF
*       The Unicode standard permanently reserves these code point values
*       for UTF-16 encoding of the high and low surrogates,
*       and they will never be assigned a character,
*       so there should be no reason to encode them.
*
*   | U+0000   |
*   |   ~      |
*   | U+D7FF   |
*   | U+D800   | high_begin
*   |   ~      |
*   | U+DBFF   | hight_end
*   | U+DC00   | low_begin
*   |   ~      |
*   | U+DFFF   | low_end
*   | U+E000   |
*   |   ~      |
*   | U+FFFF   | max_bmp
*   |   ~      |
*   | U+10FFFF |
*
*   <https://en.wikipedia.org/wiki/UTF-16>
*/

const std::array<uint8_t, 7> FIRST_BYTE_MARK = {
    0x00, // 0: 0000 0000
    0x00, // 1: 0000 0000
    0xC0, // 2: 1100 0000
    0xE0, // 3: 1110 0000
    0xF0, // 4: 1111 0000
    0xF8, // 5: 1111 1000
    0xFC  // 6: 1111 1100
};

const std::array<uint8_t, 256> UTF8_BYTES = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0000 0000 ~ 
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0010 0000 ~ 
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0100 0000 ~ 
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0110 0000 ~ 
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 1000 0000 ~ 
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 1010 0000 ~ 
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 1100 0000 ~ 
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,                                   // 1110 0000 ~ 
    3,3,3,3,3,3,3,3,                                                   // 1111 0000 ~
    4,4,4,4,                                                           // 1111 1000 ~
    5,5,5,5                                                            // 1111 1100 ~ 1111 1111
};

const std::array<uint32_t, 6> UTF8_OFFSETS = {
    0x00000000UL, // 0: 0000 0000 0000 0000 0000 0000 0000 0000
    0x00003080UL, // 1: 0000 0000 0000 0000 0011 0000 1000 0000
    0x000E2080UL, // 2: 0000 0000 0000 1110 0010 0000 1000 0000
    0x03C82080UL, // 3: 0000 0011 1100 1000 0010 0000 1000 0000
    0xFA082080UL, // 4: 1111 1010 0000 1000 0010 0000 1000 0000
    0x82082080UL  // 5: 1000 0010 0000 1000 0010 0000 1000 0000
};

/**
* @brief Replace illegal Unicode character if checkStrict is off.
*/
uint32_t check_strict(bool strict) {
    /**
    * Specials is a short Unicode block allocated at the very end of the Basic Multilingual Plane, at U+FFF0–FFFF.
    * U+FFFD REPLACEMENT CHARACTER used to replace an unknown, unrecognized or unrepresentable character
    */
    constexpr uint32_t replacement = 0x0000FFFD;
    if (strict) {
        details::throw_illegal_character_exception(__FILE__, __LINE__);
    }
    return replacement;

}

/**
* @brief Convert UTF-32 character to UTF-16.
*/
template <typename Iter16>
void code_point_utf32to16(uint32_t c, Iter16& begin, Iter16& end, bool strict) {
    // limits
    constexpr uint32_t max_utf32 = 0x0010FFFF;  //! max unicode code space size
    constexpr uint32_t high_begin = 0xD800;     //! 1101 1000 0000 0000
    constexpr uint32_t low_begin = 0xDC00;      //! 1101 1100 0000 0000
    constexpr uint32_t max_bmp = 0x0000FFFF;    //! Basic Multilingual Plane (BMP)
    constexpr int      shift = 10;
    constexpr uint32_t base = 0x0010000UL;      //! 0001 0000 0000 0000 0000
    constexpr uint32_t mask = 0x3FFUL;          //! 0000 0011 1111 1111

    // variables
    if (c <= max_bmp) {
        if (c >= high_begin && c <= low_begin) {
            *begin++ = check_strict(strict);
        }
        else {
            *begin++ = static_cast<uint16_t>(c);
        }
    }
    else if (c > max_utf32) {
        *begin++ = check_strict(strict);
    }
    else {
        if (begin + 1 > end) {
            details::throw_buffer_range_exception(__FILE__, __LINE__);
        }

        c -= base;
        *begin++ = static_cast<uint16_t>((c >> shift) + high_begin);
        *begin++ = static_cast<uint16_t>((c & mask) + low_begin);
    }
}

/**
* @brief Convert UTF-32 character to UTF-8.
*/
template <typename Iter8>
void code_point_utf32to8(uint32_t c, Iter8& begin, Iter8 end, bool strict) {
    // limits
    constexpr uint32_t max_utf32 = 0x0010FFFF;
    constexpr uint32_t byte_mark = 0x80; //! 1000 0000
    constexpr uint32_t byte_mask = 0xBF; //! 1011 1111

    // caliculate bytes to write
    short bytes;
    if (c < 0x80)         // 1000 0000
    {
        bytes = 1;
    }
    else if (c < 0x800)   // 1000 0000 0000
    {
        bytes = 2;
    }
    else if (c < 0x10000) // 0001 0000 0000 0000 0000
    {
        bytes = 3;
    }
    else if (c <= max_utf32) {
        bytes = 4;
    }
    else {
        bytes = 3;
        c = check_strict(strict);
    }

    // check range
    if (begin + bytes > end) {
        details::throw_buffer_range_exception(__FILE__, __LINE__);
    }

    // write to buffer
    begin += bytes;
    switch (bytes) {
    case 4:
        *--begin = static_cast<uint8_t>((c | byte_mark) & byte_mask);
        c >>= 6;
    case 3:
        *--begin = static_cast<uint8_t>((c | byte_mark) & byte_mask);
        c >>= 6;
    case 2:
        *--begin = static_cast<uint8_t>((c | byte_mark) & byte_mask);
        c >>= 6;
    case 1:
        *--begin = static_cast<uint8_t>(c | FIRST_BYTE_MARK[bytes]);
    }
    begin += bytes;
}

/**
* @brief Convert UTF-16 character to UTF-32.
*/
template<typename Iter16>
uint32_t code_point_utf16to32(Iter16& begin, Iter16 end, bool strict) {
    // limits
    constexpr uint32_t high_begin = 0xD800;
    constexpr uint32_t high_end = 0xDBFF;
    constexpr uint32_t low_begin = 0xDC00;
    constexpr uint32_t low_end = 0xDFFF;
    constexpr int      shift = 10;
    constexpr uint32_t base = 0x0010000UL;

    const uint32_t c1 = *begin++;
    if (c1 >= high_begin && c1 <= high_end) {
        // surrogate pair
        const uint32_t c2 = *begin++;
        if (c2 >= low_begin && c2 <= low_end) {
            return ((c1 - high_begin) << shift) + (c2 - low_begin) + base;
        }
        else {
            return check_strict(strict);
        }
    }
    else if (c1 >= low_begin && c1 <= low_end) {
        return check_strict(strict);
    }
    else {
        return c1;
    }
}

/**
* @brief Convert UTF-8 character to UTF-32.
*/
template <typename Iter8>
uint32_t code_point_utf8to32(Iter8& begin, Iter8 end, bool strict) {
    uint32_t c = 0;
    uint8_t bytes = UTF8_BYTES[*begin];

    // check range
    if (begin + bytes >= end) {
        // source buffer, check whether or not we have space to replace
        details::throw_illegal_character_exception(__FILE__, __LINE__);
    }

    // get our UTF-32 character
    switch (bytes) {
    case 5:
        c = check_strict(strict);
        c <<= 6;
        // No break. Continue to fall down.
    case 4:
        c = check_strict(strict);
        c <<= 6;
    case 3:
        c += *begin++; // uint32_t = uint32_t + uint8_t
        c <<= 6;
    case 2:
        c += *begin++;
        c <<= 6;
    case 1:
        c += *begin++;
        c <<= 6;
    case 0:
        c += *begin++;
    }
    c -= UTF8_OFFSETS[bytes];

    return c;
}

/**
* @brief Convert UTF-32 to UTF-16
* @return Number of bytes written to dst.
*/
template<typename Iter32, typename Iter16>
size_t utf32to16(Iter32 src_begin, Iter32 src_end, Iter16 dst_begin, Iter16 dst_end, bool strict = true) {
    auto src = src_begin;
    auto dst = dst_begin;
    while (src < src_end && dst < dst_end) {
        code_point_utf32to16(*src++, dst, dst_end, strict);
    }

    return dst - dst_begin;
}

/**
* @brief Convert UTF-16 to UTF-32.
* @return Number of bytes written to dst.
*/
template<typename Iter16, typename Iter32>
size_t utf16to32(Iter16 src_begin, Iter16 src_end, Iter32 dst_begin, Iter32 dst_end, bool strict) {
    auto src = src_begin;
    auto dst = dst_begin;
    while (src < src_end && dst < dst_end) {
        *dst++ = code_point_utf16to32(src, src_end, strict);
    }

    return dst - dst_begin;
}

/**
* @brief Convert UTF-8 to UTF-16.
* @return Number of bytes written to dst.
* @tparam Iter8  pointer of uint8_t.
* @tparam Iter16 pointer of uint16_t.
*/
template <typename Iter8, typename Iter16>
size_t utf8to16(Iter8 src_begin, Iter8 src_end, Iter16 dst_begin, Iter16 dst_end, bool strict = true) {
    auto src = src_begin;
    auto dst = dst_begin;
    while (src < src_end && dst < dst_end) {
        code_point_utf32to16(code_point_utf8to32(src, src_end, strict), dst, dst_end, strict);
    }

    return dst - dst_begin;
}

/**
* @brief Convert UTF-16 to UTF-8.
* @return Number of bytes written to dst.
*/
template <typename Iter16, typename Iter8>
size_t utf16to8(Iter16 src_begin, Iter16 src_end, Iter8 dst_begin, Iter8 dst_end, bool strict = true) {
    auto src = src_begin;
    auto dst = dst_begin;
    while (src < src_end && dst < dst_end) {
        code_point_utf32to8(code_point_utf16to32(src, src_end, strict), dst, dst_end, strict);
    }

    return dst - dst_begin;
}


/**
* @brief Convert UTF-8 to UTF-32.
* @return Number of bytes written to dst.
*/
template<typename Iter8, typename Iter32>
size_t utf8to32(Iter8 src_begin, Iter8 src_end, Iter32 dst_begin, Iter32 dst_end, bool strict = true) {
    auto src = src_begin;
    auto dst = dst_begin;
    while (src < src_end && dst < dst_end) {
        *dst++ = code_point_utf8to32(src, src_end, strict);
    }

    return dst - dst_begin;
}

/**
* @brief Convert UTF-32 to UTF-8.
* @return Number of bytes written to dst.
*/
template<typename Iter32, typename Iter8>
size_t utf32to8(Iter32 src_begin, Iter32 src_end, Iter8 dst_begin, Iter8 dst_end, bool strict = true) {
    auto src = src_begin;
    auto dst = dst_begin;
    while (src < src_end && dst < dst_end) {
        code_point_utf32to8(*src++, dst, dst_end, strict);
    }

    return dst - dst_begin;
}

}


template<typename C1, typename C2, typename Function>
std::string to_wide(const std::string& string, Function function, bool strict = true) {
    // types
    constexpr size_t size1 = sizeof(C1);
    constexpr size_t size2 = sizeof(C2);

    //arguments
    const size_t srclen = string.size() / size1; // source length
    const size_t dstlen = srclen;                // destination length
    auto* src = reinterpret_cast<const C1*>(string.data());
    auto* src_end = src + srclen;
    auto* dst = reinterpret_cast<C2*>(malloc(dstlen * size2));
    auto* dst_end = dst + dstlen;

    size_t out = function(src, src_end, dst, dst_end, strict);
    std::string output(reinterpret_cast<const char*>(dst), out * size2);
    free(dst);

    return output;
}

template <typename C1, typename C2, typename Function>
std::string to_narrow(const std::string& string, Function function, bool strict = true) {
    // types
    constexpr size_t size1 = sizeof(C1);
    constexpr size_t size2 = sizeof(C2);

    // arguments
    const size_t srclen = string.size() / size1;
    const size_t dstlen = srclen * 4;
    auto* src = reinterpret_cast<const C1*>(string.data());
    auto* src_end = src + srclen;
    auto* dst = reinterpret_cast<C2*>(malloc(dstlen * size2));
    auto* dst_end = dst + dstlen;

    size_t out = function(src, src_end, dst, dst_end, strict);
    std::string output(reinterpret_cast<const char*>(dst), out * size2);
    free(dst);

    return output;
}

std::string utf8to16(const std::string& string, bool strict) {
    // types
    using C1 = uint8_t;
    using C2 = uint16_t;
    using Function = decltype(details::utf8to16<const C1*, C2*>);

    return to_wide<C1, C2, Function>(string, details::utf8to16, strict);
}

std::string utf16to8(const std::string& string, bool strict) {
    // types
    using C1 = uint16_t;
    using C2 = uint8_t;
    using Function = decltype(details::utf16to8<const C1*, C2*>);

    return to_narrow<C1, C2, Function>(string, details::utf16to8, strict);
}


std::string utf16to32(const std::string& string, bool strict) {
    // types
    using C1 = uint16_t;
    using C2 = uint32_t;
    using Function = decltype(details::utf16to32<const C1*, C2*>);

    return to_wide<C1, C2, Function>(string, details::utf16to32, strict);
}

std::string utf32to16(const std::string& string, bool strict) {
    // types
    using C1 = uint32_t;
    using C2 = uint16_t;
    using Function = decltype(details::utf32to16<const C1*, C2*>);

    return to_narrow<C1, C2, Function>(string, details::utf32to16, strict);
}


std::string utf8to32(const std::string& string, bool strict) {
    // types
    using C1 = uint8_t;
    using C2 = uint32_t;
    using Function = decltype(details::utf8to32<const C1*, C2*>);

    return to_wide<C1, C2, Function>(string, details::utf8to32, strict);
}

std::string utf32to8(const std::string& string, bool strict) {
    // types
    using C1 = uint32_t;
    using C2 = uint8_t;
    using Function = decltype(details::utf32to8<const C1*, C2*>);

    return to_narrow<C1, C2, Function>(string, details::utf32to8, strict);
}

}

}