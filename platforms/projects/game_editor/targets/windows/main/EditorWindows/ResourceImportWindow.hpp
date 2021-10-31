#pragma once

#include "../ResourceExporter.hpp"

#include <imelements/window.hpp>
#include <imgui.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>


class ResourceImportWindow : public imelements::BaseWindow {

public:
    static void init(imelements::WindowManager& manager, const std::string& resource_path) {
        auto& window = manager.get_window<ResourceImportWindow>();
        window.resource_path_ = resource_path;

        ImGui::SetWindowFocus(window.name());
    }

public:
    ResourceImportWindow()
        : BaseWindow("Resource Importer", nodec::Vector2f(600, 280)) {

    }

    void on_gui() override {
        using namespace nodec;

        ImGui::InputText("Source Path", source_path, IM_ARRAYSIZE(source_path));

        if (ImGui::Button("Import")) {

            current_scene = nullptr;
            const auto scene = importer.ReadFile(source_path, 0);

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                logging::ErrorStream(__FILE__, __LINE__) << "Import failed!\n"
                    "details: " << importer.GetErrorString();

                last_import_failed = true;
            }
            else {
                current_scene = scene;
                selected_mesh_index = 0;

                last_import_failed = false;
            }
        }

        if (last_import_failed) {
            std::string message = Formatter() << "Import failed! \n" << importer.GetErrorString();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            ImGui::TextWrapped(message.c_str());
            ImGui::PopStyleColor();
        }

        if (!current_scene) {
            return;
        }

        ImGui::Separator();
        // C:\Users\onete\Desktop\retrotv\uploads_files_2941243_retrotv0319.fbx
        auto resources_header_opened = ImGui::CollapsingHeader("Resources");
        if (resources_header_opened) {
            if (current_scene->mNumMeshes > 0) {
                mesh_resource_import_gui();
            }

            if (current_scene->mNumMaterials > 0) {
                auto opened = ImGui::TreeNodeEx("Material");
                if (opened) {

                    for (int i = 0; i < current_scene->mNumMaterials; ++i) {
                        auto mat = current_scene->mMaterials[i];
                        auto name = mat->GetName();
                        
                        for (int j = 0; j < mat->GetTextureCount(aiTextureType_DIFFUSE); ++j) {
                            aiString str;
                            mat->GetTexture(aiTextureType_DIFFUSE, j, &str);
                            logging::InfoStream(__FILE__, __LINE__) << str.C_Str();
                        }
                    }
                    ImGui::TreePop();
                }
            }

            if (current_scene->mNumTextures > 0) {
                auto opened = ImGui::TreeNodeEx("Texture");
                if (opened) {
                    ImGui::TreePop();
                }
            }


            //for (auto i = 0; i < current_scene->mNumMeshes; ++i) {
            //    std::string label = Formatter() << "mesh-" << i;
            //    auto opened = ImGui::TreeNodeEx(label.c_str());

            //    if (opened) {

            //        ImGui::TreePop();
            //    }
            //}
        }

        auto nodes_header_opened = ImGui::CollapsingHeader("Scene Export");
        if (nodes_header_opened) {

        }
    }

private:
    void mesh_resource_import_gui() {
        using namespace nodec;

        auto opened = ImGui::TreeNodeEx("Mesh");
        if (opened) {
            if (ImGui::BeginListBox("##mesh-list", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing()))) {
                for (int i = 0; i < current_scene->mNumMeshes; ++i) {
                    const bool is_selected = (selected_mesh_index == i);

                    auto mesh = current_scene->mMeshes[i];
                    std::string label;
                    if (mesh->mName.length != 0) {
                        label = Formatter() << mesh->mName.C_Str() << "##mesh-" << i;
                    }
                    else {
                        label = Formatter() << "mesh-" << i;
                    }

                    if (ImGui::Selectable(label.c_str(), is_selected)) {
                        selected_mesh_index = i;
                    }
                }

                ImGui::EndListBox();
            }


            ImGui::InputText("Target Resource Name", target_mesh_resource_name, IM_ARRAYSIZE(target_mesh_resource_name));

            std::string export_path = Formatter() << resource_path_ << "/" << target_mesh_resource_name;
            if (ImGui::Button("Export")) {
                logging::InfoStream(__FILE__, __LINE__) << resource_path_;

                last_mesh_export_failed = !ResourceExporter::ExportMesh(current_scene->mMeshes[selected_mesh_index], export_path);
            }
            if (last_mesh_export_failed) {
                std::string message = Formatter() << "Export failed! \nDoes the directory of '" << export_path << "' exist?";
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                ImGui::TextWrapped(message.c_str());
                ImGui::PopStyleColor();
            }
            ImGui::TreePop();
        }
    }

private:
    char source_path[256]{ 0 };
    bool last_import_failed{ false };
    std::string last_error_string;
    Assimp::Importer importer;
    const aiScene* current_scene{ nullptr };
    std::string resource_path_;

    int selected_mesh_index;
    char target_mesh_resource_name[128]{ 0 };
    bool last_mesh_export_failed;
};