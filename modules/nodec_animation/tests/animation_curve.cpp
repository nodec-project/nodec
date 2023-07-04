#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec_animation/animation_curve.hpp>

#include <nodec/math/math.hpp>

TEST_CASE("testing add_keyframe()") {
    using namespace nodec_animation;

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

TEST_CASE("testing evaluate()") {
    using namespace nodec_animation;
    using namespace nodec::math;

    SUBCASE("Testing basic curve.") {
        AnimationCurve curve;

        curve.add_keyframe({0, 0.0f});
        curve.add_keyframe({500, 0.75f});
        curve.add_keyframe({1000, 1.0f});

        {
            auto result = curve.evaluate(0);
            CHECK(result.first == 0);
            CHECK(approx_equal(result.second, 0.0f));
        }
        {
            auto result = curve.evaluate(500);
            CHECK(result.first == 1);
            CHECK(approx_equal(result.second, 0.75f));
        }
        {
            auto result = curve.evaluate(1000);
            CHECK(result.first == 2);
            CHECK(approx_equal(result.second, 1.0f));
        }
        {
            auto result = curve.evaluate(100);
            CHECK(result.first == 0);
            CHECK(approx_equal(result.second, 0.75f / 500 * 100));
        }

        {
            auto result = curve.evaluate(10000);
            CHECK(result.first == 2);
            CHECK(approx_equal(result.second, 1.0f));
        }
    }

    SUBCASE("Testing curve with no keyframes.") {
        AnimationCurve curve;

        {
            auto result = curve.evaluate(0);
            CHECK(result.first == -1);
            CHECK(result.second == 0.0f);
        }
    }

    SUBCASE("Testing curve with 1 keyframe.") {
        AnimationCurve curve;
        curve.add_keyframe({100, 1.0f});

        {
            auto result = curve.evaluate(0);
            CHECK(result.first == 0);
            CHECK(result.second == 1.0f);
        }

        {
            auto result = curve.evaluate(100);
            CHECK(result.first == 0);
            CHECK(result.second == 1.0f);
        }

        {
            auto result = curve.evaluate(200);
            CHECK(result.first == 0);
            CHECK(result.second == 1.0f);
        }
    }

    SUBCASE("Testing curve with hint.") {
        AnimationCurve curve;

        curve.add_keyframe({0, 0.0f});
        curve.add_keyframe({50, 0.5f});
        curve.add_keyframe({100, 1.0f});

        int hint = -1;
        for (int t = 0; t <= 100; ++t) {
            CAPTURE(t);
            // std::cout << t << ", " << hint << std::endl;
            auto result = curve.evaluate(t, hint);
            CHECK(approx_equal(result.second, t / 100.f));
            hint = result.first;
        }
    }

    SUBCASE("Testing curve with loop.") {
        AnimationCurve curve;

        curve.add_keyframe({0, 0.0f});
        curve.add_keyframe({50, 0.5f});
        curve.add_keyframe({100, 1.0f});
        curve.set_wrap_mode(WrapMode::Loop);

        int hint = -1;
        for (int t = 0; t <= 500; ++t) {
            CAPTURE(t);
            // std::cout << t << ", " << hint << std::endl;
            auto result = curve.evaluate(t, hint);
            CHECK(approx_equal(result.second, (t % 100) / 100.f));
            hint = result.first;
        }
    }
}