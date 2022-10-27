#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec/type_info.hpp>

TEST_CASE("Testing type_id.") {
    const int value = 42;

    CHECK(nodec::type_id(value) == nodec::type_id<int>());
    CHECK(nodec::type_id(42) == nodec::type_id<int>());

    CHECK(nodec::type_id<int>() == nodec::type_id<int>());
    CHECK(nodec::type_id<int &>() == nodec::type_id<int &&>());
    CHECK(nodec::type_id<int &>() == nodec::type_id<int>());
    CHECK(nodec::type_id<int>() != nodec::type_id<char>());

    CHECK(&nodec::type_id<int>() == &nodec::type_id<int>());
    CHECK(&nodec::type_id<int>() != &nodec::type_id<void>());
}