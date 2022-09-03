#ifndef NODEC_SCENE__IMPL__SCENE_HPP_
#define NODEC_SCENE__IMPL__SCENE_HPP_

#include "../components/basic.hpp"
#include "../scene.hpp"
#include "../systems/hierarchy_system.hpp"

namespace nodec_scene {
namespace impl {

class SceneModule : public Scene {
public:
    SceneModule()
        : hierarchy_system_{&registry_} {
    }

    SceneRegistry &registry() noexcept override {
        return registry_;
    }

    void append_child(const SceneEntity parent, const SceneEntity child) override {
        hierarchy_system_.append_child(parent, child);
    }

    void remove_child(const SceneEntity parent, const SceneEntity child) override {
        hierarchy_system_.remove_child(parent, child);
    }

    const std::vector<SceneEntity> &root_entites() const noexcept override {
        return hierarchy_system_.root_entities();
    }

    SceneEntity create_entity(const std::string &name) override {
        using namespace nodec_scene::components;

        auto entity = registry_.create_entity();

        registry_.emplace_component<Name>(entity, name);
        registry_.emplace_component<Hierarchy>(entity);
        registry_.emplace_component<Transform>(entity);

        return entity;
    }

    // SceneSignal::SignalInterface initialized() override {
    //     return initialized_.signal_interface();
    // }

    // SceneSignal::SignalInterface stepped() override {
    //     return stepped_.signal_interface();
    // }

public:
    systems::HierarchySystem &hierarchy_system() noexcept {
        return hierarchy_system_;
    }

    // void step() {
    //     stepped_(*this);
    // }

    // void reset() {
    //     registry_.clear();
    //     initialized_(*this);
    // }

private:
    SceneRegistry registry_;
    systems::HierarchySystem hierarchy_system_;
    // SceneSignal stepped_;
    // SceneSignal initialized_;
};

} // namespace impl
} // namespace nodec_scene

#endif