#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec/formatter.hpp>
#include <iomanip>
#include <sstream>

TEST_CASE("Formatter - Enhanced Manipulator Support") {
    // setw テスト
    std::string result1 = nodec::Formatter() << nodec::setw(10) << 42;
    CHECK(result1 == "        42");  // 右揃え、幅10
    
    // setfill テスト
    std::string result2 = nodec::Formatter() << nodec::setfill('0') << nodec::setw(5) << 123;
    CHECK(result2 == "00123");
    
    // setprecision テスト
    std::string result3 = nodec::Formatter() << std::fixed << nodec::setprecision(2) << 3.14159;
    CHECK(result3 == "3.14");
    
    // 組み合わせテスト
    std::string result4 = nodec::Formatter() << nodec::setfill('*') << nodec::setw(8) << "Hi";
    CHECK(result4 == "******Hi");
}

TEST_CASE("Formatter - Number Base Support") {
    // 16進数テスト（小文字）
    std::string result1 = nodec::Formatter() << std::hex << 255;
    CHECK(result1 == "ff");
    
    // 16進数テスト（大文字）
    std::string result2 = nodec::Formatter() << std::hex << std::uppercase << 255;
    CHECK(result2 == "FF");
    
    // 16進数 with showbase
    std::string result3 = nodec::Formatter() << std::hex << std::showbase << 255;
    CHECK(result3 == "0xff");
    
    // 8進数テスト
    std::string result4 = nodec::Formatter() << std::oct << 64;
    CHECK(result4 == "100");
    
    // 8進数 with showbase
    std::string result5 = nodec::Formatter() << std::oct << std::showbase << 64;
    CHECK(result5 == "0100");
    
    // 10進数に戻す
    std::string result6 = nodec::Formatter() << std::hex << 16 << ", " << std::dec << 16;
    CHECK(result6 == "10, 16");
}

TEST_CASE("Formatter - Alignment Support") {
    // 左揃え
    std::string result1 = nodec::Formatter() << std::left << nodec::setw(8) << "Hi";
    CHECK(result1 == "Hi      ");
    
    // 右揃え（デフォルト）
    std::string result2 = nodec::Formatter() << std::right << nodec::setw(8) << "Hi";
    CHECK(result2 == "      Hi");
    
    // 内部揃え（符号付き数値）
    std::string result3 = nodec::Formatter() << std::internal << nodec::setfill('0') << nodec::setw(8) << -42;
    CHECK(result3 == "-0000042");
    
    // 内部揃え（16進数）
    std::string result4 = nodec::Formatter() << std::hex << std::showbase << std::internal 
                                           << nodec::setfill('0') << nodec::setw(8) << 255;
    CHECK(result4 == "0x0000ff");
}

TEST_CASE("Formatter - Bool Support") {
    // 数値としてのbool
    std::string result1 = nodec::Formatter() << true << ", " << false;
    CHECK(result1 == "1, 0");
    
    // boolalpha
    std::string result2 = nodec::Formatter() << std::boolalpha << true << ", " << false;
    CHECK(result2 == "true, false");
    
    // noboolalpha
    std::string result3 = nodec::Formatter() << std::boolalpha << true << ", " 
                                           << std::noboolalpha << false;
    CHECK(result3 == "true, 0");
}

TEST_CASE("Formatter - Float Formatting") {
    // fixed フォーマット
    std::string result1 = nodec::Formatter() << std::fixed << nodec::setprecision(3) << 3.14159;
    CHECK(result1 == "3.142");
    
    // scientific フォーマット
    std::string result2 = nodec::Formatter() << std::scientific << nodec::setprecision(2) << 1234.5;
    CHECK(result2 == "1.23e+03");
    
    // uppercase scientific
    std::string result3 = nodec::Formatter() << std::scientific << std::uppercase 
                                           << nodec::setprecision(2) << 1234.5;
    CHECK(result3 == "1.23E+03");
}

TEST_CASE("Formatter - ostringstream Compatibility Check") {
    // 同じフォーマットでostringreamと結果比較
    int value = 42;
    
    // Formatter
    std::string fmt_result = nodec::Formatter() << nodec::setfill('0') << nodec::setw(6) << value;
    
    // ostringstream
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(6) << value;
    std::string oss_result = oss.str();
    
    CHECK(fmt_result == oss_result);
    
    // 16進数比較
    std::string fmt_hex = nodec::Formatter() << std::hex << std::uppercase << 255;
    
    std::ostringstream oss_hex;
    oss_hex << std::hex << std::uppercase << 255;
    std::string oss_hex_result = oss_hex.str();
    
    CHECK(fmt_hex == oss_hex_result);
}

TEST_CASE("Formatter - Complex Format Chains") {
    // 複雑なフォーマットチェーン
    std::string result = nodec::Formatter() 
        << "Value: " << std::hex << std::uppercase << std::showbase 
        << nodec::setfill('0') << nodec::setw(8) << 255
        << ", Float: " << std::dec << std::fixed << nodec::setprecision(2) << 3.14159
        << ", Bool: " << std::boolalpha << true;
    
    // 期待値："Value: 00000XFF, Float: 3.14, Bool: true"
    std::string expected = "Value: 00000XFF, Float: 3.14, Bool: true";
    CHECK(result == expected);
}
