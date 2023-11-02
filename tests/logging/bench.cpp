#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <chrono>
#include <thread>

#include <nodec/logging/formatters/simple_formatter.hpp>
#include <nodec/logging/handlers/stdout_handler.hpp>
#include <nodec/logging/logging.hpp>

TEST_CASE("Single thread, 100,000 iterations") {
    int iters = 100000;

    using namespace nodec::logging;
    auto logger = get_logger();

    // auto handler = std::make_shared<handlers::StdoutHandler<formatters::SimpleFormatter>>();
    // auto conn = logger->add_handler([&](const LogRecord &record) { (*handler)(record); });

    using std::chrono::duration;
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;

    auto start = high_resolution_clock::now();
    for (auto i = 0; i < iters; ++i) {
        logger->info(__FILE__, __LINE__) << "Hello logger: msg number " << i;
    }

    auto delta = high_resolution_clock::now() - start;
    auto delta_d = duration_cast<duration<double>>(delta).count();

    MESSAGE("Elapsed: ", delta_d, " secs ", std::size_t(iters / delta_d), "/sec");

    // NOTE: '<<' operator is very slow.
    //  About 50% of total cpu consumption is occupied...
    //  Should use std::format() instead if c++17 is available.
}

TEST_CASE("10 threads, competing over the same logger object, 100,000 iterations") {
    int iters = 100000;
    int thread_count = 10;

    using namespace nodec::logging;
    auto logger = get_logger();

    // auto handler = std::make_shared<handlers::StdoutHandler<formatters::SimpleFormatter>>();
    // auto conn = logger->add_handler([&](const LogRecord &record) { (*handler)(record); });

    using std::chrono::duration;
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;

    std::vector<std::thread> threads;
    threads.reserve(thread_count);

    auto start = high_resolution_clock::now();
    for (std::size_t i = 0; i < thread_count; ++i) {
        threads.emplace_back([&, i]() {
            for (auto j = 0; j < iters; ++j) {
                logger->info(__FILE__, __LINE__) << "Hello logger: msg number " << i << ", " << j;
            }
        });
    }
    for (auto &t : threads) {
        t.join();
    };

    auto delta = high_resolution_clock::now() - start;
    auto delta_d = duration_cast<duration<double>>(delta).count();

    MESSAGE("Elapsed: ", delta_d, " secs ", std::size_t(iters / delta_d), "/sec");
}