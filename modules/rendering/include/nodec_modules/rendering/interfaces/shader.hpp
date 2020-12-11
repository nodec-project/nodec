#ifndef NODEC_MODULES__RENDERING__INTERFACES__SHADER_HPP_
#define NODEC_MODULES__RENDERING__INTERFACES__SHADER_HPP_

#include "bindable.hpp"
#include <nodec/nodec_object.hpp>

namespace nodec_modules
{
    namespace rendering
    {
        namespace interfaces
        {
            class Shader : public nodec::NodecObject, public IBindable
            {
            public:
                Shader(const std::string& shader_name);
                ~Shader();

            protected:
                void bind_impl(Rendering* rendering) override;
                void unbind_impl(Rendering* rendering) override;

            private:
                std::string shader_name;
            };
        }
    }
}

#endif