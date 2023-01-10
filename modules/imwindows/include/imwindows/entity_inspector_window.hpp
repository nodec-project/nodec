#ifndef IMWINDOWS__INSPECTOR_WINDOW_HPP_
#define IMWINDOWS__INSPECTOR_WINDOW_HPP_

#include <imessentials/window.hpp>
#include <nodec/entities/registry.hpp>
#include <nodec/type_info.hpp>
#include <nodec/vector2.hpp>

#include <imgui.h>

#include <unordered_map>
#include <vector>

namespace imwindows {

class EntityInspectorWindow final : public imessentials::BaseWindow {
    using EntityRegistry = nodec::entities::Registry;
    using Entity = nodec::entities::Entity;
    using ChangeTargetSignal = nodec::signals::Signal<void(Entity)>;

public:
    class ComponentRegistry {
        class BaseComponentHandler {
        public:
            virtual ~BaseComponentHandler(){};

            virtual const std::string &component_name() const = 0;
            virtual void on_gui(void *comp) = 0;
            virtual void remove(EntityRegistry &entity_registry, Entity entity) = 0;
            virtual void add(EntityRegistry &entity_registry, Entity entity) = 0;
        };

        template<typename Component>
        class ComponentHandler : public BaseComponentHandler {
        public:
            template<typename Func>
            ComponentHandler(const std::string &component_name, Func &&on_gui_callback)
                : component_name_{component_name}, on_gui_callback_{on_gui_callback} {
            }

            void on_gui(void *comp) override {
                on_gui_callback_(*static_cast<Component *>(comp));
            }

            void remove(EntityRegistry &entity_registry, Entity entity) override {
                entity_registry.remove_components<Component>(entity);
            }
            void add(EntityRegistry &entity_registry, Entity entity) override {
                entity_registry.emplace_component<Component>(entity);
            }

            const std::string &component_name() const override {
                return component_name_;
            }

        private:
            std::function<void(Component &)> on_gui_callback_;
            const std::string component_name_;
        };

    public:
        class RegistryInterface {
        public:
            RegistryInterface(ComponentRegistry *registry)
                : registry_{registry} {
            }

            template<typename Component, typename Callback>
            decltype(auto) register_component(const std::string &name, const Callback &&on_gui_callback) {
                registry_->register_component<Component>(name, std::forward<const Callback &&>(on_gui_callback));
            }

        private:
            ComponentRegistry *registry_{nullptr};
        };

        template<typename Component, typename Callback>
        decltype(auto) register_component(const std::string &name, const Callback &&on_gui_callback) {
            using namespace nodec;

            const auto index = type_seq_index<Component>::value();

            auto iter = handlers.find(index);
            if (iter == handlers.end()) {
                auto pair = handlers.emplace(index, new ComponentHandler<Component>{name, on_gui_callback});
                iter = pair.first;
            }
        }

        BaseComponentHandler *get_handler(nodec::type_seq_index_type type_seq_index) {
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

        decltype(auto) size() const noexcept {
            return handlers.size();
        }

        decltype(auto) registry_interface() {
            return RegistryInterface{this};
        }

    private:
        std::unordered_map<nodec::type_seq_index_type, std::unique_ptr<BaseComponentHandler>> handlers;
    };

public:
    static void help_marker(const char *desc) {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

public:
    EntityInspectorWindow(EntityRegistry *entity_registry,
                          ComponentRegistry *component_registry,
                          Entity init_target_entity,
                          ChangeTargetSignal::SignalInterface change_target_signal)
        : BaseWindow("Entity Inspector", nodec::Vector2f(300, 500)),
          entity_registry_(entity_registry),
          component_registry_(component_registry),
          target_entity_(init_target_entity),
          change_target_signal_connection_(change_target_signal.connect([&](auto entity) { inspect(entity); })) {
    }

    ~EntityInspectorWindow() {}

    void on_gui() override {
        using namespace nodec;

        if (!entity_registry_->is_valid(target_entity_)) {
            return;
        }

        struct InspectionInfo {
            int num_of_total_components{0};
            int num_of_visted_components{0};
        };

        InspectionInfo inspection_info;

        entity_registry_->visit(target_entity_, [&](const nodec::type_info &type_info, void *component) {
            ++inspection_info.num_of_total_components;

            auto *handler = component_registry_->get_handler(type_info.seq_index());
            if (!handler) return;

            ++inspection_info.num_of_visted_components;

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
            filter.Draw("Filter");
            if (ImGui::BeginListBox("##component-list")) {
                for (auto &pair : *component_registry_) {
                    auto &handler = pair.second;

                    const char *component_name = handler->component_name().c_str();

                    if (filter.IsActive() && !filter.PassFilter(component_name)) continue;

                    if (ImGui::Selectable(component_name)) {
                        handler->add(*entity_registry_, target_entity_);
                        ImGui::CloseCurrentPopup();
                    }
                }

                ImGui::EndListBox();
            }
            ImGui::EndPopup();
        }

        ImGui::Separator();

        ImGui::Text(static_cast<std::string>(Formatter() << "Visited " << inspection_info.num_of_visted_components << " / "
                                                         << inspection_info.num_of_total_components << " components.")
                        .c_str());
        if (inspection_info.num_of_visted_components < inspection_info.num_of_total_components) {
            ImGui::SameLine();
            help_marker(static_cast<std::string>(Formatter() << "To inspect a component, it must be registered through EntityInspectorWindow::ComponentRegistry.").c_str());
        }
    }

private:
    void inspect(Entity entity) {
        target_entity_ = entity;
    }

private:
    EntityRegistry *entity_registry_{nullptr};
    ComponentRegistry *component_registry_{nullptr};
    Entity target_entity_{nodec::entities::null_entity};
    nodec::signals::Connection change_target_signal_connection_;
    ImGuiTextFilter filter;
};

} // namespace imwindows

#endif