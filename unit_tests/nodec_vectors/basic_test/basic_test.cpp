#include <nodec/logging.hpp>

#include <nodec/vector2.hpp>
#include <nodec/vector3.hpp>
#include <nodec/vector4.hpp>
#include <nodec/quaternion.hpp>

#include <iostream>


using namespace nodec;

const int test = 0;

nodec::Vector2f func(nodec::Vector2f vec_a, nodec::Vector2f& vec_b)
{
    return (vec_a / 2.0f + vec_b * 3.0f) * 1.0f;
}

int main()
{
    
    logging::record_handlers += event::StaticCallback<const logging::LogRecord&>::make_shared(&logging::record_to_stdout_handler);
    logging::info("log start", __FILE__, __LINE__);

    logging::debug("--- 1 ---", __FILE__, __LINE__);
    {
        nodec::Vector3f vec = nodec::Vector3f::one;
        logging::DebugStream(__FILE__, __LINE__) << vec << std::flush;

        nodec::Vector4f vec4(1.1, 2.3, 3.4, 5.4);
        nodec::Vector4i vec4i(vec4);
        logging::DebugStream(__FILE__, __LINE__) << vec4 << std::flush;
        logging::DebugStream(__FILE__, __LINE__) << vec4i << std::flush;
        nodec::Vector4f vcast(vec4i);

        logging::DebugStream(__FILE__, __LINE__) << vcast << std::flush;
    }

    logging::debug("--- 2 ---", __FILE__, __LINE__);
    {
        Quaternionf quaternion(-1, -2, 3, 4);
        logging::DebugStream(__FILE__, __LINE__) << quaternion << std::flush;

    }
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