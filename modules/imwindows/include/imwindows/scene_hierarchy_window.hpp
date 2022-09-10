#ifndef IMWINDOWS__SCENE_HIERARCHY_WINDOW_HPP_
#define IMWINDOWS__SCENE_HIERARCHY_WINDOW_HPP_

#include <imessentials/window.hpp>
#include <nodec_scene/scene.hpp>

#include <imgui.h>

namespace imwindows {

class SceneHierarchyWindow : public imessentials::BaseWindow {
    using Scene = nodec_scene::Scene;
    using SceneEntity = nodec_scene::SceneEntity;

public:
    static decltype(auto) init(imessentials::WindowManager& manager, Scene* scene) {
        auto& window = manager.get_window<SceneHierarchyWindow>();
        window.scene_ = scene;
        ImGui::SetWindowFocus(window.name());
        return window;
    }

public:
    SceneHierarchyWindow()
        : BaseWindow("Scene Hierarchy", nodec::Vector2f(200, 500)) {

    }

    void on_gui() override {
        using namespace nodec_scene::components;
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

    decltype(auto) selected_entity_changed() {
        return selected_entity_changed_.signal_interface();
    }

private:
    void show_entity_node(const SceneEntity entity) {
        using namespace nodec;
        using namespace nodec_scene::components;

        bool node_open = false;
        {
            auto& hier = scene_->registry().get_component<Hierarchy>(entity);
            auto name = scene_->registry().try_get_component<Name>(entity);

            std::string label = nodec::Formatter() << "\"" << (name ? name->name : "") << "\" {entity: 0x" << std::hex << entity << "}";

            ImGuiTreeNodeFlags flags = (hier.children.size() > 0 ? 0x00 : ImGuiTreeNodeFlags_Leaf)
                | (entity == selected_entity_ ? ImGuiTreeNodeFlags_Selected : 0x00)
                | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

            node_open = ImGui::TreeNodeEx(label.c_str(), flags);

            if (ImGui::IsItemClicked()) {
                select(entity);
            }

            // * <https://github.com/ocornut/imgui/issues/2597>
            if (ImGui::BeginDragDropSource()) {
                ImGui::SetDragDropPayload("DND_SCENE_ENTITY", &entity, sizeof(SceneEntity));

                ImGui::Text("Move entity");

                ImGui::EndDragDropSource();
            }

            if (ImGui::BeginDragDropTarget()) {
                if (auto* payload = ImGui::AcceptDragDropPayload("DND_SCENE_ENTITY")) {
                    IM_ASSERT(payload->DataSize == sizeof(SceneEntity));
                    SceneEntity drop_entity = *static_cast<SceneEntity*>(payload->Data);

                    try {
                        scene_->append_child(entity, drop_entity);
                    }
                    catch (std::runtime_error& error) {
                        // The exception occurs when the parent of entity is set into itself. 
                        logging::ErrorStream(__FILE__, __LINE__) << error.what();
                    }
                }

                ImGui::EndDragDropTarget();
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

            if (ImGui::Button("Move to root")) {
                auto parent = scene_->registry().get_component<Hierarchy>(entity).parent;
                if (parent != nodec::entities::null_entity) {
                    scene_->remove_child(parent, entity);
                }
            }
            ImGui::EndPopup();
        }

        if (node_open) {

            if (scene_->registry().is_valid(entity)) {
                auto& hier = scene_->registry().get_component<Hierarchy>(entity);

                for (auto child : hier.children) {
                    show_entity_node(child);
                }
            }

            ImGui::TreePop();
        }
    }


    void select(SceneEntity entity) {
        if (entity == selected_entity_) {
            return;
        }
        selected_entity_ = entity;
        selected_entity_changed_(selected_entity_);
    }

private:
    Scene* scene_{ nullptr };
    SceneEntity selected_entity_{ nodec::entities::null_entity };

    nodec::signals::Signal<void(SceneEntity selected)> selected_entity_changed_;
};

}

#endif