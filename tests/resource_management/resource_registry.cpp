#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec/resource_management/resource_registry.hpp>

#include <future>
#include <thread>

class TestResource {
public:
    TestResource(const std::string &field)
        : field(field) {}

    std::string field;
};

TEST_CASE("testing the load functions.") {
    using namespace nodec;
    using namespace nodec::resource_management;

    ResourceRegistry registry;

    registry.register_resource_loader<TestResource>(
        [](const std::string &name) {
            return std::make_shared<TestResource>(name);
        },
        [](const std::string &name, ResourceRegistry::LoadNotifyer<TestResource> notifyer) {
            auto future = std::async(std::launch::async, [=]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                auto resource = std::make_shared<TestResource>(name);
                notifyer.on_loaded(name, resource);
                return resource;
            });
            return future;
        });

    {
        auto resource = registry.get_resource_direct<TestResource>("resource_direct");
        CHECK(resource);
        CHECK(resource->field == "resource_direct");
    }
    {
        auto resource = registry.get_resource_async<TestResource>("resource_async").get();
        CHECK(resource);
        CHECK(resource->field == "resource_async");
    }
}

// void counter(int timeout, const std::string &label) {
//     while (timeout > 0) {
//         nodec::logging::InfoStream(__FILE__, __LINE__) << label << ">>> " << timeout << std::endl;
//         // std::cout << std::this_thread::get_id() << ">>> " << timeout << std::endl;

//         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//         --timeout;
//     }
//     nodec::logging::InfoStream(__FILE__, __LINE__) << label << ">>> Done!" << std::endl;
//     // std::cout << std::this_thread::get_id() << ">>> Done!" << std::endl;

//     return;
// }

// class ImplTestResource : public TestResource {
// public:
//     using TestResource::TestResource;

//     int impl_filed;

//     ~ImplTestResource() {
//         nodec::logging::InfoStream(__FILE__, __LINE__) << "[~ImplTestResource()]";
//     }
// };

// class Test {
// public:
//     void test() {
//     }
// };

// class TestResourceHandler {
// public:
//     using LoadBridge = nodec::resource_management::ResourceRegistry::LoadBridge<TestResource>;

//     // decltype(auto) create_resource(const std::string& name) {
//     //     nodec::logging::InfoStream(__FILE__, __LINE__) << "[create_resource()] >>> " << name;

//     //    return executor.submit([=]() {
//     //
//     //                           });
//     //}

//     decltype(auto) load_resource(const std::string &name, LoadBridge bridge) {
//         nodec::logging::InfoStream(__FILE__, __LINE__) << "[load_resource()] >>> " << name;

//         return executor.submit([=]() {
//             counter(5, name);
//             auto resource = std::make_shared<ImplTestResource>();
//             resource->impl_filed = 999;
//             bridge.on_resource_loaded(name, resource);
//             return std::static_pointer_cast<TestResource>(resource);
//         });
//     }

//     // decltype(auto) write_resource(std::shared_ptr<TestResource> resource) {
//     //     nodec::logging::InfoStream(__FILE__, __LINE__) << "[write_resource()] >>> " << resource->name;

//     //    return executor.submit([=]() {
//     //        counter(5, nodec::Formatter() << "writing... " << resource->name);
//     //                           });
//     //}

//     // decltype(auto) delete_resource(const std::string& name, ResourceDeleteBridge bridge) {
//     //     nodec::logging::InfoStream(__FILE__, __LINE__) << "[delete_resource()] >>> " << name;
//     //     auto cb = bridge;
//     //     cb.delete_resource(name);
//     //     Test test;
//     //     test.test();
//     //     // https://stackoverflow.com/questions/2835626/c0x-lambda-capture-by-value-always-const
//     //     // https://stackoverflow.com/questions/5501959/why-does-c11s-lambda-require-mutable-keyword-for-capture-by-value-by-defau
//     //     auto f = [=]() mutable {
//     //         counter(5, nodec::Formatter() << "deleting... " << name);
//     //         test;
//     //         bridge.delete_resource(name);
//     //     };

//     //    return executor.submit([=]() {
//     //        counter(5, nodec::Formatter() << "deleting... " << name);
//     //        bridge.delete_resource(name);
//     //                           });
//     //}

// private:
//     nodec::concurrent::ThreadPoolExecutor executor;
// };

// int main() {
//     logging::InfoStream(__FILE__, __LINE__) << "Start";

//     ResourceRegistry registry;
//     TestResourceHandler handler;

//     registry.register_resource_loader<TestResource>(
//         [&](const std::string &name, TestResourceHandler::LoadBridge bridge) { return handler.load_resource(name, bridge); });

//     std::shared_ptr<TestResource> hoge_resource;
//     std::shared_ptr<TestResource> huga_resource;

//     {
//         hoge_resource = registry.get_resource<TestResource>("hoge").get();
//         huga_resource = registry.get_resource<TestResource>("huga").get();
//     }

//     {
//         auto result = registry.lookup_name<TestResource>(hoge_resource);
//         logging::InfoStream(__FILE__, __LINE__) << result.first << ", " << result.second;
//     }

//     //{
//     //    hoge_resource.reset();
//     //    auto result = registry.lookup_name<TestResource>(hoge_resource);
//     //    logging::InfoStream(__FILE__, __LINE__) << result.first << ", " << result.second;
//     //}

//     {
//         auto dummy = std::make_shared<TestResource>();
//         auto result = registry.lookup_name<TestResource>(dummy);
//         logging::InfoStream(__FILE__, __LINE__) << result.first << ", " << result.second;
//     }

//     // auto hoge_resource_future = registry.get_resource<TestResource>("hoge");
//     // auto huga_resource_future = registry.get_resource<TestResource>("huga");

//     ////
//     ////auto hoge_resource = registry.get_resource<TestResource>("hoge");
//     //
//     ////auto huga_resource = registry.new_resource<TestResource>("huga");
//     //{
//     //    //auto hoge_resource = registry.get_resource<TestResource>("hoge");
//     //    hoge_resource = hoge_resource_future.get();

//     //    logging::InfoStream(__FILE__, __LINE__) << hoge_resource->name;
//     //    hoge_resource->field = 100;
//     //}

//     //{
//     //    auto hoge_resource = registry.get_resource<TestResource>("hoge").get();
//     //    logging::InfoStream(__FILE__, __LINE__) << hoge_resource->field;
//     //    {
//     //        auto impl_hoge_resource = static_cast<ImplTestResource*>(hoge_resource.get());
//     //        logging::InfoStream(__FILE__, __LINE__) << impl_hoge_resource->impl_filed;
//     //    }
//     //    {
//     //        auto impl_hoge_resource = dynamic_cast<ImplTestResource*>(hoge_resource.get());
//     //        logging::InfoStream(__FILE__, __LINE__) << impl_hoge_resource->impl_filed;
//     //    }

//     //    {
//     //        auto impl_hoge_resource = std::dynamic_pointer_cast<ImplTestResource>(hoge_resource);
//     //        logging::InfoStream(__FILE__, __LINE__) << impl_hoge_resource->impl_filed;
//     //    }

//     //

//     //    //registry.apply_changes<TestResource>("huga");
//     //    //registry.new_resource<TestResource>("hoge");
//     //}
//     //{
//     //    logging::InfoStream(__FILE__, __LINE__) << "get huga!";
//     //    huga_resource = huga_resource_future.get();

//     //    logging::InfoStream(__FILE__, __LINE__) << huga_resource->name;
//     //}
//     // logging::InfoStream(__FILE__, __LINE__) << "test";
//     //{
//     //    std::unique_ptr<TestResource> resource(new ImplTestResource("test"));

//     //}
//     // logging::InfoStream(__FILE__, __LINE__) << "test";

//     // registry.apply_changes<TestResource>(hoge_resource->name);

//     return 0;
// }