#pragma once

#include <scene_set/components/basic.hpp>
#include <rendering/components/mesh_renderer.hpp>
#include <rendering/components/camera.hpp>

#include <nodec/math/gfx.hpp>
#include <nodec/resource_management/resource_registry.hpp>
#include <nodec/logging.hpp>

#include <imgui.h>

#include <algorithm>


class InspectorGUI {
    using MeshRenderer = rendering::components::MeshRenderer;
    using ResourceRegistry = nodec::resource_management::ResourceRegistry;
    using Name = scene_set::components::Name;
    using Transform = scene_set::components::Transform;
    using Camera = rendering::components::Camera;

public:
    InspectorGUI(ResourceRegistry* pResourceRegistry)
        : mpResourceRegistry{ pResourceRegistry } {

    }

    void OnGUIName(Name& name) {
        SetStrBuffer(mTempStrBuffer, sizeof(mTempStrBuffer), name.name);

        ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue;
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputText("##name", mTempStrBuffer, sizeof(mTempStrBuffer), input_text_flags)) {
            // on type enter
        }
        name.name = mTempStrBuffer;
    }

    void onGUITransform(Transform& trfm) {
        using namespace nodec;

        {
            ImGui::Text("Position");
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            if (ImGui::DragFloat3("##Position", trfm.local_position.v, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f")) {
                trfm.dirty = true;
            }
        }

        {
            auto euler_angles = math::gfx::euler_angles_xyz(trfm.local_rotation);

            ImGui::Text("Rotation (XYZ Euler)");

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            if (ImGui::DragFloat3("##Rotation", euler_angles.v, 0.1f, -FLT_MAX, +FLT_MAX, "%.3f")) {
                math::gfx::set_eulaer_angles_xyz(trfm.local_rotation, euler_angles);
                trfm.dirty = true;
            }
        }

        {
            ImGui::Text("Scale");
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            if (ImGui::DragFloat3("##Scale", trfm.local_scale.v, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f")) {
                trfm.dirty = true;
            }
        }
    }


    void OnGUIMeshRenderer(MeshRenderer& renderer) {
        using namespace nodec;
        using namespace rendering::resources;

        ImGui::Text("Meshes");

        for (auto i = 0; i < renderer.meshes.size(); ++i) {
            auto& mesh = renderer.meshes[i];

            std::string label = Formatter() << "##mesh-" << i;

            auto result = mpResourceRegistry->lookup_name(mesh);
            auto origResourceName = result.first;
            SetStrBuffer(mTempStrBuffer, sizeof(mTempStrBuffer), origResourceName);

            //ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1 , ,1 ,);
            int colorStackCount = 0;

            if (!mesh) {
                // if mesh empty, show invalid resource by red color.
                ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(0.0f / 7.0f, 0.5f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(0.0f / 7.0f, 0.6f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(0.0f / 7.0f, 0.7f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(0.0f / 7.0f, 0.9f, 0.9f));
                colorStackCount += 4;
            }

            if (ImGui::InputText(label.c_str(), mTempStrBuffer, sizeof(mTempStrBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                std::string newResourceName = mTempStrBuffer;

                auto newMesh = mpResourceRegistry->get_resource<Mesh>(newResourceName).get();
                if (newMesh) {
                    // only not null, set as new mesh.
                    mesh = newMesh;
                }
                else {

                }
            }

            ImGui::PopStyleColor(colorStackCount);
        }

        ImGui::Text("Materials");

        for (int i = 0; i < renderer.materials.size(); ++i) {
            auto& material = renderer.materials[i];

            auto result = mpResourceRegistry->lookup_name(material);

            auto origResourceName = result.first;

            std::string label = Formatter() << "##material-" << i;

            SetStrBuffer(mTempStrBuffer, sizeof(mTempStrBuffer), origResourceName);

            if (ImGui::InputText(label.c_str(), mTempStrBuffer, sizeof(mTempStrBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                std::string newResourceName = mTempStrBuffer;

                auto newMaterial = mpResourceRegistry->get_resource<Material>(newResourceName).get();

                if (newMaterial) {
                    material = newMaterial;
                }
            }
        }
    }

    void OnGUICamera(Camera& camera) {
        ImGui::DragFloat("Near Clip Plane", &camera.nearClipPlane);
        ImGui::DragFloat("Far Clip Plane", &camera.farClipPlane);
        ImGui::DragFloat("Fov Angle", &camera.fovAngle);
    }

private:
    void SetStrBuffer(char* pBuffer, const size_t bufferSize, const std::string& source) {
        source.copy(pBuffer, bufferSize - 1);

        auto null_pos = std::min(source.size(), bufferSize - 1);
        pBuffer[null_pos] = '\0';
    }

private:
    ResourceRegistry* mpResourceRegistry;
    char mTempStrBuffer[128]{ 0x00 };

};