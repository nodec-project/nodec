#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec/random.hpp>

#include <array>
#include <iterator>
#include <vector>


TEST_CASE("testing basic functionalities.") {
    using namespace nodec;

    random::global().uniform(0.f, 1.f);
    random::global().gauss(0.f, 1.f);

    random::global().rand_int(0, 100);

    CHECK(random::global().rand_int(0, 0) == 0);
}

TEST_CASE("testing choice().") {
    using namespace nodec;

    {
        std::array<int, 5> array{{0, 1, 2, 3, 4}};
        auto iter = random::global().choice(array.begin(), array.end());
        CHECK(0 <= *iter);
        CHECK(*iter <= 4);
    }

    {
        std::vector<int> empty_array;
        auto iter = random::global().choice(empty_array.begin(), empty_array.end());
        CHECK(iter == empty_array.end());
    }

    {
        int array[]{1, 2, 3};
        auto iter = random::global().choice(std::begin(array), std::end(array));
        CHECK(1 <= *iter);
        CHECK(*iter <= 3);
    }
}