#ifndef NODEC__ENTITIES__TYPE_INFO_HPP_
#define NODEC__ENTITIES__TYPE_INFO_HPP_


#include <stdint.h>


namespace nodec {
namespace entities {

using TypeId = uint32_t;

namespace internal {

struct type_seq final {
    static TypeId next() {
        // Since a function is defined only once, there is only one static variable.
        static TypeId value{};
        return value++;
    }
};

}



template<typename T>
struct type_seq final {
    static TypeId value() {
        static const TypeId value = internal::type_seq::next();
        return value;
    }
};

}
}


#endif