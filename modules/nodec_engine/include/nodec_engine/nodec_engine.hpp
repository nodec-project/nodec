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

    class BaseModuleReference {
    public:
        virtual ~BaseModuleReference() {};
    };

    template<typename T>
    class ModuleReference : public BaseModuleReference {
    public:
        ~ModuleReference() {}
        T* ptr{ nullptr };
    };

    struct ModuleReferenceData {
        std::unique_ptr<BaseModuleReference> reference;
    };

public:
    NodecEngine() = default;

    float engine_time() const noexcept {
        return std::chrono::duration<float>(engine_timer_.elapsed()).count();
    }

    template<typename Module>
    decltype(auto) get_module() const {
        const auto index = nodec::type_seq<Module>::value();
        if (index < moduleReferences.size() && moduleReferences[index].reference) {
            return *static_cast<ModuleReference<Module>*>(moduleReferences[index].reference.get())->ptr;
        }

        throw std::runtime_error(
            nodec::Formatter() << "ModuleNotFoundError: No module named '"
            << typeid(Module).name() << "'");
    }

protected:
    nodec::Stopwatch<std::chrono::steady_clock> engine_timer_;
    std::vector<ModuleReferenceData> moduleReferences;

private:
    NODEC_DISABLE_COPY(NodecEngine);
};

void on_boot(NodecEngine& engine);

NodecEngine& current();

} // namespace game_engine

#endif