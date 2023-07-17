#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec/utility.hpp>

#include <type_traits>

TEST_CASE("Testing to_underlying.") {
    // https://en.cppreference.com/w/cpp/utility/to_underlying

    enum class E1 : char { e };
    static_assert(std::is_same<char, decltype(nodec::to_underlying(E1::e))>::value, "failed.");

    enum struct E2 : long { e };
    static_assert(std::is_same<long, decltype(nodec::to_underlying(E2::e))>::value, "failed.");

    enum E3 : unsigned { e };
    static_assert(std::is_same<unsigned, decltype(nodec::to_underlying(e))>::value, "failed.");
}