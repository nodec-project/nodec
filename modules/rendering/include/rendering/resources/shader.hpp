#ifndef NODEC_MODULES__RENDERING__INTERFACES__SHADER_HPP_
#define NODEC_MODULES__RENDERING__INTERFACES__SHADER_HPP_

#include "bindable_resource.hpp"
#include <nodec/nodec_object.hpp>

namespace nodec_modules
{
namespace rendering
{
namespace interfaces
{

class Shader : public BindableResource
{
public:
    Shader(const std::string& shader_name, Rendering* target_rendering);
    ~Shader();

    const char* shader_name() const noexcept;


private:
    std::string shader_name_;
};

}
}
}

#endif