#ifndef NODEC_RENDERING__COMPONENTS__CAMERA_HPP_
#define NODEC_RENDERING__COMPONENTS__CAMERA_HPP_

#include <nodec/matrix4x4.hpp>

namespace nodec_rendering {
namespace components {

struct Camera {
    enum class Projection {
        //! Camera will render objects with perspective intact.
        Perspective,

        //! Camera will render objects uniformly, with no sense of perspective.
        Orthographic
    };

    float far_clip_plane{100.0f};
    float near_clip_plane{0.01f};

    Projection projection{Projection::Perspective};

    float fov_angle{45.0f};
    float ortho_width{10.0f};

    nodec::Matrix4x4f world2camera_matrix;
    nodec::Matrix4x4f projection_matrix;
};

} // namespace components
} // namespace nodec_rendering

#endif