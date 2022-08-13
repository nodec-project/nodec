#ifndef NODEC_ENGINE__GAME_ENGINE_HPP_
#define NODEC_ENGINE__GAME_ENGINE_HPP_

#include <nodec/formatter.hpp>
#include <nodec/logging.hpp>
#include <nodec/macros.hpp>
#include <nodec/stopwatch.hpp>
#include <nodec/type_info.hpp>

#include <memory>
#include <stdexcept>
#include <vector>

namespace nodec_engine {

class NodecEngine {
    class BaseModuleContainer {
    public:
        virtual ~BaseModuleContainer(){};
    };

    template<typename T>
    class ModuleContainer : public BaseModuleContainer {
    public:
        ~ModuleContainer() {}
        std::shared_ptr<T> data;
    };

    struct ModuleContainerData {
        std::unique_ptr<BaseModuleContainer> container;
    };

public:
    NodecEngine() = default;

public:
    template<typename Module>
    decltype(auto) get_module() const {
        const auto index = nodec::type_seq<Module>::value();
        if (index < modules.size() && modules[index].container) {
            return *static_cast<ModuleContainer<Module> *>(modules[index].container.get())->data;
        }

        throw std::runtime_error(
            nodec::Formatter() << "ModuleNotFoundError: No module named '"
                               << typeid(Module).name() << "'");
    }

    template<typename Module>
    decltype(auto) add_module(std::shared_ptr<Module> module) {
        const auto index = nodec::type_seq<Module>::value();

        if (!(index < modules.size())) {
            modules.resize(index + 1u);
        }

        auto &&cdata = modules[index];
        if (!cdata.container) {
            auto container = new ModuleContainer<Module>();
            container->data = module;

            cdata.container.reset(container);
        }

        return *static_cast<ModuleContainer<Module> *>(cdata.container.get())->data;
    }

    virtual float engine_time() const noexcept = 0;

public:
    using EngineSignal = nodec::signals::Signal<void(NodecEngine &)>;

    virtual EngineSignal::SignalInterface initialized() = 0;

    virtual EngineSignal::SignalInterface stepped() = 0;

private:
    std::vector<ModuleContainerData> modules;

private:
    NODEC_DISABLE_COPY(NodecEngine);
};

void on_boot(NodecEngine &engine);

} // namespace nodec_engine

#endif