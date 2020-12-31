#include <nodec/unicode.hpp>

#undef NDEBUG
//#define NDEBUG
#include <cassert>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <tuple>

/*
* @brief Test conversion of UTF-8 to and from all UUTF encodings.
*/
void test_bytes(const std::string& utf8, std::string& out_utf16, std::string& out_utf32)
{
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
std::string readfile(const std::string& path)
{
    std::ifstream stream(path, std::ios::binary);
    std::stringstream buffer;
    buffer << stream.rdbuf();
    return buffer.str();
}

void writefile(const std::string& path, const std::string& string)
{
    std::ofstream stream(path, std::ios::binary);
    stream.write(string.data(), string.size());
}

int main(int argc, char* argv[])
{
    std::string out_utf16, out_utf32;

    try
    {
        test_bytes(readfile("characters.utf8"), out_utf16, out_utf32);
        //test_bytes("HELLO", out_utf16, out_utf32);
        writefile("characters.out.utf16", out_utf16);
        writefile("characters.out.utf32", out_utf32);


        test_bytes(readfile("emoji.utf8"), out_utf16, out_utf32);
        //test_bytes("HELLO", out_utf16, out_utf32);
        writefile("emoji.out.utf16", out_utf16);
        writefile("emoji.out.utf32", out_utf32);
        //std::cout << out_utf16;
    }
    catch (const nodec::NodecException& e)
    {
        std::wcout << e.what() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "Standard Exception: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "No detail available" << std::endl;
    }
    return 0;
}