#ifndef IMWINDOWS__IMPL__HIERARCHY_WINDOW_HPP_
#define IMWINDOWS__IMPL__HIERARCHY_WINDOW_HPP_

#include <imelements/window.hpp>
#include <scene_set/scene.hpp>


#include <imgui.h>

namespace imwindows {

class SceneHierarchyWindow : public imelements::BaseWindow {
    using Scene = scene_set::Scene;
    using SceneEntity = scene_set::SceneEntity;

public:
    static void init(imelements::WindowManager& manager, Scene* scene) {
        auto& window = manager.get_window<SceneHierarchyWindow>();
        window.scene_ = scene;
        ImGui::SetWindowFocus(window.name());
    }

public:
    SceneHierarchyWindow()
        : BaseWindow("Hierarchy", nodec::Vector2f(200, 500)) {

    }

    void on_gui() override {
        using namespace scene_set::components;
        using namespace nodec::entities;

        if (!scene_) {
            return;
        }

        if (ImGui::Button("Create")) {
            auto entity = scene_->create_entity("New Entity");
        }

        // Make the copy of the roots.
        // Iterator may be invalidated by adding or deleting elements while iterating.
        auto roots = scene_->root_entites();

        for (auto& root : roots) {
            show_entity_node(root);
        }
    }

private:
    void show_entity_node(const scene_set::SceneEntity entity) {
        using namespace scene_set::components;

        bool node_open = false;
        {
            auto& hier = scene_->registry().get_component<Hierarchy>(entity);
            auto name = scene_->registry().try_get_component<Name>(entity);

            std::string label = nodec::Formatter() << "\"" << (name ? name->name : "") << "\" {entity: 0x" << std::hex << entity << "}";

            ImGuiTreeNodeFlags flags = (hier.children.size() > 0 ? 0x00 : ImGuiTreeNodeFlags_Leaf)
                | (entity == selected_entity_ ? ImGuiTreeNodeFlags_Selected : 0x00);

            node_open = ImGui::TreeNodeEx(label.c_str(), flags);

            if (ImGui::IsItemClicked()) {
                selected_entity_ = entity;
            }
        }

        if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
        {
            if (ImGui::Button("Create")) {
                auto child = scene_->create_entity("New Entity");
                scene_->append_child(entity, child);
            }

            if (ImGui::Button("Delete")) {
                scene_->registry().destroy_entity(entity);
            }
            ImGui::EndPopup();
        }

        if (node_open) {

            //nodec::logging::InfoStream(__FILE__, __LINE__) << "open";
            if (scene_->registry().is_valid(entity)) {
                auto& hier = scene_->registry().get_component<Hierarchy>(entity);

                for (auto child : hier.children) {
                    show_entity_node(child);
                }
            }

            ImGui::TreePop();
        }
    }

    void set_selection(SceneEntity entity) {

    }

private:
    Scene* scene_{ nullptr };
    SceneEntity selected_entity_{ nodec::entities::null_entity };


};

}

#endif