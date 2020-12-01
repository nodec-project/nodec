#ifndef NODEC__MACROS_HPP_
#define NODEC__MACROS_HPP_

/**
* Disables the copy constructor and operator= for the given class.
* 
* Use in the private section of the class.
*/
#define NODEC_DISABLE_COPY(...)                             \
    __VA_ARGS__(const __VA_ARGS__ &) = delete;              \
    __VA_ARGS__ & operator= (const __VA_ARGS__ &) = delete;

#endif