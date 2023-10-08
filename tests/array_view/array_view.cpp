#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec/array_view.hpp>

#include <array>
#include <vector>

TEST_CASE("Testing constructors") {
    using namespace nodec;

    SUBCASE("T array[N]") {
        int array[3];
        ArrayView<int> view(array);
        CHECK(view.size() == 3);
    }

    SUBCASE("const T array[N}]") {
        const int array[3]{0, 1, 2};
        ArrayView<const int> view(array);
        CHECK(view.size() == 3);
    }

    SUBCASE("std::array<T, N>") {
        std::array<int, 3> array;
        ArrayView<int> view(array);
        CHECK(view.size() == 3);
    }

    SUBCASE("const std::array<T, N>") {
        const std::array<int, 3> array{0, 1, 2};
        ArrayView<const int> view(array);

        CHECK(view.size() == 3);
    }

    SUBCASE("std::vector<T>") {
        std::vector<int> array(3);
        ArrayView<int> view(array);
        CHECK(view.size() == 3);
    }

    SUBCASE("const std::vector<T>") {
        const std::vector<int> array(3);
        ArrayView<const int> view(array);
        CHECK(view.size() == 3);
    }
}

TEST_CASE("Testing iterator") {
    using namespace nodec;

    SUBCASE("std::array<T, N>") {
        std::array<int, 3> array{0, 1, 2};
        ArrayView<int> view(array);

        std::vector<int> result;
        std::copy(view.begin(), view.end(), std::back_inserter(result));
        CHECK(result.size() == 3);
        CHECK(result[0] == 0);
        CHECK(result[1] == 1);
        CHECK(result[2] == 2);
    }
}