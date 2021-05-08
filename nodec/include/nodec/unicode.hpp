#ifndef NODEC__UNICODE_HPP_
#define NODEC__UNICODE_HPP_

#include <nodec/error_formatter.hpp>

#include <stdexcept>
#include <iostream>
#include <array>
#include <string>

namespace nodec {
namespace unicode {

class IllegalCharacterException : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class BufferRangeException : public  std::runtime_error {
public:
    using runtime_error::runtime_error;
};


namespace details {

inline void throw_illegal_character_exception(const char* file, size_t line) {
    throw std::runtime_error(error_fomatter::type_file_line<std::runtime_error>(
        "Illegal character found.",
        file, line
        ));
}


inline void throw_buffer_range_exception(const char* file, size_t line) {
    throw std::runtime_error(error_fomatter::type_file_line<std::runtime_error>(
        "Cannot add characters to buffer, output is too small.",
        file, line
        ));
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
    
    constexpr size_t src_csize = sizeof(SrcChar); // the size of CharT in SrcString 
    constexpr size_t dst_csize = sizeof(DstChar); // the size of CharT in DstString

    static_assert(src_csize <= c1_size && dst_csize <= c2_size, "A");

    //arguments
    const size_t srclen = string.size() * src_csize / c1_size; // source length (the number of code units)
    const size_t dstlen = srclen;                              // destination length
    auto* src = reinterpret_cast<const C1*>(string.data());
    auto* src_end = src + srclen;
    auto* dst = reinterpret_cast<C2*>(malloc(dstlen * c2_size));
    auto* dst_end = dst + dstlen;

    size_t out = function(src, src_end, dst, dst_end, strict);
    DstString output(reinterpret_cast<const DstChar*>(dst), out * c2_size / dst_csize);
    free(dst);

    return output;
}

//
//template<typename CodeUnit>
//struct IterateFunctions {};
//
//template<>
//struct IterateFunctions<uint8_t> {
//    static uint32_t iterate(const uint8_t*& iter, const uint8_t* end) {
//        return code_point_utf8to32(iter, end, true);
//    }
//};
//
//template<>
//struct IterateFunctions<uint16_t> {
//    static uint32_t iterate(const uint16_t*& iter, const uint16_t* end) {
//        return code_point_utf16to32(iter, end, true);
//    }
//};
//
//template<>
//struct IterateFunctions<uint32_t> {
//    static uint32_t iterate(const uint32_t*& iter, const uint32_t* end) {
//        if (iter >= end) {
//            details::throw_illegal_character_exception(__FILE__, __LINE__);
//        }
//        return *iter++;
//    }
//};
//

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

    return details::to_wide<C1, C2, Function>(string, details::utf8to16, strict);
}

/**
* @brief Convert UTF-16 string to UTF-8 string.
*/
std::string utf16to8(const std::string& string, bool strict = true);

/**
* @brief Convert UTF-16 string to UTF-32 string.
*/
std::string utf16to32(const std::string& string, bool strict = true);

/**
* @brief Convert UTF-32 string to UTF-16 string.
*/
std::string utf32to16(const std::string& string, bool strict = true);

/**
* @brief Convert UTF-8 string to UTF-32 string.
*/
std::string utf8to32(const std::string& string, bool strict = true);

/**
* @brief Convert UTF-32 string to UTF-8 string.
*/
std::string utf32to8(const std::string& string, bool strict = true);


//
//struct CodePoint {
//    uint32_t value{ 0 };
//    const char bytes[4];
//
//    ////const char[5] bytes{};
//    //std::string bytes;
//};
//
//template <typename C>
//class CodePointViewIterator {
//public:
//    using CodeUnit = C;
//
//    using value_type = uint32_t;
//    using pointer = const value_type*;
//    using reference = const value_type&;
//
//    /**
//    * @brief Construct end of iterator.
//    */
//    CodePointViewIterator()
//        : iter(nullptr), end(nullptr) {
//    }
//
//    CodePointViewIterator(const CodeUnit* iter, const CodeUnit* end)
//        : iter(iter), end(end) {
//    }
//
//
//    /**
//    * @brief Construct start of iterator.
//    */
//    Iterator(const std::string& string) {
//        const size_t length = string.size() / sizeof(CodeUnitType);
//        iter = reinterpret_cast<const CodeUnitType*>(string.data());
//        end = iter + length;
//        iterate();
//    }
//
//    Iterator(const Iterator& iterator)
//        : iter(iterator.iter)
//        , end(iterator.end)
//        , code_point(iterator.code_point) {
//    }
//
//    CodePointViewIterator& operator=(const CodePointViewIterator&) = default;
//    ~CodePointViewIterator() = default;
//
//    reference operator*() const {
//        return code_point;
//    }
//
//    pointer operator->() const {
//        return &code_point;
//    }
//
//
//    /**
//    * @note Prefix ++ overload.
//    */
//    Iterator& operator++() {
//        iterate();
//        return *this;
//    }
//
//    /**
//    * @note Postfix ++ overload.
//    */
//    Iterator operator++(int) {
//        auto tmp = *this;
//        iterate();
//        return tmp;
//    }
//
//
//private:
//    void iterate() {
//        if (iter && iter >= end) {
//            iter = nullptr;
//            return;
//        }
//
//        auto byte_begin = iter;
//        code_point.code_point = details::IterateFunctions<CodeUnitType>::iterate(iter, end);
//        code_point.bytes.assign(reinterpret_cast<const char*>(byte_begin), (iter - byte_begin) * sizeof(CodeUnitType));
//    }
//
//    bool equal(const Iterator& other) const {
//        return iter == other.iter;
//    }
//
//    /**
//    * @return true if the iterators refer to the same code unit,
//    *   or are both at end-of-string.
//    */
//    friend bool operator== (const Iterator& left, const Iterator& right) {
//        return left.equal(right);
//    }
//
//    /**
//    * @return true if the iterators refer to different code unit,
//    *   or if one is at end-of-string and the other is not.
//    */
//    friend bool operator!= (const Iterator& left, const Iterator& right) {
//        return !left.equal(right);
//    }
//
//private:
//    // const pointer:
//    //  A pointer to const data does not allow modification of the data through the pointer.
//
//    const CodeUnit* iter;
//    const CodeUnit* end;
//    uint32_t code_point;
//};
//
//template<typename C>
//class BasicCodePointView {
//public:
//    using CodeUnit = C;
//
//    using iterator = CodePointViewIterator<C>;
//    using const_iterator = iterator;
//
//    BasicCodePointView(const std::string& str) {
//        size_t length = str.size() / sizeof(CodeUnitType);
//        iter = reinterpret_cast<const CodeUnitType*>(string.data());
//        end = iter + length;
//        iterate();
//    }
//
//    BasicCodePointView(const char* str, size_t len) {
//
//    }
//
//    const_iterator begin() const noexcept {
//
//    }
//
//    const_iterator end() const noexcept {
//
//    }
//
//private:
//
//
//
//};
//
//using UTF8CodePoint = CodePoint<uint8_t>;
//using UTF16CodePoint = CodePoint<uint16_t>;
//using UTF32CodePoint = CodePoint<uint32_t>;
//
//using UTF8Iterator = Iterator<uint8_t>;
//using UTF16Iterator = Iterator<uint16_t>;
//using UTF32Iterator = Iterator<uint32_t>;
//
//using UTF8CodePointView = CodePointView<UTF8Iterator>;

} // namespace unicode
}

#endif
