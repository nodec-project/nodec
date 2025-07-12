#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <nodec/logging/formatters/simple_formatter.hpp>
#include <nodec/logging/level.hpp>
#include <nodec/logging/log_record.hpp>

// ベンチマーク測定用のヘルパークラス
class BenchmarkTimer {
public:
    BenchmarkTimer(const std::string &name, int iterations = 1): name_(name), iterations_(iterations) {
        start_time_ = std::chrono::high_resolution_clock::now();
    }

    ~BenchmarkTimer() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time_);
        double avg_time = static_cast<double>(duration.count()) / iterations_;
        MESSAGE(name_, ": ", duration.count(), " microseconds total, ", avg_time, " microseconds per operation");
    }

private:
    std::string name_;
    int iterations_;
    std::chrono::high_resolution_clock::time_point start_time_;
};

TEST_CASE("SimpleFormatter - Output Format Structure") {
    using namespace std::chrono;
    using namespace nodec::logging;
    using namespace std::string_view_literals;

    formatters::SimpleFormatter formatter;
    LogRecord record{system_clock::time_point(), "TestLogger"sv, Level::Info, "Test message"sv, "test.cpp"sv, 100};
    std::string result = formatter(record);

    // MESSAGE("Formatted output: ", result);
    CHECK(result == "[INFO]  [TestLogger] - Test message\n(test.cpp line 100)\n");
}

TEST_CASE("SimpleFormatter - 10'000 Format Benchmark") {
    const int iterations = 10'000;

    using namespace std::chrono;
    using namespace nodec::logging;
    using namespace std::string_view_literals;

    MESSAGE("--- SimpleFormatter 10'000 Format Benchmark ---");

    formatters::SimpleFormatter formatter;

    {
        BenchmarkTimer timer("SimpleFormatter - 10'000 format operations", iterations);

        for (int i = 0; i < iterations; ++i) {
            LogRecord record{
                system_clock::now(),
                "BenchLogger"sv,
                Level::Info,
                "Benchmark message " + std::to_string(i),
                __FILE__,
                __LINE__};

            std::string result = formatter(record);

            // 結果を使用して最適化を防ぐ
            volatile auto len = result.length();
            (void)len;
        }
    }

    // テストが正常に実行されたことを確認
    CHECK(true);
}
