#include <nodec/unicode.hpp>
#include <nodec/logging.hpp>

//#undef NDEBUG
//#define NDEBUG

#include <cassert>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <tuple>

using namespace nodec;

/*
* @brief Test conversion of UTF-8 to and from all UTF encodings.
*/
void test_bytes(const std::string& utf8, std::string& out_utf16, std::string& out_utf32) {
    //std::cout << utf8;
    // utf8 <==> utf32
    out_utf32 = nodec::unicode::utf8to32(utf8);
    assert(nodec::unicode::utf32to8(out_utf32) == utf8);

    //std::cout << out_utf32;
    // utf16 <==> utf32
    out_utf16 = nodec::unicode::utf32to16(out_utf32);
    assert(nodec::unicode::utf16to32(out_utf16) == out_utf32);

    // utf16 <==> utf8
    assert(nodec::unicode::utf16to8(out_utf16) == utf8);
    assert(nodec::unicode::utf8to16(utf8) == out_utf16);
}

/*
* @brief Read bytes from file.
*/
std::string readfile(const std::string& path) {
    std::ifstream stream(path, std::ios::binary);
    std::stringstream buffer;
    buffer << stream.rdbuf();
    return buffer.str();
}

void writefile(const std::string& path, const std::string& string) {
    std::ofstream stream(path, std::ios::binary);
    stream.write(string.data(), string.size());
}

int main(int argc, char* argv[]) {
    logging::record_handlers().connect(logging::record_to_stdout_handler);

    try {
        {
        }
        //{
        //    std::u16string utf16(u"ハロー、World 🎉 from ソース.");
        //    for (auto iter = utf16.begin(); iter != utf16.end(); ++iter) {
        //        
        //        logging::InfoStream(__FILE__, __LINE__) << *iter;

        //    }
        //    return 0;
        //}

        //{
        //    char16_t utf16[] = u"ハロー、World 🎉 from ソース.";
        //    std::string utf16_bytes(reinterpret_cast<const char*>(utf16), (std::size(utf16) - 1) * 2);

        //    std::string utf8_bytes = unicode::utf16to8(utf16_bytes);
        //    writefile("out.utf8", utf8_bytes);
        //    return 0;
        //}
        //{
        //    //writefile("hello_world_from_source.utf8", reinterpret_cast<const char*>(u8"ハロー、World 🎉 from ソース.")); // C++20
        //    writefile("hello_world_from_source.utf8", u8"ハロー、World 🎉 from ソース.");
        //    return 0;
        //}
        //{
        //    std::string out_utf16, out_utf32;
        //    test_bytes(readfile("hello_world_from_file.utf8"), out_utf16, out_utf32);
        //    writefile("hello_world_from_file.out.utf16", out_utf16);
        //    writefile("hello_world_from_file.out.utf32", out_utf32);
        //    return 0;
        //}
        //{
        //    std::string utf8_string;
        //    utf8_string = readfile("hello_world_from_file.utf8");
        //    return 0;
        //}

        //{
        //    std::string out_utf16, out_utf32;


        //    test_bytes(readfile("characters.utf8"), out_utf16, out_utf32);
        //    //test_bytes("HELLO", out_utf16, out_utf32);
        //    writefile("characters.out.utf16", out_utf16);
        //    writefile("characters.out.utf32", out_utf32);


        //    test_bytes(readfile("emoji.utf8"), out_utf16, out_utf32);
        //    //test_bytes("HELLO", out_utf16, out_utf32);
        //    writefile("emoji.out.utf16", out_utf16);
        //    writefile("emoji.out.utf32", out_utf32);
        //    //std::cout << out_utf16;
        //}
    }
    catch (const std::exception& e) {
        logging::FatalStream(__FILE__, __LINE__) << e.what();
    }
    return 0;
}