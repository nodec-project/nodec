#ifndef NODEC__NODEC_OBJECT_HPP_
#define NODEC__NODEC_OBJECT_HPP_

#include <memory>

namespace nodec {

	class NodecObject {

	};

	class NodecObjectReference {
	public:
		/**
		* @brief Default constructor
		*/
		//NodecObjectReference();

		/**
		* @brief Constructor with the provied pointer to manage
		*/
		explicit NodecObjectReference(NodecObject* obj);

		/**
		* @brief Copy constructor
		*/
		//NodecObjectReference(const NodecObjectReference& ref);

		/**
		* @brief the destructor
		*/
		~NodecObjectReference();

	private:
		NodecObject* obj;

		class ReferenceCounter;
		std::shared_ptr<ReferenceCounter> reference_counter;
	};

}

#endif