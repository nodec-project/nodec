
#include <nodec/vector2.hpp>

#include <iostream>

const int test = 0;

nodec::Vector2f func(nodec::Vector2f vec_a, nodec::Vector2f& vec_b)
{
    return (vec_a / 2.0f + vec_b * 3.0f) * 1.0f;
}

int main()
{
    auto a = test + 1;
    //std::cout << (nodec::Vector2f::zero + nodec::Vector2f::zero).x;

    std::cout << nodec::Vector2f::one + nodec::Vector2f::right << std::endl;
    
    //nodec::Vector2f::zero + nodec::Vector2f::zero;

    nodec::Vector2f vec_a(-1, 1), vec_b(1, 2);

    std::cout << vec_a + vec_b << std::endl;

    std::cout << func(vec_a, vec_b) << std::endl;

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