#ifndef NODEC_RENDERING__RESOURCES__TEXTURE_HPP_
#define NODEC_RENDERING__RESOURCES__TEXTURE_HPP_

#include <string>

namespace nodec_rendering {
namespace resources {

class Texture {
public:
    /**
     * @brief Returns the height of the texture in pixels.
     */
    virtual int height() const = 0;

    /**
     * @brief Returns the width of the texture in pixels.
     */
    virtual int width() const = 0;
};

} // namespace resources
} // namespace nodec_rendering

#endif