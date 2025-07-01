#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <iostream>
#include <sstream>
#include <iomanip>

TEST_CASE("ostringstream behavior verification") {
    std::ostringstream oss;
    oss << std::hex << std::uppercase << std::showbase << std::setfill('0') << std::setw(8) << 255;
    std::string result = oss.str();
    std::cout << "ostringstream result: " << result << std::endl;
    
    // 期待される動作をここで確認
    CHECK(result.length() > 0);
}
