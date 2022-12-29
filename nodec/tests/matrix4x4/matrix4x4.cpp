#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec/matrix4x4.hpp>

TEST_CASE("Testing set.") {
    using namespace nodec;

    Matrix4x4f matrix;
    
    float c1[4] {0.f, 1.f, 2.f, 3.f};
    float c2[4] {4.f, 5.f, 6.f, 7.f};
    float c3[4] {8.f, 9.f, 10.f, 11.f};
    float c4[4] {12.f, 13.f, 14.f, 15.f};

    matrix.set(c1, c2, c3, c4);

    CHECK(matrix.c[0] == Vector4f(c1));
    CHECK(matrix.c[1] == Vector4f(c2));
    CHECK(matrix.c[2] == Vector4f(c3));
    CHECK(matrix.c[3] == Vector4f(c4));
}