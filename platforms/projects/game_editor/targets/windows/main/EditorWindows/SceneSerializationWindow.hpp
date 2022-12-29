#pragma once

#include "../ResourceExporter.hpp"
#include "../ResourceImporter.hpp"

#include <imessentials/window.hpp>
#include <nodec_scene_serialization/scene_serialization.hpp>

#include <imgui.h>

class SceneSerializationWindow final : public imessentials::BaseWindow {
    using SceneEntity = nodec_scene::SceneEntity;
    using SelectedEntityChangedSignal = nodec::signals::Signal<void(SceneEntity)>;
    using ResourceRegistry = nodec::resource_management::ResourceRegistry;
    using SceneSerialization = nodec_scene_serialization::SceneSerialization;
    using Scene = nodec_scene::Scene;

public:
    SceneSerializationWindow(nodec_scene::Scene *scene,
                             SceneSerialization *scene_serialization,
                             const std::string &resource_path,
                             ResourceRegistry *resource_registry,
                             SceneEntity selected_entity,
                             SelectedEntityChangedSignal::SignalInterface selected_entity_changed_signal)
        : BaseWindow("Scene Serialization", nodec::Vector2f(400, 200)),
          scene_(scene),
          scene_serialization_(scene_serialization),
          resource_path_(resource_path),
          resource_registry_(resource_registry),
          selected_entity_(selected_entity),
          selected_entity_changed_signal_connection_(selected_entity_changed_signal.connect([=](auto entity) { selected_entity_ = entity; })) {}

    void on_gui() override {
        using namespace nodec;
        using namespace nodec::entities;
        using namespace nodec_scene::components;

        // auto import_header_opened = ImGui::CollapsingHeader("Import");

        if (ImGui::BeginTabBar("TabBar")) {
            if (ImGui::BeginTabItem("Import")) {
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
                        success = ResourceImporter::ImportSceneGraph(import_path, null_entity, *resource_registry_, *scene_, *scene_serialization_);
                        break;
                    }
                    case 1: // Selected Entity
                    {
                        if (scene_->registry().is_valid(selected_entity_)) {
                            success = ResourceImporter::ImportSceneGraph(import_path, selected_entity_, *resource_registry_, *scene_, *scene_serialization_);
                        }
                        break;
                    }
                    default:
                        break;
                    }

                    if (success) {
                        import_messages.emplace_back(ImVec4(0.0f, 1.0f, 0.0f, 1.0f),
                                                     Formatter() << "Scene import success: " << import_path);
                    } else {
                        import_messages.emplace_back(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
                                                     Formatter() << "Scene import failed: " << import_path);
                    }
                }

                print_messages(import_messages);

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Export")) {
                ImGui::Combo("Source", &export_target, "Root\0Selected Entity");

                set_str_buffer(temp_str_buffer, IM_ARRAYSIZE(temp_str_buffer), export_path);
                ImGui::InputText("Target", temp_str_buffer, IM_ARRAYSIZE(temp_str_buffer));
                export_path = temp_str_buffer;

                bool success = false;

                if (ImGui::Button("Export")) {
                    export_messages.clear();

                    std::string dest_path = Formatter() << resource_path_ << "/" << export_path;

                    switch (export_target) {
                    case 0: // Root
                    {
                        std::vector<SceneEntity> roots;

                        auto root = scene_->hierarchy_system().root_hierarchy().first;
                        while (root != null_entity) {
                            roots.emplace_back(root);
                            root = scene_->registry().get_component<Hierarchy>(root).next;
                        }

                        success = ResourceExporter::ExportSceneGraph(roots, scene_->registry(), *scene_serialization_, *resource_registry_, dest_path);
                        break;
                    }

                    case 1: // Selected Entity
                    {
                        if (scene_->registry().is_valid(selected_entity_)) {
                            std::vector<SceneEntity> roots{selected_entity_};
                            success = ResourceExporter::ExportSceneGraph(roots, scene_->registry(), *scene_serialization_, *resource_registry_, dest_path);
                        }
                        break;
                    }

                    default:
                        break;
                    }

                    if (success) {
                        export_messages.emplace_back(ImVec4(0.0f, 1.0f, 0.0f, 1.0f),
                                                     Formatter() << "Scene export success: " << dest_path);
                    } else {
                        export_messages.emplace_back(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
                                                     Formatter() << "Scene export failed: " << dest_path);
                    }
                }

                print_messages(export_messages);

                // ImGui::Text("This is the Avocado tab!\nblah blah blah blah blah");
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }

private:
    struct MessageRecord {
        MessageRecord(const ImVec4 &color, const std::string &message)
            : color(color), message(message) {
        }

        ImVec4 color;
        std::string message;
    };

private:
    static void set_str_buffer(char *buffer, const size_t buffer_size, const std::string &source) {
        source.copy(buffer, buffer_size - 1);

        auto null_pos = std::min(source.size(), buffer_size - 1);
        buffer[null_pos] = '\0';
    }

    static void print_messages(const std::vector<MessageRecord> &messages) {
        for (auto &record : messages) {
            ImGui::PushStyleColor(ImGuiCol_Text, record.color);
            ImGui::TextWrapped(record.message.c_str());
            ImGui::PopStyleColor();
        }
    }

private:
    std::string import_path;
    std::string export_path{"level0.scene"};

    std::vector<MessageRecord> export_messages;
    std::vector<MessageRecord> import_messages;

    int import_target;
    int export_target;
    char temp_str_buffer[512]{0};

    std::string resource_path_;
    Scene *scene_;
    ResourceRegistry *resource_registry_;
    SceneSerialization *scene_serialization_;

    SceneEntity selected_entity_;

    SelectedEntityChangedSignal::Connection selected_entity_changed_signal_connection_;
};