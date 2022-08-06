#ifndef NODEC__TYPE_INFO_HPP_
#define NODEC__TYPE_INFO_HPP_

#include <atomic>
#include <stdint.h>

namespace nodec {

using TypeId = uint32_t;

namespace internal {

struct type_seq final {
    static TypeId next() {
        // Since a function is defined only once, there is only one static variable.
        static std::atomic<TypeId> value{};
        return value++;
    }
};

} // namespace internal

template<typename T>
struct type_seq final {
    static TypeId value() {
        static const TypeId value = internal::type_seq::next();
        return value;
    }
};

} // namespace nodec

#endif