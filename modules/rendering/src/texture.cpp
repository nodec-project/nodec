#include <nodec_modules/rendering/interfaces/texture.hpp>
#include <nodec_modules/rendering/interfaces/rendering.hpp>

namespace nodec_modules
{
namespace rendering
{
namespace interfaces
{

Texture::Texture(const std::string& path, Rendering* target_rendering) :
    BindableResource(target_rendering, "Texture"),
    path_(path)
{

}

Texture::~Texture()
{
    target_rendering->unbind_texture(this);
}

const char* Texture::path() const noexcept
{
    return path_.c_str();
}
}
}
}