#include <nodec/unicode.hpp>

#include <array>


namespace nodec
{
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
    namespace unicode
    {
        

        namespace detail
        {

            /**
            * UTF-8 Number of bytes |   Byte 1  | ...
            * ----------------------+-----------+
            *            1          | 0xxx xxxx |
            *            2          | 110x xxxx |
            *            3          | 111x xxxx |
            *            4          | 1111 0xxx |
            */
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
                5,5,5,5,                                                           // 1111 1100 ~ 1111 1111
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
            uint32_t check_strict(bool strict)
            {
                /**
                * Specials is a short Unicode block allocated at the very end of the Basic Multilingual Plane, at U+FFF0–FFFF.
                * U+FFFD � REPLACEMENT CHARACTER used to replace an unknown, unrecognized or unrepresentable character
                */
                constexpr uint32_t replacement = 0x0000FFFD;
                if (strict)
                {
                    throw IllegalCharacterError(__FILE__, __LINE__);
                }
                return replacement;

            }

            /**/
            //template <typename Iter8>

            /*
            * @brief Convert UTF-8 character to UTF-32.
            */
            template <typename Iter8>
            uint32_t utf8to32(Iter8& begin, Iter8 end, bool strict)
            {
                uint32_t c = 0;


                uint8_t bytes = UTF8_BYTES[*begin];

                // check range
                if (begin + bytes >= end)
                {
                    // source buffer, check whether or not we have space to replace
                    throw IllegalCharacterError(__FILE__, __LINE__);
                }

                // get our UTF-32 character
                switch (bytes)
                {
                case 5:
                    c = check_strict(strict);
                    c <<= 6;
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
            * @tparam Iter8  pointer of uint8_t
            * @tparam Iter16 pointer of uint16_t
            */
            template <typename Iter8, typename Iter16>
            size_t utf8to16(Iter8 src_begin, Iter8 src_end, Iter16 dst_begin, Iter16 dst_end, bool strict = true)
            {
                auto src = src_begin;
                auto dst = dst_begin;
                while (src < src_end && dst < dst_end)
                {
                    utf8to32(src, src_end, strict)
                }
            }

            template<typename Iter8, typename Iter32>
            size_t utf8to32(Iter8 src_begin, Iter8 src_end, Iter32 dst_begin, Iter32 dst_end, bool strict = true)
            {
                auto src = src_begin;
                auto dst = dst_begin;
                while (src < src_end && dst < dst_end)
                {

                }
            }

        }


        template<typename C1, typename C2, typename Function>
        std::string to_wide(const std::string& string, Function function)
        {
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

            size_t out = function(src, src_end, dst, dst_end, true);

        }

        std::string utf8to16(const std::string& string)
        {
            using C1 = uint8_t;
            using C2 = uint16_t;
            using Function = decltype(detail::utf8to16<const C1*, C2*>);
            return to_wide<C1, C2, Function>(string, detail::utf8to16);
        }


    }

}