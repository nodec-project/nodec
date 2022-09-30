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
    // utf8 <==> utf32
    out_utf32 = unicode::utf8to32<std::string>(utf8);
    assert(unicode::utf32to8<std::string>(out_utf32) == utf8);

    // utf16 <==> utf32
    out_utf16 = unicode::utf32to16<std::string>(out_utf32);
    assert(unicode::utf16to32<std::string>(out_utf16) == out_utf32);

    // utf16 <==> utf8
    assert(unicode::utf16to8<std::string>(out_utf16) == utf8);
    assert(unicode::utf8to16<std::string>(utf8) == out_utf16);
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
            //logging::InfoStream(__FILE__, __LINE__) << utf8;

            /*{
                std::string utf8("Hello, ワールド! 🎉");
                writefile("out.utf8", utf8);
                return 0;
            }*/

            //{
            //    std::string utf8 = readfile("hello_world.utf8");
            //    return 0;
            //}

            //std::u16string utf8(u"Hello, ワールド! 🎉");
            std::string utf8("Hello, ワールド! 🎉");
            {
                logging::InfoStream info(__FILE__, __LINE__);
                for (auto iter = utf8.begin(); iter != utf8.end(); ) {
                    info << std::hex << unicode::iterate_utf8(iter, utf8.end()) << " ";
                    //info << std::hex << unicode::iterate_utf16(iter, utf8.end()) << " ";
                }
            }
            {
                logging::InfoStream info(__FILE__, __LINE__);
                for (auto iter = utf8.cbegin(); iter != utf8.cend(); ) {
                    info << std::hex << unicode::iterate_utf8(iter, utf8.cend()) << " ";
                }
            }
            {
                auto pos = utf8.find("🎉");
                if (pos != utf8.npos) {
                    logging::InfoStream(__FILE__, __LINE__) << "Find tada!";

                    auto begin = utf8.begin() + pos;
                    auto iter = begin;
                    unicode::iterate_utf8(iter, utf8.end());

                    logging::InfoStream(__FILE__, __LINE__) << "tada size = " << iter - begin << " bytes";
                    logging::InfoStream(__FILE__, __LINE__) << "Replace tada with sushi.";

                    utf8.replace(begin, iter, "🍣");
                }
            }

            auto utf16 = unicode::utf8to16<std::wstring>(utf8);
            {
                logging::InfoStream info(__FILE__, __LINE__);
                for (auto c : utf16) {
                    info << std::hex << c << " ";
                }
            }
            {
                logging::InfoStream info(__FILE__, __LINE__);
                for (auto iter = utf16.begin(); iter != utf16.end();) {
                    info << std::hex << unicode::iterate_utf16(iter, utf16.end()) << " ";
                    //info << std::hex << unicode::iterate_utf8(iter, utf16.end()) << " ";
                }
            }

            auto utf32 = unicode::utf8to32<std::u32string>(utf8);
            {
                logging::InfoStream info(__FILE__, __LINE__);
                for (auto c : utf32) {
                    info << std::hex << c << " ";
                }
            }

            writefile("out.utf8", unicode::utf32to8<std::string>(utf32));
            writefile("out.utf16", unicode::utf32to16<std::string>(utf32));

            return 0;
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
        //    std::string out_utf16, out_utf32;

        //    test_bytes(readfile("characters.utf8"), out_utf16, out_utf32);
        //    writefile("characters.out.utf16", out_utf16);
        //    writefile("characters.out.utf32", out_utf32);

        //    test_bytes(readfile("emoji.utf8"), out_utf16, out_utf32);
        //    writefile("emoji.out.utf16", out_utf16);
        //    writefile("emoji.out.utf32", out_utf32);
        //}
    }
    catch (const std::exception& e) {
        logging::FatalStream(__FILE__, __LINE__) << e.what();
    }
    return 0;
}