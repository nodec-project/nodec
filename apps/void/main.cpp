//#include <nodec_modules/input/keyboard.hpp>
#include <nodec_modules/game_engine/interfaces/game_engine.hpp>

#include <nodec/logging.hpp>

namespace nodec_game_engine = nodec_modules::game_engine::interfaces;

void on_boot(nodec_game_engine::GameEngine& engine)
{
    nodec::logging::info("booting...", __FILE__, __LINE__);
    nodec::logging::info("HELLO WORLD!", __FILE__, __LINE__);


}