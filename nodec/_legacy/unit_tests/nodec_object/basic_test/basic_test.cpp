#include <iostream>

#include <nodec/nodec_object.hpp>

using namespace nodec;

class ChildObject : public NodecObject
{
public:
    ChildObject(const std::string& name):
        NodecObject(name)
    {
        std::cout << "constructor in " << name << std::endl;
    }
public:
    
    ~ChildObject()
    {
        std::cout << "destructor in " << name << std::endl;
    }

    NodecObject::Holder<ChildObject> child;
    NodecObject::Reference<ChildObject> child_ref;
};

class RootObject : public NodecObject
{
public:
    RootObject() :
        NodecObject("root"),
        value(0)
    {

    }

public:
    //RootObject()
    //    : NodecObject("root")
    //{
    //    std::cout << "constructor in " << name << std::endl;
    //}

    void test(int val)
    {
        auto shared = shared_from(this);
        if (shared)
        {
            shared->value = val;
        }
    }

    ~RootObject()
    {
        std::cout << "destructor in " << name << std::endl;
    }
    int value;
    NodecObject::Holder<ChildObject> child_a;
    NodecObject::Holder<ChildObject> child_b;
};

int main()
{

    auto root_object = nodec::NodecObject::instanciate<RootObject>();
    std::cout << root_object->id() << std::endl;

    {

        root_object->child_a = nodec::NodecObject::instanciate<ChildObject>("child_a");
        std::cout << root_object->child_a->id() << std::endl;
        //RootObject rootObject;
        //rootObject.child_a.reset(new ChildObject("child_a"));
        //rootObject.child_b.reset(new ChildObject("child_b"));

        //rootObject.child_a->child.reset(new ChildObject("child_a_1"));
        //rootObject.child_a->child->child.reset(new ChildObject("child_a_1_1"));

        //std::cin.get();

        //rootObject.child_a->child.reset();

        //std::cin.get();
    }

    std::cout << "--- 2 ---" << std::endl;
    {
        std::cout << root_object->child_a->id() << std::endl;

        std::cout << root_object->value << std::endl;
        root_object->test(100);
        std::cout << root_object->value << std::endl;

    }
    std::cin.get();

    return 0;
}