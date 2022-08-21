#ifndef IMWINDOWS__MATERIAL_EDITOR_WINDOW_HPP_
#define IMWINDOWS__MATERIAL_EDITOR_WINDOW_HPP_

#include <imelements/window.hpp>

#include <imgui.h>

namespace imwindows {

class MaterialEditorWindow : public imelements::BaseWindow {
public:
    static decltype(auto) init(imelements::WindowManager &manager) {
    }
};
} // namespace imwindows
#endif