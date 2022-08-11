/*
 *
 */

#ifndef APP__APP_HPP_
#define APP__APP_HPP_

#define CEREAL_THREAD_SAFE 1

#define EDITOR_MODE

#include <nodec_engine/nodec_engine.hpp>
#include <nodec_input/input.hpp>
#include <nodec_rendering/components/mesh_renderer.hpp>
#include <nodec_rendering/resources/mesh.hpp>
#include <nodec_scene_audio/components/audio_listener.hpp>
#include <nodec_scene_audio/components/audio_source.hpp>
#include <nodec_scene_audio/resources/audio_clip.hpp>
#include <resources/resources.hpp>
#include <scene_serialization/scene_serialization.hpp>
#include <scene_set/components/basic.hpp>
#include <scene_set/scene.hpp>
#include <screen/screen.hpp>

#include <nodec/logging.hpp>

#include <cereal/types/polymorphic.hpp>

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/xml.hpp>

#ifdef EDITOR_MODE
#    include <imgui.h>
#    include <scene_editor/scene_editor.hpp>
#endif

//
//#include <game_engine/game_engine.hpp>
//

//
//#include <scene_set/scene_registry.hpp>
//#include <scene_set/components/standard.hpp>
//#include <scene_set/systems/hierarchy_system.hpp>
//#include <scene_set/systems/transform_system.hpp>

//
//#include <game_engine/game_engine.hpp>
//#include <game_engine/behavior.hpp>
//
//#include <nodec_modules/rendering/interfaces/rendering.hpp>
//
//#include <input/keyboard/keyboard.hpp>
//#include <input/mouse/mouse.hpp>
//
//#include <nodec_extentions/material_set/brdf_material.hpp>
//#include <nodec_extentions/mesh_factory/mesh_factory.hpp>
//
//#include <nodec/scene_set/scene_object.hpp>
//#include <nodec/event.hpp>
//#include <nodec/logging.hpp>
//#include <nodec/math/math.hpp>
//#include <nodec/math/gfx.hpp>
//
//
// class App
//{
//    using Camera = nodec_modules::rendering::interfaces::Camera;
//
// public:
//    static nodec::NodecObject::Reference<Camera> main_camera;
//
//};

#endif
