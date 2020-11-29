#ifndef TEST_INTERFACE_HPP_
#define TEST_INTERFACE_HPP_

#include<nodec/module_interface.hpp>

class TestInterface : public nodec::ModuleInterface{
public:
    TestInterface()
        :value(0)
    {
     }

    static int get_static_value() { return static_value; }
    virtual int get_value() { return value; };
    //int get_value()  { return value; }
    //virtual int get_value() const = 0;
protected:
    static int static_value;
    int value; // << BAD!; cannot change This value in derived class(module).
};

#endif