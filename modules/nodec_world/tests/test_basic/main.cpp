#include <nodec_world/impl/world_module.hpp>
#include <nodec/logging.hpp>

#include <memory>


int main() {
    using namespace nodec;

    logging::record_handlers().connect(logging::record_to_stdout_handler);

    auto world_module = std::make_unique<nodec_world::impl::WorldModule>();

    nodec_world::World &world = *world_module;

    {
        world.initialized().connect([](nodec_world::World &world) {
            logging::InfoStream(__FILE__, __LINE__) << "[Client] >>> World initialized. ptr_world:" << &world;
        });

        world.stepped().connect([](nodec_world::World &world) {
            logging::InfoStream(__FILE__, __LINE__) << "[Client] >>> World stepped. ptr_world:" << &world;
        });
    }

    world_module->reset();
    world_module->step();

}