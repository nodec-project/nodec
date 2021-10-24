#ifndef NODEC_MODULES__RENDERING__INTERFACES__TEXTURE_HPP_
#define NODEC_MODULES__RENDERING__INTERFACES__TEXTURE_HPP_

#include "bindable_resource.hpp"

namespace nodec_modules
{
namespace rendering
{
namespace interfaces
{

class Texture : public BindableResource
{
public:
    Texture(const std::string& path, Rendering* target_rendering);
    ~Texture();

    const char* path() const noexcept;
private:
    std::string path_;
};

}
}
}
#endif