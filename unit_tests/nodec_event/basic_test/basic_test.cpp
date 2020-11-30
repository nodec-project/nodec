

#include <nodec/event.hpp>
#include <nodec/nodec_object.hpp>

#include <iostream>
#include <memory>

class EventListener
{
public:
    void member_func_int(int x, int y)
    {
        std::cout << "In member_func_int(): x=" << x << "; y=" << y << std::endl;
    }

};

class SampleObject : public nodec::NodecObject
{
public:
    SampleObject(std::string name)
        :
        NodecObject(name)
    {
        std::cout << name << ".constructor was called." << std::endl;
    }

    ~SampleObject() override
    {
        std::cout << name << ".destructor was called." << std::endl;
    }

    void OnEvent(std::string test_string, int i)
    {
        std::cout << name << ".OnEvent() was called. test_string=" << test_string << "; i=" << i << std::endl;
    }

    void OnEventWithException(std::string test_string, int i)
    {
        std::cout << name << ".OnEventWithException() was called. test_string=" << test_string << "; i=" << i << std::endl;
        throw nodec::NodecException("Unhandled Exception", __FILE__, __LINE__);
    }

    nodec::NodecObjectHolder<SampleObject> child;

};

void static_func_int(int x, int y)
{
    std::cout << "In static_func_int(): x=" << x << "; y=" << y << std::endl;
}

void static_func_string_int(std::string test_string, int i)
{
    std::cout << "In static_func_string_int(): test_string=" << test_string << "; i=" << i << std::endl;
}

int main()
{
    
    std::cout << "--- START ---" << std::endl;

    
    nodec::event::Event<int, int> int_int_event;
    nodec::event::Event<std::string, int> string_int_event;


    {
        auto sample_object = std::make_shared<SampleObject>("sample1");
        {
            sample_object->child = std::make_shared<SampleObject>("sample-child");
            nodec::NodecObjectReference<SampleObject> sample_child_object = sample_object->child;
            auto sample_child_object_callback = std::make_shared<nodec::event::MemeberCallback<SampleObject, std::string, int>>(sample_object->child, &SampleObject::OnEvent);
            auto sample_child_object_callback_with_exception = std::make_shared<nodec::event::MemeberCallback<SampleObject, std::string, int>>(sample_object->child, &SampleObject::OnEventWithException);
            string_int_event += sample_child_object_callback;
            string_int_event += sample_child_object_callback_with_exception;
            string_int_event.invoke("Now lets go", 3000);
        }


        auto static_int_callback = std::make_shared<nodec::event::StaticCallback<int, int>>(static_func_int);
        auto static_int_callback_sub = std::make_shared<nodec::event::StaticCallback<int, int>>(static_func_int);
        auto sample_object_callback = std::make_shared<nodec::event::MemeberCallback<SampleObject, std::string, int>>(sample_object, &SampleObject::OnEvent);

        std::cout << "--- 1 ---" << std::endl;
        static_int_callback->invoke(0, 1);
        static_int_callback_sub->invoke(10, 11);
        sample_object_callback->invoke("Hey !", -11);

        std::cout << "--- 2 ---" << std::endl;
        int_int_event.hook(static_int_callback);
        int_int_event += static_int_callback;
        int_int_event += static_int_callback_sub;
        //int_int_event -= static_int_callback;
        //int_int_event -= static_int_callback;
        int_int_event.invoke(0, 1);

        std::cout << "--- 3 ---" << std::endl;
        string_int_event += sample_object_callback;
        string_int_event.invoke("Look!", 90);
    }

    std::cout << "--- 4 ---" << std::endl;
    string_int_event.invoke("Look!", 90);

    std::cout << "--- 5 ---" << std::endl;
    {
        auto static_string_int_callback = std::make_shared<nodec::event::StaticCallback<std::string, int>>(static_func_string_int);
        static_string_int_callback->invoke("Hello Test", 0);
        string_int_event += static_string_int_callback;
    }
    string_int_event.invoke("Are you OK?", 1234);

    std::cout << "--- END ---" << std::endl;
    return 0;
}