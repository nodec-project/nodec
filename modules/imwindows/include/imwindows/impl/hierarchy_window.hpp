#ifndef IMWINDOWS__IMPL__HIERARCHY_WINDOW_HPP_
#define IMWINDOWS__IMPL__HIERARCHY_WINDOW_HPP_

#include <imelements/window.hpp>
#include <scene_set/scene_registry.hpp>
#include <scene_set/components/standard.hpp>

#include <imgui.h>

namespace imwindows {
namespace impl {

class HierarchyWindow : public imelements::BaseWindow {

public:
    static void init(imelements::WindowManager& manager, const scene_set::SceneRegistry* scene_registry) {
        auto& window = manager.get_window<HierarchyWindow>();
        window.scene_registry_ = scene_registry;
        ImGui::SetWindowFocus(window.name());
    }

public:
    HierarchyWindow()
        : BaseWindow("Hierarchy", nodec::Vector2f(200, 500)) {

    }

    void on_gui() override {
        if (!scene_registry_) {
            return;
        }

        auto view = scene_registry_->view<const scene_set::components::Hierarchy>();
        for (auto entity : view) {
            auto &hier = scene_registry_->get_component<scene_set::components::Hierarchy>(entity);

            if (hier.parent != nodec::entities::null_entity) {
                continue;
            }

            show_entity_node(entity);
        }
    }

private:
    void show_entity_node(const scene_set::SceneEntity entity) {
        auto &hier = scene_registry_->get_component<scene_set::components::Hierarchy>(entity);
        auto name = scene_registry_->try_get_component<scene_set::components::Name>(entity);

        std::string label = nodec::Formatter() << "\"" << (name ? name->name : "") << "\" {entity: 0x" << std::hex << entity << "}";

        ImGuiTreeNodeFlags flags = (hier.children.size() > 0 ? 0x00 : ImGuiTreeNodeFlags_Leaf) 
            | (entity == selected_entity_ ? ImGuiTreeNodeFlags_Selected : 0x00);

        bool node_open = ImGui::TreeNodeEx(label.c_str(), flags);

        if (ImGui::IsItemClicked()) {
            selected_entity_ = entity;
        }

        if (node_open) {
            for (auto child : hier.children) {
                show_entity_node(child);
            }

            ImGui::TreePop();
        }

    }

private:
    const scene_set::SceneRegistry* scene_registry_{ nullptr };
    scene_set::SceneEntity selected_entity_{ nodec::entities::null_entity };

};

}
}

#endif