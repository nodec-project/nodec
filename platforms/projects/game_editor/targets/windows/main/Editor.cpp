// Thank you, Microsoft, for file WinDef.h with min/max redefinition.
#define NOMINMAX

#include "Editor.hpp"
#include "EditorConfig.hpp"

#include "EditorWindows/AssetImportWindow.hpp"
#include "EditorWindows/ControlWindow.hpp"
#include "EditorWindows/SceneSerializationWindow.hpp"
#include "EditorWindows/SceneViewWindow.hpp"

#include <imwindows/entity_inspector_window.hpp>
#include <imwindows/log_window.hpp>
#include <imwindows/material_editor_window.hpp>
#include <imwindows/scene_hierarchy_window.hpp>

//#include <nodec_rendering/components/main_camera.hpp>

Editor::Editor(Engine *engine)
    : engine_{engine} {
    using namespace nodec;
    using namespace imwindows;
    using namespace imessentials;
    using namespace nodec_scene::components;
    using namespace nodec_rendering::components;
    using namespace nodec_scene_audio::components;
    using namespace nodec_scene_serialization::components;
    using namespace nodec_physics::components;

    window_manager().register_window<ControlWindow>([=]() {
        return std::make_unique<ControlWindow>(this);
    });

    window_manager().register_window<SceneViewWindow>([=]() {
        return std::make_unique<SceneViewWindow>(engine->window().GetGraphics(),
                                                 engine->world_module().scene(), engine->scene_renderer());
    });

    window_manager().register_window<SceneHierarchyWindow>([=]() {
        auto window = std::make_unique<SceneHierarchyWindow>(&engine->world_module().scene());
        window->selected_entity_changed().connect([=](auto entity) { selection().select(entity); });
        return window;
    });

    window_manager().register_window<EntityInspectorWindow>([=]() {
        return std::make_unique<EntityInspectorWindow>(&engine->world_module().scene().registry(),
                                                       &inspector_component_registry_impl(),
                                                       selection().active_scene_entity(),
                                                       selection().active_scene_entity_changed());
    });

    window_manager().register_window<LogWindow>([=]() {
        return std::make_unique<LogWindow>();
    });

    window_manager().register_window<MaterialEditorWindow>([=]() {
        return std::make_unique<MaterialEditorWindow>(&engine->resources_module().registry());
    });

    window_manager().register_window<AssetImportWindow>([=]() {
        return std::make_unique<AssetImportWindow>(engine->resources_module().resource_path(),
                                                   &engine->world_module().scene(),
                                                   &engine->resources_module().registry());
    });

    window_manager().register_window<SceneSerializationWindow>([=]() {
        return std::make_unique<SceneSerializationWindow>(&engine->world_module().scene(),
                                                          &engine->scene_serialization(),
                                                          engine->resources_module().resource_path(),
                                                          &engine->resources_module().registry(),
                                                          selection().active_scene_entity(),
                                                          selection().active_scene_entity_changed());
    });

    register_menu_item("Window/Control", [=]() {
        auto &window = window_manager().get_window<ControlWindow>();
        window.focus();
    });

    register_menu_item("Window/Scene View", [=]() {
        auto &window = window_manager().get_window<SceneViewWindow>();
        window.focus();
    });

    register_menu_item("Window/Scene Hierarchy", [=]() {
        auto &window = window_manager().get_window<SceneHierarchyWindow>();
        window.focus();
    });

    register_menu_item("Window/Entity Inspector", [=]() {
        auto &window = window_manager().get_window<EntityInspectorWindow>();
        window.focus();
    });

    register_menu_item("Window/Log", [&]() {
        auto &window = window_manager().get_window<LogWindow>();
        window.focus();
    });

    register_menu_item("Window/Material Editor", [&]() {
        auto &window = window_manager().get_window<MaterialEditorWindow>();
        window.focus();
    });

    register_menu_item("Window/Asset Importer", [&]() {
        auto &window = window_manager().get_window<AssetImportWindow>();
        window.focus();
    });

    register_menu_item("Window/Scene Serialization", [&]() {
        auto &window = window_manager().get_window<SceneSerializationWindow>();
        window.focus();
    });

    inspector_gui_.reset(new InspectorGUI(&engine->resources_module().registry()));

    inspector_component_registry_impl().register_component<Name>(
        "Name",
        [=](auto &name) {
            inspector_gui_->on_gui_name(name);
        });

    inspector_component_registry_impl().register_component<RigidBody>(
        "Rigid Body", [=](auto &rigid_body) {
            inspector_gui_->on_gui_rigid_body(rigid_body);
        });

    inspector_component_registry_impl().register_component<PhysicsShape>(
        "Physics Shape", [=](auto &shape) {
            inspector_gui_->on_gui_physics_shape(shape);
        });

    inspector_component_registry_impl().register_component<Transform>(
        "Transform",
        [=](auto &trfm) {
            inspector_gui_->on_gui_transform(trfm);
        });

    inspector_component_registry_impl().register_component<MeshRenderer>(
        "Mesh Renderer",
        [=](auto &renderer) {
            inspector_gui_->on_gui_mesh_renderer(renderer);
        });

    inspector_component_registry_impl().register_component<Camera>(
        "Camera",
        [=](auto &camera) {
            inspector_gui_->on_gui_camera(camera);
        });

    inspector_component_registry_impl().register_component<SceneLighting>(
        "Scene Lighting",
        [=](auto &lighting) {
            inspector_gui_->on_gui_scene_lighting(lighting);
        });

    inspector_component_registry_impl().register_component<DirectionalLight>(
        "Directional Light",
        [=](auto &light) {
            inspector_gui_->on_gui_directional_light(light);
        });

    inspector_component_registry_impl().register_component<PointLight>(
        "Point Light",
        [=](auto &light) {
            inspector_gui_->on_gui_point_light(light);
        });

    inspector_component_registry_impl().register_component<AudioSource>(
        "Audio Source",
        [=](auto &source) {
            inspector_gui_->on_gui_audio_source(source);
        });

    inspector_component_registry_impl().register_component<ImageRenderer>(
        "Image Renderer",
        [=](auto &renderer) {
            inspector_gui_->on_gui_image_renderer(renderer);
        });

    inspector_component_registry_impl().register_component<PostProcessing>(
        "Post Processing",
        [=](auto &process) {
            inspector_gui_->on_gui_post_processing(process);
        });

    inspector_component_registry_impl().register_component<TextRenderer>(
        "Text Renderer",
        [=](auto &renderer) {
            inspector_gui_->on_gui_text_renderer(renderer);
        });

    inspector_component_registry_impl().register_component<NonSerialized>(
        "Non Serialized",
        [=](auto &comp) {});

    inspector_component_registry_impl().register_component<NonVisible>(
        "Non Visible",
        [=](auto &non_visible) {
            inspector_gui_->on_gui_non_visible(non_visible);
        });

    // inspector_component_registry_impl().register_component<MainCamera>(
    //     "Main Camera",
    //     [=](auto &camera) {
    //     });

    [=]() {
        std::ifstream file("editor-config.json", std::ios::binary);
        if (!file) return;

        EditorConfig config;

        try {
            cereal::JSONInputArchive archive(file);
            archive(config);
        } catch (std::exception &e) {
            logging::WarnStream(__FILE__, __LINE__)
                << "Failed to load editor configuration.\n"
                << "details: \n"
                << e.what();
            return;
        }

        if (!config.resource_path.empty()) {
            engine->resources_module().set_resource_path(config.resource_path);
        }

        if (!config.font.path.empty()) {
            auto &io = ImGui::GetIO();
            io.Fonts->AddFontFromFileTTF(config.font.path.c_str(), config.font.pixel_size, NULL, io.Fonts->GetGlyphRangesJapanese());
        }
    }();
}

void Editor::setup() {
    using namespace imwindows;

    window_manager().get_window<ControlWindow>();

    // TODO: At first we should fix SceneRenderer.
    //  SceneRenderer is so buggy for using multi-target rendering.
    // window_manager().get_window<SceneViewWindow>();

    window_manager().get_window<SceneHierarchyWindow>();
    window_manager().get_window<EntityInspectorWindow>();
    window_manager().get_window<LogWindow>();
    window_manager().get_window<MaterialEditorWindow>();
    window_manager().get_window<AssetImportWindow>();
    window_manager().get_window<SceneSerializationWindow>();
}

void Editor::update() {
    switch (state_) {
    case State::Playing:
        engine_->world_module().step();
        break;

    case State::Paused:

        if (do_one_step_) {
            engine_->world_module().step(1 / 60.0f);
            do_one_step_ = false;
        }

        break;
    }

    // switch (mode_) {
    // case Mode::Edit:
    //     update_on_edit_mode();
    //     break;
    // default: break;
    // }

    imessentials::impl::show_main_menu();

    ImGuizmo::BeginFrame();

    window_manager_impl().update_windows();

    bool showDemoWindow = true;
    ImGui::ShowDemoWindow(&showDemoWindow);
}
//
// void Editor::update_on_edit_mode() {
//    auto &scene_registry = engine_->world_module().scene().registry();
//}
