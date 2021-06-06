#ifndef IMWINDOWS__IMPL__HIERARCHY_WINDOW_HPP_
#define IMWINDOWS__IMPL__HIERARCHY_WINDOW_HPP_

#include <imelements/window.hpp>
#include <scene_set/scene_registry.hpp>

#include <imgui.h>

namespace imwindows {
namespace impl {

class HierarchyWindow : public imelements::BaseWindow {

public:
    static void init(imelements::WindowManager& manager, scene_set::SceneRegistry* scene_registry) {
        auto& window = manager.get_window<HierarchyWindow>();
        window.scene_registry_ = scene_registry;
        ImGui::SetWindowFocus(window.name());
    }

public:
    HierarchyWindow()
        : BaseWindow("Hierarchy", nodec::Vector2f(200, 500)) {

    }

    void on_gui() override {
        if (!scene_registry_) {
            return;
        }


    }

private:
    scene_set::SceneRegistry* scene_registry_{ nullptr };

};

}
}

#endif