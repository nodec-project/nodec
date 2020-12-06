//#include <nodec_modules/input/keyboard.hpp>
#include <nodec_modules/game_engine/interfaces/game_engine.hpp>
#include <nodec_modules/rendering/interfaces/rendering.hpp>

#include <nodec/logging.hpp>
#include <nodec/event.hpp>

namespace nodec_game_engine = nodec_modules::game_engine::interfaces;
namespace nodec_rendering = nodec_modules::rendering::interfaces;

void on_frame_update(nodec_rendering::Rendering& rendering)
{
    //nodec::logging::debug("frame update", __FILE__, __LINE__);
    nodec::logging::debug_stream(__FILE__, __LINE__) << rendering.frame_delta_time() << std::flush;
}

void on_boot(nodec_game_engine::GameEngine& engine)
{
    nodec::logging::info("booting...", __FILE__, __LINE__);
    nodec::logging::info("HELLO WORLD!", __FILE__, __LINE__);
    
    auto on_frame_update_callback = nodec::event::StaticCallback<nodec_rendering::Rendering&>::make_shared(&on_frame_update);
    engine.rendering().on_frame_update += on_frame_update_callback;

}
