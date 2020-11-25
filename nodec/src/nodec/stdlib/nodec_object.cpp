#include <string>

#include <nodec/stdlib/nodec_object.hpp>

namespace nodec {
    namespace stdlib {
        NodecObject::NodecObject(const std::string& name)
            : name(name)
        {}

        NodecObject::~NodecObject()
        {}
    }
}