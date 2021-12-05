#pragma once

#include <imelements/window.hpp>

#include <imgui.h>


class SceneSerializationWindow : public imelements::BaseWindow {

public:

    static void init(imelements::WindowManager& manager) {

        auto& window = manager.get_window<SceneSerializationWindow>();

        ImGui::SetWindowFocus(window.name());
    }

public:
    SceneSerializationWindow()
        : BaseWindow("Scene Serialization", nodec::Vector2f(600, 200)) {

    }

    void on_gui() override {
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


    void on_import_gui() {

    }

    void on_export_gui() {

    }

private:
    std::string import_path;
    std::string export_path;

    int import_target;
    int export_target;
    char temp_str_buffer[512]{ 0 };

};