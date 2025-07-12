#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <chrono>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <sstream>
#include <iomanip>

#include <nodec/formatter.hpp>

// ベンチマーク測定用のヘルパークラス
class BenchmarkTimer {
public:
    BenchmarkTimer(const std::string &name): name_(name) {
        start_time_ = std::chrono::high_resolution_clock::now();
    }

    ~BenchmarkTimer() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time_);
        std::cout << name_ << ": " << duration.count() << " microseconds" << std::endl;
    }

private:
    std::string name_;
    std::chrono::high_resolution_clock::time_point start_time_;
};

TEST_CASE("Benchmark - Simple Concatenation") {
    const int iterations = 100000;

    std::cout << "\n--- Simple Concatenation Test ---" << std::endl;

    // nodec::FormatterLegacyのベンチマーク
    {
        BenchmarkTimer timer("nodec::FormatterLegacy - Simple concatenation");
        for (int i = 0; i < iterations; ++i) {
            std::string result = nodec::FormatterLegacy() << "Hello" << " " << "World" << i;
            // 結果を使用して最適化を防ぐ
            volatile auto len = result.length();
            (void)len;
        }
    }

    // std::ostringstreamのベンチマーク（比較用）
    {
        BenchmarkTimer timer("std::ostringstream - Simple concatenation");
        for (int i = 0; i < iterations; ++i) {
            std::ostringstream oss;
            oss << "Hello" << " " << "World" << i;
            std::string result = oss.str();
            // 結果を使用して最適化を防ぐ
            volatile auto len = result.length();
            (void)len;
        }
    }

    // nodec::StringBuilderStreamのベンチマーク
    {
        BenchmarkTimer timer("StringBuilderStream - Simple concatenation");
        for (int i = 0; i < iterations; ++i) {
            std::string result;
            nodec::StringBuilderStream builder(result);

            builder << "Hello" << " " << "World" << i;
            // 結果を使用して最適化を防ぐ
            volatile auto len = result.length();
            (void)len;
        }
    }

    // std::string + 演算子のベンチマーク（比較用）
    {
        BenchmarkTimer timer("std::string + operator - Simple concatenation");
        for (int i = 0; i < iterations; ++i) {
            std::string result = std::string("Hello") + " " + "World" + std::to_string(i);
            // 結果を使用して最適化を防ぐ
            volatile auto len = result.length();
            (void)len;
        }
    }

    // テストが正常に実行されたことを確認
    CHECK(true);
}

TEST_CASE("Benchmark - Numeric Formatting") {
    const int iterations = 100000;

    std::cout << "\n--- Numeric Formatting Test ---" << std::endl;

    // nodec::FormatterLegacyのベンチマーク
    {
        BenchmarkTimer timer("nodec::FormatterLegacy - Numeric formatting");
        for (int i = 0; i < iterations; ++i) {
            std::string result = nodec::FormatterLegacy() << "Number: " << i
                                                          << ", Float: " << (i * 3.14)
                                                          << ", Hex: " << std::hex << i;
            volatile auto len = result.length();
            (void)len;
        }
    }

    // nodec::FormatterStreamのベンチマーク
    {
        BenchmarkTimer timer("StringBuilderStream - Numeric formatting");
        for (int i = 0; i < iterations; ++i) {
            std::string result;
            nodec::StringBuilderStream builder(result);
            builder << "Number: " << i << ", Float: " << (i * 3.14) << ", Hex: " << std::hex << i;
            volatile auto len = result.length();
            (void)len;
        }
    }

    // std::ostringstreamのベンチマーク（比較用）
    {
        BenchmarkTimer timer("std::ostringstream - Numeric formatting");
        for (int i = 0; i < iterations; ++i) {
            std::ostringstream oss;
            oss << "Number: " << i << ", Float: " << (i * 3.14) << ", Hex: " << std::hex << i;
            std::string result = oss.str();
            volatile auto len = result.length();
            (void)len;
        }
    }

    // テストが正常に実行されたことを確認
    CHECK(true);
}

TEST_CASE("Benchmark - Long Chain") {
    const int iterations = 10000;

    std::cout << "\n--- Long Chain Test ---" << std::endl;

    // nodec::FormatterLegacyのベンチマーク - 長いチェーン
    {
        BenchmarkTimer timer("nodec::FormatterLegacy - Long chain");
        for (int i = 0; i < iterations; ++i) {
            std::string result = nodec::FormatterLegacy() << "Start: " << i
                                                          << ", Value1: " << (i * 2)
                                                          << ", Value2: " << (i * 3.5)
                                                          << ", Value3: " << (i % 100)
                                                          << ", Value4: " << std::hex << i
                                                          << ", Value5: " << std::dec << (i + 1000)
                                                          << ", End";
            volatile auto len = result.length();
            (void)len;
        }
    }

    // StringBuilderStreamのベンチマーク
    {
        BenchmarkTimer timer("StringBuilderStream - Long chain");
        for (int i = 0; i < iterations; ++i) {
            std::string result;
            nodec::StringBuilderStream builder(result);
            builder << "Start: " << i
                    << ", Value1: " << (i * 2)
                    << ", Value2: " << (i * 3.5)
                    << ", Value3: " << (i % 100)
                    << ", Value4: " << std::hex << i
                    << ", Value5: " << std::dec << (i + 1000)
                    << ", End";
            volatile auto len = result.length();
            (void)len;
        }
    }

    // std::ostringstreamのベンチマーク（比較用）
    {
        BenchmarkTimer timer("std::ostringstream - Long chain");
        for (int i = 0; i < iterations; ++i) {
            std::ostringstream oss;
            oss << "Start: " << i
                << ", Value1: " << (i * 2)
                << ", Value2: " << (i * 3.5)
                << ", Value3: " << (i % 100)
                << ", Value4: " << std::hex << i
                << ", Value5: " << std::dec << (i + 1000)
                << ", End";
            std::string result = oss.str();
            volatile auto len = result.length();
            (void)len;
        }
    }

    // std::string + 演算子のベンチマーク（比較用）
    {
        BenchmarkTimer timer("std::string + operator - Long chain");
        for (int i = 0; i < iterations; ++i) {
            std::string result = std::string("Start: ") + std::to_string(i)
                                 + ", Value1: " + std::to_string(i * 2)
                                 + ", Value2: " + std::to_string(i * 3.5)
                                 + ", Value3: " + std::to_string(i % 100)
                                 + ", Value4: " + std::to_string(i) // 16進数は複雑なので10進数で代用
                                 + ", Value5: " + std::to_string(i + 1000)
                                 + ", End";
            volatile auto len = result.length();
            (void)len;
        }
    }

    // テストが正常に実行されたことを確認
    CHECK(true);
}
