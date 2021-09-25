/*
*
*/
#include <nodec_engine/impl/nodec_engine_module.hpp>
#include <nodec/logging.hpp>

#include <cassert>


namespace nodec_engine {
namespace impl {

namespace {
NodecEngineModule* current{ nullptr };
}

NodecEngineModule::NodecEngineModule() {
    nodec::logging::InfoStream(__FILE__, __LINE__)
        << "[NodecEngineModule] >>> The engine is created!" << std::flush;
}

NodecEngineModule::~NodecEngineModule() {
    nodec::logging::InfoStream(__FILE__, __LINE__)
        << "[NodecEngineModule] >>> The engine is destroyed.\n"
        << "engine_time: " << engine_time() << "[s]" << std::flush;

    if (current == this) {
        current = nullptr;
    }
}

void NodecEngineModule::reset() {
    step_func = &NodecEngineModule::step_first;

    nodec::logging::InfoStream(__FILE__, __LINE__)
        << "[NodecEngineModule] >>> Initializing...";
    {
        engine_timer_.reset();

        initialized_(*this);
    }

    nodec::logging::InfoStream(__FILE__, __LINE__)
        << "[NodecEngineModule] >>> Initializing finished.";

    nodec::logging::InfoStream(__FILE__, __LINE__)
        << "[NodecEngineModule] >>> Booting...";
    {
        on_boot(*this);
    }
    nodec::logging::InfoStream(__FILE__, __LINE__)
        << "[NodecEngineModule] >>> Booting finished.";

    state_ = State::Inactive;
    return;
}


void NodecEngineModule::step() {
    assert(state_ == State::Inactive || state_ == State::Active);
    
    (this->*step_func)();
}

void NodecEngineModule::step_first() {
    engine_timer_.start();
    started_(*this);
    state_ = State::Active;
    step_func = &NodecEngineModule::step_cycle;

    stepped_(*this);
}

void NodecEngineModule::step_cycle() {
    stepped_(*this);
}


void set_current(NodecEngineModule* engine) {
    current = engine;
}

} // namespace impl

NodecEngine& current() {
    if (!impl::current) {
        throw std::runtime_error(nodec::error_fomatter::with_type_file_line<std::runtime_error>(
            "No engine Exception. Current Engine not assigned. May be not instanciated or already deleted.",
            __FILE__, __LINE__));
    }
    return *impl::current;
}



} // namespace game_engine