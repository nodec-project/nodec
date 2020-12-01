#include <nodec_modules/game_engine/game_engine.hpp>

namespace nodec_modules
{
    namespace game_engine
    {
        input::Keyboard& GameEngine::keyboard() const noexcept
        {
            return (*keyboard_);
        }

        input::Mouse& GameEngine::mouse() const noexcept
        {
            return (*mouse_);
        }
    }
}