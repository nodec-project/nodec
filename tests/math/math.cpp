#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec/math/math.hpp>

TEST_CASE("testing approx_equal") {
    using namespace nodec::math;
    constexpr float NaN = std::numeric_limits<float>::quiet_NaN();
    constexpr float POSITIVE_INFINITY = std::numeric_limits<float>::infinity();
    constexpr float NEGATIVE_INFINITY = -std::numeric_limits<float>::infinity();
    constexpr float MAX_VALUE = std::numeric_limits<float>::max();
    const float MIN_VALUE = std::nextafter(0.0f, 1.0f);
    // NOTE: Significant digits
    //  float(float32): 6-7
    //  double(float64): 15-16
    //
    // implementation notes:
    //  * https://floating-point-gui.de/errors/NearlyEqualsTest.java

    SUBCASE("Regular large numbers") {
        CHECK(approx_equal(1000000.f, 1000001.f));
        CHECK(approx_equal(1000001.f, 1000000.f));
        CHECK(!approx_equal(10000.f, 10001.f));
        CHECK(!approx_equal(10001.f, 10000.f));
    }

    SUBCASE("Negative large numbers") {
        CHECK(approx_equal(-1000000.f, -1000001.f));
        CHECK(approx_equal(-1000001.f, -1000000.f));
        CHECK(!approx_equal(-10000.f, -10001.f));
        CHECK(!approx_equal(-10001.f, -10000.f));
    }

    SUBCASE("Numbers around 1") {
        CHECK(approx_equal(1.0000001f, 1.0000002f));
        CHECK(approx_equal(1.0000002f, 1.0000001f));
        CHECK(!approx_equal(1.0002f, 1.0001f));
        CHECK(!approx_equal(1.0001f, 1.0002f));
    }

    SUBCASE("Numbers around -1") {
        CHECK(approx_equal(-1.000001f, -1.000002f));
        CHECK(approx_equal(-1.000002f, -1.000001f));
        CHECK(!approx_equal(-1.0001f, -1.0002f));
        CHECK(!approx_equal(-1.0002f, -1.0001f));
    }

    SUBCASE("Number between 1 and 0") {
        CHECK(approx_equal(0.000000001000001f, 0.000000001000002f));
        CHECK(approx_equal(0.000000001000002f, 0.000000001000001f));
        CHECK(!approx_equal(0.000000000001002f, 0.000000000001001f));
        CHECK(!approx_equal(0.000000000001001f, 0.000000000001002f));
    }
    SUBCASE("Number between -1 and 0") {
        CHECK(approx_equal(-0.000000001000001f, -0.000000001000002f));
        CHECK(approx_equal(-0.000000001000002f, -0.000000001000001f));
        CHECK(!approx_equal(-0.000000000001002f, -0.000000000001001f));
        CHECK(!approx_equal(-0.000000000001001f, -0.000000000001002f));
    }

    SUBCASE("Small difference away from zero") {
        CHECK(approx_equal(0.3f, 0.30000003f));
        CHECK(approx_equal(-0.3f, -0.30000003f));
    }

    SUBCASE("Comparison involving zero") {
        CHECK(approx_equal(0.0f, 0.0f));
        CHECK(approx_equal(0.0f, -0.0f));
        CHECK(approx_equal(-0.0f, -0.0f));
        CHECK(!approx_equal(0.00000001f, 0.0f));
        CHECK(!approx_equal(0.0f, 0.00000001f));
        CHECK(!approx_equal(-0.00000001f, 0.0f));
        CHECK(!approx_equal(0.0f, -0.00000001f));

        CHECK(approx_equal(0.0f, 1e-40f, 0.01f));
        CHECK(approx_equal(1e-40f, 0.0f, 0.01f));
        CHECK(!approx_equal(1e-40f, 0.0f, 0.000001f, 0.0f));
        CHECK(!approx_equal(0.0f, 1e-40f, 0.000001f, 0.0f));

        CHECK(approx_equal(0.0f, -1e-40f, 0.1f));
        CHECK(approx_equal(-1e-40f, 0.0f, 0.1f));
        CHECK(!approx_equal(-1e-40f, 0.0f, 0.00000001f, 0.0f));
        CHECK(!approx_equal(0.0f, -1e-40f, 0.00000001f, 0.0f));

        CHECK(approx_equal(0.0f, 1e-40f));
        CHECK(approx_equal(1e-40f, 0.0f));
    }

    SUBCASE("Comparison involving extreme values (overflow potential)") {
        CHECK(approx_equal(MAX_VALUE, MAX_VALUE));
        CHECK(!approx_equal(MAX_VALUE, -MAX_VALUE));
        CHECK(!approx_equal(-MAX_VALUE, MAX_VALUE));
        CHECK(!approx_equal(MAX_VALUE, MAX_VALUE / 2));
        CHECK(!approx_equal(MAX_VALUE, -MAX_VALUE / 2));
        CHECK(!approx_equal(-MAX_VALUE, MAX_VALUE / 2));
    }

    SUBCASE("Comparison involving infinities") {
        CHECK(approx_equal(POSITIVE_INFINITY, POSITIVE_INFINITY));
        CHECK(approx_equal(-POSITIVE_INFINITY, -POSITIVE_INFINITY));
        CHECK(!approx_equal(-POSITIVE_INFINITY, POSITIVE_INFINITY));
        CHECK(!approx_equal(POSITIVE_INFINITY, MAX_VALUE));
        CHECK(!approx_equal(-POSITIVE_INFINITY, -MAX_VALUE));
    }
    SUBCASE("Comparisons involving NaN values") {
        CHECK(!approx_equal(NaN, NaN));
        CHECK(!approx_equal(NaN, 0.0f));
        CHECK(!approx_equal(-0.0f, NaN));
        CHECK(!approx_equal(NaN, -0.0f));
        CHECK(!approx_equal(0.0f, NaN));
        CHECK(!approx_equal(NaN, POSITIVE_INFINITY));
        CHECK(!approx_equal(POSITIVE_INFINITY, NaN));
        CHECK(!approx_equal(NaN, NEGATIVE_INFINITY));
        CHECK(!approx_equal(NEGATIVE_INFINITY, NaN));
        CHECK(!approx_equal(NaN, MAX_VALUE));
        CHECK(!approx_equal(MAX_VALUE, NaN));
        CHECK(!approx_equal(NaN, -MAX_VALUE));
        CHECK(!approx_equal(-MAX_VALUE, NaN));
        CHECK(!approx_equal(NaN, MIN_VALUE));
        CHECK(!approx_equal(MIN_VALUE, NaN));
        CHECK(!approx_equal(NaN, -MIN_VALUE));
        CHECK(!approx_equal(-MIN_VALUE, NaN));
    }

    SUBCASE("Comparisons of numbers on opposite sides of 0") {
        CHECK(!approx_equal(1.000000001f, -1.0f));
        CHECK(!approx_equal(-1.0f, 1.000000001f));
        CHECK(!approx_equal(-1.000000001f, 1.0f));
        CHECK(!approx_equal(1.0f, -1.000000001f));
        CHECK(approx_equal(10 * MIN_VALUE, 10 * -MIN_VALUE));
        CHECK(!approx_equal(10000 * MIN_VALUE, 10000 * -MIN_VALUE, default_rel_tol<float>, 0.0f));
    }

    SUBCASE("The really tricky part - comparisons of numbers very close to zero.") {
        CHECK(approx_equal(MIN_VALUE, MIN_VALUE));
        CHECK(approx_equal(MIN_VALUE, -MIN_VALUE));
        CHECK(approx_equal(-MIN_VALUE, MIN_VALUE));
        CHECK(approx_equal(MIN_VALUE, 0.f));
        CHECK(approx_equal(0.f, MIN_VALUE));
        CHECK(approx_equal(-MIN_VALUE, 0.f));
        CHECK(approx_equal(0.f, -MIN_VALUE));

        CHECK(!approx_equal(0.000000001f, -MIN_VALUE));
        CHECK(!approx_equal(0.000000001f, MIN_VALUE));
        CHECK(!approx_equal(MIN_VALUE, 0.000000001f));
        CHECK(!approx_equal(-MIN_VALUE, 0.000000001f));
    }
}

TEST_CASE("testing matrix inv.") {
    using namespace nodec;

    {
        Matrix4x4f mat{
            5, -2, 2, 7,
            1, 0, 0, 3,
            -3, 1, 5, 0,
            3, -1, -9, 4};

        float determinant;

        auto inv = math::inv(mat, &determinant);

        CHECK(math::approx_equal(mat * inv, Matrix4x4f::identity, math::default_rel_tol<float>, 0.000001f));
        CHECK(math::approx_equal(determinant, 88.f));
    }

    {
        Matrix4x4f mat{
            0, 1, 2, 3,
            4, 5, 6, 7,
            8, 9, 10, 11,
            12, 13, 14, 15};

        float determinant;

        auto inv = math::inv(mat, &determinant);

        CHECK(math::approx_equal(determinant, 0.f));
    }
}

TEST_CASE("testing cross()") {
    using namespace nodec;

    Vector3i a(1, 2, 3);
    Vector3i b(4, 5, 6);

    auto c = math::cross(a, b);

    CHECK(c.x == -3);
    CHECK(c.y == 6);
    CHECK(c.z == -3);
}
