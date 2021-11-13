#ifndef IMWINDOWS__INSPECTOR_WINDOW_HPP_
#define IMWINDOWS__INSPECTOR_WINDOW_HPP_

#include <imelements/window.hpp>
#include <nodec/entities/registry.hpp>
#include <nodec/vector2.hpp>
#include <nodec/type_info.hpp>

#include <imgui.h>

#include <vector>
#include <unordered_map>


namespace imwindows {


class EntityInspectorWindow : public imelements::BaseWindow {

    using EntityRegistry = nodec::entities::Registry;
    using Entity = nodec::entities::Entity;
    using ChangeTargetSignal = nodec::signals::Signal<void(Entity)>;

public:


    class ComponentRegistry {

        class BaseComponentHandler {
        public:
            virtual ~BaseComponentHandler() {};

            virtual const std::string& component_name() const = 0;
            virtual void on_gui(void* comp) = 0;
            virtual void remove(EntityRegistry& entity_registry, Entity entity) = 0;
            virtual void add(EntityRegistry& entity_registry, Entity entity) = 0;
        };

        template<typename Component>
        class ComponentHandler : public BaseComponentHandler {
        public:
            template <typename Func>
            ComponentHandler(const std::string& component_name, Func&& on_gui_callback)
                : component_name_{ component_name }
                , on_gui_callback_{ on_gui_callback }{

            }

            void on_gui(void* comp) override {
                on_gui_callback_(*static_cast<Component*>(comp));
            }

            void remove(EntityRegistry& entity_registry, Entity entity) override {
                entity_registry.remove_components<Component>(entity);

            }
            void add(EntityRegistry& entity_registry, Entity entity) override {
                entity_registry.emplace_component<Component>(entity);
            }

            const std::string& component_name() const override {
                return component_name_;
            }

        private:
            std::function<void(Component&)> on_gui_callback_;
            const std::string component_name_;
        };


    public:

        class RegistryInterface {
        public:
            RegistryInterface(ComponentRegistry* registry)
                : registry_{ registry } {
            }

            template<typename Component, typename Callback>
            decltype(auto) register_component(const std::string& name, const Callback&& on_gui_callback) {
                registry_->register_component<Component>(name, std::forward<const Callback&&>(on_gui_callback));
            }

        private:
            ComponentRegistry* registry_{ nullptr };

        };

        template<typename Component, typename Callback>
        decltype(auto) register_component(const std::string& name, const Callback&& on_gui_callback) {
            using namespace nodec;

            const auto index = type_seq<Component>::value();

            auto iter = handlers.find(index);
            if (iter == handlers.end()) {
                auto pair = handlers.emplace(index, new ComponentHandler<Component>{ name , on_gui_callback });
                iter = pair.first;

            }
        }

        BaseComponentHandler* get_handler(nodec::TypeId type_seq_index) {
            using namespace nodec;

            auto iter = handlers.find(type_seq_index);
            if (iter == handlers.end()) {
                return nullptr;
            }

            return iter->second.get();
        }

        decltype(auto) begin() {
            return handlers.begin();
        }

        decltype(auto) end() {
            return handlers.end();
        }


        decltype(auto) registry_interface() {
            return RegistryInterface{ this };
        }

    private:
        std::unordered_map<nodec::TypeId, std::unique_ptr<BaseComponentHandler>> handlers;
    };


public:
    static decltype(auto) init(imelements::WindowManager& manager,
                               EntityRegistry* entity_registry,
                               ComponentRegistry* component_registry,
                               Entity init_target_entity,
                               ChangeTargetSignal::SignalInterface change_target_signal) {
        auto& window = manager.get_window<EntityInspectorWindow>();
        window.entity_registry_ = entity_registry;
        window.component_registry_ = component_registry;
        window.target_entity_ = init_target_entity;
        window.change_target_signal_connection_ = change_target_signal.connect([&](auto entity) {window.inspect(entity); });
        ImGui::SetWindowFocus(window.name());
    }

public:
    EntityInspectorWindow()
        : BaseWindow("Entity Inspector", nodec::Vector2f(300, 500)) {

    }

    ~EntityInspectorWindow() {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "Destroyed";
    }

    void on_gui() override {
        using namespace nodec;

        if (!entity_registry_->is_valid(target_entity_)) {
            return;
        }

        logging::InfoStream info(__FILE__, __LINE__);
        entity_registry_->visit(target_entity_, [&](int type_seq_index, void* component) {

            auto* handler = component_registry_->get_handler(type_seq_index);
            if (!handler) {
                return;
            }

            auto opened = ImGui::CollapsingHeader(handler->component_name().c_str());

            // this context item is binded with last element (CollapsingHeader).
            if (ImGui::BeginPopupContextItem()) {
                if (ImGui::Button("Remove")) {
                    handler->remove(*entity_registry_, target_entity_);
                }
                ImGui::EndPopup();
            }

            if (opened) {
                handler->on_gui(component);
            }

                                });

        ImGui::Separator();

        if (ImGui::Button("Add component")) {
            ImGui::OpenPopup("add-component-popup");
        }

        if (ImGui::BeginPopup("add-component-popup")) {
            if (ImGui::BeginListBox("##component-list")) {
                for (auto& pair : *component_registry_) {
                    auto& handler = pair.second;

                    if (ImGui::Selectable(handler->component_name().c_str())) {
                        handler->add(*entity_registry_, target_entity_);
                        ImGui::CloseCurrentPopup();
                    }
                }

                ImGui::EndListBox();
            }
            ImGui::EndPopup();
        }
    }

private:
    void inspect(Entity entity) {
        target_entity_ = entity;
    }

private:
    EntityRegistry* entity_registry_{ nullptr };
    ComponentRegistry* component_registry_{ nullptr };
    Entity target_entity_{ nodec::entities::null_entity };
    ChangeTargetSignal::Connection change_target_signal_connection_;

};


}


#endif