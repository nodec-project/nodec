#include <nodec_modules/game_engine/game_engine_module.hpp>

namespace nodec_modules
{
    namespace game_engine
    {
        GameEngineModule::GameEngineModule() :
            GameEngine("nodec_modules::game_engine::GameEngineModule")
        {
            keyboard_module_ = nodec::NodecObject::instanciate<input::KeyboardModule>();
            mouse_module_ = nodec::NodecObject::instanciate<input::MouseModule>();
            rendering_module_ = nodec::NodecObject::instanciate<rendering::RenderingModule>();
        }

        // === interface ===
        input::interfaces::Keyboard& GameEngineModule::keyboard() const noexcept
        {
            return (*keyboard_module_);
        }

        input::interfaces::Mouse& GameEngineModule::mouse() const noexcept
        {
            return (*mouse_module_);
        }

        rendering::interfaces::Rendering& GameEngineModule::rendering() const noexcept
        {
            return (*rendering_module_);
        }

        // End interface ===


        input::KeyboardModule& GameEngineModule::keyboard_module() const noexcept
        {
            return (*keyboard_module_);
        }

        input::MouseModule& GameEngineModule::mouse_module() const noexcept
        {
            return (*mouse_module_);
        }

        rendering::RenderingModule& GameEngineModule::rendering_module() const noexcept
        {
            return (*rendering_module_);
        }

        

    }
}