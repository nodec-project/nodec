#pragma once

#include <nodec_rendering/components/camera.hpp>
#include <nodec_rendering/components/image_renderer.hpp>
#include <nodec_rendering/components/light.hpp>
#include <nodec_rendering/components/mesh_renderer.hpp>
#include <nodec_scene/components/basic.hpp>
#include <nodec_scene_audio/components/audio_source.hpp>

#include <nodec/logging.hpp>
#include <nodec/math/gfx.hpp>
#include <nodec/resource_management/resource_registry.hpp>

#include <imgui.h>

#include <algorithm>

class InspectorGUI {
    using MeshRenderer = nodec_rendering::components::MeshRenderer;
    using ResourceRegistry = nodec::resource_management::ResourceRegistry;
    using Name = nodec_scene::components::Name;
    using Transform = nodec_scene::components::Transform;
    using Camera = nodec_rendering::components::Camera;
    using Light = nodec_rendering::components::Light;
    using AudioSource = nodec_scene_audio::components::AudioSource;
    using AudioClip = nodec_scene_audio::resources::AudioClip;
    using ImageRenderer = nodec_rendering::components::ImageRenderer;

public:
    InspectorGUI(ResourceRegistry *pResourceRegistry)
        : mpResourceRegistry{pResourceRegistry} {
    }

    void OnGUIName(Name &name) {
        SetStrBuffer(mTempStrBuffer, sizeof(mTempStrBuffer), name.name);

        ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue;
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputText("##name", mTempStrBuffer, sizeof(mTempStrBuffer), input_text_flags)) {
            // on type enter
        }
        name.name = mTempStrBuffer;
    }

    void onGUITransform(Transform &trfm) {
        using namespace nodec;
        {
            ImGui::Text("Position");
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            if (ImGui::DragFloat3("##Position", trfm.local_position.v, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f")) {
                trfm.dirty = true;
            }
        }

        {
            static Vector3f eulerAngles;
            static bool isActive = false;

            if (!isActive) {
                eulerAngles = math::gfx::euler_angles_xyz(trfm.local_rotation);
            }

            ImGui::Text("Rotation (XYZ Euler)");

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

            if (ImGui::DragFloat3("##Rotation", eulerAngles.v, 0.1f, -FLT_MAX, +FLT_MAX, "%.3f")) {
                math::gfx::set_eulaer_angles_xyz(trfm.local_rotation, eulerAngles);
                trfm.dirty = true;
            }

            isActive = ImGui::IsItemActive();
        }

        {
            ImGui::Text("Scale");
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            if (ImGui::DragFloat3("##Scale", trfm.local_scale.v, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f")) {
                trfm.dirty = true;
            }
        }
    }

    void OnGUIMeshRenderer(MeshRenderer &renderer) {
        using namespace nodec;
        using namespace nodec_rendering::resources;
        using namespace nodec_rendering;
        {
            ImGui::PushID("meshes");
            ImGui::Text("Meshes");

            for (auto i = renderer.meshes.size(); i-- != 0;) {
                ImGui::PushID(i);
                auto &mesh = renderer.meshes[i];
                mesh = ResourceNameEdit("", mesh);
                ImGui::SameLine();
                if (ImGui::Button("-")) {
                    renderer.meshes.erase(renderer.meshes.begin() + i);
                }
                ImGui::PopID();
            }

            if (ImGui::Button("+")) {
                auto empty = mpResourceRegistry->get_resource<Mesh>("").get();
                renderer.meshes.emplace_back(empty);
            }

            ImGui::PopID();
        }
        {
            ImGui::PushID("materials");

            ImGui::Text("Materials");
            for (auto i = renderer.materials.size(); i-- != 0;) {
                ImGui::PushID(i);
                auto &material = renderer.materials[i];
                material = ResourceNameEdit("", material);
                ImGui::SameLine();
                if (ImGui::Button("-")) {
                    renderer.materials.erase(renderer.materials.begin() + i);
                }
                ImGui::PopID();
            }

            if (ImGui::Button("+")) {
                auto empty = mpResourceRegistry->get_resource<Material>("").get();
                renderer.materials.emplace_back(empty);
            }

            ImGui::PopID();
        }
    }

    void OnGUICamera(Camera &camera) {
        ImGui::DragFloat("Near Clip Plane", &camera.nearClipPlane);
        ImGui::DragFloat("Far Clip Plane", &camera.farClipPlane);
        ImGui::DragFloat("Fov Angle", &camera.fovAngle);
    }

    void OnGUILight(Light &light) {
        using namespace nodec_rendering::components;

        int currentType = static_cast<int>(light.type);
        ImGui::Combo("Type", &currentType, "Directional\0Point\0Spot");
        light.type = static_cast<LightType>(currentType);

        ImGui::ColorEdit4("Color", light.color.v, ImGuiColorEditFlags_Float);

        ImGui::DragFloat("Intensity", &light.intensity, 0.005f, 0.0f, 1.0f);
    }

    void OnGuiAudioSource(AudioSource &source) {
        source.clip = ResourceNameEdit("Clip", source.clip);

        ImGui::Checkbox("Is Playing", &source.is_playing);

        {
            int position = source.position.count();
            ImGui::DragInt("Position", &position);
            source.position = std::chrono::milliseconds(position);
        }

        ImGui::Checkbox("Loop", &source.loop);
    }

    void OnGuiImageRenderer(ImageRenderer &renderer) {
        renderer.image = ResourceNameEdit("Image", renderer.image);
        renderer.material = ResourceNameEdit("Material", renderer.material);
        ImGui::DragInt("Pixels Per Unit", &renderer.pixelsPerUnit);
    }

private:
    template<typename T>
    std::shared_ptr<T> ResourceNameEdit(const char *label, std::shared_ptr<T> resource) {
        std::string origName;
        bool found;
        std::tie(origName, found) = mpResourceRegistry->lookup_name(resource);

        SetStrBuffer(mTempStrBuffer, sizeof(mTempStrBuffer), origName);

        if (ImGui::InputText(label, mTempStrBuffer, sizeof(mTempStrBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
            std::string newName = mTempStrBuffer;
            if (newName.empty()) {
                // if empty, set resource to null.
                resource.reset();
            } else {
                auto newResource = mpResourceRegistry->get_resource<T>(newName).get();
                resource = newResource ? newResource : resource;
            }
        }
        return resource;
    }

    void SetStrBuffer(char *pBuffer, const size_t bufferSize, const std::string &source) {
        source.copy(pBuffer, bufferSize - 1);

        auto null_pos = std::min(source.size(), bufferSize - 1);
        pBuffer[null_pos] = '\0';
    }

private:
    ResourceRegistry *mpResourceRegistry;
    char mTempStrBuffer[1024]{0x00};
};