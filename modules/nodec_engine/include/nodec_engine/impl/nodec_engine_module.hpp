#ifndef NODEC_ENGINE__IMPL__NODEC_ENGINE_MODULE_HPP_
#define NODEC_ENGINE__IMPL__NODEC_ENGINE_MODULE_HPP_

#include <nodec_engine/nodec_engine.hpp>
#include <nodec/signals.hpp>
#include <nodec/logging.hpp>

#include <vector>
#include <string>
#include <memory>


namespace nodec_engine {
namespace impl {

class NodecEngineModule : public NodecEngine {
public:
    NodecEngineModule();
    ~NodecEngineModule();

public:
    void reset();
    void step();

public:
    template<typename Module, typename... Args>
    decltype(auto) add_module(Args &&... args) {
        const auto index = nodec::type_seq<Module>::value();

        if (!(index < modules.size())) {
            modules.resize(index + 1u);
        }

        auto&& mData = modules[index];
        if (!mData.container) {
            auto container = new ModuleContainer<Module>();
            container->module.reset(new Module(args...));
            
            mData.container.reset(container);
        }

        return *static_cast<ModuleContainer<Module>*>(mData.container.get())->module.get();
    }

public:
    using EngineSignal = nodec::signals::Signal<void(NodecEngineModule&)>;

    decltype(auto) initialized() {
        return initialized_.connection_point();
    }

    decltype(auto) started() {
        return started_.connection_point();
    }

    decltype(auto) stepped() {
        return stepped_.connection_point();
    }

private:
    enum class State {
        Unconfigured,
        Inactive,
        Active
    };

    State state_{ State::Unconfigured };

    void step_first();
    void step_cycle();

    void (NodecEngineModule::* step_func)() {&NodecEngineModule::step_first};

private:
    EngineSignal initialized_;
    EngineSignal started_;
    EngineSignal stepped_;
};

void set_current(NodecEngineModule* engine);

}  // namespace impl
}  // namespace game_engine

#endif