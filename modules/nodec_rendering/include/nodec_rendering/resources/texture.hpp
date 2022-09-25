#ifndef NODEC_RENDERING__RESOURCES__TEXTURE_HPP_
#define NODEC_RENDERING__RESOURCES__TEXTURE_HPP_

#include <nodec/macros.hpp>

namespace nodec_rendering {
namespace resources {

class Texture {
public:
    Texture() {}
    virtual ~Texture() {}

    /**
     * @brief Returns the height of the texture in pixels.
     */
    virtual int height() const noexcept = 0;

    /**
     * @brief Returns the width of the texture in pixels.
     */
    virtual int width() const noexcept = 0;

private:
    NODEC_DISABLE_COPY(Texture)
};

} // namespace resources
} // namespace nodec_rendering

#endif