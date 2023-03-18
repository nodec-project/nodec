#ifndef NODEC_SCENE_SERIALIZATION__COMPONENTS__PREFAB_HPP_
#define NODEC_SCENE_SERIALIZATION__COMPONENTS__PREFAB_HPP_

#include "../serializable_component.hpp"

#include <string>

namespace nodec_scene_serialization {
namespace components {

/**
 * @brief
 * @note
 * * 拡張子は, ".entity"
 * * ロード時すでにあるコンポーネントは上書きされない
 * * SerializableEntityNodeをキャッシュしておくか検討
 *
 * @note
 * Prefab is basically loaded but not unloaded.
 *
 * @note
 * Considering the name "Serializable".
 * ただ, Serializableであることとファイルに保存することは別であると思うので,
 * Prefabなのかなと思った.
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
    }
};

} // namespace components
} // namespace nodec_scene_serialization

NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(nodec_scene_serialization::components::Prefab)

#endif