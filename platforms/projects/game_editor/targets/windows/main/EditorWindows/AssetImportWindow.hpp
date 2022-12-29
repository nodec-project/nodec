#pragma once

#include "../ResourceExporter.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <imessentials/window.hpp>
#include <imgui.h>

#include <nodec/formatter.hpp>
#include <nodec/logging.hpp>

#include <cassert>

class AssetImportWindow final : public imessentials::BaseWindow {
    using ResourceRegistry = nodec::resource_management::ResourceRegistry;

public:
    AssetImportWindow(const std::string &resource_path,
                      nodec_scene::Scene *dest_scene,
                      ResourceRegistry *resource_registry)
        : BaseWindow("Asset Importer", nodec::Vector2f(600, 280)),
          resource_path_(resource_path),
          dest_scene_(dest_scene),
          resource_registry_(resource_registry) {
        assert(resource_registry && dest_scene);
        importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
    }

    void on_gui() override {
        using namespace nodec;

        ImGui::InputText("Source Path", source_path, IM_ARRAYSIZE(source_path));

        if (ImGui::Button("Import")) {
            current_scene = nullptr;
            const auto scene = importer.ReadFile(
                source_path,
                aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_MakeLeftHanded | aiProcess_FlipUVs | aiProcess_FlipWindingOrder);

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                logging::ErrorStream(__FILE__, __LINE__) << "Import failed!\n"
                                                            "details: "
                                                         << importer.GetErrorString();

                last_import_failed = true;
            } else {
                current_scene = scene;
                setup_current_scene(scene);
                last_import_failed = false;
            }
        }

        if (last_import_failed) {
            std::string message = Formatter() << "Import failed! \n"
                                              << importer.GetErrorString();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            ImGui::TextWrapped(message.c_str());
            ImGui::PopStyleColor();
        }

        if (!current_scene) {
            return;
        }

        ImGui::Separator();
        // C:\Users\onete\Desktop\models\Alicia\FBX\Alicia_solid_Unity.FBX
        // C:\Users\onete\Desktop\models\retrotv\uploads_files_2941243_retrotv0319.fbx
        // C:\Users\onete\Desktop\models\UnityChanKAGURA_URP-release-1.0.1\Assets\UnityChanKAGURA\Models\FBX\UnityCHanKAGURA.fbx
        auto resources_header_opened = ImGui::CollapsingHeader("Resources");
        if (resources_header_opened) {
            ImGui::InputText("Resource Name Prefix", resource_name_prefix, IM_ARRAYSIZE(resource_name_prefix));

            if (current_scene->mNumMeshes > 0) {
                mesh_resource_import_gui();
            }

            if (current_scene->mNumMaterials > 0) {
                material_resource_import_gui();
            }
        }

        auto nodes_header_opened = ImGui::CollapsingHeader("Scene Export");
        if (nodes_header_opened) {
            if (ImGui::Button("Export")) {
                export_messages_.clear();

                for (unsigned int i = 0; i < current_scene->mNumMeshes; ++i) {
                    auto mesh = current_scene->mMeshes[i];
                    std::string dest_path = Formatter() << resource_path_ << "/"
                                                        << resource_name_prefix << resource_name_map_[Formatter() << "mesh-" << i].target;

                    if (ResourceExporter::ExportMesh(mesh, dest_path)) {
                        export_messages_.emplace_back(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), Formatter() << "Mesh export success: " << dest_path);
                    } else {
                        export_messages_.emplace_back(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
                                                      Formatter() << "Mesh export failed: " << dest_path << "\n"
                                                                  << "Make sure the file path exists.");
                    }
                }

                for (unsigned int i = 0; i < current_scene->mNumMaterials; ++i) {
                    auto mat = current_scene->mMaterials[i];
                    std::string dest_path = Formatter() << resource_path_ << "/"
                                                        << resource_name_prefix << resource_name_map_[Formatter() << "material-" << i].target;

                    if (ResourceExporter::ExportMaterial(mat, dest_path)) {
                        export_messages_.emplace_back(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), Formatter() << "Material export success: " << dest_path);
                    } else {
                        export_messages_.emplace_back(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
                                                      Formatter() << "Material export failed: " << dest_path << "\n"
                                                                  << "Make sure the file path exists.");
                    }
                }

                ResourceExporter::ExportScene(current_scene, *dest_scene_, resource_name_prefix, resource_name_map_, *resource_registry_);
            }

            for (auto &record : export_messages_) {
                ImGui::PushStyleColor(ImGuiCol_Text, record.color);
                ImGui::TextWrapped(record.message.c_str());
                ImGui::PopStyleColor();
            }
        }
    }

private:
    void setup_current_scene(const aiScene *scene) {
        using namespace nodec;

        resource_name_map_.clear();

        for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
            auto mesh = scene->mMeshes[i];

            ResourceNameEntry entry;

            entry.source = mesh->mName.C_Str();
            entry.target = Formatter() << entry.source << "##mesh-" << i << ".mesh";

            resource_name_map_.emplace(Formatter() << "mesh-" << i, entry);
        }

        for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
            auto mat = scene->mMaterials[i];

            ResourceNameEntry entry;

            entry.source = mat->GetName().C_Str();
            entry.target = Formatter() << entry.source << ".material";

            resource_name_map_.emplace(Formatter() << "material-" << i, entry);
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

                for (unsigned int i = 0; i < current_scene->mNumMeshes; ++i) {
                    ImGui::TableNextRow();

                    auto mesh = current_scene->mMeshes[i];
                    auto &entry = resource_name_map_[Formatter() << "mesh-" << i];

                    std::string label = Formatter() << "##mesh-" << i;

                    ImGui::TableNextColumn();
                    ImGui::Text(entry.source.c_str());

                    ImGui::TableNextColumn();

                    set_str_buffer(temp_str_buffer, IM_ARRAYSIZE(temp_str_buffer), entry.target);
                    ImGui::InputText(label.c_str(), temp_str_buffer, IM_ARRAYSIZE(temp_str_buffer));
                    entry.target = temp_str_buffer;
                }

                ImGui::EndTable();
            }
            ImGui::TreePop();
        }
    }

    void material_resource_import_gui() {
        using namespace nodec;

        auto opened = ImGui::TreeNodeEx("Material");

        if (opened) {
            for (unsigned int i = 0; i < current_scene->mNumMaterials; ++i) {
                auto mat = current_scene->mMaterials[i];
                auto &entry = resource_name_map_[Formatter() << "material-" << i];
                std::string label = Formatter() << entry.source << "##material-" << i;

                auto mat_opened = ImGui::TreeNode(label.c_str());
                if (mat_opened) {
                    set_str_buffer(temp_str_buffer, IM_ARRAYSIZE(temp_str_buffer), entry.target);
                    ImGui::InputText("Target Path", temp_str_buffer, IM_ARRAYSIZE(temp_str_buffer));
                    entry.target = temp_str_buffer;
                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }
    }

    void texture_resource_import_gui(aiMaterial *material, aiTextureType type) {
        using namespace nodec;

        for (unsigned int i = 0; i < material->GetTextureCount(type); ++i) {
        }
        std::string str = Formatter() << material->GetTextureCount(type);

        ImGui::Text(str.c_str());
    }

private:
    using ResourceNameEntry = ResourceExporter::ResourceNameEntry;

    struct MessageRecord {
        MessageRecord(const ImVec4 &color, const std::string &message)
            : color(color), message(message) {
        }

        ImVec4 color;
        std::string message;
    };

    static void set_str_buffer(char *buffer, const size_t buffer_size, const std::string &source) {
        source.copy(buffer, buffer_size - 1);

        auto null_pos = std::min(source.size(), buffer_size - 1);
        buffer[null_pos] = '\0';
    }

private:
    std::string resource_path_;
    ResourceRegistry *resource_registry_;
    nodec_scene::Scene *dest_scene_;

    char source_path[256]{0};
    bool last_import_failed{false};
    Assimp::Importer importer;
    const aiScene *current_scene{nullptr};

    char resource_name_prefix[128]{0};
    ResourceExporter::ResourceNameMap resource_name_map_;
    std::vector<MessageRecord> export_messages_;

    char temp_str_buffer[128]{0};
};