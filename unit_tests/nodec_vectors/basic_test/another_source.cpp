
#include <nodec/vector2.hpp>

#include <iostream>

const int test = 0;

int func()
{
    auto a = test + 1;
    //nodec::Vector2f::zero + nodec::Vector2f::zero;
    //nodec::Vector2f::zero + nodec::Vector2f::zero;

    nodec::Vector2<double> vec_a(0, 1), vec_b(1, 1);

    vec_a + vec_b;
    /*auto vecf = nodec::Vector2f::zero;
    vecf = nodec::Vector2f(0, 0);*/
    //auto veci = nodec::Vector2i::zero;
    //auto vecui = nodec::Vector2<unsigned int>::down();

    //int a = test;
    //a++;
    //std::cout << vecui.y << std::endl;
    //std::cout << vecf << std::endl;
    return 0;
}