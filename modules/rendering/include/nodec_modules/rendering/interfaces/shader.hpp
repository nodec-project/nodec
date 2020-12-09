#ifndef NODEC_MODULES__RENDERING__INTERFACES__SHADER_HPP_
#define NODEC_MODULES__RENDERING__INTERFACES__SHADER_HPP_

#include <nodec/nodec_object.hpp>

namespace nodec_modules
{
    namespace rendering
    {
        namespace interfaces
        {
            class Shader : public nodec::NodecObject
            {
            public:
                Shader(const char* shader_name);
                ~Shader();

            private:
                const char* shader_name;
            };
        }
    }
}

#endif