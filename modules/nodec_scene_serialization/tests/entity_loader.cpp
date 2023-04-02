#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec_scene_serialization/impl/entity_loader_impl.hpp>

#include <future>
#include <thread>

TEST_CASE("testing async load.") {
    using namespace nodec::resource_management;
    using namespace nodec_scene;
    using namespace nodec_scene_serialization;

    Scene scene;
    ResourceRegistry resource_registry;
    SceneSerialization serialization;

    resource_registry.register_resource_loader<SerializableEntity>([](const std::string &name) {
        if (name != "test") return std::shared_ptr<SerializableEntity>();
        return std::make_shared<SerializableEntity>(); }, [](const std::string &name, ResourceRegistry::LoadNotifyer<SerializableEntity> notifyer) {
            auto future = std::async(std::launch::async, [=]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));

                std::shared_ptr<SerializableEntity> resource;
                if (name == "test") resource = std::make_shared<SerializableEntity>();
                notifyer.on_loaded(name, resource);
                return resource;
            });
            return future; });

    impl::EntityLoaderImpl loader(serialization, scene, resource_registry);

    auto target = scene.create_entity();

    auto async_operation = loader.load_async("test", target);

    while (!async_operation.is_done()) {
        loader.update();
    }

    // CHECK(scene.registry().is_valid(async_operation.result()));

    // std::promise<int> promise;
    // std::unique_ptr<std::thread> thread;
    // {
    //     auto future = promise.get_future();

    //     thread = std::make_unique<std::thread>([](std::promise<int> promise) {
    //         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    //         MESSAGE("Succeeded.");
    //         promise.set_value(100);
    //     },
    //                                            std::move(promise));
    // }
    // MESSAGE("Ended");
    // thread->join();
}