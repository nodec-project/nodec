#include "sub.hpp"

#include <nodec/logging.hpp>

void sub_func()
{
    nodec::logging::info("this message sent from sub_func(). Can you see it?", __FILE__, __LINE__);
}