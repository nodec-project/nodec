#ifndef NODEC__NODEC_OBJECT_HPP_
#define NODEC__NODEC_OBJECT_HPP_

#include <nodec/macros.hpp>

#include <memory>
#include <string>

namespace nodec
{

class NodecObject : public std::enable_shared_from_this<NodecObject>
{
public:
    template<typename T>
    using Holder = std::shared_ptr<T>;

    template<typename T>
    using Reference = std::weak_ptr<T>;

    using ID = uint64_t;

public:
    /**
     * @brief
        Create a new NodecObject with specified name.
     *  This MUST NOT be called directory, see `instanciate()` instead.
     *
     */
    NodecObject(const std::string& name);

public:

    /**
     * @brief Destructor.
     */
    virtual ~NodecObject();


public:
    //! object name.
    std::string name;


    ID id() const noexcept;


private:
    ID id_;

private:
    NODEC_DISABLE_COPY(NodecObject);

public:
    template<typename T, typename... Args>
    static Holder<T> instanciate(Args&&... args)
    {
        static_assert(std::is_base_of<NodecObject, T>::value, "T must be derived from NodecObject.");

        //struct Obj : T {};

        //auto obj = std::shared_ptr<T>(new T());
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

};

// <https://stackoverflow.com/questions/657155/how-to-enable-shared-from-this-of-both-parent-and-derived>

template <typename Base>
inline std::shared_ptr<Base>
shared_from_base(std::enable_shared_from_this<Base>* base)
{
    return base->shared_from_this();
}
template <typename Base>
inline std::shared_ptr<const Base>
shared_from_base(std::enable_shared_from_this<Base> const* base)
{
    return base->shared_from_this();
}
template <typename That>
inline std::shared_ptr<That>
shared_from(That* that)
{
    return std::static_pointer_cast<That>(shared_from_base(that));
}
}

#endif