#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec/signals/scoped_block.hpp>
#include <nodec/signals/signal.hpp>

namespace global_func_test {

int global_count = 0;

void callback(int count) {
    global_count = count;
}

TEST_CASE("testing connect global function.") {
    nodec::signals::Signal<void(int count)> signal;

    signal.signal_interface().connect(callback);
    signal(33);
    CHECK(global_count == 33);
}

}; // namespace global_func_test

namespace lambda_func_test {

int global_count = 0;

TEST_CASE("testing connect lambda function.") {
    SUBCASE("small capture lambda test") {
        nodec::signals::Signal<void(int)> signal;
        int count = 0;

        signal.signal_interface().connect([&](int c) { count += c; });
        signal(33);
        CHECK(count == 33);
    }

    SUBCASE("non-captured lambda test") {
        nodec::signals::Signal<void(int)> signal;

        signal.signal_interface().connect([](int c) { global_count += c; });
        signal(33);
        CHECK(global_count == 33);
    }
}

} // namespace lambda_func_test

TEST_CASE("testing multiple connections.") {
    using namespace nodec::signals;

    Signal<void()> signal;

    int mask = 0x00;

    signal.signal_interface().connect([&]() { mask |= 0x01; });
    signal.signal_interface().connect([&]() { mask |= 0x02; });
    signal.signal_interface().connect([&]() { mask |= 0x04; });

    signal();
    CHECK(mask == 0x07);
}

TEST_CASE("testing connection management.") {
    using namespace nodec::signals;

    Signal<void()> signal;

    int count = 0;

    {
        Connection connection = signal.signal_interface().connect([&]() { ++count; });
        signal();

        CHECK(count == 1);
    }
}

TEST_CASE("testing blocking conenction.") {
    using namespace nodec::signals;

    Signal<void(int)> signal;

    {
        int x = 0;

        Connection conn = signal.signal_interface().connect([&](int count) {
            x += count;
            conn.block();
            signal(count);
            conn.unblock();
        });

        signal(33);
        CHECK(x == 33);

        signal(33);
        CHECK(x == 66);
    }

    {
        int x = 0;

        Connection conn = signal.signal_interface().connect([&](int count) {
            ScopedBlock<Connection> block(conn);
            x += count;
            signal(count);

            // Is it OK?
            conn.block();
        });

        signal(33);
        CHECK(x == 33);

        signal(33);
        CHECK(x == 66);
    }
}


TEST_CASE("testing the assignment of connection.") {
    using namespace nodec::signals;

    Signal<void()> signal;

    Connection conn;

    conn = signal.signal_interface().connect([&]() {});
}

// using namespace nodec;

// // A function callback
// void on_update(float delta) {
//     //std::cout << "Hello in a static. " << delta << std::endl;
//     logging::InfoStream(__FILE__, __LINE__) << "Hello in a static. " << delta;
// }

// int on_update_int(int delta) {
//     std::cout << "Hello in a static on_return. " << delta << std::endl;
//     return 1;
// }

// // A member function callback
// class MyClass {
// public:
//     void on_update(float delta) {
//         //std::cout << "Hello in a member. " << delta << std::endl;
//         logging::InfoStream(__FILE__, __LINE__) << "Hello in a member. " << delta;
//     }
// };

// template<typename>
// class TypeTest;

// template<typename ...Args>
// class TypeTest<void(Args...)> {

// };

// void func_const(const int v) {
//     logging::InfoStream(__FILE__, __LINE__) << v;
// }

// int main() {
//     logging::record_handlers().connect(logging::record_to_stdout_handler);

//     logging::info("Start", __FILE__, __LINE__);

//     try {

//         //{
//         //    TypeTest<void(const int&)> test;
//         //    signals::Signal<void(const int)> sig;

//         //    sig.connect(func_const);
//         //
//         //    const float cv = 1;
//         //    sig(cv);

//         //    //std::function<void(const int)> func;
//         //    //func(cv);
//         //    ////func(1.9f);
//         //    //func_const(1.0f);
//         //    return 0;
//         //}

//         {
//             using UpdateSignal = signals::Signal<void(float)>;
//             UpdateSignal update_;
//             UpdateSignal update; update = std::move(update_);
//             //auto update = signals::Signal<void(float)>();
//             //signals::SignalInterface<void(float)>& update_interface_ = update;
//             //UpdateSignal::Interface& update_interface_ = update;
//             auto update_interface = update.connection_point();

//             //auto update_interface = std::move(update_interface_);

//             //auto connection = update.connect(on_update).assign();
//             //{
//             //    auto connection_ = update.connect(on_update);
//             //    auto connection = connection_.assign();
//             //}

//             //auto connection = static_cast<UpdateSignal::Interface::Connection>(update.connect(on_update));
//             //UpdateSignal::Interface::Connection connection = update.connect(on_update);
//             update_interface.connect(on_update);

//             //auto connection_ = update.connect(on_update);
//             //UpdateSignal::Interface::Connection connection(nullptr);
//             //connection.block();
//             //connection = std::move(connection_);
//             //UpdateSignal::Interface::Connection connection = std::move(connection_);
//             {
//                 MyClass my_obj;
//                 int cnt = 0;
//                 UpdateSignal::Connection connection = update.connection_point().connect(
//                     [&](float delta) {
//                         my_obj.on_update(delta);
//                         connection.block();
//                         update((float)++cnt);
//                         connection.unblock();
//                     });

//                 update(3.14);
//             }
//             //connection = update.connect(on_update);
//             update(3.14);
//             {

//                 //auto connection = update.connect([](float delta) {logging::InfoStream(__FILE__, __LINE__) << "Hello in lambda 0. " << delta; }).lock();
//                 //UpdateSignal::Interface::Connection connection = update.connect(
//                 //    [](float delta) {
//                 //        std::cout << "Hello in lambda 0. " << delta << std::endl;
//                 //        //logging::InfoStream(__FILE__, __LINE__) << "Hello in lambda 0. " << delta;

//                 //    });
//                 update(3.14);
//                 //connection.disconnect();
//                 //connection.unblock();
//             }
//             {
//                 auto connection = update.connection_point().connect(
//                     [](float delta) {
//                         //std::cout << "Hello in lambda 1. " << delta << std::endl;
//                         logging::InfoStream(__FILE__, __LINE__) << "Hello in lambda 1. " << delta;

//                     });
//             }

//             MyClass my_obj;
//             //update.connect(&my_obj, &MyClass::on_update);
//             auto connection = update.connection_point().connect([&](float delta) {my_obj.on_update(delta); }).assign();
//             connection.disconnect();
//             //connection.block();
//             update(3.14);
//         }

//     }
//     catch (std::exception& e) {
//         //std::cout << "FATAL: " << e.what() << std::endl;
//         logging::FatalStream(__FILE__, __LINE__) << e.what();
//     }
//     return 0;
// }