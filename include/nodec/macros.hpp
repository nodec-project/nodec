#ifndef NODEC__MACROS_HPP_
#define NODEC__MACROS_HPP_

#include <memory>
#include <utility>

/**
 * Disables the copy constructor and operator= for the given class.
 *
 * Use in the private section of the class.
 */
#define NODEC_DISABLE_COPY(...) \
    __VA_ARGS__(const __VA_ARGS__ &) = delete; \
    __VA_ARGS__ &operator=(const __VA_ARGS__ &) = delete;

/**
 * Defines aliases and static functions for using the Class with smart pointers.
 *
 * Use in the public section of the class.
 * Make sure to include `<memory>` in the header when using this.
 */
#define NODEC_SMART_PTR_DEFINITIONS(...) \
    NODEC_SHARED_PTR_DEFINITIONS(__VA_ARGS__) \
    NODEC_WEAK_PTR_DEFINITIONS(__VA_ARGS__) \
    NODEC_UNIQUE_PTR_DEFINITIONS(__VA_ARGS__)

#define __NODEC_SHARED_PTR_ALIAS(...) \
    using SharedPtr = std::shared_ptr<__VA_ARGS__>; \
    using ConstSharedPtr = std::shared_ptr<const __VA_ARGS__>;

#define __NODEC_MAKE_SHARED_DEFINITION(...) \
    template<typename... Args> \
    static std::shared_ptr<__VA_ARGS__> \
    make_shared(Args &&...args) { \
        return std::make_shared<__VA_ARGS__>(std::forward<Args>(args)...); \
    }

/**
 * Defines aliases and static functions for using the Class with shared_ptrs.
 */
#define NODEC_SHARED_PTR_DEFINITIONS(...) \
    __NODEC_SHARED_PTR_ALIAS(__VA_ARGS__) \
    __NODEC_MAKE_SHARED_DEFINITION(__VA_ARGS__)

#define __NODEC_WEAK_PTR_ALIAS(...) \
    using WeakPtr = std::weak_ptr<__VA_ARGS__>; \
    using ConstWeakPtr = std::weak_ptr<const __VA_ARGS__>;

/**
 * Defines aliases and static functions for using the Class with weak_ptrs.
 */
#define NODEC_WEAK_PTR_DEFINITIONS(...) __NODEC_WEAK_PTR_ALIAS(__VA_ARGS__)

#define __NODEC_UNIQUE_PTR_ALIAS(...) using UniquePtr = std::unique_ptr<__VA_ARGS__>;

#define __NODEC_MAKE_UNIQUE_DEFINITION(...) \
    template<typename... Args> \
    static std::unique_ptr<__VA_ARGS__> \
    make_unique(Args &&...args) { \
        return std::unique_ptr<__VA_ARGS__>(new __VA_ARGS__(std::forward<Args>(args)...)); \
    }

/**
 * Defines aliases and static functions for using the Class with unique_ptrs.
 */
#define NODEC_UNIQUE_PTR_DEFINITIONS(...) \
    __NODEC_UNIQUE_PTR_ALIAS(__VA_ARGS__) \
    __NODEC_MAKE_UNIQUE_DEFINITION(__VA_ARGS__)

#define NODEC_STRING_JOIN(arg1, arg2) NODEC_DO_STRING_JOIN(arg1, arg2)
#define NODEC_DO_STRING_JOIN(arg1, arg2) arg1##arg2

#endif