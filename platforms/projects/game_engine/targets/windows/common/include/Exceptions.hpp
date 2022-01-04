#pragma once

#include <nodec/error_formatter.hpp>

#include <stdexcept>

#include <Windows.h>

namespace Exceptions {

inline void ThrowHrException(HRESULT hr, const char* file, size_t line) {
    using namespace nodec;

    throw std::runtime_error(
        error_fomatter::with_type_file_line<std::runtime_error>(
            Formatter()
            << "[Error Code] 0x" << std::hex << std::uppercase << hr << std::dec
            << " (" << (unsigned long)hr << ")",
            file, line));
}

constexpr void ThrowIfFailed(HRESULT hr, const char* file, size_t line) {
    if (FAILED(hr)) { ThrowHrException(hr, file, line); }
}



}