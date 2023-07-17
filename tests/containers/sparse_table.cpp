#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec/containers/sparse_table.hpp>

#include <array>

TEST_CASE("Testing emplace.") {
    using namespace nodec::containers;

    {
        SparseTable<int> sparse;

        std::array<std::pair<std::size_t, int>, 12> expected{
            {{0, 0},
             {1, 1},
             {2, 2},
             {3, 3},
             {4, 4},
             {5, 5},
             {6, 6},
             {7, 7},
             {8, 8},
             {9, 9},
             {0x0FFFFF, 10},
             {0x1FFFFF, 11}}};

        for (const auto &pair : expected) {
            // CHECK(pair.first != 0x0FFFFF);
            sparse[pair.first] = pair.second;
        }

        for (const auto &pair : expected) {
            auto *value = sparse.try_get(pair.first);
            INFO(pair.first, pair.second);
            CHECK(value != nullptr);
            CHECK(*value == pair.second);
        }
    }

    SUBCASE("Testing that buckets size is less than group size.") {
        SparseTable<int> sparse;

        for (int i = 0; i < DEFAULT_SPARSE_GROUP_SIZE; ++i) {
            sparse[i] = i;
        }
    }

    // MESSAGE(sizeof(BasicSparseGroup<int, 48>)); // 40
    // MESSAGE(sizeof(int *));
    // MESSAGE(sizeof(std::vector<int>));
    // MESSAGE(sizeof(std::unique_ptr<int>));
}