#ifndef NODEC_ENGINE__GAME_ENGINE_HPP_
#define NODEC_ENGINE__GAME_ENGINE_HPP_

#include <nodec/macros.hpp>
#include <nodec/stopwatch.hpp>
#include <nodec/type_info.hpp>
#include <nodec/logging.hpp>
#include <nodec/formatter.hpp>

#include <vector>
#include <memory>
#include <stdexcept>


namespace nodec_engine {

class NodecEngine {

    class BaseContainer {
    public:
        virtual ~BaseContainer() {};
    };

    template<typename T>
    class ModuleContainer : public BaseContainer {
    public:
        ~ModuleContainer() {}
        std::unique_ptr<T> module;
    };

    struct ModuleData {
        std::unique_ptr<BaseContainer> container;
    };

public:
    NodecEngine() = default;

    float engine_time() const noexcept {
        return std::chrono::duration<float>(engine_timer_.elapsed()).count();
    }

    template<typename Module>
    decltype(auto) get_module() const {
        const auto index = nodec::type_seq<Module>::value();
        if (index < modules.size() && modules[index].container) {
            return *static_cast<ModuleContainer<Module>*>(modules[index].container.get())->module.get();
        }

        throw std::runtime_error(
            nodec::Formatter() << "ModuleNotFoundError: No module named '" 
            << typeid(Module).name() << "'");
    }

protected:
    nodec::Stopwatch<std::chrono::steady_clock> engine_timer_;
    std::vector<ModuleData> modules;

private:
    NODEC_DISABLE_COPY(NodecEngine);
};

void on_boot(NodecEngine& engine);

NodecEngine& current();

} // namespace game_engine

#endif