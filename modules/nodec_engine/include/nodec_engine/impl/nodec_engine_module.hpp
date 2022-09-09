#ifndef NODEC_ENGINE__IMPL__NODEC_ENGINE_MODULE_HPP_
#define NODEC_ENGINE__IMPL__NODEC_ENGINE_MODULE_HPP_

#include <nodec/logging.hpp>
#include <nodec/signals.hpp>
#include <nodec_engine/nodec_engine.hpp>

#include <cassert>
#include <memory>
#include <string>
#include <vector>

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

        state_ = State::Active;
    }

public:
    float engine_time() const noexcept override {
        return std::chrono::duration<float>(engine_timer_.elapsed()).count();
    }

private:
    void boot() {
        nodec::logging::InfoStream(__FILE__, __LINE__)
            << "[NodecEngineModule] >>> Booting...";
        {
            on_boot(*this);
        }
        nodec::logging::InfoStream(__FILE__, __LINE__)
            << "[NodecEngineModule] >>> Booting finished.";
    }

private:
    enum class State {
        Unconfigured,
        Active
    };

    State state_{State::Unconfigured};

    nodec::Stopwatch<std::chrono::steady_clock> engine_timer_;
};

} // namespace impl
} // namespace nodec_engine

#endif