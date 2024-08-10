#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec/vector2.hpp>
#include <nodec/vector3.hpp>
#include <nodec/vector4.hpp>


TEST_CASE("Testing operator+") {
    using namespace nodec;

    CHECK_EQ(Vector2f(0.f, 1.f) + Vector2f(10.f, 11.f), Vector2f(10.f, 12.f));
    CHECK_EQ(Vector3f(0.f, 1.f, 2.f) + Vector3f(10.f, 11.f, 12.f), Vector3f(10.f, 12.f, 14.f));
    CHECK_EQ(Vector4f(0.f, 1.f, 2.f, 3.f) + Vector4f(10.f, 11.f, 12.f, 13.f), Vector4f(10.f, 12.f, 14.f, 16.f));
}

TEST_CASE("Testing constants") {
    using namespace nodec;

    CHECK(Vector2f::ones == Vector2f(1.f, 1.f));
    CHECK(Vector3f::ones == Vector3f(1.f, 1.f, 1.f));
    CHECK(Vector4f::ones == Vector4f(1.f, 1.f, 1.f, 1.f));
}

TEST_CASE("Testing constructor") {
    using namespace nodec;

    static_assert(Vector3f(1.f, 2.f, 3.f).x == 1.f);
}