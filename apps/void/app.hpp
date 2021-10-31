/*
* 
*/

#ifndef APP__APP_HPP_
#define APP__APP_HPP_

#include <nodec_engine/nodec_engine.hpp>
#include <screen/screen.hpp>
#include <scene_set/scene.hpp>
#include <resources/resources.hpp>

#include <nodec/logging.hpp>


#define EDITOR_MODE

#ifdef EDITOR_MODE
#include <scene_editor/scene_editor.hpp>
#include <imgui.h>
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
//class App
//{
//    using Camera = nodec_modules::rendering::interfaces::Camera;
//
//public:
//    static nodec::NodecObject::Reference<Camera> main_camera;
//
//};

#endif
