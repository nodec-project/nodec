#include <nodec/signals.hpp>
#include <nodec/logging.hpp>
#include <nodec/entities/storage.hpp>

#include <iostream>

using namespace nodec;

// A function callback
void on_update(float delta) {
    std::cout << "Hello in a static. " << delta << std::endl;
    //logging::InfoStream(__FILE__, __LINE__) << "Hello in a static. " << delta;
}

// A member function callback
class MyClass {
public:
    void on_update(float delta) {
        std::cout << "Hello in a member. " << delta << std::endl;
        //logging::InfoStream(__FILE__, __LINE__) << "Hello in a member. " << delta;
    }
};

int main() {
    logging::record_handlers().connect(logging::record_to_stdout_handler);
    //logging::record_handlers += event::StaticCallback<const logging::LogRecord&>::make_shared(&logging::record_to_stdout_handler);
    logging::info("Start", __FILE__, __LINE__);

    try {
        using UpdateSignal = signals::Signal<void(float)>;
        UpdateSignal update_;
        UpdateSignal update; update = std::move(update_);
        //auto update = signals::Signal<void(float)>();
        //signals::SignalInterface<void(float)>& update_interface_ = update;
        //UpdateSignal::Interface& update_interface_ = update;
        auto& update_interface = update.interface();

        //auto update_interface = std::move(update_interface_);

        update.connect(on_update);

        {

            //auto connection = update.connect([](float delta) {logging::InfoStream(__FILE__, __LINE__) << "Hello in lambda 0. " << delta; }).lock();
            UpdateSignal::Interface::Connection connection = update.connect(
                [](float delta) {
                    std::cout << "Hello in lambda 0. " << delta << std::endl;
                    //logging::InfoStream(__FILE__, __LINE__) << "Hello in lambda 0. " << delta;

                });
            update(3.14);
            connection.disconnect();
            //connection.unblock();
        }
        {
            auto connection = update.connect(
                [](float delta) {
                    std::cout << "Hello in lambda 1. " << delta << std::endl;
                    //logging::InfoStream(__FILE__, __LINE__) << "Hello in lambda 1. " << delta;
                });
        }

        MyClass my_obj;
        //update.connect(&my_obj, &MyClass::on_update);
        update.connect([&](float delta) {my_obj.on_update(delta); });

        update(3.14);


    }
    catch (std::exception& e) {
        std::cout << "FATAL: " << e.what() << std::endl;
        //logging::FatalStream(__FILE__, __LINE__) << e.what();
    }
    return 0;
}