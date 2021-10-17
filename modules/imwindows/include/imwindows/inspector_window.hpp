#ifndef IMWINDOWS__INSPECTOR_WINDOW_HPP_
#define IMWINDOWS__INSPECTOR_WINDOW_HPP_

#include <imelements/window.hpp>
#include <nodec/entities/registry.hpp>
#include <nodec/vector2.hpp>

namespace imwindows {


template<typename Component>
class BaseComponentEditor {
public:

    virtual void on_gui(Component& component) = 0;

};


class InspectorWindow : public imelements::BaseWindow {

    using EntityRegistry = nodec::entities::Registry;
    using Entity = nodec::entities::Entity;
    using ChangeTargetSignal = nodec::signals::Signal<void(Entity)>;

public:
    static decltype(auto) init(imelements::WindowManager& manager,
                               EntityRegistry* entity_registry,
                               ChangeTargetSignal::SignalInterface change_target_signal) {
        auto& window = manager.get_window<InspectorWindow>();
        window.entity_registry_ = entity_registry;
        window.change_target_signal_connection_ = change_target_signal.connect([&](auto entity) {window.inspect(entity); });
        ImGui::SetWindowFocus(window.name());
    }

public:
    InspectorWindow()
        : BaseWindow("Inspector", nodec::Vector2f(200, 500)) {

    }

    ~InspectorWindow() {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "Destroyed";
    }

    void on_gui() override {

    }

private:
    void inspect(Entity entity) {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "inspect " << entity;

        target_entity_ = entity;
    }

private:
    EntityRegistry* entity_registry_;
    Entity target_entity_{ nodec::entities::null_entity };
    ChangeTargetSignal::Connection change_target_signal_connection_;

};


}


#endif