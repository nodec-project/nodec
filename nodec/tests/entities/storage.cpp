#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec/entities/storage.hpp>

#include <array>
#include <cstdint>

TEST_CASE("Testing storage iterator.") {
    using namespace nodec::entities;

    BasicStorage<std::uint32_t, int> storage;

    std::array<std::pair<std::uint32_t, char>, 3> expected{
        {
            {1, 'a'},
            {0, 'b'},
            {2, 'c'},

        }};

    storage.emplace(expected[2].first, expected[2].second);
    storage.emplace(expected[1].first, expected[1].second);
    storage.emplace(expected[0].first, expected[0].second);

    int i = 0;
    for (auto iter = storage.begin(); iter != storage.end(); ++iter) {
        INFO(i);
        REQUIRE(i < expected.size());
        CHECK(*iter == expected[i].first);
        ++i;
    }
}

TEST_CASE("Testing erase().") {
    using namespace nodec::entities;

    BasicStorage<std::uint32_t, int> storage;

    storage.emplace(0, 'a');
    storage.emplace(1, 'a');
    storage.emplace(2, 'a');

    REQUIRE(storage.size() == 3);

    CHECK(storage.erase(storage.begin(), storage.end()) == 3);

    CHECK(storage.size() == 0);
}