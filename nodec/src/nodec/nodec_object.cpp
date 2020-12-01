#include <string>

#include <nodec/nodec_object.hpp>

namespace nodec
{

    NodecObject::NodecObject(const std::string& name) : name_(name) {}

    NodecObject::~NodecObject() {}

    const char* NodecObject::name() const noexcept
    {
        return name_.c_str();
    }
}
