
#define CEREAL_THREAD_SAFE 1

// Thank you, Microsoft, for file WinDef.h with min/max redefinition.
#define NOMINMAX

#include "Editor.hpp"
#include "EditorConfig.hpp"

#include "EditorWindows/AssetImportWindow.hpp"
#include "EditorWindows/ControlWindow.hpp"
#include "EditorWindows/SceneSerializationWindow.hpp"

#include <imwindows/entity_inspector_window.hpp>
#include <imwindows/log_window.hpp>
#include <imwindows/material_editor_window.hpp>
#include <imwindows/scene_hierarchy_window.hpp>

Editor::Editor(Engine *engine)
    : engine_{engine} {
    using namespace nodec;
    using namespace imwindows;
    using namespace imessentials;
    using namespace nodec_scene::components;
    using namespace nodec_rendering::components;
    using namespace nodec_scene_audio::components;
    using namespace nodec_scene_serialization::components;

    register_menu_item("Window/Control",
                       [=]() { ControlWindow::init(window_manager(), this); });

    register_menu_item("Window/Scene Hierarchy",
                       [=]() {
                           auto &window = SceneHierarchyWindow::init(window_manager(), &engine->world_module().scene());
                           window.selected_entity_changed().connect([=](auto entity) { selection().select(entity); });
                       });

    register_menu_item("Window/Entity Inspector",
                       [=]() {
                           EntityInspectorWindow::init(
                               window_manager(),
                               &engine->world_module().scene().registry(),
                               &inspector_component_registry_impl(),
                               selection().active_scene_entity(),
                               selection().active_scene_entity_changed());
                       });

    register_menu_item("Window/Log",
                       [=]() { LogWindow::init(window_manager()); });

    register_menu_item("Window/Material Editor",
                       [=]() { MaterialEditorWindow::init(
                                   window_manager(),
                                   &engine->resources_module().registry()); });

    register_menu_item("Window/Asset Importer",
                       [=]() {
                           AssetImportWindow::init(
                               window_manager(),
                               engine->resources_module().resource_path(),
                               &engine->world_module().scene(),
                               &engine->resources_module().registry());
                       });

    register_menu_item("Window/Scene Serialization",
                       [=]() {
                           SceneSerializationWindow::init(
                               window_manager(),
                               &engine->world_module().scene(),
                               &engine->scene_serialization(),
                               engine->resources_module().resource_path(),
                               &engine->resources_module().registry(),
                               selection().active_scene_entity(),
                               selection().active_scene_entity_changed());
                       });

    inspector_gui_.reset(new InspectorGUI(&engine->resources_module().registry()));

    inspector_component_registry_impl().register_component<Name>(
        "Name",
        [=](auto &name) {
            inspector_gui_->OnGUIName(name);
        });

    inspector_component_registry_impl().register_component<Transform>(
        "Transform",
        [=](auto &trfm) {
            inspector_gui_->onGUITransform(trfm);
        });

    inspector_component_registry_impl().register_component<MeshRenderer>(
        "Mesh Renderer",
        [=](auto &renderer) {
            inspector_gui_->OnGUIMeshRenderer(renderer);
        });

    inspector_component_registry_impl().register_component<Camera>(
        "Camera",
        [=](auto &camera) {
            inspector_gui_->OnGUICamera(camera);
        });

    inspector_component_registry_impl().register_component<SceneLighting>(
        "Scene Lighting",
        [=](auto &lighting) {
            inspector_gui_->OnGuiSceneLighting(lighting);
        });

    inspector_component_registry_impl().register_component<DirectionalLight>(
        "Directional Light",
        [=](auto &light) {
            inspector_gui_->OnGUIDirectionalLight(light);
        });

    inspector_component_registry_impl().register_component<PointLight>(
        "Point Light",
        [=](auto &light) {
            inspector_gui_->OnGUIPointLight(light);
        });

    inspector_component_registry_impl().register_component<AudioSource>(
        "Audio Source",
        [=](auto &source) {
            inspector_gui_->OnGuiAudioSource(source);
        });

    inspector_component_registry_impl().register_component<ImageRenderer>(
        "Image Renderer",
        [=](auto &renderer) {
            inspector_gui_->OnGuiImageRenderer(renderer);
        });

    inspector_component_registry_impl().register_component<PostProcessing>(
        "Post Processing",
        [=](auto &process) {
            inspector_gui_->OnGUIPostProcessing(process);
        });

    inspector_component_registry_impl().register_component<TextRenderer>(
        "Text Renderer",
        [=](auto &renderer) {
            inspector_gui_->OnGuiTextRenderer(renderer);
        });

    inspector_component_registry_impl().register_component<NonSerialized>(
        "Non Serialized",
        [=](auto &comp) {});

    inspector_component_registry_impl().register_component<NonVisible>(
        "Non Visible",
        [=](auto &non_visible) {
            inspector_gui_->OnGuiNonVisible(non_visible);
        });

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