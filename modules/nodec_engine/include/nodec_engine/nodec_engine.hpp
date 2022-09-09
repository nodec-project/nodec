#ifndef NODEC_ENGINE__NODEC_ENGINE_HPP_
#define NODEC_ENGINE__NODEC_ENGINE_HPP_

#include <nodec/formatter.hpp>
#include <nodec/logging.hpp>
#include <nodec/macros.hpp>
#include <nodec/stopwatch.hpp>
#include <nodec/type_info.hpp>

#include <cassert>
#include <memory>
#include <stdexcept>
#include <vector>

namespace nodec_engine {

class NodecEngine {
public:
    NodecEngine() = default;

public:
    template<typename Module>
    decltype(auto) get_module() const {
        const auto index = nodec::type_seq_index<Module>::value();
        return *std::static_pointer_cast<Module>(modules.at(index));
    }

    template<typename Module>
    decltype(auto) add_module(std::shared_ptr<Module> module) {
        assert(static_cast<bool>(module) && "Null module cannot be assigned.");

        const auto index = nodec::type_seq_index<Module>::value();

        if (!(index < modules.size())) modules.resize(index + 1u);
        modules[index] = module;
        return *std::static_pointer_cast<Module>(module);
    }

    virtual float engine_time() const noexcept = 0;

private:
    std::vector<std::shared_ptr<void>> modules;

private:
    NODEC_DISABLE_COPY(NodecEngine);
};

void on_boot(NodecEngine &engine);

} // namespace nodec_engine

#endif