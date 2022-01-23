#pragma once


#include <nodec/formatter.hpp>

#include <stdexcept>

#include <Windows.h>

namespace Exceptions {

inline void ThrowHrException(HRESULT hr, const char* file, size_t line) {
    using namespace nodec;

    throw std::runtime_error(
        ErrorFormatter<std::runtime_error>(file, line)
        << "[Error Code] 0x" << std::hex << std::uppercase << hr << std::dec
        << " (" << (unsigned long)hr << ")"
    );
}

constexpr void ThrowIfFailed(HRESULT hr, const char* file, size_t line) {
    if (FAILED(hr)) { ThrowHrException(hr, file, line); }
}



}