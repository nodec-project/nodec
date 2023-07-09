#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec/ranges.hpp>
#include <nodec_animation/resources/animation_clip.hpp>

#include <algorithm>
#include <vector>
// #include <ranges>

struct ComponentA {
    int prop;
};
struct ComponentB {
    int prop;
};

TEST_CASE("Testing set_curve") {
    using namespace nodec_animation::resources;
    using namespace nodec_animation;

    SUBCASE("root path") {
        AnimationClip clip;
        clip.set_curve<ComponentA>("", "prop", AnimationCurve{});
        clip.set_curve<ComponentB>("/", "prop", AnimationCurve{});

        CHECK(clip.root_entity().children.size() == 0);
        CHECK(clip.root_entity().components.size() == 2);
    }

    SUBCASE("child paths") {
        AnimationClip clip;
        clip.set_curve<ComponentA>("a", "prop", AnimationCurve{});
        clip.set_curve<ComponentA>("b", "prop", AnimationCurve{});
        clip.set_curve<ComponentA>("a/a", "prop", AnimationCurve{});

        CHECK(clip.root_entity().children.size() == 2);
        CHECK(clip.root_entity().components.size() == 0);

        CHECK(clip.root_entity().child("a").child("a").components.size() == 1);
    }

    // auto split_string = [](const std::string &s, const char *delim) {
    //     // https://gist.github.com/ScottHutchinson/6b699c997a33c33130821922c11d25c3
    //     std::vector<std::string> elems;
    //     size_t start{};
    //     size_t end{};

    //     do {
    //         end = s.find_first_of(delim, start);
    //         elems.emplace_back(s.substr(start, end - start));
    //         start = end + 1;
    //     } while (end != std::string::npos);
    //     return elems;
    // };

    // auto parts = split_string("", "/");

    // for (auto &part : parts) {
    //     MESSAGE(part);
    // }


    // parts = split_string("a/b", "/");

    // for (auto &part : parts) {
    //     MESSAGE(part);
    // }
    // parts = split_string("/", "/");

    // for (auto &part : parts) {
    //     MESSAGE(part);
    // }
}