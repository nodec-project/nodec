#ifndef NODEC__STDLIB__NODEC_OBJECT_HPP_
#define NODEC__STDLIB__NODEC_OBJECT_HPP_

#include <string>
#include <memory>

namespace nodec {
    namespace stdlib {

        class NodecObject {
        public:
            /**
            * @brief Create a new NodecObject with specified name.
            */
            NodecObject(const std::string& name);

            /**
            * @brief Destructor.
            */
            virtual ~NodecObject();

        public:
            //! object name.
            std::string name;
        };


        template<class NodecObjectType>
        using NodecObjectHolder = std::shared_ptr<NodecObjectType>;

        template<class NodecObjectType>
        using NodecObjectReference = std::weak_ptr<NodecObjectType>;
    }
}
#endif