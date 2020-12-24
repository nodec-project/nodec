#ifndef NODEC__MACROS_HPP_
#define NODEC__MACROS_HPP_

#include <memory>
#include <utility>

/**
* Disables the copy constructor and operator= for the given class.
*
* Use in the private section of the class.
*/
#define NODEC_DISABLE_COPY(...)                             \
    __VA_ARGS__(const __VA_ARGS__ &) = delete;              \
    __VA_ARGS__ & operator= (const __VA_ARGS__ &) = delete;


#define __NODEC_SHARED_PTR_ALIAS(...)                        \
  using SharedPtr = std::shared_ptr<__VA_ARGS__>;            \
  using ConstSharedPtr = std::shared_ptr<const __VA_ARGS__>;

#define __NODEC_MAKE_SHARED_DEFINITION(...)                             \
  template<typename ... Args>                                           \
  static std::shared_ptr<__VA_ARGS__>                                   \
  make_shared(Args && ... args)                                         \
  {                                                                     \
    return std::make_shared<__VA_ARGS__>(std::forward<Args>(args) ...); \
  }

/**
* Defines aliases and static functions for using the Class with shared_ptrs.
*/
#define NODEC_SHARED_PTR_DEFINITIONS(...)     \
  __NODEC_SHARED_PTR_ALIAS(__VA_ARGS__)       \
  __NODEC_MAKE_SHARED_DEFINITION(__VA_ARGS__)



#endif