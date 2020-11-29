#include "test_interface.hpp"

#include <iostream>



void client_func()
{
    std::cout << "in client_func(): TestInterface::get_static_value()=" << TestInterface::get_static_value() << std::endl;
}

void client_func_with_interface(TestInterface& test_interface)
{
    std::cout << "in client_func_with_interface(): " << test_interface.get_value() << std::endl;
}