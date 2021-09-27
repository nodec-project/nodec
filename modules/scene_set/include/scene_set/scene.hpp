#ifndef SCENE_SET__SCENE_HPP_
#define SCENE_SET__SCENE_HPP_

#include <scene_set/scene_registry.hpp>
#include <nodec/entities/registry.hpp>
#include <nodec/macros.hpp>

namespace scene_set {


class Scene {

public:
    Scene() {}

    SceneRegistry& registry() {
        return registry_;
    }


protected:
    SceneRegistry registry_;

private:
    NODEC_DISABLE_COPY(Scene);

};


}

#endif