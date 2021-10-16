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


void set_current(NodecEngineModule* engine) {
    current = engine;
}

NodecEngineModule::~NodecEngineModule() {
    nodec::logging::InfoStream(__FILE__, __LINE__)
        << "[NodecEngineModule] >>> The engine is destroyed.\n"
        << "engine_time: " << engine_time() << "[s]";

    if (current == this) {
        current = nullptr;
    }
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