#pragma once

#include <nodec_rendering/components/camera.hpp>
#include <nodec_rendering/components/directional_light.hpp>
#include <nodec_rendering/components/image_renderer.hpp>
#include <nodec_rendering/components/mesh_renderer.hpp>
#include <nodec_rendering/components/non_visible.hpp>
#include <nodec_rendering/components/point_light.hpp>
#include <nodec_rendering/components/post_processing.hpp>
#include <nodec_rendering/components/scene_lighting.hpp>
#include <nodec_rendering/components/text_renderer.hpp>
#include <nodec_scene/components/name.hpp>
#include <nodec_scene/components/transform.hpp>
#include <nodec_scene_audio/components/audio_source.hpp>

#include <imessentials/list.hpp>
#include <imessentials/text_buffer.hpp>

#include <nodec/logging.hpp>
#include <nodec/math/gfx.hpp>
#include <nodec/resource_management/resource_registry.hpp>

#include <imgui.h>

#include <algorithm>

class InspectorGUI {
    using MeshRenderer = nodec_rendering::components::MeshRenderer;
    using ResourceRegistry = nodec::resource_management::ResourceRegistry;
    using Name = nodec_scene::components::Name;
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

    void onGUITransform(nodec_scene::components::Transform &trfm);

    void OnGUIMeshRenderer(MeshRenderer &renderer) {
        using namespace nodec;
        using namespace nodec_rendering::resources;
        using namespace nodec_rendering;
        {
            imessentials::list_edit(
                "Meshes", renderer.meshes,
                [&](int index, auto &mesh) {
                    mesh = ResourceNameEdit("", mesh);
                },
                [&]() {
                    auto empty = mpResourceRegistry->get_resource_direct<Mesh>("");
                    renderer.meshes.emplace_back(empty);
                },
                [&](int index, auto &mesh) {});
        }
        {
            imessentials::list_edit(
                "Materials", renderer.materials,
                [&](int index, auto &material) {
                    material = ResourceNameEdit("", material);
                },
                [&]() {
                    auto empty = mpResourceRegistry->get_resource_direct<Material>("");
                    renderer.materials.emplace_back(empty);
                },
                [&](int index, auto &material) {});
        }
    }

    void OnGUICamera(nodec_rendering::components::Camera & camera);

    void OnGUIDirectionalLight(nodec_rendering::components::DirectionalLight &light) {
        using namespace nodec_rendering::components;

        ImGui::ColorEdit4("Color", light.color.v, ImGuiColorEditFlags_Float);

        ImGui::DragFloat("Intensity", &light.intensity, 0.005f, 0.0f, 1.0f);
    }

    void OnGUIPointLight(nodec_rendering::components::PointLight &light) {
        using namespace nodec_rendering::components;

        ImGui::ColorEdit4("Color", light.color.v, ImGuiColorEditFlags_Float);

        ImGui::DragFloat("Intensity", &light.intensity, 0.005f, 0.0f, 1.0f);

        ImGui::DragFloat("Range", &light.range, 0.005f);
    }

    void OnGUIPostProcessing(nodec_rendering::components::PostProcessing &postProcessing) {
        using namespace nodec_rendering::components;
        using namespace nodec_rendering::resources;

        {
            ImGui::PushID("effects");
            ImGui::Text("Effects");
            for (auto iter = postProcessing.effects.begin(); iter != postProcessing.effects.end();) {
                ImGui::PushID(&*iter);

                ImGui::Checkbox("##enabled", &(iter->enabled));
                ImGui::SameLine();

                auto &material = iter->material;
                material = ResourceNameEdit("##material", material);

                ImGui::SameLine();
                if (ImGui::Button("-")) {
                    iter = postProcessing.effects.erase(iter);
                    ImGui::PopID();
                    continue;
                }
                ++iter;
                ImGui::PopID();
            }

            if (ImGui::Button("+")) {
                auto material = mpResourceRegistry->get_resource<Material>("").get();
                postProcessing.effects.push_back({false, material});
            }

            ImGui::PopID();
        }
    }

    void OnGuiSceneLighting(nodec_rendering::components::SceneLighting &lighting) {
        ImGui::ColorEdit4("Ambient Color", lighting.ambient_color.v, ImGuiColorEditFlags_Float);
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
        ImGui::DragInt("Pixels Per Unit", &renderer.pixels_per_unit);
    }

    void OnGuiTextRenderer(nodec_rendering::components::TextRenderer &renderer) {
        auto buffer = imessentials::get_text_buffer(1024, renderer.text);
        ImGui::InputTextMultiline("Text", buffer.data(), buffer.size());
        renderer.text = buffer.data();
        renderer.font = ResourceNameEdit("Font", renderer.font);
        renderer.material = ResourceNameEdit("Material", renderer.material);

        ImGui::ColorEdit4("Color", renderer.color.v, ImGuiColorEditFlags_Float);

        ImGui::DragInt("Pixel Size", &renderer.pixel_size);
        ImGui::DragInt("Pixels Per Unit", &renderer.pixels_per_unit);
    }

    void OnGuiNonVisible(nodec_rendering::components::NonVisible &nonVisible) {
        ImGui::Checkbox("Self", &nonVisible.self);
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