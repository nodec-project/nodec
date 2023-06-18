#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec/ranges.hpp>

#include <algorithm>
#include <map>
#include <unordered_map>

TEST_CASE("Testing keys()") {
    using namespace nodec;

    SUBCASE("using map") {
        std::map<std::string, int> map{
            {"0", 0},
            {"1", 1},
            {"2", 2},
            {"3", 3},
            {"4", 4},
        };

        auto keys = views::keys(map);

        static_assert(std::is_same<decltype(*keys.begin()), const std::string &>::value, "failed");
        static_assert(std::is_same<decltype(keys.begin())::value_type, const std::string>::value, "failed");
        static_assert(std::is_same<decltype(keys.begin())::reference, const std::string &>::value, "failed");
        static_assert(std::is_same<decltype(keys.begin())::pointer, const std::string *>::value, "failed");

        // decltype(map.begin())::value_type
        CHECK(keys.size() == 5);
        auto iter = keys.begin();
        CHECK(*iter++ == "0");
        CHECK(*iter++ == "1");
        CHECK(*iter++ == "2");
        CHECK(*iter++ == "3");
        CHECK(*iter++ == "4");
        CHECK(iter == keys.end());
    }

    SUBCASE("using const map") {
        const std::map<std::string, int> map{
            {"0", 0},
            {"1", 1},
            {"2", 2},
            {"3", 3},
            {"4", 4},
        };

        auto keys = views::keys(map);

        static_assert(std::is_same<decltype(*keys.begin()), const std::string &>::value, "failed");
        static_assert(std::is_same<decltype(keys.begin())::value_type, const std::string>::value, "failed");
        static_assert(std::is_same<decltype(keys.begin())::reference, const std::string &>::value, "failed");
        static_assert(std::is_same<decltype(keys.begin())::pointer, const std::string *>::value, "failed");

        CHECK(keys.size() == 5);
        auto iter = keys.begin();
        CHECK(*iter++ == "0");
        CHECK(*iter++ == "1");
        CHECK(*iter++ == "2");
        CHECK(*iter++ == "3");
        CHECK(*iter++ == "4");
        CHECK(iter == keys.end());
    }

    SUBCASE("using unordered_map") {
        std::unordered_map<std::string, int> map{
            {"0", 0},
            {"1", 1},
            {"2", 2},
            {"3", 3},
            {"4", 4},
        };

        auto keys = views::keys(map);

        static_assert(std::is_same<decltype(*keys.begin()), const std::string &>::value, "failed");
        static_assert(std::is_same<decltype(keys.begin())::value_type, const std::string>::value, "failed");
        static_assert(std::is_same<decltype(keys.begin())::reference, const std::string &>::value, "failed");
        static_assert(std::is_same<decltype(keys.begin())::pointer, const std::string *>::value, "failed");

        CHECK(keys.size() == 5);
    }

    SUBCASE("using const unordered_map") {
        const std::unordered_map<std::string, int> map{
            {"0", 0},
            {"1", 1},
            {"2", 2},
            {"3", 3},
            {"4", 4},
        };

        auto keys = views::keys(map);

        static_assert(std::is_same<decltype(*keys.begin()), const std::string &>::value, "failed");
        static_assert(std::is_same<decltype(keys.begin())::value_type, const std::string>::value, "failed");
        static_assert(std::is_same<decltype(keys.begin())::reference, const std::string &>::value, "failed");
        static_assert(std::is_same<decltype(keys.begin())::pointer, const std::string *>::value, "failed");

        CHECK(keys.size() == 5);
    }

    SUBCASE("using std::copy") {
        std::map<std::string, int> map{
            {"0", 0},
            {"1", 1},
            {"2", 2},
            {"3", 3},
            {"4", 4},
        };

        std::vector<std::string> result;
        auto keys = views::keys(map);

        std::copy(keys.begin(), keys.end(), std::back_inserter(result));
        CHECK(result.size() == 5);
        CHECK(result[0] == "0");
        CHECK(result[1] == "1");
        CHECK(result[2] == "2");
        CHECK(result[3] == "3");
        CHECK(result[4] == "4");
    }
}