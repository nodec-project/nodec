#ifndef COMPONENT_GUI__SCENE_SET__BASIC_HPP
#define COMPONENT_GUI__SCENE_SET__BASIC_HPP

#include <scene_set/components/basic.hpp>


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
    constexpr float align = 70;
    ImGuiStyle& style = ImGui::GetStyle();
    float child_w = (ImGui::GetContentRegionAvail().x - 2 * style.ItemSpacing.x - align) / 3 - 10; // 10 is one char size (maybe).
    if (child_w < 1.0f) {
        child_w = 1.0f;
    }

    ImGui::PushItemWidth(child_w);
    ImGui::Text("Position"); ImGui::SameLine(align);
    ImGui::DragFloat("X##position-x", &trfm.local_position.x, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f"); ImGui::SameLine();
    ImGui::DragFloat("Y##position-y", &trfm.local_position.y, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f"); ImGui::SameLine();
    ImGui::DragFloat("Z##position-z", &trfm.local_position.z, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
    ImGui::PopItemWidth();

    ImGui::Text("Rotation");

    ImGui::PushItemWidth(child_w);
    ImGui::Text("Scale"); ImGui::SameLine(align);
    ImGui::DragFloat("X##scale-x", &trfm.local_scale.x, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f"); ImGui::SameLine();
    ImGui::DragFloat("Y##scale-y", &trfm.local_scale.y, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f"); ImGui::SameLine();
    ImGui::DragFloat("Z##scale-z", &trfm.local_scale.z, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
    ImGui::PopItemWidth();



}

}
}


#endif