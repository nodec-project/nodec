#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <nodec/formatter.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>

TEST_CASE("Complex format chain debugging") {
    // 複雑なフォーマットチェーンのデバッグ
    std::string result = nodec::Formatter() 
        << "Value: " << std::hex << std::uppercase << std::showbase 
        << nodec::setfill('0') << nodec::setw(8) << 255
        << ", Float: " << std::dec << std::fixed << nodec::setprecision(2) << 3.14159
        << ", Bool: " << std::boolalpha << true;
    std::cout << "Formatter result: '" << result << "'" << std::endl;
    
    // ostringstream での同じチェーン
    std::ostringstream oss;
    oss << "Value: " << std::hex << std::uppercase << std::showbase 
        << std::setfill('0') << std::setw(8) << 255
        << ", Float: " << std::dec << std::fixed << std::setprecision(2) << 3.14159
        << ", Bool: " << std::boolalpha << true;
    std::string expected = oss.str();
    std::cout << "ostringstream result: '" << expected << "'" << std::endl;
    
    CHECK(result == expected);
}
