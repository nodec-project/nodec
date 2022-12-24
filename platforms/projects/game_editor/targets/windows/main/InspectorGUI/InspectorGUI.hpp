#pragma once

#include <nodec_physics/components/physics_shape.hpp>
#include <nodec_physics/components/rigid_body.hpp>
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
public:
    InspectorGUI(nodec::resource_management::ResourceRegistry *resource_registry)
        : resource_registry_{resource_registry} {
    }

    void on_gui_name(nodec_scene::components::Name &name) {
        auto &buffer = imessentials::get_text_buffer(1024, name.name);

        ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue;
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputText("##name", buffer.data(), buffer.size(), input_text_flags)) {
            // on type enter
        }
        name.name = buffer.data();
    }

    void on_gui_transform(nodec_scene::components::Transform &trfm);

    void on_gui_mesh_renderer(nodec_rendering::components::MeshRenderer &renderer) {
        using namespace nodec;
        using namespace nodec_rendering::resources;
        using namespace nodec_rendering;
        {
            imessentials::list_edit(
                "Meshes", renderer.meshes,
                [&](int index, auto &mesh) {
                    mesh = resource_name_edit("", mesh);
                },
                [&]() {
                    auto empty = resource_registry_->get_resource_direct<Mesh>("");
                    renderer.meshes.emplace_back(empty);
                },
                [&](int index, auto &mesh) {});
        }
        {
            imessentials::list_edit(
                "Materials", renderer.materials,
                [&](int index, auto &material) {
                    material = resource_name_edit("", material);
                },
                [&]() {
                    auto empty = resource_registry_->get_resource_direct<Material>("");
                    renderer.materials.emplace_back(empty);
                },
                [&](int index, auto &material) {});
        }
    }

    void on_gui_camera(nodec_rendering::components::Camera &camera);

    void on_gui_rigid_body(nodec_physics::components::RigidBody &rigid_body) {
        ImGui::DragFloat("Mass", &rigid_body.mass);
    }

    void on_gui_physics_shape(nodec_physics::components::PhysicsShape &shape) {
        using namespace nodec_physics::components;

        switch (shape.shape_type) {
        case PhysicsShape::ShapeType::Box:
            break;
        default:
            ImGui::Text("Unsupported shape.");
            break;
        }
    }

    void on_gui_directional_light(nodec_rendering::components::DirectionalLight &light) {
        using namespace nodec_rendering::components;

        ImGui::ColorEdit4("Color", light.color.v, ImGuiColorEditFlags_Float);

        ImGui::DragFloat("Intensity", &light.intensity, 0.005f, 0.0f, 1.0f);
    }

    void on_gui_point_light(nodec_rendering::components::PointLight &light) {
        using namespace nodec_rendering::components;

        ImGui::ColorEdit4("Color", light.color.v, ImGuiColorEditFlags_Float);

        ImGui::DragFloat("Intensity", &light.intensity, 0.005f, 0.0f, 1.0f);

        ImGui::DragFloat("Range", &light.range, 0.005f);
    }

    void on_gui_post_processing(nodec_rendering::components::PostProcessing &processing) {
        using namespace nodec_rendering::components;
        using namespace nodec_rendering::resources;

        {
            ImGui::PushID("effects");
            ImGui::Text("Effects");
            for (auto iter = processing.effects.begin(); iter != processing.effects.end();) {
                ImGui::PushID(&*iter);

                ImGui::Checkbox("##enabled", &(iter->enabled));
                ImGui::SameLine();

                auto &material = iter->material;
                material = resource_name_edit("##material", material);

                ImGui::SameLine();
                if (ImGui::Button("-")) {
                    iter = processing.effects.erase(iter);
                    ImGui::PopID();
                    continue;
                }
                ++iter;
                ImGui::PopID();
            }

            if (ImGui::Button("+")) {
                auto material = resource_registry_->get_resource<Material>("").get();
                processing.effects.push_back({false, material});
            }

            ImGui::PopID();
        }
    }

    void on_gui_scene_lighting(nodec_rendering::components::SceneLighting &lighting) {
        ImGui::ColorEdit4("Ambient Color", lighting.ambient_color.v, ImGuiColorEditFlags_Float);
    }

    void on_gui_audio_source(nodec_scene_audio::components::AudioSource &source) {
        source.clip = resource_name_edit("Clip", source.clip);

        ImGui::Checkbox("Is Playing", &source.is_playing);

        {
            int position = source.position.count();
            ImGui::DragInt("Position", &position);
            source.position = std::chrono::milliseconds(position);
        }

        ImGui::Checkbox("Loop", &source.loop);
    }

    void on_gui_image_renderer(nodec_rendering::components::ImageRenderer &renderer) {
        renderer.image = resource_name_edit("Image", renderer.image);
        renderer.material = resource_name_edit("Material", renderer.material);
        ImGui::DragInt("Pixels Per Unit", &renderer.pixels_per_unit);
    }

    void on_gui_text_renderer(nodec_rendering::components::TextRenderer &renderer) {
        auto buffer = imessentials::get_text_buffer(1024, renderer.text);
        ImGui::InputTextMultiline("Text", buffer.data(), buffer.size());
        renderer.text = buffer.data();
        renderer.font = resource_name_edit("Font", renderer.font);
        renderer.material = resource_name_edit("Material", renderer.material);

        ImGui::ColorEdit4("Color", renderer.color.v, ImGuiColorEditFlags_Float);

        ImGui::DragInt("Pixel Size", &renderer.pixel_size);
        ImGui::DragInt("Pixels Per Unit", &renderer.pixels_per_unit);
    }

    void on_gui_non_visible(nodec_rendering::components::NonVisible &nonVisible) {
        ImGui::Checkbox("Self", &nonVisible.self);
    }

private:
    template<typename T>
    std::shared_ptr<T> resource_name_edit(const char *label, std::shared_ptr<T> resource) {
        std::string orig_name;
        bool found;
        std::tie(orig_name, found) = resource_registry_->lookup_name(resource);

        auto &buffer = imessentials::get_text_buffer(1024, orig_name);

        if (ImGui::InputText(label, buffer.data(), buffer.size(), ImGuiInputTextFlags_EnterReturnsTrue)) {
            std::string new_name = buffer.data();
            if (new_name.empty()) {
                // if empty, set resource to null.
                resource.reset();
            } else {
                auto new_resource = resource_registry_->get_resource<T>(new_name).get();
                resource = new_resource ? new_resource : resource;
            }
        }
        return resource;
    }

private:
    nodec::resource_management::ResourceRegistry *resource_registry_;
};