#ifndef NODEC__UNICODE_HPP_
#define NODEC__UNICODE_HPP_

#include <nodec/formatter.hpp>

#include <stdexcept>
#include <iostream>
#include <array>
#include <string>

namespace nodec {

/**
* @note
*   This code is based on the followings. Thank you!
*   * <https://github.com/Alexhuszagh/UTFPP>
*/
namespace unicode {


namespace details {

inline void throw_illegal_character_exception(const char* file, size_t line) {
    throw std::runtime_error(ErrorFormatter<std::runtime_error>(file, line)
        << "Illegal character found."
    );
}


inline void throw_buffer_range_exception(const char* file, size_t line) {
    throw std::runtime_error(ErrorFormatter<std::runtime_error>(file, line)
        << "Cannot add characters to buffer, output is too small."
    );
}


extern const std::array<uint8_t, 7> FIRST_BYTE_MARK;
extern const std::array<uint8_t, 256> UTF8_BYTES;
extern const std::array<uint32_t, 6> UTF8_OFFSETS;

/**
* @brief Replace illegal Unicode character if checkStrict is off.
*/
uint32_t check_strict(bool strict);


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


template<typename C1, typename C2, typename Function, typename DstString, typename SrcString>
DstString to_wide(const SrcString& string, Function function, bool strict = true) {
    // types
    using SrcChar = typename SrcString::value_type;
    using DstChar = typename DstString::value_type;

    constexpr size_t c1_size = sizeof(C1);
    constexpr size_t c2_size = sizeof(C2);

    constexpr size_t src_char_size = sizeof(SrcChar); // the character size of SrcString
    constexpr size_t dst_char_size = sizeof(DstChar); // the character size of DstString

    constexpr size_t src_chars_per_unit = c1_size / src_char_size;
    constexpr size_t dst_chars_per_unit = c2_size / dst_char_size;

    static_assert(src_chars_per_unit > 0 && dst_chars_per_unit > 0,
        "The character size of the string must be set so that it is exactly divisible by the size of the code unit.");

    //arguments
    const size_t srclen = string.size() / src_chars_per_unit; // source length (the number of code units)
    const size_t dstlen = srclen;                             // destination length
    auto* src = reinterpret_cast<const C1*>(string.data());
    auto* src_end = src + srclen;
    auto* dst = reinterpret_cast<C2*>(malloc(dstlen * c2_size));
    auto* dst_end = dst + dstlen;

    size_t out = function(src, src_end, dst, dst_end, strict);
    DstString output(reinterpret_cast<const DstChar*>(dst), out * dst_chars_per_unit);
    free(dst);

    return output;
}

template <typename C1, typename C2, typename Function, typename DstString, typename SrcString>
DstString to_narrow(const SrcString& string, Function function, bool strict = true) {
    // types
    using SrcChar = typename SrcString::value_type;
    using DstChar = typename DstString::value_type;

    constexpr size_t c1_size = sizeof(C1);
    constexpr size_t c2_size = sizeof(C2);

    constexpr size_t src_char_size = sizeof(SrcChar); // the character size of SrcString
    constexpr size_t dst_char_size = sizeof(DstChar); // the character size of DstString

    constexpr size_t src_chars_per_unit = c1_size / src_char_size;
    constexpr size_t dst_chars_per_unit = c2_size / dst_char_size;

    static_assert(src_chars_per_unit > 0 && dst_chars_per_unit > 0,
        "The character size of the string must be set so that it is exactly divisible by the size of the code unit.");

    // arguments
    const size_t srclen = string.size() / src_chars_per_unit; // source length (the number of code units)
    const size_t dstlen = srclen * 4;                         // destination length
    auto* src = reinterpret_cast<const C1*>(string.data());
    auto* src_end = src + srclen;
    auto* dst = reinterpret_cast<C2*>(malloc(dstlen * c2_size));
    auto* dst_end = dst + dstlen;

    size_t out = function(src, src_end, dst, dst_end, strict);
    DstString output(reinterpret_cast<const DstChar*>(dst), out * dst_chars_per_unit);
    free(dst);

    return output;
}


template<typename It>
class IteratorAdapter : public It {
public:
    using value_type = std::make_unsigned_t<typename It::value_type>;

public:
    IteratorAdapter(const It& it) {
        It::operator=(it);
    }

    value_type operator*() const noexcept {
        return static_cast<value_type>(It::operator*());
    }

    IteratorAdapter operator++(int) noexcept {
        auto tmp = *this;
        It::operator++();
        return tmp;
    }

    IteratorAdapter operator--(int) noexcept {
        auto tmp = *this;
        It::operator--();
        return tmp;
    }
};


} // namespace details


/**
* @brief Convert UTF-8 string to UTF-16 string.
*/
template<typename U16String, typename U8String>
U16String utf8to16(const U8String& string, bool strict = true) {
    // types
    using C1 = uint8_t;
    using C2 = uint16_t;
    using Function = decltype(details::utf8to16<const C1*, C2*>);

    return details::to_wide<C1, C2, Function, U16String>(string, details::utf8to16, strict);
}


/**
* @brief Convert UTF-16 string to UTF-32 string.
*/
template<typename U32String, typename U16String>
U32String utf16to32(const U16String& string, bool strict = true) {
    // types
    using C1 = uint16_t;
    using C2 = uint32_t;
    using Function = decltype(details::utf16to32<const C1*, C2*>);

    return details::to_wide<C1, C2, Function, U32String>(string, details::utf16to32, strict);
}


/**
* @brief Convert UTF-8 string to UTF-32 string.
*/
template<typename U32String, typename U8String>
U32String utf8to32(const U8String& string, bool strict = true) {
    // types
    using C1 = uint8_t;
    using C2 = uint32_t;
    using Function = decltype(details::utf8to32<const C1*, C2*>);

    return details::to_wide<C1, C2, Function, U32String>(string, details::utf8to32, strict);
}


/**
* @brief Convert UTF-16 string to UTF-8 string.
*/
template<typename U8String, typename U16String>
U8String utf16to8(const U16String& string, bool strict = true) {
    // types
    using C1 = uint16_t;
    using C2 = uint8_t;
    using Function = decltype(details::utf16to8<const C1*, C2*>);

    return details::to_narrow<C1, C2, Function, U8String>(string, details::utf16to8, strict);
}


/**
* @brief Convert UTF-32 string to UTF-16 string.
*/
template<typename U16String, typename U32String>
U16String utf32to16(const U32String& string, bool strict = true) {
    // types
    using C1 = uint32_t;
    using C2 = uint16_t;
    using Function = decltype(details::utf32to16<const C1*, C2*>);

    return details::to_narrow<C1, C2, Function, U16String>(string, details::utf32to16, strict);
}


/**
* @brief Convert UTF-32 string to UTF-8 string.
*/
template<typename U8String, typename U32String>
U8String utf32to8(const U32String& string, bool strict = true) {
    // types
    using C1 = uint32_t;
    using C2 = uint8_t;
    using Function = decltype(details::utf32to8<const C1*, C2*>);

    return details::to_narrow<C1, C2, Function, U8String>(string, details::utf32to8, strict);
}


/**
* @param iter
*   Iterator with an iterate size of 1 byte.
*   After function execution, this iterator points to the first code unit of the next code point.
*
* @param end
*   The end iterator of the string.
*
* @param strict
*   If enabled, raise an exception when an illegal character is found.
*
* @return code point
*/
template<typename Iter8>
uint32_t iterate_utf8(Iter8& iter, const Iter8& end, bool strict = true) {
    static_assert(sizeof(typename Iter8::value_type) == 1, "The element size of a UTF-8 iterator must be one byte.");

    details::IteratorAdapter<Iter8> iter_adapter(iter);
    auto code_point = details::code_point_utf8to32(iter_adapter,
        details::IteratorAdapter<Iter8>(end),
        strict);

    iter = iter_adapter;
    return code_point;
}


/**
* @param iter
*   Iterator with an iterate size of 2 bytes.
*   After function execution, this iterator points to the first code unit of the next code point.
*
* @param end
*   The end iterator of the string.
*
* @param strict
*   If enabled, raise an exception when an illegal character is found.
*
* @return code point
*/
template<typename Iter16>
uint32_t iterate_utf16(Iter16& iter, const Iter16& end, bool strict = true) {
    static_assert(sizeof(typename Iter16::value_type) == 2, "The element size of a UTF-16 iterator must be two bytes.");

    details::IteratorAdapter<Iter16> iter_adapter(iter);
    auto code_point = details::code_point_utf16to32(iter_adapter,
        details::IteratorAdapter<Iter16>(end),
        strict);
    iter = iter_adapter;
    return code_point;
}

} // namespace unicode
}

#endif
