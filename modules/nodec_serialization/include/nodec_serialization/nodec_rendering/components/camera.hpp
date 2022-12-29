#ifndef NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__CAMERA_HPP_
#define NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__CAMERA_HPP_

#include <nodec_rendering/components/camera.hpp>
#include <nodec_scene_serialization/base_serializable_component.hpp>


namespace nodec_rendering {
namespace components {

class SerializableCamera : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializableCamera()
        : BaseSerializableComponent(this) {
    }

    float far_clip_plane{100.0f};
    float near_clip_plane{0.01f};

    Camera::Projection projection{Camera::Projection::Perspective};

    float fov_angle{45.0f};
    float ortho_width{10.0f};

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

CEREAL_REGISTER_TYPE(nodec_rendering::components::SerializableCamera)
CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec_scene_serialization::BaseSerializableComponent, nodec_rendering::components::SerializableCamera)

#endif