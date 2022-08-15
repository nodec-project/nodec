#ifndef NODEC_RENDERING__COMPONENTS__CAMERA_HPP_
#define NODEC_RENDERING__COMPONENTS__CAMERA_HPP_

namespace nodec_rendering {
namespace components {

struct Camera {
    float farClipPlane{100.0f};
    float nearClipPlane{0.01f};
    float fovAngle{45.0f};
};

} // namespace components
} // namespace nodec_rendering

#endif