#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <nodec/formatter.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>

TEST_CASE("Detailed hex format debugging") {
    // Formatterでのテスト
    nodec::Formatter fmt;
    std::string result = fmt << std::hex << std::uppercase << std::showbase 
                             << nodec::setfill('0') << nodec::setw(8) << 255;
    std::cout << "Formatter result: '" << result << "'" << std::endl;
    
    // ostringstreamでのテスト
    std::ostringstream oss;
    oss << std::hex << std::uppercase << std::showbase << std::setfill('0') << std::setw(8) << 255;
    std::string expected = oss.str();
    std::cout << "ostringstream result: '" << expected << "'" << std::endl;
    
    CHECK(result == expected);
}

TEST_CASE("Step-by-step hex formatting") {
    // ステップごとにフォーマットを確認
    nodec::Formatter fmt;
    
    // 1. 基本の16進数
    std::string result1 = nodec::Formatter() << std::hex << 255;
    std::cout << "hex only: '" << result1 << "'" << std::endl;
    
    // 2. 大文字
    std::string result2 = nodec::Formatter() << std::hex << std::uppercase << 255;
    std::cout << "hex + uppercase: '" << result2 << "'" << std::endl;
    
    // 3. showbaseを追加
    std::string result3 = nodec::Formatter() << std::hex << std::uppercase << std::showbase << 255;
    std::cout << "hex + uppercase + showbase: '" << result3 << "'" << std::endl;
    
    // 4. setfillとsetwを追加
    std::string result4 = nodec::Formatter() << std::hex << std::uppercase << std::showbase 
                                             << nodec::setfill('0') << nodec::setw(8) << 255;
    std::cout << "full format: '" << result4 << "'" << std::endl;
}
