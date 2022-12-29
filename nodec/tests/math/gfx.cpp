#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec/math/gfx.hpp>

TEST_CASE("Testing decompose_trs.") {
    using namespace nodec;

    Vector3f t_expected(1.f, 2.f, 3.f);
    Quaternionf r_expected = math::gfx::quatenion_from_angle_axis(30.f, Vector3f(1.f, 0.f, 0.f));
    Vector3f s_expected(11.f, 13.f, 15.f);

    auto trs = nodec::math::gfx::trs(t_expected, r_expected, s_expected);

    Vector3f t;
    Quaternionf r;
    Vector3f s;
    CHECK(math::gfx::decompose_trs(trs, t, r, s));

    CHECK(t == t_expected);
    CHECK(s == s_expected);
    CHECK(r == r_expected);
}