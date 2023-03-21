#ifndef NODEC_SCENE_SERIALIZATION__COMPONENTS__PREFAB_HPP_
#define NODEC_SCENE_SERIALIZATION__COMPONENTS__PREFAB_HPP_

#include "../serializable_component.hpp"

#include <string>

namespace nodec_scene_serialization {
namespace components {

/**
 * @brief
 * @note
 * * The file extension is ".entity".
 * * Not overwrite the already existed components.
 * * Consider caching the loaded SerializableEntity.
 *
 * @note
 * Prefab is basically loaded but not unloaded.
 *
 * @note
 * Should we name it "Serializable"?
 * However, I think being Serializable and being saved to a file are two separate things,
 * so I thought it might be a Prefab.
 */
class Prefab : public BaseSerializableComponent {
public:
    Prefab()
        : BaseSerializableComponent(this) {}

    /**
     * @brief
     * serializable.
     */
    std::string source;

    /**
     * @brief This property is true if the prefab is currently active.
     *
     * [serializable]
     */
    bool active{true};

    // Can we specify how to load?
    // Synchronously or asynchronously?
    // loadPolicy?

    // status

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("source", source));
        archive(cereal::make_nvp("active", active));
    }
};

} // namespace components
} // namespace nodec_scene_serialization

NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(nodec_scene_serialization::components::Prefab)

#endif