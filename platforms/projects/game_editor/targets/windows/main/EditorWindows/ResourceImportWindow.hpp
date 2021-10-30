#pragma once

#include <imelements/window.hpp>
#include <imgui.h>
#include <assimp/Importer.hpp>

class ResourceImportWindow : public imelements::BaseWindow {

public:
    static void init(imelements::WindowManager& manager) {
        auto& window = manager.get_window<ResourceImportWindow>();
        ImGui::SetWindowFocus(window.name());
    }

public:
    ResourceImportWindow()
        : BaseWindow("Resource Importer", nodec::Vector2f(400, 300)) {

    }

    void on_gui() override {
        using namespace nodec;


        ImGui::InputText("Source Path", source_path, IM_ARRAYSIZE(source_path));
        //ImGui::InputText("Dest Path", sourcePathBuffer, buffer_size);


        if (ImGui::Button("Import")) {
            logging::info("click", __FILE__, __LINE__);
            logging::InfoStream(__FILE__, __LINE__) << source_path;
            //logging::InfoStream(__FILE__, __LINE__) << sourcePathBuffer;

            Assimp::Importer importer;
            const auto scene = importer.ReadFile(source_path, 0);

            if (!scene) {
                logging::ErrorStream(__FILE__, __LINE__) << "Import failed!";
                last_import_failed = true;
            }
            else {
                last_import_failed = false;
            }

        }

        if (last_import_failed) {
            ImGui::SameLine(); ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Import failed!");
        }


    }

private:
    char source_path[256]{ 0 };
    bool last_import_failed{ false };

};