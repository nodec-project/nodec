#ifndef NODEC_RENDERING__COMPONENTS__POINT_LIGHT_HPP_
#define NODEC_RENDERING__COMPONENTS__POINT_LIGHT_HPP_

#include <nodec/vector4.hpp>

namespace nodec_rendering {
namespace components {

struct PointLight {
    nodec::Vector4f color{1.0f, 1.0f, 1.0f, 1.0f};
    float intensity{1.0f};
    float range{10.0f};
};

} // namespace components
} // namespace nodec_rendering

#endif