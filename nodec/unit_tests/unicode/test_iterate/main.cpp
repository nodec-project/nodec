#include <nodec/unicode.hpp>
#include <nodec/logging.hpp>

#include <algorithm>
#include <iostream>
#include <string>

using namespace nodec;

int main()
{
    logging::record_handlers().connect(logging::record_to_stdout_handler);
    //std::string_view
    //char16_t* test = u"test";

    //"test" == "test1";

    uint32_t a = 0x01;

    auto b = u8'a';
    std::string;
    if (a == b) {

    }
    try
    {
        std::string utf8 = u8"ハロー、World 🎉.";


        //auto* test = "test";
        //auto* test1 = "test1";
        ////test1 = test;
        //if (test == test1) {

        //}


        // ---

        nodec::unicode::UTF8View view(utf8);

        nodec::unicode::UTF8Iterator utf8_iterator(utf8);
        nodec::unicode::UTF8Iterator utf8_iterator_last;

        std::for_each(utf8_iterator, utf8_iterator_last, [](nodec::unicode::UTF8CodePoint code_point)
            {
                //std::cout << ": " << std::endl;
                std::cout << code_point.code_point << std::endl;
                if (code_point.bytes == "あ")
                {   
                    std::cout << "EQ" << std::endl;
                }
            });
        //while (utf8_iterator != utf8_iterator_last)
        //{
        //    std::cout << (*utf8_iterator).code_point << std::endl;
        //    if (utf8_iterator->bytes == "あ")
        //    {
        //        std::cout << "EQ" << std::endl;
        //    }
        //    utf8_iterator++;
        //}

        std::cout << std::endl;

        // ---

        std::string utf16 = nodec::unicode::utf8to16(utf8);

        nodec::unicode::UTF16Iterator utf16_iterator(utf16);
        nodec::unicode::UTF16Iterator utf16_iterator_last;

        //while (utf16_iterator != utf16_iterator_last)
        //{
        //    std::cout << (*utf16_iterator).code_point << std::endl;
        //    if (utf16_iterator->bytes == "あ")
        //    {
        //        std::cout << "EQ" << std::endl;
        //    }
        //    utf16_iterator++;
        //}
        std::for_each(utf16_iterator, utf16_iterator_last, [](nodec::unicode::UTF16CodePoint code_point)
            {
                //std::cout << ": "  << std::endl;
                std::cout << code_point.code_point << std::endl;
                if (code_point.bytes == nodec::unicode::utf8to16("✔"))
                {
                    std::cout << "EQ" << std::endl;
                }
            });
        std::cout << std::endl;

        // ---

        std::string utf32 = nodec::unicode::utf8to32(utf8);

        nodec::unicode::UTF32Iterator utf32_iterator(utf32);
        nodec::unicode::UTF32Iterator utf32_iterator_last;

        while (utf32_iterator != utf32_iterator_last)
        {
            std::cout << (*utf32_iterator).code_point << std::endl;
            utf32_iterator++;
        }

        //std::cout << utf32_iterator.code_point() << std::endl;
    }

    catch (const std::exception& e)
    {
        logging::FatalStream(__FILE__, __LINE__) << e.what();
    }
    return 0;
}