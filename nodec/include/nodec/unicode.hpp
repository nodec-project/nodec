#ifndef NODEC__UNICODE_HPP_
#define NODEC__UNICODE_HPP_

#include <nodec/nodec_exception.hpp>

#include <iostream>
#include <string>

namespace nodec
{
    namespace unicode
    {
        class IllegalCharacterException : public NodecException
        {
        public:
            IllegalCharacterException(const char* file, size_t line) noexcept
                :NodecException("Illegal character found.", file, line)
            {
            };

            const char* type() { return "IllegalCharacterException"; }
        };

        class BufferRangeException : public NodecException
        {
        public:
            BufferRangeException(const char* file, size_t line) noexcept
                :NodecException("Cannot add characters to buffer, output is too small.", file, line)
            {
            };
            const char* type() { return "BufferRangeException"; }
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

        namespace detail
        {
            template<typename Iter16>
            uint32_t code_point_utf16to32(Iter16& begin, Iter16 end, bool strict);

            template <typename Iter8>
            uint32_t code_point_utf8to32(Iter8& begin, Iter8 end, bool strict);

            template<typename CodeUnitType>
            struct IterateFunctions {};

            template<>
            struct IterateFunctions<uint32_t>
            {
                using CodeUnitType = uint32_t;
                static uint32_t iterate(const uint32_t*& iter, const uint32_t* end)
                {
                    if (iter >= end)
                    {
                        throw IllegalCharacterException(__FILE__, __LINE__);
                    }
                    return *iter++;
                }
            };

            template<>
            struct IterateFunctions<uint8_t>
            {
                using CodeUnitType = uint8_t;
                static uint32_t iterate(const uint8_t*& iter, const uint8_t* end)
                {
                    return code_point_utf8to32(iter, end, true);
                }
            };

            template<>
            struct IterateFunctions<uint16_t>
            {
                using CodeUnitType = uint16_t;
                static uint32_t iterate(const uint16_t*& iter, const uint16_t* end)
                {
                    return code_point_utf16to32(iter, end, true);
                }
            };

        }

        template <typename C>
        struct CodePoint
        {
            using CodeUnitType = C;

            uint32_t code_point = 0;
            std::string bytes;
        };

        template <typename C>
        class Iterator
        {
        public:
            using CodeUnitType = typename detail::IterateFunctions<C>::CodeUnitType;

            /**
            * @brief Construct end of iterator.
            */
            Iterator()
                :
                iter(nullptr),
                end(nullptr)
            {
            }

            /**
            * @brief Construct start of iterator.
            */
            Iterator(const std::string& string)
            {
                const size_t length = string.size() / sizeof(CodeUnitType);
                iter = reinterpret_cast<const CodeUnitType*>(string.data());
                end = iter + length;
                iterate();
            }

            Iterator(const Iterator& iterator)
                :
                iter(iterator.iter),
                end(iterator.end),
                code_point(iterator.code_point)
            {
            }

            Iterator& operator=(const Iterator&) = default;
            ~Iterator() = default;

            const CodePoint<CodeUnitType>& operator*() const
            {
                return code_point;
            }

            const CodePoint<CodeUnitType>* operator->() const
            {
                return std::addressof(operator*());
            }

            /**
            * @note Prefix ++ overload.
            */
            Iterator& operator++()
            {
                iterate();
                return *this;
            }

            /**
            * @note Postfix ++ overload.
            */
            Iterator operator++(int)
            {
                auto tmp = *this;
                iterate();
                return tmp;
            }


        private:
            void iterate()
            {
                if (iter && iter >= end)
                {
                    iter = nullptr;
                    return;
                }

                auto byte_begin = iter;
                code_point.code_point = detail::IterateFunctions<CodeUnitType>::iterate(iter, end);
                code_point.bytes.assign(reinterpret_cast<const char*>(byte_begin), (iter - byte_begin) * sizeof(CodeUnitType));
            }

            bool equal(const Iterator& other) const
            {
                return iter == other.iter;
            }

            /**
            * @return true if the iterators refer to the same code unit,
            *   or are both at end-of-string.
            */
            friend bool operator== (const Iterator& left, const Iterator& right)
            {
                return left.equal(right);
            }

            /**
            * @return true if the iterators refer to different code unit,
            *   or if one is at end-of-string and the other is not.
            */
            friend bool operator!= (const Iterator& left, const Iterator& right)
            {
                return !left.equal(right);
            }

            // const pointer:
            //  A pointer to const data does not allow modification of the data through the pointer.

            const CodeUnitType* iter;
            const CodeUnitType* end;
            CodePoint<CodeUnitType> code_point;
        };

        using UTF8Iterator = Iterator<uint8_t>;
        using UTF16Iterator = Iterator<uint16_t>;
        using UTF32Iterator = Iterator<uint32_t>;

        using UTF8CodePoint = CodePoint<uint8_t>;
        using UTF16CodePoint = CodePoint<uint16_t>;
        using UTF32CodePoint = CodePoint<uint32_t>;
    }
}

#endif
