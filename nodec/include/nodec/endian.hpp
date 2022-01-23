#ifndef NODEC__ENDIAN_HPP_
#define NODEC__ENDIAN_HPP_

#include <cstddef>
#include <cstdint>


// refs of implementation
//  * <https://github.com/steinwurf/endian>
//  * <https://github.com/qt/qtbase/blob/6.3/src/corelib/global/qendian.h>

namespace nodec {
namespace endian {

namespace internal {

template<typename T, uint8_t Bytes = sizeof(T)>
struct little_endian_conv {
};

template<typename T>
struct little_endian_conv<T, 2> {
    static void to_host(const T& src, T& dest) {
        const auto* bytes = reinterpret_cast<const uint8_t*>(&src);
        auto* value = reinterpret_cast<uint16_t*>(&dest);
        *value = bytes[1] << 8 | bytes[0];
    }

    static void from_host(const T& src, T& dest) {
        const auto* value = reinterpret_cast<const uint16_t*>(&src);
        auto* bytes = reinterpret_cast<uint8_t*>(&dest);
        bytes[0] = (*value & 0x00ff);
        bytes[1] = (*value & 0xff00) >> 8;
    }
};

template<typename T>
struct little_endian_conv<T, 4> {
    static void to_host(const T& src, T& dest) {
        const auto* bytes = reinterpret_cast<const uint8_t*>(&src);
        auto* value = reinterpret_cast<uint32_t*>(&dest);
        *value = bytes[3] << 24 | bytes[2] << 16 | bytes[1] << 8 | bytes[0];
    }

    static void from_host(const T& src, T& dest) {
        const auto* value = reinterpret_cast<const uint32_t*>(&src);
        auto* bytes = reinterpret_cast<uint8_t*>(&dest);
        bytes[0] = (*value & 0x000000ff);
        bytes[1] = (*value & 0x0000ff00) >> 8;
        bytes[2] = (*value & 0x00ff0000) >> 16;
        bytes[3] = (*value & 0xff000000) >> 24;
    }
};

template<typename T>
struct little_endian_conv<T, 8> {
    static void to_host(const T& src, T& dest) {
        const auto* bytes = reinterpret_cast<const uint8_t*>(&src);
        auto* value = reinterpret_cast<uint64_t*>(&dest);
        *value = 0
            | (uint64_t)bytes[7] << 56 | (uint64_t)bytes[6] << 48 | (uint64_t)bytes[5] << 40 | (uint64_t)bytes[4] << 32
            | (uint64_t)bytes[3] << 24 | (uint64_t)bytes[2] << 16 | (uint64_t)bytes[1] << 8 | (uint64_t)bytes[0];
    }

    static void from_host(const T& src, T& dest) {
        const auto* value = reinterpret_cast<const uint64_t*>(&src);
        auto* bytes = reinterpret_cast<uint8_t*>(&dest);
        bytes[0] = (*value & 0x00000000000000ff);
        bytes[1] = (*value & 0x000000000000ff00) >> 8;
        bytes[2] = (*value & 0x0000000000ff0000) >> 16;
        bytes[3] = (*value & 0x00000000ff000000) >> 24;
        bytes[4] = (*value & 0x000000ff00000000) >> 32;
        bytes[5] = (*value & 0x0000ff0000000000) >> 40;
        bytes[6] = (*value & 0x00ff000000000000) >> 48;
        bytes[7] = (*value & 0xff00000000000000) >> 56;
    }
};

template<typename T, uint8_t Bytes = sizeof(T)>
struct big_endian_conv {

};

template<typename T>
struct big_endian_conv<T, 2> {
    static void to_host(const T& src, T& dest) {
        const auto* bytes = reinterpret_cast<const uint8_t*>(&src);
        auto* value = reinterpret_cast<uint16_t*>(&dest);
        *value = bytes[0] << 8 | bytes[1];
    }

    static void from_host(const T& src, T& dest) {
        const auto* value = reinterpret_cast<const uint16_t*>(&src);
        auto* bytes = reinterpret_cast<uint8_t*>(&dest);
        bytes[0] = (*value & 0xff00) >> 8;
        bytes[1] = (*value & 0x00ff);
    }
};

template<typename T>
struct big_endian_conv<T, 4> {
    static void to_host(const T& src, T& dest) {
        const auto* bytes = reinterpret_cast<const uint8_t*>(&src);
        auto* value = reinterpret_cast<uint32_t*>(&dest);
        *value = bytes[0] << 24 | bytes[1] << 16 | bytes[2] << 8 | bytes[3];
    }

    static void from_host(const T& src, T& dest) {
        const auto* value = reinterpret_cast<const uint32_t*>(&src);
        auto* bytes = reinterpret_cast<uint8_t*>(&dest);
        bytes[0] = (*value & 0xff000000) >> 24;
        bytes[1] = (*value & 0x00ff0000) >> 16;
        bytes[2] = (*value & 0x0000ff00) >> 8;
        bytes[3] = (*value & 0x000000ff);
    }
};

template<typename T>
struct big_endian_conv<T, 8> {
    static void to_host(const T& src, T& dest) {
        const auto* bytes = reinterpret_cast<const uint8_t*>(&src);
        auto* value = reinterpret_cast<uint64_t*>(&dest);
        *value = 0
            | (uint64_t)bytes[0] << 56 | (uint64_t)bytes[1] << 48 | (uint64_t)bytes[2] << 40 | (uint64_t)bytes[3] << 32
            | (uint64_t)bytes[4] << 24 | (uint64_t)bytes[5] << 16 | (uint64_t)bytes[6] << 8 | (uint64_t)bytes[7];
    }

    static void from_host(const T& src, T& dest) {
        const auto* value = reinterpret_cast<const uint64_t*>(&src);
        auto* bytes = reinterpret_cast<uint8_t*>(&dest);
        bytes[0] = (*value & 0xff00000000000000) >> 56;
        bytes[1] = (*value & 0x00ff000000000000) >> 48;
        bytes[2] = (*value & 0x0000ff0000000000) >> 40;
        bytes[3] = (*value & 0x000000ff00000000) >> 32;
        bytes[4] = (*value & 0x00000000ff000000) >> 24;
        bytes[5] = (*value & 0x0000000000ff0000) >> 16;
        bytes[6] = (*value & 0x000000000000ff00) >> 8;
        bytes[7] = (*value & 0x00000000000000ff);
    }
};

}

template<typename T>
T little_to_host(const T& src) {
    T dest{};
    internal::little_endian_conv<T>::to_host(src, dest);
    return dest;
}

template<typename T>
T host_to_little(const T& src) {
    T dest{};
    internal::little_endian_conv<T>::from_host(src, dest);
    return dest;
}
template<typename T>
T big_to_host(const T& src) {
    T dest{};
    internal::big_endian_conv<T>::to_host(src, dest);
    return dest;
}

template<typename T>
T host_to_big(const T& src) {
    T dest{};
    internal::big_endian_conv<T>::from_host(src, dest);
    return dest;
}

}
}

#endif