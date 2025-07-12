#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <iomanip>
#include <sstream>

#include <nodec/string_builder.hpp>

TEST_CASE("StringBuilder - Enhanced Manipulator Support") {
    // setw テスト
    std::string buffer1;
    nodec::StringBuilder stream1(buffer1);
    stream1 << std::setw(10) << 42;
    CHECK(buffer1 == "        42"); // 右揃え、幅10

    // setfill テスト
    std::string buffer2;
    nodec::StringBuilder stream2(buffer2);
    stream2 << std::setfill('0') << std::setw(5) << 123;
    CHECK(buffer2 == "00123");

    // setprecision テスト
    std::string buffer3;
    nodec::StringBuilder stream3(buffer3);
    stream3 << std::fixed << std::setprecision(2) << 3.14159;
    CHECK(buffer3 == "3.14");

    // 組み合わせテスト
    std::string buffer4;
    nodec::StringBuilder stream4(buffer4);
    stream4 << std::setfill('*') << std::setw(8) << "Hi";
    CHECK(buffer4 == "******Hi");
}

TEST_CASE("StringBuilder - Number Base Support") {
    // 16進数テスト（小文字）
    std::string buffer1;
    nodec::StringBuilder stream1(buffer1);
    stream1 << std::hex << 255;
    CHECK(buffer1 == "ff");

    // 16進数テスト（大文字）
    std::string buffer2;
    nodec::StringBuilder stream2(buffer2);
    stream2 << std::hex << std::uppercase << 255;
    CHECK(buffer2 == "FF");

    // 16進数 with showbase
    std::string buffer3;
    nodec::StringBuilder stream3(buffer3);
    stream3 << std::hex << std::showbase << 255;
    CHECK(buffer3 == "0xff");

    // 8進数テスト
    std::string buffer4;
    nodec::StringBuilder stream4(buffer4);
    stream4 << std::oct << 64;
    CHECK(buffer4 == "100");

    // 8進数 with showbase
    std::string buffer5;
    nodec::StringBuilder stream5(buffer5);
    stream5 << std::oct << std::showbase << 64;
    CHECK(buffer5 == "0100");

    // 10進数に戻す
    std::string buffer6;
    nodec::StringBuilder stream6(buffer6);
    stream6 << std::hex << 16 << ", " << std::dec << 16;
    CHECK(buffer6 == "10, 16");
}

TEST_CASE("StringBuilder - Alignment Support") {
    // 左揃え
    std::string buffer1;
    nodec::StringBuilder stream1(buffer1);
    stream1 << std::left << std::setw(8) << "Hi";
    CHECK(buffer1 == "Hi      ");

    // 右揃え（デフォルト）
    std::string buffer2;
    nodec::StringBuilder stream2(buffer2);
    stream2 << std::right << std::setw(8) << "Hi";
    CHECK(buffer2 == "      Hi");

    // 内部揃え（符号付き数値）
    std::string buffer3;
    nodec::StringBuilder stream3(buffer3);
    stream3 << std::internal << std::setfill('0') << std::setw(8) << -42;
    CHECK(buffer3 == "-0000042");

    // 内部揃え（16進数）
    std::string buffer4;
    nodec::StringBuilder stream4(buffer4);
    stream4 << std::hex << std::showbase << std::internal
            << std::setfill('0') << std::setw(8) << 255;
    CHECK(buffer4 == "0x0000ff");
}

TEST_CASE("StringBuilder - Bool Support") {
    // 数値としてのbool
    std::string buffer1;
    nodec::StringBuilder stream1(buffer1);
    stream1 << true << ", " << false;
    CHECK(buffer1 == "1, 0");

    // boolalpha
    std::string buffer2;
    nodec::StringBuilder stream2(buffer2);
    stream2 << std::boolalpha << true << ", " << false;
    CHECK(buffer2 == "true, false");

    // noboolalpha
    std::string buffer3;
    nodec::StringBuilder stream3(buffer3);
    stream3 << std::boolalpha << true << ", "
            << std::noboolalpha << false;
    CHECK(buffer3 == "true, 0");
}

TEST_CASE("StringBuilder - Float Formatting") {
    // fixed フォーマット
    std::string buffer1;
    nodec::StringBuilder stream1(buffer1);
    stream1 << std::fixed << std::setprecision(3) << 3.14159;
    CHECK(buffer1 == "3.142");

    // scientific フォーマット
    std::string buffer2;
    nodec::StringBuilder stream2(buffer2);
    stream2 << std::scientific << std::setprecision(2) << 1234.5;
    CHECK(buffer2 == "1.23e+03");

    // uppercase scientific
    std::string buffer3;
    nodec::StringBuilder stream3(buffer3);
    stream3 << std::scientific << std::uppercase
            << std::setprecision(2) << 1234.5;
    CHECK(buffer3 == "1.23E+03");
}

TEST_CASE("StringBuilder - ostringstream Compatibility Check") {
    // 同じフォーマットでostringreamと結果比較
    int value = 42;

    // StringBuilder
    std::string buffer1;
    nodec::StringBuilder stream1(buffer1);
    stream1 << std::setfill('0') << std::setw(6) << value;

    // ostringstream
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(6) << value;
    std::string oss_result = oss.str();

    CHECK(buffer1 == oss_result);

    // 16進数比較
    std::string buffer2;
    nodec::StringBuilder stream2(buffer2);
    stream2 << std::hex << std::uppercase << 255;

    std::ostringstream oss_hex;
    oss_hex << std::hex << std::uppercase << 255;
    std::string oss_hex_result = oss_hex.str();

    CHECK(buffer2 == oss_hex_result);
}

TEST_CASE("StringBuilder - Complex Format Chains") {
    // 複雑なフォーマットチェーン
    std::string buffer;
    nodec::StringBuilder stream(buffer);
    stream << "Value: " << std::hex << std::uppercase << std::showbase
           << std::setfill('0') << std::setw(8) << 255
           << ", Float: " << std::dec << std::fixed << std::setprecision(2) << 3.14159
           << ", Bool: " << std::boolalpha << true;

    std::string expected = "Value: 00000XFF, Float: 3.14, Bool: true";
    CHECK(buffer == expected);
}
