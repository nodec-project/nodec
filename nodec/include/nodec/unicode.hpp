#ifndef NODEC__UNICODE_HPP_
#define NODEC__UNICODE_HPP_

#include <nodec/nodec_exception.hpp>

#include <string>

namespace nodec
{
    namespace unicode
    {
        class IllegalCharacterError : public NodecException
        {
        public:
            IllegalCharacterError(const char* file, size_t line) noexcept
                :NodecException("Illegal character found during conversion.", file, line)
            {
            };

            const char* type() { return "IllegalCharacterError"; }
        };

        class BufferRangeError : public NodecException
        {
        public:
            BufferRangeError(const char* file, size_t line) noexcept
                :NodecException("Cannot add characters to buffer, output is too small.", file, line)
            {
            };
            const char* type() { return "BufferRangeError"; }
        };

        std::string utf8to16(const std::string& string);
    }
}

#endif
