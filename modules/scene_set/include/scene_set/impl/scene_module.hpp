#ifndef SCENE_SET__IMPL__SCENE_HPP_
#define SCENE_SET__IMPL__SCENE_HPP_

#include <scene_set/scene.hpp>
#include <scene_set/components/standard.hpp>
#include <scene_set/systems/hierarchy_system.hpp>


namespace scene_set {
namespace impl {

class SceneModule : public Scene {
public:
    SceneModule()
        : hierarchy_system_{ &registry_ } {
    }

    SceneRegistry& registry() {
        return registry_;
    }

    void append_child(const SceneEntity parent, const SceneEntity child) override {
        hierarchy_system_.append_child(parent, child);
    }

    void remove_child(const SceneEntity parent, const SceneEntity child) override {
        hierarchy_system_.remove_child(parent, child);
    }

    const std::vector<SceneEntity>& root_entites() override {
        return hierarchy_system_.root_entities();
    }

    SceneEntity create_entity(const std::string& name) override {
        using namespace scene_set::components;

        auto entity = registry_.create_entity();

        registry_.emplace_component<Name>(entity, name);
        registry_.emplace_component<Hierarchy>(entity);
        registry_.emplace_component<Transform>(entity);

        return entity;
    }

public:
    systems::HierarchySystem& hierarchy_system() {
        return hierarchy_system_;
    }


private:
    SceneRegistry registry_;
    systems::HierarchySystem hierarchy_system_;


};


}
}


#endif