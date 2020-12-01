#ifndef NODEC__NODEC_OBJECT_HPP_
#define NODEC__NODEC_OBJECT_HPP_

#include <nodec/macros.hpp>

#include <memory>
#include <string>

namespace nodec
{

    class NodecObject
    {
    public:
        /**
         * @brief Create a new NodecObject with specified name.
         */
        NodecObject(const std::string& name);

        /**
         * @brief Destructor.
         */
        virtual ~NodecObject();

        const char* name() const noexcept;

    protected:
        //! object name.
        std::string name_;

    private:
        NODEC_DISABLE_COPY(NodecObject);

    };

    template <class NodecObjectType>
    using NodecObjectHolder = std::shared_ptr<NodecObjectType>;

    template <class NodecObjectType>
    using NodecObjectReference = std::weak_ptr<NodecObjectType>;

    template <class NodecObjectType, typename... Args>
    inline NodecObjectHolder<NodecObjectType> make_nodec_object(Args&& ... args)
    {
        return std::make_shared<NodecObjectType>(std::forward<Args>(args) ...);
    }
}

#endif