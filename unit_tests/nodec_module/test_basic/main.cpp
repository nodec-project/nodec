

#include "test_interface.hpp"

#include <iostream>


void client_func();
void client_func_with_interface(TestInterface& test_interface);

int TestInterface::static_value = 0;

// Implementation of the TestInterface.
class TestModule : public TestInterface{
public:
    /*int get_value() const override
    {
        
    }*/
    void HandleUpdateStaticValue(int val)
    {
        TestInterface::static_value = val;
    }

    void HandleUpdateValue(int val)
    {
        value = val;
    }
private:
    //int value;
};

//
//class TestInterfaceImpl : public TestInterface
//{
//    friend class TestModule;
//};
//
//class TestModule : public nodec::module::Module
//{
//public:
//    void HandleUpdateStaticValue(int value)
//    {
//        TestInterfaceImpl::static_value = value;
//    }
//    void HandleUpdateValue(TestInterfaceImpl& interface, int value)
//    {
//        interface.value = value;
//    }
//};



int main()
{
    /*TestInterface test_interface;
    std::cout << test_interface.get_value() << std::endl;
    */

    TestModule test_module;
    TestInterface& test_interface = test_module;

    std::cout << TestInterface::get_static_value() << std::endl;
    std::cout << test_interface.get_value() << std::endl;

    client_func();
    client_func_with_interface(test_interface);

    test_module.HandleUpdateStaticValue(100);
    test_module.HandleUpdateValue(200);
    std::cout << "--- UPDATE ---" << std::endl;
    //test_module.HandleUpdateValue(test_interface, 200); // << ERROR

    std::cout << TestInterface::get_static_value() << std::endl;
    std::cout << test_interface.get_value() << std::endl;

    client_func();
    client_func_with_interface(test_interface);


    return 0;
}