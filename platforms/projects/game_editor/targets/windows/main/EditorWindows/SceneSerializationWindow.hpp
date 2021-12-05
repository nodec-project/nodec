#pragma once

#include "../ResourceExporter.hpp"

#include <imelements/window.hpp>
#include <scene_serialization/scene_serialization.hpp>

#include <imgui.h>


class SceneSerializationWindow : public imelements::BaseWindow {
    using SceneEntity = scene_set::SceneEntity;
    using SelectedEntityChangedSignal = nodec::signals::Signal<void(SceneEntity)>;
    using ResourceRegistry = nodec::resource_management::ResourceRegistry;
    using SceneSerialization = scene_serialization::SceneSerialization;
    using Scene = scene_set::Scene;

public:

    static void init(
        imelements::WindowManager& manager,
        scene_set::Scene* scene,
        SceneSerialization* scene_serialization,
        const std::string& resource_path,
        SceneEntity selected_entity,
        SelectedEntityChangedSignal::SignalInterface selected_entity_changed_signal
    ) {

        auto& window = manager.get_window<SceneSerializationWindow>();

        window.scene = scene;
        window.scene_serialization = scene_serialization;
        window.resource_path = resource_path;

        window.selected_entity = selected_entity;

        window.selected_entity_changed_signal_connection
            = selected_entity_changed_signal.connect([&](auto entity) {
            window.selected_entity = entity;
                });

        ImGui::SetWindowFocus(window.name());
    }

public:
    SceneSerializationWindow()
        : BaseWindow("Scene Serialization", nodec::Vector2f(600, 200)) {

    }

    void on_gui() override {
        using namespace nodec;
        //auto import_header_opened = ImGui::CollapsingHeader("Import");

        if (ImGui::BeginTabBar("TabBar"))
        {
            if (ImGui::BeginTabItem("Import"))
            {
                set_str_buffer(temp_str_buffer, IM_ARRAYSIZE(temp_str_buffer), import_path);
                ImGui::InputText("Source", temp_str_buffer, IM_ARRAYSIZE(temp_str_buffer));
                import_path = temp_str_buffer;

                ImGui::Combo("Target", &import_target, "Root\0Selected Entity");

                if (ImGui::Button("Import")) {

                }
                //ImGui::Text("This is the Avocado tab!\nblah blah blah blah blah");

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Export"))
            {
                ImGui::Combo("Source", &export_target, "Root\0Selected Entity");

                set_str_buffer(temp_str_buffer, IM_ARRAYSIZE(temp_str_buffer), export_path);
                ImGui::InputText("Target", temp_str_buffer, IM_ARRAYSIZE(temp_str_buffer));
                export_path = temp_str_buffer;

                if (ImGui::Button("Export")) {
                    std::string dest_path = Formatter() << resource_path << "/" << export_path;

                    ResourceExporter::ExportSceneGraph(scene->root_entites(), scene->registry(), *scene_serialization, dest_path);
                }
                //ImGui::Text("This is the Avocado tab!\nblah blah blah blah blah");
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }

private:
    void set_str_buffer(char* buffer, const size_t buffer_size, const std::string& source) {
        source.copy(buffer, buffer_size - 1);

        auto null_pos = std::min(source.size(), buffer_size - 1);
        buffer[null_pos] = '\0';
    }



private:
    std::string import_path;
    std::string export_path{"level0.scene"};

    int import_target;
    int export_target;
    char temp_str_buffer[512]{ 0 };

    std::string resource_path;
    Scene* scene;
    ResourceRegistry* resource_registry;
    SceneSerialization* scene_serialization;

    SceneEntity selected_entity;

    SelectedEntityChangedSignal::Connection selected_entity_changed_signal_connection;

};