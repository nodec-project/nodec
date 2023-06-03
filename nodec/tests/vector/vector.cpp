#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec/vector2.hpp>
#include <nodec/vector3.hpp>
#include <nodec/vector4.hpp>


template<typename T>
void test_operator_plus(const T& a, const T& b, const T& expected) {
    CHECK(a + b == expected);
    CHECK(b + a == expected);
}

TEST_CASE("testing operator+") {
    using namespace nodec;

    test_operator_plus(Vector2f(0.f, 1.f), Vector2f(10.f, 11.f), Vector2f(10.f, 12.f));
    test_operator_plus(Vector3f(0.f, 1.f, 2.f), Vector3f(10.f, 11.f, 12.f), Vector3f(10.f, 12.f, 14.f));
    test_operator_plus(Vector4f(0.f, 1.f, 2.f, 3.f), Vector4f(10.f, 11.f, 12.f, 13.f), Vector4f(10.f, 12.f, 14.f, 16.f));
}