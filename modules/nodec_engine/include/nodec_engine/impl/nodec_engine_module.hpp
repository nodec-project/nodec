#ifndef NODEC_ENGINE__IMPL__NODEC_ENGINE_MODULE_HPP_
#define NODEC_ENGINE__IMPL__NODEC_ENGINE_MODULE_HPP_

#include <nodec_engine/nodec_engine.hpp>
#include <nodec/signals.hpp>
#include <nodec/logging.hpp>

#include <vector>
#include <string>
#include <memory>
#include <cassert>


namespace nodec_engine {
namespace impl {

class NodecEngineModule : public NodecEngine {
public:
    NodecEngineModule() {
        nodec::logging::InfoStream(__FILE__, __LINE__)
            << "[NodecEngineModule] >>> The engine is created!";

    }

    ~NodecEngineModule() {
        nodec::logging::InfoStream(__FILE__, __LINE__)
            << "[NodecEngineModule] >>> The engine is destroyed.\n"
            << "engine_time: " << engine_time() << "[s]";

    }

public:
    void configure() {
        assert(state_ == State::Unconfigured);

        engine_timer_.start();

        boot();
        initialize();

        state_ = State::Inactive;
    }

    void step() {
        assert(state_ != State::Unconfigured);

        (this->*step_func)();
    }

    void reset() {
        assert(state_ != State::Unconfigured);

        step_func = &NodecEngineModule::step_first;
        initialize();

        state_ = State::Inactive;
    }

public:

    float engine_time() const noexcept override {
        return std::chrono::duration<float>(engine_timer_.elapsed()).count();
    }


    EngineSignal::SignalInterface initialized() override {
        return initialized_.signal_interface();
    }

    EngineSignal::SignalInterface stepped() override {
        return stepped_.signal_interface();
    }

private:
    void initialize() {
        nodec::logging::InfoStream(__FILE__, __LINE__)
            << "[NodecEngineModule] >>> Initializing...";
        {
            initialized_(*this);
        }
        nodec::logging::InfoStream(__FILE__, __LINE__)
            << "[NodecEngineModule] >>> Initializing finished.";
    }

    void boot() {
        nodec::logging::InfoStream(__FILE__, __LINE__)
            << "[NodecEngineModule] >>> Booting...";
        {
            on_boot(*this);
        }
        nodec::logging::InfoStream(__FILE__, __LINE__)
            << "[NodecEngineModule] >>> Booting finished.";
    }

    void step_first() {
        assert(state_ == State::Inactive);

        state_ = State::Active;
        step_func = &NodecEngineModule::step_cycle;

        //nodec::logging::InfoStream(__FILE__, __LINE__) << "[step_first]";
        stepped_(*this);
    }

    void step_cycle() {
        assert(state_ == State::Active);

        //nodec::logging::InfoStream(__FILE__, __LINE__) << "[step_cycle]";
        stepped_(*this);
    }


private:
    enum class State {
        Unconfigured,
        Inactive,
        Active
    };

    State state_{ State::Unconfigured };

    void (NodecEngineModule::* step_func)() { &NodecEngineModule::step_first };

    EngineSignal initialized_;
    EngineSignal stepped_;

    nodec::Stopwatch<std::chrono::steady_clock> engine_timer_;
};


}  // namespace impl
}  // namespace game_engine

#endif