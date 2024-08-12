#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec/unicode.hpp>

#include <fstream>
#include <string>

#ifndef TEST_DATA_DIR
#    define TEST_DATA_DIR ""
#endif

std::string readfile(const std::string &path) {
    std::ifstream stream(path, std::ios::binary);
    std::stringstream buffer;
    buffer << stream.rdbuf();
    return buffer.str();
}

TEST_CASE("Test conversion of UTF-8 to and from all UTF encodings.") {
    using namespace nodec;

    SUBCASE("characters.utf8") {
        // utf8 <==> utf32
        auto utf8 = readfile(std::string(TEST_DATA_DIR) + "/characters.utf8");
        auto utf32 = unicode::utf8to32<std::string>(utf8);
        CHECK(unicode::utf32to8<std::string>(utf32) == utf8);

        // utf16 <==> utf32
        auto utf16 = unicode::utf32to16<std::string>(utf32);
        CHECK(unicode::utf16to32<std::string>(utf16) == utf32);

        // utf16 <==> utf8
        CHECK(unicode::utf16to8<std::string>(utf16) == utf8);
        CHECK(unicode::utf8to16<std::string>(utf8) == utf16);
    }

    SUBCASE("emoji.utf8") {
        // utf8 <==> utf32
        auto utf8 = readfile(std::string(TEST_DATA_DIR) + "/emoji.utf8");
        auto utf32 = unicode::utf8to32<std::string>(utf8);
        CHECK(unicode::utf32to8<std::string>(utf32) == utf8);

        // utf16 <==> utf32
        auto utf16 = unicode::utf32to16<std::string>(utf32);
        CHECK(unicode::utf16to32<std::string>(utf16) == utf32);

        // utf16 <==> utf8
        CHECK(unicode::utf16to8<std::string>(utf16) == utf8);
        CHECK(unicode::utf8to16<std::string>(utf8) == utf16);
    }
}