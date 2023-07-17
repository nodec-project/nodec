#ifndef NODEC__MEMORY__HPP_
#define NODEC__MEMORY__HPP_

#include <memory>
#include <type_traits>

namespace nodec {

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

/**
 * @brief Returns a std::shared_ptr<T> that shares ownership of *that with all existing std::shared_ptr that refer to *that.
 *
 * implementation refs:
 * <https://stackoverflow.com/questions/657155/how-to-enable-shared-from-this-of-both-parent-and-derived>
 *
 * @tparam That
 * @param that
 * @return std::shared_ptr<That>
 */
template<typename That>
inline std::shared_ptr<That>
shared_from(That *that) {
    return std::static_pointer_cast<That>(shared_from_base(that));
}

// This is dangerous if you do that...
// unique_void_ptr ptr;
// ptr.reset(new T());
// // Oh, T instance will be never released.

// class unique_void_ptr : public std::unique_ptr<void, void (*)(const void *)> {
//     using base_type = std::unique_ptr<void, void (*)(const void *)>;
// public:
//     template<typename Deleter>
//     unique_void_ptr(pointer ptr, Deleter &&deleter)
//         : base_type{ptr, deleter} {}

//     unique_void_ptr()
//         : base_type{nullptr, nullptr} {}
// };

// //using unique_void_ptr = std::unique_ptr<void, void (*)(const void *)>;

// /**
//  * @brief
//  *
//  * implementation refs:
//  * * <https://stackoverflow.com/questions/39288891/why-is-shared-ptrvoid-legal-while-unique-ptrvoid-is-ill-formed>
//  *
//  * @tparam T
//  * @tparam Args
//  * @param args
//  * @return unique_void_ptr
//  */
// template<typename Type, typename... Args>
// unique_void_ptr make_unique_void(Args &&...args) {
//     static_assert(!std::is_array<Type>::value, "Array types are not supported");
//     auto *ptr = new Type(std::forward<Args>(args)...);

//     return unique_void_ptr(ptr, [](const void *data) {
//         auto *p = static_cast<Type const *>(data);
//         delete p;
//     });
// }

} // namespace nodec

#endif