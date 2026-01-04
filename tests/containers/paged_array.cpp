#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec/containers/paged_array.hpp>

#include <limits>
#include <string>

using namespace nodec::containers;

TEST_CASE("Testing basic access") {
    PagedArray<int> arr;

    SUBCASE("operator[] allocates page and returns reference") {
        arr[0] = 42;
        CHECK(arr[0] == 42);

        arr[100] = 100;
        CHECK(arr[100] == 100);
    }

    SUBCASE("try_get returns nullptr for unallocated page") {
        CHECK(arr.try_get(0) == nullptr);
        CHECK(arr.try_get(1000) == nullptr);
        CHECK(arr.try_get(100000) == nullptr);
    }

    SUBCASE("try_get returns pointer after allocation") {
        arr[500] = 123;
        auto *ptr = arr.try_get(500);
        REQUIRE(ptr != nullptr);
        CHECK(*ptr == 123);
    }

    SUBCASE("assure allocates and returns pointer") {
        auto *ptr = arr.assure(999);
        REQUIRE(ptr != nullptr);
        *ptr = 777;
        CHECK(arr[999] == 777);
    }
}

TEST_CASE("Testing page management") {
    PagedArray<int> arr;

    SUBCASE("has_page returns false for unallocated") {
        CHECK_FALSE(arr.has_page(0));
        CHECK_FALSE(arr.has_page(1023));
        CHECK_FALSE(arr.has_page(1024));
    }

    SUBCASE("has_page returns true after allocation") {
        arr[0] = 1;
        CHECK(arr.has_page(0));
        CHECK(arr.has_page(1023));  // same page
        CHECK_FALSE(arr.has_page(1024));  // next page

        arr[1024] = 2;
        CHECK(arr.has_page(1024));
    }

    SUBCASE("allocated_page_count tracks pages") {
        CHECK(arr.allocated_page_count() == 0);

        arr[0] = 1;
        CHECK(arr.allocated_page_count() == 1);

        arr[1024] = 2;
        CHECK(arr.allocated_page_count() == 2);

        arr[100] = 3;  // same page as index 0
        CHECK(arr.allocated_page_count() == 2);
    }

    SUBCASE("clear deallocates all pages") {
        arr[0] = 1;
        arr[1024] = 2;
        arr[2048] = 3;
        CHECK(arr.allocated_page_count() == 3);

        arr.clear();
        CHECK(arr.allocated_page_count() == 0);
        CHECK(arr.try_get(0) == nullptr);
    }

    SUBCASE("memory_usage calculation") {
        CHECK(arr.memory_usage() == 0);

        arr[0] = 1;
        // 1 page pointer + 1 page of ints (default page size = 1024)
        size_t expected = sizeof(int *) + 1024 * sizeof(int);
        CHECK(arr.memory_usage() == expected);
    }
}

TEST_CASE("Testing sparse indices") {
    PagedArray<int> arr;

    SUBCASE("large sparse index") {
        arr[1000000] = 42;
        CHECK(arr[1000000] == 42);
        CHECK(arr.has_page(1000000));

        // Other indices in same page should be accessible
        size_t page_start = (1000000 / 1024) * 1024;
        CHECK(arr.has_page(page_start));
    }

    SUBCASE("multiple sparse indices") {
        arr[0] = 0;
        arr[10000] = 1;
        arr[100000] = 2;
        arr[1000000] = 3;

        CHECK(arr[0] == 0);
        CHECK(arr[10000] == 1);
        CHECK(arr[100000] == 2);
        CHECK(arr[1000000] == 3);
    }
}

TEST_CASE("Testing different page sizes") {
    SUBCASE("PageSize 1024") {
        BasicPagedArray<int, 1024> arr;
        arr[0] = 1;
        arr[1023] = 2;
        CHECK(arr.has_page(0));
        CHECK(arr.has_page(1023));
        CHECK_FALSE(arr.has_page(1024));

        arr[1024] = 3;
        CHECK(arr.has_page(1024));
        CHECK(arr.allocated_page_count() == 2);
    }

    SUBCASE("PageSize 256") {
        BasicPagedArray<int, 256> arr;
        arr[255] = 1;
        CHECK(arr.has_page(0));
        CHECK_FALSE(arr.has_page(256));

        arr[256] = 2;
        CHECK(arr.allocated_page_count() == 2);
    }
}

TEST_CASE("Testing with complex types") {
    SUBCASE("std::string") {
        PagedArray<std::string> arr;

        arr[0] = "hello";
        arr[100] = "world";

        CHECK(arr[0] == "hello");
        CHECK(arr[100] == "world");

        auto *ptr = arr.try_get(0);
        REQUIRE(ptr != nullptr);
        CHECK(*ptr == "hello");
    }

    SUBCASE("struct with initialized flag") {
        struct Element {
            bool initialized{false};
            int value{0};
        };

        PagedArray<Element> arr;

        // Uninitialized element
        auto *ptr = arr.assure(42);
        REQUIRE(ptr != nullptr);
        CHECK_FALSE(ptr->initialized);

        // Initialize
        ptr->initialized = true;
        ptr->value = 123;

        CHECK(arr[42].initialized);
        CHECK(arr[42].value == 123);
    }
}

TEST_CASE("Testing iterator") {
    PagedArray<int> arr;

    SUBCASE("empty array") {
        int count = 0;
        for (auto &elem : arr) {
            (void)elem;
            ++count;
        }
        CHECK(count == 0);
    }

    SUBCASE("single page iteration") {
        arr[0] = 10;
        arr[1] = 20;
        arr[2] = 30;

        int count = 0;
        for (auto &elem : arr) {
            (void)elem;
            ++count;
        }
        // Iterates all slots in allocated page (default page size = 1024)
        CHECK(count == 1024);
    }

    SUBCASE("iterator index()") {
        arr[100] = 42;

        auto it = arr.begin();
        CHECK(it.index() == 0);

        // Advance to index 100
        for (int i = 0; i < 100; ++i) {
            ++it;
        }
        CHECK(it.index() == 100);
        CHECK(*it == 42);
    }

    SUBCASE("skips unallocated pages") {
        arr[0] = 1;      // page 0
        arr[2048] = 2;   // page 2 (page 1 skipped, page size = 1024)

        std::vector<size_t> pages_visited;
        for (auto it = arr.begin(); it != arr.end(); ++it) {
            size_t page = it.index() / 1024;
            if (pages_visited.empty() || pages_visited.back() != page) {
                pages_visited.push_back(page);
            }
        }

        CHECK(pages_visited.size() == 2);
        CHECK(pages_visited[0] == 0);
        CHECK(pages_visited[1] == 2);
    }
}

TEST_CASE("Testing move semantics") {
    PagedArray<int> arr1;
    arr1[0] = 42;
    arr1[1024] = 100;  // page 1 (page size = 1024)

    SUBCASE("move constructor") {
        PagedArray<int> arr2(std::move(arr1));

        CHECK(arr2[0] == 42);
        CHECK(arr2[1024] == 100);
        CHECK(arr2.allocated_page_count() == 2);
    }

    SUBCASE("move assignment") {
        PagedArray<int> arr2;
        arr2[0] = 999;

        arr2 = std::move(arr1);

        CHECK(arr2[0] == 42);
        CHECK(arr2[1024] == 100);
    }
}

TEST_CASE("Testing fill_value") {
    constexpr size_t INVALID = std::numeric_limits<size_t>::max();

    SUBCASE("constructor with fill_value") {
        BasicPagedArray<size_t, 1024> arr(INVALID);

        // Allocate a page by accessing an element
        arr[0] = 42;

        // Element 0 was explicitly set
        CHECK(arr[0] == 42);

        // Other elements in the same page should have fill_value
        auto *ptr = arr.try_get(1);
        REQUIRE(ptr != nullptr);
        CHECK(*ptr == INVALID);

        auto *ptr2 = arr.try_get(500);
        REQUIRE(ptr2 != nullptr);
        CHECK(*ptr2 == INVALID);
    }

    SUBCASE("new pages initialized with fill_value") {
        BasicPagedArray<size_t, 1024> arr(INVALID);

        // Access different pages
        arr[0] = 100;     // page 0
        arr[1024] = 200;  // page 1
        arr[2048] = 300;  // page 2

        // Check explicitly set values
        CHECK(arr[0] == 100);
        CHECK(arr[1024] == 200);
        CHECK(arr[2048] == 300);

        // Check fill_value in each page
        CHECK(*arr.try_get(1) == INVALID);
        CHECK(*arr.try_get(1025) == INVALID);
        CHECK(*arr.try_get(2049) == INVALID);
    }

    SUBCASE("sentinel value pattern for sparse mapping") {
        // Simulates entity storage sparse array usage
        BasicPagedArray<size_t, 1024> sparse_array(INVALID);

        // "Add" some entities
        sparse_array[100] = 0;   // entity 100 -> packed index 0
        sparse_array[5000] = 1;  // entity 5000 -> packed index 1
        sparse_array[200] = 2;   // entity 200 -> packed index 2

        // Check valid entries
        auto check_valid = [&](size_t index) {
            auto *ptr = sparse_array.try_get(index);
            return ptr != nullptr && *ptr != INVALID;
        };

        CHECK(check_valid(100));
        CHECK(check_valid(5000));
        CHECK(check_valid(200));

        // Check invalid entries (same page but not set)
        CHECK_FALSE(check_valid(101));
        CHECK_FALSE(check_valid(5001));

        // "Remove" an entity by setting to INVALID
        sparse_array[100] = INVALID;
        CHECK_FALSE(check_valid(100));
    }

    SUBCASE("move preserves fill_value") {
        BasicPagedArray<size_t, 1024> arr1(INVALID);
        arr1[0] = 42;

        // Move construct
        BasicPagedArray<size_t, 1024> arr2(std::move(arr1));
        CHECK(arr2[0] == 42);

        // New page in moved array should use same fill_value
        arr2[1024] = 100;
        CHECK(*arr2.try_get(1025) == INVALID);
    }

    SUBCASE("default fill_value is zero") {
        BasicPagedArray<int, 1024> arr;  // no fill_value specified

        arr[0] = 42;

        // Other elements should be zero (default constructed)
        CHECK(*arr.try_get(1) == 0);
        CHECK(*arr.try_get(500) == 0);
    }
}
