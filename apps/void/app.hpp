#ifndef APP__APP_HPP_
#define APP__APP_HPP_

#include <nodec_modules/game_engine/interfaces/game_engine.hpp>
#include <nodec_modules/game_engine/interfaces/behavior.hpp>

#include <nodec_modules/rendering/interfaces/rendering.hpp>

#include <input/keyboard/keyboard.hpp>
#include <input/mouse/mouse.hpp>

#include <nodec_extentions/material_set/brdf_material.hpp>
#include <nodec_extentions/mesh_factory/mesh_factory.hpp>

#include <nodec/scene_set/scene_object.hpp>

#include <nodec/event.hpp>
#include <nodec/logging.hpp>
#include <nodec/math/math.hpp>
#include <nodec/math/gfx.hpp>


class App
{
    using Camera = nodec_modules::rendering::interfaces::Camera;

public:
    static nodec::NodecObject::Reference<Camera> main_camera;

};

#endif
