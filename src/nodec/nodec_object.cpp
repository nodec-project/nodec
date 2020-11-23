#include <nodec/nodec_object.hpp>

namespace nodec {

	class NodecObjectReference::ReferenceCounter {
	public:
		/**
		* @brief 
		*/
		void acquire() {

		}

		/**
		* @brief
		*/
		void release() {

		}
	public:
		//! Reference counter
		long* pn;
	};

	NodecObjectReference::NodecObjectReference(NodecObject* obj)
		:reference_counter(new ReferenceCounter)
	{

	}

	NodecObjectReference::~NodecObjectReference() {

	}
}