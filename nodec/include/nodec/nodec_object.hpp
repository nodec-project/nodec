#ifndef NODEC__NODEC_OBJECT_HPP_
#define NODEC__NODEC_OBJECT_HPP_

#include <nodec/macros.hpp>
#include <nodec/memory.hpp>

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

} // namespace nodec

#endif