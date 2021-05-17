/*
*
*/
#include <game_engine/impl/game_engine_module.hpp>
#include <nodec/logging.hpp>


namespace game_engine {
namespace impl {

namespace {
GameEngineModule* current{ nullptr };
}

GameEngineModule::GameEngineModule() {
    nodec::logging::InfoStream(__FILE__, __LINE__)
        << "[GameEngineModule] >>> Engine strat up..." << std::flush;


    screen_module_.size_internal.set(1920, 1080);
    screen_module_.resolution_internal.set(1920, 1080);
    screen_module_.title_internal = "Nodec Game Engine";


    engine_time_stopwatch_.reset();
    engine_time_stopwatch_.start();

    nodec::logging::InfoStream(__FILE__, __LINE__)
        << "[GameEngineModule] >>> Startup completed successfully.\n"
        << "engine_time: " << engine_time() << "[s]" << std::flush;
}

GameEngineModule::~GameEngineModule() {
    nodec::logging::InfoStream(__FILE__, __LINE__)
        << "[GameEngineModule] >>> Engine now shuting down.\n"
        << "engine_time: " << engine_time() << "[s]" << std::flush;

    if (current == this) {
        current = nullptr;
    }
}



void set_current(GameEngineModule* engine) {
    current = engine;
}

bool boot(GameEngineModule& game_engine_module) noexcept {
    nodec::logging::InfoStream(__FILE__, __LINE__) << "[GameEngineModule] >>> Booting..." << std::flush;
    bool success = false;
    try {
        on_boot(game_engine_module);
        success = true;
    }
    catch (const std::exception& e) {
        nodec::logging::ErrorStream(__FILE__, __LINE__)
            << "[GameEngineModule] >>> A Exception has been occured while booting.\n"
            << "detail: " << e.what() << std::flush;
    }
    catch (...) {
        nodec::logging::ErrorStream(__FILE__, __LINE__)
            << "[GameEngineModule] >>> An UnknownException has been occured while booting.\n"
            << "detail: Unavailable." << std::flush;
    }

    nodec::logging::InfoStream(__FILE__, __LINE__)
        << "[GameEngineModule] >>> Booting finished.\n"
        << "success: " << success << "\n"
        << "engine_time: " << game_engine_module.engine_time() << "[s]" << std::flush;

    return success;
}

} // namespace impl

GameEngine& current() {
    if (!impl::current) {
        throw std::runtime_error(nodec::error_fomatter::with_type_file_line<std::runtime_error>(
            "No engine Exception. Current Engine not assigned. May be not instanciated or already deleted.",
            __FILE__, __LINE__));
    }
    return *impl::current;
}



} // namespace game_engine