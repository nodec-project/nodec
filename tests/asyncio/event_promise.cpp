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

    SUBCASE("Void Promise") {
        EventPromiseFactory(event_loop)
            .resolve()
            .then([&log]() {
                log.push_back("then1: void resolved");
            })
            .then([&log]() {
                log.push_back("then2: still void");
            });

        event_loop.spin();

        CHECK(log.size() == 2);
        CHECK(log[0] == "then1: void resolved");
        CHECK(log[1] == "then2: still void");
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
            // auto resolve_callback = std::make_shared<std::function<void(int)>>(resolve);

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
        .delay(milliseconds(50))
        .then([&log, start_time]() {
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

TEST_CASE("Promise flattening") {
    using namespace nodec::asyncio;
    EventLoop event_loop;
    std::vector<std::string> log;

    SUBCASE("Flattening with non-void return type") {
        // テスト1: Promiseを返すthenコールバック
        EventPromiseFactory(event_loop)
            .resolve(10)
            .then([&event_loop, &log](int value) {
                log.push_back("First then: " + std::to_string(value));
                // 別のPromiseを返す
                return EventPromiseFactory(event_loop).resolve(value * 2);
            })
            .then([&log](int value) {
                // 内部Promiseの結果 (20) が自動的に解決されるはず
                log.push_back("Second then with flattened value: " + std::to_string(value));
            });

        event_loop.spin();

        CHECK(log.size() == 2);
        CHECK(log[0] == "First then: 10");
        CHECK(log[1] == "Second then with flattened value: 20");
    }

    SUBCASE("Flattening with void return type") {
        log.clear();

        EventPromiseFactory(event_loop)
            .resolve()
            .then([&event_loop, &log]() {
                log.push_back("First then (void)");
                // void戻り値のPromiseを返す
                return EventPromiseFactory(event_loop).resolve();
            })
            .then([&log]() {
                log.push_back("Second then after void Promise");
            });

        event_loop.spin();

        CHECK(log.size() == 2);
        CHECK(log[0] == "First then (void)");
        CHECK(log[1] == "Second then after void Promise");
    }

    SUBCASE("Nested Promise flattening") {
        log.clear();

        EventPromiseFactory(event_loop)
            .resolve(5)
            .then([&event_loop, &log](int value) {
                log.push_back("Outer promise: " + std::to_string(value));
                // ネストしたPromiseを返す
                return EventPromiseFactory(event_loop)
                    .resolve(value + 1)
                    .then([&event_loop, &log](int inner_value) {
                        log.push_back("Inner promise: " + std::to_string(inner_value));
                        // さらに別のPromiseを返す
                        return EventPromiseFactory(event_loop).resolve(inner_value * 2);
                    });
            })
            .then([&log](int final_value) {
                // 最終的なPromiseの結果 (12) が得られるはず
                log.push_back("Final value after multiple flattening: " + std::to_string(final_value));
            });

        event_loop.spin();

        CHECK(log.size() == 3);
        CHECK(log[0] == "Outer promise: 5");
        CHECK(log[1] == "Inner promise: 6");
        CHECK(log[2] == "Final value after multiple flattening: 12");
    }

    SUBCASE("Flattening with rejected Promise") {
        log.clear();

        EventPromiseFactory(event_loop)
            .resolve(10)
            .then([&event_loop](int value) {
                // エラーを含むPromiseを返す
                return EventPromiseFactory(event_loop)
                    .create<int>([value](auto resolve, auto reject) {
                        reject(std::make_exception_ptr(
                            std::runtime_error("Error from inner Promise: " + std::to_string(value))));
                    });
            })
            .then([&log](int value) {
                // このコールバックは実行されないはず
                log.push_back("This should not be called: " + std::to_string(value));
                return value;
            })
            .catch_error([&log](std::exception_ptr err) {
                try {
                    std::rethrow_exception(err);
                } catch (const std::runtime_error &e) {
                    // 内部Promiseからのエラーが伝播するはず
                    log.push_back("Caught error: " + std::string(e.what()));
                    return 42; // リカバリー値
                }
            })
            .then([&log](int value) {
                log.push_back("After error recovery: " + std::to_string(value));
            });

        event_loop.spin();

        CHECK(log.size() == 2);
        CHECK(log[0] == "Caught error: Error from inner Promise: 10");
        CHECK(log[1] == "After error recovery: 42");
    }
}

TEST_CASE("Parallel execution") {
    using namespace nodec::asyncio;
    using namespace std::chrono;
    EventLoop event_loop;

    auto start_time = high_resolution_clock::now();

    EventPromiseFactory(event_loop)
        .resolve()
        .then([&]() {
            return EventPromiseFactory(event_loop).delay(std::chrono::milliseconds(1000));
        })
        .then([]() {
            std::cout << "1 second delay" << std::endl;
        });
    EventPromiseFactory(event_loop)
        .resolve()
        .then([&]() {
            return EventPromiseFactory(event_loop).delay(std::chrono::milliseconds(2000));
        })
        .then([]() {
            std::cout << "2 second delay" << std::endl;
        });
    ;
    EventPromiseFactory(event_loop)
        .resolve()
        .then([&]() {
            return EventPromiseFactory(event_loop).delay(std::chrono::milliseconds(3000));
        })
        .then([]() {
            std::cout << "3 second delay" << std::endl;
        });
    ;

    event_loop.spin();

    auto end_time = high_resolution_clock::now();
    auto elapsed = duration_cast<milliseconds>(end_time - start_time).count();
    MESSAGE("Elapsed time: ", std::to_string(elapsed) + " ms");
    CHECK(elapsed >= 3000); // 3秒以上経過しているはず
    CHECK(elapsed < 4000);  // 4秒未満であるべき
}

// // リトライ関数の実装
// template<typename Func>
// auto retry_with_delay(
//     nodec::asyncio::EventLoop &event_loop,
//     Func func,
//     int max_retries,
//     std::chrono::milliseconds delay) {
//     using namespace nodec::asyncio;
//     using Result = decltype(func());

//     // 再帰関数をラムダ内に定義
//     std::function<EventPromise<Result>(int)> attempt =
//         [&event_loop, func, delay, attempt](int retries_left) -> EventPromise<Result> {
//         // 条件確認
//         Result result = func();

//         // 条件を満たしたか、リトライ回数が0になったらそのまま返す
//         if (result || retries_left <= 0) {
//             return EventPromiseFactory(event_loop).resolve(result);
//         }

//         // リトライ
//         return EventPromiseFactory(event_loop)
//             .delay(delay)
//             .then([&]() {
//                 // 遅延後に再度試行
//                 return attempt(retries_left - 1);
//             });
//     };

//     // 初回実行
//     return attempt(max_retries);
// }

template<typename Func>
auto retry(nodec::asyncio::EventLoop &event_loop, Func func, int retry_count) {
    // 戻り値の型を推論
    using ReturnType = decltype(func());

    // 最初の呼び出し
    auto promise = func();

    // エラー発生時に再試行するチェーンを構築
    for (int i = 1; i <= retry_count; ++i) {
        promise = promise.catch_error([func](std::exception_ptr) {
            // エラーが発生したら再度関数を実行
            return func();
        });
    }

    return promise;
}

TEST_CASE("test retry") {
    using namespace nodec::asyncio;
    using namespace std::chrono;
    EventLoop event_loop;

    int retry_count = 0;
    auto func = [&]() {
        bool condition = false; // ここで条件をチェックするロジックを実装
        std::cout << "Retry count: " << retry_count << std::endl;
        ++retry_count;
        return condition;
    };

    retry(
        event_loop,
        [&]() { return EventPromiseFactory(event_loop)
                    .delay(milliseconds(500))
                    .then([&]() {
                        std::cout << "Retry count: " << retry_count << std::endl;
                        ++retry_count;
                        if (retry_count < 30) {
                            throw std::runtime_error("Retry error");
                        }
                        return true;
                    }); }, 5)
        .then([&](bool result) {
            std::cout << "Final result: " << result << std::endl;
        })
        .catch_error([&](std::exception_ptr err) {
            try {
                std::rethrow_exception(err);
            } catch (const std::runtime_error &e) {
                std::cout << "Caught error: " << e.what() << std::endl;
            }
        });

        // retry_with_delay(event_loop, func, 5, milliseconds(100));

        // EventPromiseFactory(event_loop)
        //     .resolve(func())
        //     .then([&](bool result) {
        //         if (!result) {
        //             return EventPromiseFactory(event_loop).delay(std::chrono::milliseconds(1000)).then([&]() {
        //                 return func();
        //             });
        //         }
        //         return EventPromiseFactory(event_loop).resolve(result);
        //     })
        //     .then([&](bool result) {
        //         if (!result) {
        //             return EventPromiseFactory(event_loop).delay(std::chrono::milliseconds(1000)).then([&]() {
        //                 return func();
        //             });
        //         }
        //         return EventPromiseFactory(event_loop).resolve(result);
        //     })
        //     .then([](bool result) {
        //         if (!result) {
        //             throw std::runtime_error("Failed after retries");
        //         }
        //         std::cout << "Condition met after retries" << std::endl;
        //     })
        //     .catch_error([](std::exception_ptr err) {
        //         try {
        //             std::rethrow_exception(err);
        //         } catch (const std::runtime_error &e) {
        //             std::cout << "Caught error: " << e.what() << std::endl;
        //         }
        //     });

        event_loop.spin();
}
