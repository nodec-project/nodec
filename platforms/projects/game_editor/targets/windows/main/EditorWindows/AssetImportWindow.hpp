#pragma once

#include "../AssetExporter.hpp"

#include <imelements/window.hpp>
#include <imgui.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <nodec/logging.hpp>
#include <nodec/formatter.hpp>

#include <cassert>


class AssetImportWindow : public imelements::BaseWindow {
    using ResourceRegistry = nodec::resource_management::ResourceRegistry;

public:
    static void init(imelements::WindowManager& manager, const std::string& resource_path,
                     scene_set::Scene* dest_scene,
                     ResourceRegistry *resource_registry) {
        assert(resource_registry && dest_scene);

        auto& window = manager.get_window<AssetImportWindow>();
        window.resource_path_ = resource_path;
        window.dest_scene_ = dest_scene;
        window.resource_registry_ = resource_registry;

        ImGui::SetWindowFocus(window.name());
    }

public:
    AssetImportWindow()
        : BaseWindow("Asset Importer", nodec::Vector2f(600, 280)) {

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
                setup_current_scene(scene);

                //selected_mesh_index = 0;

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
            ImGui::InputText("Resource Name Prefix", resource_name_prefix, IM_ARRAYSIZE(resource_name_prefix));

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
        }

        auto nodes_header_opened = ImGui::CollapsingHeader("Scene Export");
        if (nodes_header_opened) {

            if (ImGui::Button("Export")) {


                AssetExporter::ExportScene(current_scene, *dest_scene_, resource_name_map_, *resource_registry_);
            }
        }
    }

private:


    void setup_current_scene(const aiScene* scene) {
        using namespace nodec;

        resource_name_map_.clear();

        for (int i = 0; i < scene->mNumMeshes; ++i) {
            auto mesh = scene->mMeshes[i];

            ResourceNameEntry entry;

            entry.source = mesh->mName.C_Str();
            entry.target = Formatter() << entry.source << "##mesh-" << i << ".mesh";

            resource_name_map_.emplace(Formatter() << "mesh-" << i, entry);
        }

    }

    void mesh_resource_import_gui() {
        using namespace nodec;

        auto opened = ImGui::TreeNodeEx("Mesh");
        if (opened) {

            if (ImGui::BeginTable("mesh-list", 2)) {

                ImGui::TableSetupColumn("Source");
                ImGui::TableSetupColumn("Target");
                ImGui::TableHeadersRow();

                for (int i = 0; i < current_scene->mNumMeshes; ++i) {
                    ImGui::TableNextRow();

                    auto mesh = current_scene->mMeshes[i];
                    auto& entry = resource_name_map_[Formatter() << "mesh-" << i];
                    
                    std::string label = Formatter() << "##mesh-" << i;

                    ImGui::TableNextColumn();
                    ImGui::Text(entry.source.c_str());

                    ImGui::TableNextColumn();
                    constexpr size_t buffer_size = 128;
                    char name_buffer[buffer_size];
                    entry.target.copy(name_buffer, buffer_size - 1);
                    auto null_pos = std::min(entry.target.size(), buffer_size - 1);
                    name_buffer[null_pos] = '\0';
                    ImGui::InputText(label.c_str(), name_buffer, buffer_size);
                    entry.target = name_buffer;
                }

                ImGui::EndTable();
            }

            //if (ImGui::BeginListBox("##mesh-list", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing()))) {
            //    for (int i = 0; i < current_scene->mNumMeshes; ++i) {
            //        const bool is_selected = (selected_mesh_index == i);

            //        auto mesh = current_scene->mMeshes[i];
            //        std::string label;
            //        if (mesh->mName.length != 0) {
            //            label = Formatter() << mesh->mName.C_Str() << "##mesh-" << i;
            //        }
            //        else {
            //            label = Formatter() << "mesh-" << i;
            //        }

            //        if (ImGui::Selectable(label.c_str(), is_selected)) {
            //            selected_mesh_index = i;
            //        }
            //    }

            //    ImGui::EndListBox();
            //}


            //ImGui::InputText("Target Resource Name", target_mesh_resource_name, IM_ARRAYSIZE(target_mesh_resource_name));

            //std::string export_path = Formatter() << resource_path_ << "/" << target_mesh_resource_name;
            //if (ImGui::Button("Export")) {
            //    logging::InfoStream(__FILE__, __LINE__) << resource_path_;

            //    last_mesh_export_failed = !ResourceExporter::ExportMesh(current_scene->mMeshes[selected_mesh_index], export_path);
            //}
            //if (last_mesh_export_failed) {
            //    std::string message = Formatter() << "Export failed! \nDoes the directory of '" << export_path << "' exist?";
            //    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            //    ImGui::TextWrapped(message.c_str());
            //    ImGui::PopStyleColor();
            //}
            ImGui::TreePop();
        }
    }

private:
    using ResourceNameEntry = AssetExporter::ResourceNameEntry;

private:
    std::string resource_path_;
    ResourceRegistry* resource_registry_;
    scene_set::Scene* dest_scene_;
    

    char source_path[256]{ 0 };
    bool last_import_failed{ false };
    std::string last_error_string;
    Assimp::Importer importer;
    const aiScene* current_scene{ nullptr };

    char resource_name_prefix[128]{ 0 };
    AssetExporter::ResourceNameMap resource_name_map_;

    bool last_mesh_export_failed;
};