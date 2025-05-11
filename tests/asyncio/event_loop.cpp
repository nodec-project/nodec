#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec/asyncio/event_loop.hpp>
#include <nodec/asyncio/event_promise.hpp>

TEST_CASE("testing push events") {
    using namespace nodec::asyncio;

    EventLoop event_loop;

    std::vector<int> flags;
    flags.reserve(3);

    event_loop.schedule([&]() {
        flags.push_back(1);
    });
    event_loop.schedule([&]() {
        flags.push_back(2);
    });
    event_loop.schedule([&]() {
        flags.push_back(3);
    });

    event_loop.spin();

    CHECK(flags.size() == 3);
    CHECK(flags[0] == 1);
    CHECK(flags[1] == 2);
    CHECK(flags[2] == 3);
}

TEST_CASE("test push events with nested") {
    using namespace nodec::asyncio;

    EventLoop event_loop;

    std::vector<int> flags;
    flags.reserve(3);

    event_loop.schedule([&]() {
        flags.push_back(1);
        event_loop.schedule([&]() {
            flags.push_back(2);
            event_loop.schedule([&]() {
                flags.push_back(3);
            });
        });
    });

    event_loop.spin();

    CHECK(flags.size() == 3);
    CHECK(flags[0] == 1);
    CHECK(flags[1] == 2);
    CHECK(flags[2] == 3);
}

TEST_CASE("Single-threaded EventLoop benchmark") {
    using namespace nodec::asyncio;
    using namespace std::chrono;

    // std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // スレッドを少し待機させる

    // ベンチマーク設定
    constexpr size_t NUM_EVENTS = 100000;                    // イベント総数（必要に応じて調整）
    std::vector<size_t> BATCH_SIZES = {0, 100, 1000, 10000}; // テストするバッチサイズ

    struct BenchmarkResult {
        double total_time_ms;
        double events_per_second;
        double avg_latency_us;
    };

    // 各バッチサイズでベンチマークを実行
    for (size_t batch_size : BATCH_SIZES) {
        SUBCASE(("Batch size: " + std::string(batch_size == 0 ? "unlimited" : std::to_string(batch_size))).c_str()) {
            EventLoop event_loop;
            std::atomic<size_t> counter{0};

            // タイミング開始
            auto start_time = high_resolution_clock::now();

            // すべてのイベントをスケジュール
            for (size_t i = 0; i < NUM_EVENTS; ++i) {
                event_loop.schedule([&counter]() {
                    counter.fetch_add(1, std::memory_order_relaxed);
                });
            }

            // イベントの処理
            size_t processed = 0;
            while (processed < NUM_EVENTS) {
                size_t batch = event_loop.spin_once(batch_size);
                processed += batch;
                if (batch == 0) break; // すべて処理完了
            }

            // タイミング終了
            auto end_time = high_resolution_clock::now();
            duration<double, std::milli> elapsed = end_time - start_time;

            // 結果の計算
            BenchmarkResult result;
            result.total_time_ms = elapsed.count();
            result.events_per_second = (NUM_EVENTS * 1000.0) / result.total_time_ms;
            result.avg_latency_us = (result.total_time_ms * 1000.0) / NUM_EVENTS;

            // 検証
            CHECK(counter.load() == NUM_EVENTS);
            CHECK(processed == NUM_EVENTS);

            // 結果の出力（doctestの出力に表示される）
            MESSAGE("Events processed: ", NUM_EVENTS);
            MESSAGE("Total time: ", result.total_time_ms, " ms");
            MESSAGE("Throughput: ", std::fixed, std::setprecision(2),
                    result.events_per_second, " events/second");
            MESSAGE("Avg latency: ", result.avg_latency_us, " us/event");

            // 最低限のスループット要件（適切な値に調整）
            // 実行環境に大きく依存するため、初回実行後に適切なしきい値に調整することを推奨
            CHECK(result.events_per_second > 10000); // 最低10,000イベント/秒
        }
    }
}

// TEST_CASE("test") {
//     using namespace nodec;
//     EventLoop event_loop;

//     struct TestProcessFlow {
//         EventLoop &event_loop;

//         int process_a_retry_count = 0;

//         void run_process_a() {
//             std::cout << "run_process_a()..." << std::endl;
//             if (process_a_retry_count++ > 5) {
//                 std::cout << "run_process_a() retry count exceeded" << std::endl;
//                 return;
//             }
//             bool condition = process_a_retry_count > 3; // 条件判定
//             if (!condition) {
//                 event_loop.schedule([this]() { run_process_a(); }, std::chrono::milliseconds(1000));
//                 return;
//             }
//             event_loop.schedule([this]() { run_process_b(); });
//         }

//         void run_process_b() {
//             std::cout << "run_process_b()..." << std::endl;
//         }

//         void start() {
//             event_loop.schedule([this]() { run_process_a(); });
//         }
//     };

//     TestProcessFlow test_process_flow{event_loop};
//     test_process_flow.start();
//     event_loop.spin();
// }
