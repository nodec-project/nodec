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
        }

        input::interfaces::Keyboard& GameEngineModule::keyboard() const noexcept
        {
            return (*keyboard_module_);
        }

        input::interfaces::Mouse& GameEngineModule::mouse() const noexcept
        {
            return (*mouse_module_);
        }

        input::KeyboardModule& GameEngineModule::keyboard_module() const noexcept
        {
            return (*keyboard_module_);
        }

        input::MouseModule& GameEngineModule::mouse_module() const noexcept
        {
            return (*mouse_module_);
        }
    }
}