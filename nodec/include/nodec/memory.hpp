#ifndef NODEC__MEMORY__HPP_
#define NODEC__MEMORY__HPP_

#include "logging.hpp"

#include <memory>

namespace nodec {

// <https://stackoverflow.com/questions/657155/how-to-enable-shared-from-this-of-both-parent-and-derived>

template<typename Base>
inline std::shared_ptr<Base>
shared_from_base(std::enable_shared_from_this<Base> *base) {
    return base->shared_from_this();
}
template<typename Base>
inline std::shared_ptr<const Base>
shared_from_base(std::enable_shared_from_this<Base> const *base) {
    return base->shared_from_this();
}
template<typename That>
inline std::shared_ptr<That>
shared_from(That *that) {
    return std::static_pointer_cast<That>(shared_from_base(that));
}

// <https://stackoverflow.com/questions/39288891/why-is-shared-ptrvoid-legal-while-unique-ptrvoid-is-ill-formed>

// using unique_void_ptr = std::unique_ptr<void, void(*)(void const*)>;
//
// template<typename T>
// decltype(auto) make_unique_void(T* ptr) {
//     return std::unique_ptr<void>(ptr, [](void const* data) {
//         T const* p = static_cast<T const*>(data);
//         delete p;
//     });
// }

} // namespace nodec

#endif