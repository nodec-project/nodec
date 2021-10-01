#include <nodec/logging.hpp>
#include <nodec/resource_management/resource_registry.hpp>

#include <stdexcept>

class Resource {
public:

    Resource(const std::string& name)
        : name{ name } {

    }

    const std::string name;
};

class TestResource : public Resource {
public:
    using Resource::Resource;

    int field;

};

class TestResourceHandler {
public:
    using ResourceFuture = nodec::resource_management::ResourceRegistry::ResourceFuture<TestResource>;
    using ResourceBlockAccess = nodec::resource_management::ResourceRegistry::ResourceBlockAccess<TestResource>;


    ResourceFuture create_resource(const std::string& name, ResourceBlockAccess access) {
        nodec::logging::InfoStream(__FILE__, __LINE__) << name;
        return {};

    }

    ResourceFuture load_resource(const std::string& name, ResourceBlockAccess access) {
        nodec::logging::InfoStream(__FILE__, __LINE__) << name;

        std::promise<std::shared_ptr<TestResource>> promise;
        auto resource = std::make_shared<TestResource>(name);

        access.register_resource(resource);
        promise.set_value(resource);
        return promise.get_future();

    }

    std::future<bool> write_resource(std::shared_ptr<TestResource> resource) {
        nodec::logging::InfoStream(__FILE__, __LINE__) << resource->name;

        return {};
    }
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
            [&](const std::string& name, TestResourceHandler::ResourceBlockAccess access) {return handler.create_resource(name, access); },
            [&](const std::string& name, TestResourceHandler::ResourceBlockAccess access) {return handler.load_resource(name, access); },
            [&](std::shared_ptr<TestResource> resource) {return handler.write_resource(resource); }
        );

        auto hoge_resource = registry.get_resource<TestResource>("hoge").get();
        //auto huga_resource = registry.new_resource<TestResource>("huga").get();

        logging::InfoStream(__FILE__, __LINE__) << hoge_resource->name;
        hoge_resource->field = 100;

        {
            logging::InfoStream(__FILE__, __LINE__) << registry.get_resource<TestResource>("hoge").get()->field;
        }

    }
    catch (std::exception& e) {
        logging::FatalStream(__FILE__, __LINE__) << e.what();
    }

    return 0;
}