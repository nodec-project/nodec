#pragma once

#include "../ResourceExporter.hpp"
#include "../ResourceImporter.hpp"

#include <imessentials/window.hpp>
#include <nodec_scene_serialization/scene_serialization.hpp>

#include <imgui.h>


class SceneSerializationWindow : public imessentials::BaseWindow {
    using SceneEntity = nodec_scene::SceneEntity;
    using SelectedEntityChangedSignal = nodec::signals::Signal<void(SceneEntity)>;
    using ResourceRegistry = nodec::resource_management::ResourceRegistry;
    using SceneSerialization = nodec_scene_serialization::SceneSerialization;
    using Scene = nodec_scene::Scene;

public:

    static void init(
        imessentials::WindowManager& manager,
        nodec_scene::Scene* scene,
        SceneSerialization* scene_serialization,
        const std::string& resource_path,
        ResourceRegistry* resource_registry,
        SceneEntity selected_entity,
        SelectedEntityChangedSignal::SignalInterface selected_entity_changed_signal
    ) {

        auto& window = manager.get_window<SceneSerializationWindow>();

        window.scene = scene;
        window.scene_serialization = scene_serialization;
        window.resource_path = resource_path;
        window.resource_registry = resource_registry;

        window.selected_entity = selected_entity;

        window.selected_entity_changed_signal_connection
            = selected_entity_changed_signal.connect([&](auto entity) {
            window.selected_entity = entity;
                });

        ImGui::SetWindowFocus(window.name());
    }

public:
    SceneSerializationWindow()
        : BaseWindow("Scene Serialization", nodec::Vector2f(400, 200)) {

    }

    void on_gui() override {
        using namespace nodec;
        using namespace nodec::entities;

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
                    import_messages.clear();

                    bool success = false;
                    switch (import_target) {
                    case 0: // Root
                    {
                        success = ResourceImporter::ImportSceneGraph(import_path, null_entity, *resource_registry, *scene, *scene_serialization);
                        break;
                    }
                    case 1: // Selected Entity
                    {
                        if (scene->registry().is_valid(selected_entity)) {
                            success = ResourceImporter::ImportSceneGraph(import_path, selected_entity, *resource_registry, *scene, *scene_serialization);
                        }
                        break;
                    }
                    default:
                        break;
                    }

                    if (success) {
                        import_messages.emplace_back(ImVec4(0.0f, 1.0f, 0.0f, 1.0f),
                            Formatter() << "Scene import success: " << import_path
                        );
                    }
                    else {
                        import_messages.emplace_back(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
                            Formatter() << "Scene import failed: " << import_path
                        );
                    }
                }

                print_messages(import_messages);

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Export"))
            {
                ImGui::Combo("Source", &export_target, "Root\0Selected Entity");

                set_str_buffer(temp_str_buffer, IM_ARRAYSIZE(temp_str_buffer), export_path);
                ImGui::InputText("Target", temp_str_buffer, IM_ARRAYSIZE(temp_str_buffer));
                export_path = temp_str_buffer;

                bool success = false;

                if (ImGui::Button("Export")) {
                    export_messages.clear();

                    std::string dest_path = Formatter() << resource_path << "/" << export_path;

                    switch (export_target) {
                    case 0: // Root
                    {
                        success = ResourceExporter::ExportSceneGraph(scene->root_entites(), scene->registry(), *scene_serialization, dest_path);
                        break;
                    }

                    case 1: // Selected Entity
                    {
                        if (scene->registry().is_valid(selected_entity)) {
                            std::vector<SceneEntity> roots{ selected_entity };
                            success = ResourceExporter::ExportSceneGraph(roots, scene->registry(), *scene_serialization, dest_path);
                        }
                        break;
                    }

                    default:
                        break;
                    }

                    if (success) {
                        export_messages.emplace_back(ImVec4(0.0f, 1.0f, 0.0f, 1.0f),
                            Formatter() << "Scene export success: " << dest_path
                        );
                    }
                    else {
                        export_messages.emplace_back(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
                            Formatter() << "Scene export failed: " << dest_path
                        );
                    }
                }

                print_messages(export_messages);

                //ImGui::Text("This is the Avocado tab!\nblah blah blah blah blah");
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }

private:
    struct MessageRecord {
        MessageRecord(const ImVec4& color, const std::string& message)
            : color(color)
            , message(message) {
        }

        ImVec4 color;
        std::string message;
    };

private:
    static void set_str_buffer(char* buffer, const size_t buffer_size, const std::string& source) {
        source.copy(buffer, buffer_size - 1);

        auto null_pos = std::min(source.size(), buffer_size - 1);
        buffer[null_pos] = '\0';
    }

    static void print_messages(const std::vector<MessageRecord>& messages) {
        for (auto& record : messages) {
            ImGui::PushStyleColor(ImGuiCol_Text, record.color);
            ImGui::TextWrapped(record.message.c_str());
            ImGui::PopStyleColor();
        }
    }


private:
    std::string import_path;
    std::string export_path{ "level0.scene" };

    std::vector<MessageRecord> export_messages;
    std::vector<MessageRecord> import_messages;

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