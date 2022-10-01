#ifndef IMESSENTIALS__LIST_HPP_
#define IMESSENTIALS__LIST_HPP_

#include <imgui.h>

namespace imessentials {

template<typename List, typename EachItemCallback, typename AddCallback, typename RemoveCallback>
void list_edit(
    const char *label, List &list, EachItemCallback &&on_each_item,
    AddCallback &&on_added, RemoveCallback &&on_removed) {
    ImGui::PushID(label);
    ImGui::Text(label);

    int index = 0;
    for (auto iter = list.begin(); iter != list.end(); ++index) {
        ImGui::PushID(index);

        on_each_item(index, *iter);

        ImGui::SameLine();
        if (ImGui::Button("-")) {
            on_removed(index, *iter);
            iter = list.erase(iter);
            ImGui::PopID();
            continue;
        }

        ++iter;
        ImGui::PopID();
    }

    if (ImGui::Button("+")) {
        on_added();
    }
    ImGui::PopID();
}

} // namespace imessentials

#endif