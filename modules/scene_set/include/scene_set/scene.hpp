#ifndef SCENE_SET__SCENE_HPP_
#define SCENE_SET__SCENE_HPP_

#include <scene_set/scene_registry.hpp>
#include <nodec/entities/registry.hpp>
#include <nodec/macros.hpp>

namespace scene_set {


class Scene {

public:
    Scene() {}
    virtual ~Scene() {}


    virtual SceneRegistry& registry() = 0;

    virtual void append_child(const SceneEntity parent, const SceneEntity child) = 0;
    virtual void remove_child(const SceneEntity parent, const SceneEntity child) = 0;



private:
    NODEC_DISABLE_COPY(Scene);

};


}

#endif