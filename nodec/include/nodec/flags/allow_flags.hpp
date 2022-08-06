#ifndef NODEC__FLAGS__ALOW_FLAGS_HPP_
#define NODEC__FLAGS__ALOW_FLAGS_HPP_

#include <type_traits>

namespace nodec {
namespace flags {

template<class E, class Enabler = void>
struct is_flags : public std::false_type {
};

} // namespace flags
} // namespace nodec

#define NODEC_ALLOW_FLAGS_FOR_ENUM(name) \
    namespace nodec { \
    namespace flags { \
    template<> \
    struct is_flags<name> : std::true_type {}; \
    } \
    }

#endif
