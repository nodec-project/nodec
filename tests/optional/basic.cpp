#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

//#include <optional>

#include <nodec/logging.hpp>
#include <nodec/optional.hpp>

#include <array>
#include <string>
#include <vector>


TEST_CASE("testing optional constructor") {
    // https://cpprefjp.github.io/reference/optional/optional/op_constructor.html

    {
        using int_optional = nodec::optional<int>;
        static_assert(std::is_move_constructible<int_optional>::value, "fail");
    }
    {
        class Test {
        public:
            Test(int a, int b) 
            : a{a}, b{b} {}

            Test(Test &&) = default;
            //Test(Test &&other)
            //    : a{std::move(other.a)}, b{std::move(other.b)} {}

            int a;
            int b;
        };

        nodec::optional<Test> a = Test{0 ,1};
        nodec::optional<Test> b = std::move(a);

        CHECK(b.value().a == 0);
        CHECK(b.value().b == 1);
    }
    {
        nodec::optional<int> p;
        CHECK(!p.has_value());
    }
    {
        nodec::optional<int> p = nodec::nullopt;
        CHECK(!p.has_value());
    }
    {
        nodec::optional<int> a = 3;
        nodec::optional<int> b = a;
        CHECK(a.value() == 3);
        CHECK(b.value() == 3);
    }
    {
        nodec::optional<std::string> a = "Hello";
        nodec::optional<std::string> b = std::move(a);
        CHECK(b.value() == "Hello");
    }

    //{
    //    std::optional<std::string> a = "Hello";
    //    std::optional<std::string> b = std::move(a);
    //    CHECK(b.value() == "Hello");
    //}

    {
        nodec::optional<float> float_value;
        nodec::optional<std::array<int, 5>> int_array;
        nodec::optional<int[5]> native_int_array;
    }
    
    {
        nodec::optional<const char*> a = "Hello";
        nodec::optional<std::string> b = a;

        CHECK(b.value() == "Hello");
    }

    {
        nodec::optional<std::string> a{nodec::in_place, "ABC"};

        REQUIRE(a.has_value());
        CHECK(a.value() == "ABC");
    }

    {
        nodec::optional<std::string> p(nodec::in_place, 3, 'A');

        REQUIRE(p.has_value());
        CHECK(p.value() == "AAA");
    }

    {
        std::allocator<int> alloc;
        nodec::optional<std::vector<int>> p{nodec::in_place, {3, 1, 4}, alloc};
        CHECK(p.value()[0] == 3);
        CHECK(p.value()[1] == 1);
        CHECK(p.value()[2] == 4);
    }

    {
        nodec::optional<std::string> p1 = "Hello";
        CHECK(p1.value() == "Hello");

        nodec::optional<std::vector<int>> p2{3};
        CHECK(p2.value().size() == 3);
    }

    {
        class Test {
        public:
            Test(int a, int b)
                : a{a}, b{b} {}
            int a;
            int b;
        };
        {
            nodec::optional<Test> p{nodec::in_place, 0, 1};
            CHECK(p.value().a == 0);
            CHECK(p.value().b == 1);
        }

        {
            nodec::optional<Test> p;
        }
    }

    {
        struct Base {};
        struct Derived : Base {};

        nodec::optional<std::shared_ptr<Derived>> a = std::make_shared<Derived>();
        nodec::optional<std::shared_ptr<Base>> b = std::move(a);
    }
}

TEST_CASE("testing optional operator*") {
    nodec::optional<int> p = 3;

    REQUIRE(p);
    CHECK(*p == 3);

    *p = 42;
    CHECK(*p == 42);
}


TEST_CASE("testing optional operator->") {
    nodec::optional<std::string> p = "Hello";

    REQUIRE(p);

    CHECK(p->size() == 5);
}


TEST_CASE("testing optional operator=") {
    {
        nodec::optional<int> p = 3;
        p = nodec::nullopt;
        CHECK(!p.has_value());
    }
    {
        nodec::optional<int> a = 3;
        nodec::optional<int> b = 1;

        a = b;
        CHECK(a.value() == 1);
    }
    {
        nodec::optional<int> a = 3;
        nodec::optional<int> b = 1;

        a = std::move(b);
        CHECK(a.value() == 1);
    }

    {
        nodec::optional<std::string> a = "Hello";
        nodec::optional<std::string> b = "World";

        a = std::move(b);
        CHECK(a.value() == "World");
    }
    {
        nodec::optional<int> p;

        p = 3;
        CHECK(p.has_value());
        CHECK(p.value() == 3);
    }
    {
        nodec::optional<long long> a = 3LL;
        nodec::optional<int> b = 1;

        a = b;
        CHECK(a.value() == 1LL);
    }
    {
        nodec::optional<long long> a = 3LL;
        nodec::optional<int> b = 1;

        a = std::move(b);
        CHECK(a.value() == 1LL);
    }
    {
        class TestA {
        public:
            TestA(int a, int b)
                : a{a}, b{b} {}

            TestA(const TestA &other)
                : a{other.a}, b{other.b} {}

            TestA &operator=(const TestA &) = default;
            int a;
            int b;
        };

        nodec::optional<TestA> a;
        TestA b{1, 2};
        a = b;
    }
}

TEST_CASE("testing optional emplace") {
    {
        nodec::optional<std::string> p;
        p.emplace(3, 'A');
        CHECK(p.value() == "AAA");
    }
    {
        nodec::optional<std::vector<int>> p;
        std::allocator<int> alloc;
        p.emplace({3, 1, 4}, alloc);
        CHECK(p.value()[0] == 3);
        CHECK(p.value()[1] == 1);
        CHECK(p.value()[2] == 4);
    }
}