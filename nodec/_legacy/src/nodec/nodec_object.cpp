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
    // using just incremental id
    // id space size is 2^64 (huge big).
    // even if 1 object was created per second, it will take about 583,344,214,028 years to use all ids.
    static ID id_counter = 0;

    id_ = id_counter;
    if (++id_counter < id_)
    {
        // id_counter is overflowed
        logging::warn("NodecObject id counter is overflowed. It will cause id confliction.", __FILE__, __LINE__);
    }
}


NodecObject::~NodecObject() {}

NodecObject::ID
NodecObject::id() const noexcept { return id_; }

}
