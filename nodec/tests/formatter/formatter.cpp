#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec/formatter.hpp>

TEST_CASE("Testing Formatter") {
    std::string str = nodec::Formatter() << "A"
                                         << "B";
    CHECK(str == "AB");
}