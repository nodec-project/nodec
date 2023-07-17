#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec/quaternion.hpp>

TEST_CASE("testing operator+") {
    using namespace nodec;

    CHECK_EQ(Quaternionf(0.f, 1.f, 2.f, 3.f) + Quaternionf(10.f, 11.f, 12.f, 13.f), Quaternionf(10.f, 12.f, 14.f, 16.f));
}

TEST_CASE("Arithmetic operations between quaternion and scalar value") {
    using namespace nodec;

    const Quaternionf q(1.f, 2.f, 3.f, 4.f);
    const float s = 2.f;

    SUBCASE("quaternion * scalar") {
        const auto result = q * s;
        CHECK(result.x == q.x * s);
        CHECK(result.y == q.y * s);
        CHECK(result.z == q.z * s);
        CHECK(result.w == q.w * s);
    }

    SUBCASE("scalar * quaternion") {
        const auto result = s * q;
        CHECK(result.x == q.x * s);
        CHECK(result.y == q.y * s);
        CHECK(result.z == q.z * s);
        CHECK(result.w == q.w * s);
    }

    SUBCASE("quaternion *= scalar") {
        auto result = q;
        result *= s;
        CHECK(result.x == q.x * s);
        CHECK(result.y == q.y * s);
        CHECK(result.z == q.z * s);
        CHECK(result.w == q.w * s);
    }

    SUBCASE("quaternion / scalar") {
        const auto result = q / s;
        CHECK(result.x == q.x / s);
        CHECK(result.y == q.y / s);
        CHECK(result.z == q.z / s);
        CHECK(result.w == q.w / s);
    }

    SUBCASE("quaternion /= scalar") {
        auto result = q;
        result /= s;
        CHECK(result.x == q.x / s);
        CHECK(result.y == q.y / s);
        CHECK(result.z == q.z / s);
        CHECK(result.w == q.w / s);
    }
}