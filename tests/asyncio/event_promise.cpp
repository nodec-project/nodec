#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec/asyncio/event_loop.hpp>
#include <nodec/asyncio/event_promise.hpp>

#include <thread>

TEST_CASE("EventPromise basic usage") {
    using namespace nodec::asyncio;
    EventLoop event_loop;

    std::vector<std::string> log;
    SUBCASE("Simple Promise chain") {
        EventPromiseFactory(event_loop)
            .resolve(10)
            .then([&log](int value) {
                log.push_back("then1: " + std::to_string(value));
                return value * 2;
            })
            .then([&log](int value) {
                log.push_back("then2: " + std::to_string(value));
                return std::to_string(value) + " converted to string";
            })
            .then([&log](const std::string &value) {
                log.push_back("then3: " + value);
            });

        // イベントループを実行してPromiseを解決
        event_loop.spin();

        // 検証
        CHECK(log.size() == 3);
        CHECK(log[0] == "then1: 10");
        CHECK(log[1] == "then2: 20");
        CHECK(log[2] == "then3: 20 converted to string");
    }

    SUBCASE("Error handling with catch") {
        log.clear();

        auto promise = EventPromiseFactory(event_loop)
                           .create<int>([](auto resolve, auto reject) {
                               reject(std::make_exception_ptr(std::runtime_error("Test error")));
                           })
                           .then([&log](int value) {
                               log.push_back("then: " + std::to_string(value)); // 実行されないはず
                               return value * 2;
                           })
                           .catch_error([&log](std::exception_ptr err) {
                               try {
                                   std::rethrow_exception(err);
                               } catch (const std::runtime_error &e) {
                                   log.push_back("catch: " + std::string(e.what()));
                                   return 42; // 回復値
                               }
                           })
                           .then([&log](int value) {
                               log.push_back("recovery: " + std::to_string(value));
                           });

        event_loop.spin();

        CHECK(log.size() == 2);
        CHECK(log[0] == "catch: Test error");
        CHECK(log[1] == "recovery: 42");
    }
}

TEST_CASE("Thread safety") {
    using namespace nodec::asyncio;
    EventLoop event_loop;

    std::thread thread;

    EventPromiseFactory(event_loop)
        .create<int>([&](auto resolve, auto reject) {
            //auto resolve_callback = std::make_shared<std::function<void(int)>>(resolve);

            thread = std::thread([&event_loop, resolve]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                event_loop.schedule([resolve]() {
                    resolve(10);
                });
            });
        })
        .then([&](int value) {
            CHECK(value == 10);
        });

    thread.join();
    event_loop.spin();
}

TEST_CASE("EventPromise with delay") {
    using namespace nodec::asyncio;
    using namespace std::chrono;
    EventLoop event_loop;

    std::vector<std::string> log;

    auto start_time = high_resolution_clock::now();

    EventPromiseFactory(event_loop)
        .delay(milliseconds(50), [&log, start_time]() {
            auto elapsed = duration_cast<milliseconds>(high_resolution_clock::now() - start_time).count();
            CHECK(elapsed >= 50);
            log.push_back("Delayed execution after " + std::to_string(elapsed) + " ms");
            return 100;
        })
        .then([&log](int value) {
            log.push_back("Delayed value: " + std::to_string(value));
        });

    event_loop.spin();

    CHECK(log.size() == 2);
    CHECK(log[0].find("Delayed execution after") != std::string::npos);
    CHECK(log[1] == "Delayed value: 100");
}