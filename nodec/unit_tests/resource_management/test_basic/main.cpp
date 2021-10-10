#include <nodec/logging.hpp>
#include <nodec/resource_management/resource_registry.hpp>
#include <nodec/concurrent/thread_pool_executor.hpp>
#include <nodec/formatter.hpp>

#include <thread>
#include <stdexcept>
#include <unordered_set>

void counter(int timeout, const std::string& label) {
    while (timeout > 0) {
        nodec::logging::InfoStream(__FILE__, __LINE__) << label << ">>> " << timeout << std::endl;
        //std::cout << std::this_thread::get_id() << ">>> " << timeout << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        --timeout;
    }
    nodec::logging::InfoStream(__FILE__, __LINE__) << label << ">>> Done!" << std::endl;
    //std::cout << std::this_thread::get_id() << ">>> Done!" << std::endl;

    return;
}

class Resource {
public:

    Resource(const std::string& name)
        : name{ name } {

    }

    virtual ~Resource() {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[~Resource()]";
    }
    const std::string name;

};

class TestResource : public Resource {
public:
    using Resource::Resource;

    int field;

    ~TestResource() {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[~TestResource()]";
    }

};

class ImplTestResource : public TestResource {
public:
    using TestResource::TestResource;

    int impl_filed;

    ~ImplTestResource() {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[~ImplTestResource()]";
    }
};

class Test {
public:
    void test() {

    }
};

class TestResourceHandler {
public:
    using ResourceLoadBridge = nodec::resource_management::ResourceRegistry::ResourceLoadBridge<TestResource>;
    using ResourceDeleteBridge = nodec::resource_management::ResourceRegistry::ResourceDeleteBridge<TestResource>;


    decltype(auto) create_resource(const std::string& name) {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[create_resource()] >>> " << name;

        return executor.submit([=]() {
            
                               });
    }

    decltype(auto) load_resource(const std::string& name, ResourceLoadBridge bridge) {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[load_resource()] >>> " << name;

        return executor.submit([=]() {
            counter(5, name);
            auto resource = std::make_shared<ImplTestResource>(name);
            resource->impl_filed = 999;
            bridge.on_resource_loaded(resource);
            return std::static_pointer_cast<TestResource>(resource);
                               });
    }

    decltype(auto) write_resource(std::shared_ptr<TestResource> resource) {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[write_resource()] >>> " << resource->name;

        return executor.submit([=]() {
            counter(5, nodec::Formatter() << "writing... " << resource->name);
                               });
    }

    decltype(auto) delete_resource(const std::string& name, ResourceDeleteBridge bridge) {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[delete_resource()] >>> " << name;
        auto cb = bridge;
        cb.delete_resource(name);
        Test test;
        test.test();
        // https://stackoverflow.com/questions/2835626/c0x-lambda-capture-by-value-always-const
        // https://stackoverflow.com/questions/5501959/why-does-c11s-lambda-require-mutable-keyword-for-capture-by-value-by-defau
        auto f = [=]() mutable {
            counter(5, nodec::Formatter() << "deleting... " << name);
            test;
            bridge.delete_resource(name);
        };

        return executor.submit([=]() {
            counter(5, nodec::Formatter() << "deleting... " << name);
            bridge.delete_resource(name);
                               });
    }

private:
    nodec::concurrent::ThreadPoolExecutor executor;
};

int main() {
    using namespace nodec;
    using namespace nodec::resource_management;

    logging::record_handlers().connect(logging::record_to_stdout_handler);

    try {
        logging::InfoStream(__FILE__, __LINE__) << "Start";

        ResourceRegistry registry;
        TestResourceHandler handler;

        registry.register_resource_handlers<TestResource>(
            [&](const std::string& name) {return handler.create_resource(name); },
            [&](const std::string& name, TestResourceHandler::ResourceLoadBridge bridge) {return handler.load_resource(name, bridge); },
            [&](std::shared_ptr<TestResource> resource) {return handler.write_resource(resource); },
            [&](const std::string& name, TestResourceHandler::ResourceDeleteBridge bridge) {return handler.delete_resource(name, bridge); }
        );


        std::shared_ptr<TestResource> hoge_resource;
        std::shared_ptr<TestResource> huga_resource;


        {
            hoge_resource = registry.get_resource<TestResource>("hoge").get();
            huga_resource = registry.get_resource<TestResource>("huga").get();
        }

        auto hoge_resource_future = registry.get_resource<TestResource>("hoge");
        auto huga_resource_future = registry.get_resource<TestResource>("huga");


        //
        //auto hoge_resource = registry.get_resource<TestResource>("hoge");
        
        //auto huga_resource = registry.new_resource<TestResource>("huga");
        {
            //auto hoge_resource = registry.get_resource<TestResource>("hoge");
            hoge_resource = hoge_resource_future.get();

            logging::InfoStream(__FILE__, __LINE__) << hoge_resource->name;
            hoge_resource->field = 100;
        }

        {
            auto hoge_resource = registry.get_resource<TestResource>("hoge").get();
            logging::InfoStream(__FILE__, __LINE__) << hoge_resource->field;
            {
                auto impl_hoge_resource = static_cast<ImplTestResource*>(hoge_resource.get());
                logging::InfoStream(__FILE__, __LINE__) << impl_hoge_resource->impl_filed;
            }
            {
                auto impl_hoge_resource = dynamic_cast<ImplTestResource*>(hoge_resource.get());
                logging::InfoStream(__FILE__, __LINE__) << impl_hoge_resource->impl_filed;
            }

            {
                auto impl_hoge_resource = std::dynamic_pointer_cast<ImplTestResource>(hoge_resource);
                logging::InfoStream(__FILE__, __LINE__) << impl_hoge_resource->impl_filed;
            }

            

            //registry.apply_changes<TestResource>("huga");
            //registry.new_resource<TestResource>("hoge");
        }
        {
            logging::InfoStream(__FILE__, __LINE__) << "get huga!";
            huga_resource = huga_resource_future.get();


            logging::InfoStream(__FILE__, __LINE__) << huga_resource->name;
        }
        logging::InfoStream(__FILE__, __LINE__) << "test";
        {
            std::unique_ptr<TestResource> resource(new ImplTestResource("test"));

        }
        logging::InfoStream(__FILE__, __LINE__) << "test";

        registry.apply_changes<TestResource>(hoge_resource->name);

    }
    catch (std::exception& e) {
        logging::FatalStream(__FILE__, __LINE__) << e.what();
    }

    return 0;
}