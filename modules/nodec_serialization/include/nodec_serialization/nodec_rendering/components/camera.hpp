#ifndef NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__CAMERA_HPP_
#define NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__CAMERA_HPP_

#include <nodec_rendering/components/camera.hpp>
#include <nodec_scene_serialization/serializable_component.hpp>

namespace nodec_rendering {
namespace components {

class SerializableCamera : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializableCamera()
        : BaseSerializableComponent(this) {
    }

    SerializableCamera(const Camera &other)
        : BaseSerializableComponent(this),
          far_clip_plane(other.far_clip_plane),
          near_clip_plane(other.near_clip_plane),
          projection(other.projection),
          fov_angle(other.fov_angle),
          ortho_width(other.ortho_width) {}

    operator Camera() const noexcept {
        Camera value;
        value.far_clip_plane = far_clip_plane;
        value.near_clip_plane = near_clip_plane;
        value.projection = projection;
        value.fov_angle = fov_angle;
        value.ortho_width = ortho_width;
        return value;
    }

    float far_clip_plane{100.0f};
    float near_clip_plane{0.01f};

    float fov_angle{45.0f};
    float ortho_width{10.0f};

    Camera::Projection projection{Camera::Projection::Perspective};

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("far_clip_plane", far_clip_plane));
        archive(cereal::make_nvp("near_clip_plane", near_clip_plane));
        archive(cereal::make_nvp("projection", projection));
        archive(cereal::make_nvp("fov_angle", fov_angle));
        archive(cereal::make_nvp("ortho_width", ortho_width));
    }
};

} // namespace components
} // namespace nodec_rendering

NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(nodec_rendering::components::SerializableCamera)

#endif