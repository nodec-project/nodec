#ifndef IMWINDOWS__MATERIAL_EDITOR_WINDOW_HPP_
#define IMWINDOWS__MATERIAL_EDITOR_WINDOW_HPP_

#include <imessentials/window.hpp>

#include <nodec_rendering/resources/material.hpp>

#include <nodec/resource_management/resource_registry.hpp>

#include <imgui.h>

namespace imwindows {

class MaterialEditorWindow : public imessentials::BaseWindow {
    using Material = nodec_rendering::resources::Material;

    enum class ControlType {
        DragFloat,
        ColorEdit
    };

public:
    MaterialEditorWindow(nodec::resource_management::ResourceRegistry *registry)
        : BaseWindow{"Material Editor", nodec::Vector2f{200, 500}},
          registry_(registry) {
    }

    void on_gui() override {
        using namespace nodec_rendering;
        using namespace nodec_rendering::resources;

        if (!registry_) return;

        // set_str_buffer(temp_str_buffer, IM_ARRAYSIZE(temp_str_buffer));
        // static char target_path[1024]{0};
        auto material_name = registry_->lookup_name<Material>(target_material_).first;
        set_str_buffer(temp_str_buffer, sizeof(temp_str_buffer), material_name);
        if (ImGui::InputText("Target", temp_str_buffer, IM_ARRAYSIZE(temp_str_buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
            target_material_ = registry_->get_resource<Material>(temp_str_buffer).get();
        }

        ImGui::Separator();

        if (!target_material_) {
            ImGui::Text("No Target");
            return;
        }

        {
            auto shader = target_material_->shader();
            auto shader_name = registry_->lookup_name<Shader>(shader).first;

            set_str_buffer(temp_str_buffer, sizeof(temp_str_buffer), shader_name);
            if (ImGui::InputText("Shader", temp_str_buffer, sizeof(temp_str_buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                std::string new_name = temp_str_buffer;
                if (new_name.empty()) {
                    target_material_->set_shader(nullptr);
                } else {
                    auto shader_to_set = registry_->get_resource<Shader>(temp_str_buffer).get();
                    if (shader_to_set) target_material_->set_shader(shader_to_set);
                }
            }
        }

        auto shader = target_material_->shader();
        if (!shader) {
            ImGui::Text("No Shader");
            return;
        }

        {
            int current = static_cast<int>(target_material_->cull_mode());
            ImGui::Combo("Cull Mode", &current, "Off\0Front\0Back");
            target_material_->set_cull_mode(static_cast<CullMode>(current));
        }

        // Float properties
        {
            ImGui::PushID("float-properties");
            for (int i = 0; i < shader->float_property_count(); ++i) {
                ImGui::PushID(i);
                auto &name = shader->get_float_property_name(i);
                auto current = target_material_->get_float_property(name);
                ImGui::DragFloat(name.c_str(), &current, 0.01f);
                target_material_->set_float_property(name, current);
                ImGui::PopID();
            }
            ImGui::PopID();
        }

        // Vector4 properties
        {
            ImGui::PushID("vector4-properties");
            static ControlType control_type = ControlType::ColorEdit;
            if (ImGui::RadioButton("Float", control_type == ControlType::DragFloat)) control_type = ControlType::DragFloat;
            ImGui::SameLine();
            if (ImGui::RadioButton("Color", control_type == ControlType::ColorEdit)) control_type = ControlType::ColorEdit;

            for (int i = 0; i < shader->vector4_property_count(); ++i) {
                ImGui::PushID(i);
                auto &name = shader->get_vector4_property_name(i);
                auto current = target_material_->get_vector4_property(name).value();
                switch (control_type) {
                case ControlType::DragFloat:
                    ImGui::DragFloat4(name.c_str(), current.v, 0.01f);
                    break;
                default:
                case ControlType::ColorEdit:
                    ImGui::ColorEdit4(name.c_str(), current.v, ImGuiColorEditFlags_Float);
                    break;
                }
                target_material_->set_vector4_property(name, current);
                ImGui::PopID();
            }
            ImGui::PopID();
        }

        {
            ImGui::PushID("texture_entries");

            for (int i = 0; i < shader->texture_entry_count(); ++i) {
                ImGui::PushID(i);

                auto &name = shader->get_texture_entry_name(i);
                auto current = target_material_->get_texture_entry(name).value();

                ImGui::Text(name.c_str());
                {
                    int filter_mode = static_cast<int>(current.sampler.filter_mode);
                    ImGui::Combo("Filter Mode", &filter_mode, "Point\0Bilinear\0Anisotropic");
                    current.sampler.filter_mode = static_cast<Sampler::FilterMode>(filter_mode);

                    int wrap_mode = static_cast<int>(current.sampler.wrap_mode);
                    ImGui::Combo("Wrap Mode", &wrap_mode, "Wrap\0Clamp");
                    current.sampler.wrap_mode = static_cast<Sampler::WrapMode>(wrap_mode);
                }

                {
                    // if (current.texture) {
                    //     ImVec2 size{static_cast<float>(current.texture->width()), static_cast<float>(current.texture->height())};
                    //     ImGui::Image(current.texture.get(), size);
                    // }
                    auto texture_name = registry_->lookup_name<Texture>(current.texture).first;

                    set_str_buffer(temp_str_buffer, sizeof(temp_str_buffer), texture_name);
                    if (ImGui::InputText("Texture", temp_str_buffer, sizeof(temp_str_buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                        std::string new_name = temp_str_buffer;
                        if (new_name.empty()) {
                            current.texture.reset();
                        } else {
                            auto texture_to_set = registry_->get_resource<Texture>(temp_str_buffer).get();

                            if (texture_to_set) current.texture = texture_to_set;
                        }
                    }
                }

                target_material_->set_texture_entry(name, current);
                ImGui::PopID();
            }
            ImGui::PopID();
        }
    }

private:
    static void set_str_buffer(char *buffer, const size_t buffer_size, const std::string &source) {
        source.copy(buffer, buffer_size - 1);

        auto null_pos = std::min(source.size(), buffer_size - 1);
        buffer[null_pos] = '\0';
    }

private:
    nodec::resource_management::ResourceRegistry *registry_{nullptr};
    char temp_str_buffer[1024]{0};
    std::shared_ptr<Material> target_material_;
};
} // namespace imwindows
#endif