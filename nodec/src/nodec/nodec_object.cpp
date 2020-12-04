#include <nodec/nodec_object.hpp>
#include <nodec/logging.hpp>

#include <string>

//#include <iostream>

namespace nodec
{

    NodecObject::NodecObject(const std::string& name) :
        name(name),
        id_(0)
    {
        static uint64_t id_counter = 0;

        id_ = id_counter;
        if (++id_counter < id_)
        {
            // id_counter is overflowed
            logging::warn("NodecObject id counter is overflowed. It will cause id confliction.", __FILE__, __LINE__);
        }

        //std::cout << "in base contructor. " << std::endl;
    }

    NodecObject::~NodecObject()
    {
        //std::cout << "in base destructor." << std::endl;
    }

    uint64_t NodecObject::id() { return id_; }

}
