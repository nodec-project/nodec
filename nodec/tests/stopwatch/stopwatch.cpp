#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec/stopwatch.hpp>

#include <chrono>
#include <thread>

// template<typename DurationA, typename DurationB>
// bool nearly_equal(const DurationA& a, const DurationB& b) {
//     using namespace std::chrono;

//     auto d = a - b;
//     d = d > d.zero() ? d : -d;
//     return d < milliseconds(50);
// }

TEST_CASE("Testing initial state") {
    using namespace nodec;
    using namespace std::chrono;

    Stopwatch sw;

    CHECK(sw.is_running() == false);
    CHECK(sw.elapsed() == milliseconds(0));
    CHECK(sw.lap() == milliseconds(0));
}

TEST_CASE("Testing elapsed time between start() and stop()") {
    using namespace nodec;
    using namespace std::chrono;

    Stopwatch sw;

    sw.start();
    std::this_thread::sleep_for(milliseconds(100));
    sw.stop();

    CHECK(sw.elapsed() >= milliseconds(100));

    // Wait for 100 ms while stopwatch is stopped.
    std::this_thread::sleep_for(milliseconds(100));

    sw.start();
    std::this_thread::sleep_for(milliseconds(100));
    sw.stop();

    CHECK(sw.elapsed() >= milliseconds(200));
}

TEST_CASE("Testing is_running() after start() and stop()") {
    using namespace nodec;

    Stopwatch sw;

    CHECK(sw.is_running() == false);

    sw.start();

    CHECK(sw.is_running() == true);

    sw.stop();

    CHECK(sw.is_running() == false);
}

TEST_CASE("Testing reset()") {
    using namespace nodec;
    using namespace std::chrono;

    Stopwatch sw;

    sw.start();
    std::this_thread::sleep_for(milliseconds(100));
    sw.stop();

    CHECK(sw.elapsed() > milliseconds(100));

    sw.reset();

    CHECK(sw.is_running() == false);
    CHECK(sw.elapsed() == milliseconds(0));
    CHECK(sw.lap() == milliseconds(0));
}

TEST_CASE("Testing lap elapsed") {
    using namespace nodec;
    using namespace std::chrono;

    Stopwatch sw;

    // No effect
    CHECK(sw.lap() == milliseconds(0));

    sw.start();

    std::this_thread::sleep_for(milliseconds(100));
    CHECK(sw.lap() >= milliseconds(100));
    std::this_thread::sleep_for(milliseconds(100));
    CHECK(sw.lap() >= milliseconds(100));
    std::this_thread::sleep_for(milliseconds(100));
    CHECK(sw.lap() >= milliseconds(100));
}

TEST_CASE("Testing lap elapsed after stop") {
    using namespace nodec;
    using namespace std::chrono;

    Stopwatch sw;
    sw.start();

    std::this_thread::sleep_for(milliseconds(100));

    // 100ms
    CHECK(sw.lap() >= milliseconds(100));
    // {
    //     // 100ms
    //     auto lap = sw.lap<float>();
    //     CHECK(lap >= milliseconds(100));
    //     CHECK(lap <= milliseconds(200));
    // }

    std::this_thread::sleep_for(milliseconds(100));
    sw.stop();

    // 100ms
    CHECK(sw.lap() >= milliseconds(100));
}

TEST_CASE("Testing restart()") {
    using namespace nodec;
    using namespace std::chrono;

    SUBCASE("start() -> restart()") {
        Stopwatch sw;
        sw.start();
        std::this_thread::sleep_for(milliseconds(100));
        CHECK(sw.lap() >= milliseconds(100));
        sw.restart();
        std::this_thread::sleep_for(milliseconds(100));
        CHECK(sw.lap() >= milliseconds(100));
    }
    SUBCASE("restart() -> restart()") {
        Stopwatch sw;
        sw.restart();
        std::this_thread::sleep_for(milliseconds(100));
        CHECK(sw.lap() >= milliseconds(100));
        sw.restart();
        std::this_thread::sleep_for(milliseconds(100));
        CHECK(sw.lap() >= milliseconds(100));
    }
}