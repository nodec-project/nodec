#ifndef NODEC_RENDERING__COMPONENTS__DIRECTIONAL_LIGHT_HPP_
#define NODEC_RENDERING__COMPONENTS__DIRECTIONAL_LIGHT_HPP_

#include <nodec/vector4.hpp>

namespace nodec_rendering {
namespace components {

struct DirectionalLight {
    nodec::Vector4f color{1.0f, 1.0f, 1.0f, 1.0f};
    float intensity{1.0f};
};

} // namespace components
} // namespace nodec_rendering

#endif