#include <nodec/nodec_object.hpp>
#include <nodec/logging.hpp>

#include <iostream>

using namespace nodec;

//using std::shared_ptr;
using std::cout;

class Child;

class Root : public NodecObject
{
public:
    Root(const std::string& name) :
        NodecObject(name)
    {
        logging::InfoStream(__FILE__, __LINE__) << name << ": created." << std::flush;
        child_1 = NodecObject::instanciate<Child>("child_1");
        child_2 = NodecObject::instanciate<Child>("child_2");
    }

    ~Root()
    {
        /*child_1.reset();
        child_2.reset();*/
        logging::InfoStream(__FILE__, __LINE__) << name << ": deleted." << std::flush;

    }

    void say(const std::string& something)
    {
        logging::InfoStream(__FILE__, __LINE__) << something << std::flush;

    }

    NodecObject::Holder<Child> child_1;
    NodecObject::Holder<Child> child_2;

};

Root* p_root;

class Child : public NodecObject
{
    public:
        Child(const std::string& name) :
            NodecObject(name)
        {
            logging::InfoStream(__FILE__, __LINE__) << name << ": created." << std::flush;
            //logging::InfoStream(__FILE__, __LINE__) << "root is " << p_root->name << std::flush;
        }

        ~Child()
        {
            logging::InfoStream(__FILE__, __LINE__) << "root is " << p_root->name << std::flush;
            p_root->say("Hello");
            logging::InfoStream(__FILE__, __LINE__) << name << ": deleted." << std::flush;
        }

};






int main()
{

    logging::record_handlers += event::StaticCallback<const logging::LogRecord&>::make_shared(&logging::record_to_stdout_handler);
    logging::info("log start", __FILE__, __LINE__);
    
    cout << " " << std::endl;
    
    auto root_object = NodecObject::instanciate<Root>("root");
    p_root = root_object.get();

}