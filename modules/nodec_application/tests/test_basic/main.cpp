#include <nodec_engine/impl/nodec_engine_module.hpp>

#include <nodec/logging.hpp>
#include <nodec/memory.hpp>

#include <cassert>


using namespace nodec;

void nodec_engine::on_boot(nodec_engine::NodecEngine &engine) {


}

class TestModule {
public:
    TestModule(int field) :
        field{ field } {

    }

    ~TestModule() {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "destroyed";
    }

    int field;

    void method() {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "called";
    }
};

class DerivedModule : public TestModule {
public:
    using TestModule::TestModule;

    ~DerivedModule() {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "destroyed";
    }

    void public_method() {

        nodec::logging::InfoStream(__FILE__, __LINE__) << "called";

    }
};

struct TestB {
    std::string field;

};


int main() {
    logging::record_handlers().connect(logging::record_to_stdout_handler);

    logging::DebugStream(__FILE__, __LINE__) << "Start";

    try {


        nodec_engine::impl::NodecEngineModule engine;
        nodec_engine::impl::set_current(&engine);

        //std::unique_ptr<void> ptr;


        //nodec::logging::DebugStream(__FILE__, __LINE__) << "{" << 0 << "} located at [" << 0 << "] is being deleted.\n";
        //auto ptr = make_unique_void(new DerivedModule(0));

        engine.started().connect([](auto& engine) {logging::debug("started", __FILE__, __LINE__); });
        engine.initialized().connect([](auto& engine) {logging::debug("initialized", __FILE__, __LINE__); });
        engine.stepped().connect([](auto& engine) {logging::debug("stepped", __FILE__, __LINE__); });

        std::unique_ptr<DerivedModule> derived(new DerivedModule(100));
        engine.register_module<TestModule>(derived.get());
        logging::DebugStream(__FILE__, __LINE__) << derived->field;


        std::unique_ptr<TestB> test_b(new TestB{"hoge"});
        engine.register_module<TestB>(test_b.get());
        engine.reset();


        for (auto i = 0; i < 5; ++i) {
            engine.step();
        }
        {
            auto& module = engine.get_module<TestModule>();
            logging::InfoStream(__FILE__, __LINE__) << module.field;

        }
        {

            auto& module = engine.get_module<TestB>();
            logging::InfoStream(__FILE__, __LINE__) << module.field;
        }
    }
    catch (std::exception& e) {
        nodec::logging::FatalStream(__FILE__, __LINE__) << e.what();
    }


    return 0;
}