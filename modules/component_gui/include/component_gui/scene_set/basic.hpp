#ifndef COMPONENT_GUI__SCENE_SET__BASIC_HPP
#define COMPONENT_GUI__SCENE_SET__BASIC_HPP

#include <scene_set/components/basic.hpp>
#include <nodec/math/gfx.hpp>

#include <imgui.h>
#include <algorithm>

namespace scene_set {
namespace components {

void on_gui_name(Name &name) {
    constexpr int buffer_size = 128;
    char str[buffer_size];

    name.name.copy(str, buffer_size - 1);
    auto null_pos = name.name.size() < buffer_size - 1 ? name.name.size() : buffer_size - 1;
    str[null_pos] = '\0';

    ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue;
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    if (ImGui::InputText("##name", str, buffer_size, input_text_flags)) {
        // on type enter
    }
    name.name = str;
}

void on_gui_transform(Transform& trfm) {
    using namespace nodec;

    //constexpr float align = 70;
    //ImGuiStyle& style = ImGui::GetStyle();
    //float child_w = (ImGui::GetContentRegionAvail().x - 2 * style.ItemSpacing.x - align) / 3 - 10; // 10 is one char size (maybe).
    //if (child_w < 1.0f) {
    //    child_w = 1.0f;
    //}

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
        if (ImGui::DragFloat3("##Rotation", euler_angles.v, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f")) {
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

}
}


#endif