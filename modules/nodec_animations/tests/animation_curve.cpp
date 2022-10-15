#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec_animations/animation_curve.hpp>

TEST_CASE("testing add_keyframe") {
    using namespace nodec_animations;

    AnimationCurve curve;

    curve.add_keyframe({0, 0.0f});
    curve.add_keyframe({1000, 1.0f});

    CHECK(curve[0].ticks == 0);
    CHECK(curve[1].ticks == 1000);

    curve.add_keyframe({500, 0.75f});

    CHECK(curve[0].ticks == 0);
    CHECK(curve[1].ticks == 500);    
    CHECK(curve[2].ticks == 1000);
}