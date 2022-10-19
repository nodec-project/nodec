#ifndef NODEC__OBJECT_HPP_
#define NODEC__OBJECT_HPP_

#include <nodec/macros.hpp>
#include <nodec/memory.hpp>

#include <memory>
#include <string>

namespace nodec
{

class Object : public std::enable_shared_from_this<Object>
{
public:
    template<typename T>
    using Holder = std::shared_ptr<T>;

    template<typename T>
    using Reference = std::weak_ptr<T>;

    template<typename T>
    using Unique = std::unique_ptr<T>;

private:
    NODEC_DISABLE_COPY(Object)

public:
    template<typename T, typename ... Args>
    static Holder<T> make_holder(Args && ...args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template<typename T, typename ... Args>
    static Unique<T> make_unique(Args && ...args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

};



} // namespace nodec

#endif