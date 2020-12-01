#include <nodec/nodec_object.hpp>

#include <string>

namespace nodec
{

    NodecObject::NodecObject(const std::string& name) : name(name) {}

    NodecObject::~NodecObject() {}

}
