#include <nodec_modules/game_engine/game_engine_module.hpp>

namespace nodec_modules
{
    namespace game_engine
    {
        GameEngineModule::GameEngineModule() :
            GameEngine("nodec_modules::game_engine::GameEngineModule")
        {
            keyboard_module_ = nodec::make_nodec_object<input::KeyboardModule>();
            keyboard_ = keyboard_module_;

            mouse_module_ = nodec::make_nodec_object<input::MouseModule>();
            mouse_ = mouse_module_;
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