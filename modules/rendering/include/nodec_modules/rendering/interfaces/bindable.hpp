#ifndef NODEC_MODULES__RENDERING__INTERFACES__BINDABLE_HPP_
#define NODEC_MODULES__RENDERING__INTERFACES__BINDABLE_HPP_

#include <nodec/nodec_object.hpp>

#include <unordered_map>

namespace nodec_modules
{
    namespace rendering
    {
        namespace interfaces
        {
            class Rendering;

            class IBindable
            {

            public:
                void bind(Rendering* rendering);
                void unbind();

            protected:
                virtual void bind_impl(Rendering* rendering) = 0;
                virtual void unbind_impl(Rendering* rendering) = 0;

            private:
                /**
                * rendering module MUST be alive before destorying this.
                */
                std::unordered_map<nodec::NodecObject::ID, Rendering*> binded_renderings;
            };
        }
    }
}


#endif