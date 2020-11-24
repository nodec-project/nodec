#include <string>

#include <nodec/nodec_object.hpp>

namespace nodec {
    NodecObject::NodecObject(const std::string& name)
        : name(name)
    {}

    NodecObject::~NodecObject()
    {}
}