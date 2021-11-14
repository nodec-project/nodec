#pragma once

#include <rendering/components/mesh_renderer.hpp>

#include <nodec/resource_management/resource_registry.hpp>
#include <nodec/logging.hpp>

#include <imgui.h>


class InspectorGUI {
    using MeshRenderer = rendering::components::MeshRenderer;
    using ResourceRegistry = nodec::resource_management::ResourceRegistry;

public:
    InspectorGUI(ResourceRegistry* pResourceRegistry)
        : mpResourceRegistry{ pResourceRegistry } {

    }

    void OnGUIMeshRenderer(MeshRenderer& renderer) {
        using namespace nodec;
        using namespace rendering::resources;

        constexpr int strBufferSize = 128;
        char strBuffer[strBufferSize];

        ImGui::Text("Meshes");

        for (auto i = 0; i < renderer.meshes.size(); ++i) {
            auto& mesh = renderer.meshes[i];

            std::string label = Formatter() << "##mesh-" << i;

            auto result = mpResourceRegistry->lookup_name(mesh);
            auto origResourceName = result.first;
            SetStrBuffer(strBuffer, strBufferSize, origResourceName);

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


            if (ImGui::InputText(label.c_str(), strBuffer, strBufferSize, ImGuiInputTextFlags_EnterReturnsTrue)) {
                std::string newResourceName = strBuffer;

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

        for (auto& material : renderer.materials) {

        }
    }

    void SetStrBuffer(char* pBuffer, const size_t bufferSize, const std::string& source) {
        source.copy(pBuffer, bufferSize - 1);

        auto null_pos = std::min(source.size(), bufferSize - 1);
        pBuffer[null_pos] = '\0';
    }

private:
    ResourceRegistry* mpResourceRegistry;


};