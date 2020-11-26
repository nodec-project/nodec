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

        /**
        * @brief Convert UTF-8 string to UTF-16 string.
        */
        std::string utf8to16(const std::string& string, bool strict = true);

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
    }
}

#endif
