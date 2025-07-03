#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include <nodec/formatter.hpp>
#include <nodec/optimized_ostringstream.hpp>

// ベンチマーク測定用のヘルパークラス
class BenchmarkTimer {
public:
    BenchmarkTimer(const std::string& name) : name_(name) {
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
    
    // nodec::Formatterのベンチマーク
    {
        BenchmarkTimer timer("nodec::Formatter - Simple concatenation");
        for (int i = 0; i < iterations; ++i) {
            std::string result = nodec::Formatter() << "Hello" << " " << "World" << i;
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
    
    // nodec::optimized_ostringstream (FastFormatter)のベンチマーク
    {
        BenchmarkTimer timer("FastFormatter - Simple concatenation");
        for (int i = 0; i < iterations; ++i) {
            nodec::optimized_ostringstream oss;
            oss << "Hello" << " " << "World" << i;
            std::string result = oss.str();
            // 結果を使用して最適化を防ぐ
            volatile auto len = result.length();
            (void)len;
        }
    }
    
    // FastFormatter view() - コピーなし
    {
        BenchmarkTimer timer("FastFormatter view() - Simple concatenation");
        for (int i = 0; i < iterations; ++i) {
            nodec::optimized_ostringstream oss;
            oss << "Hello" << " " << "World" << i;
            std::string_view result = oss.view(); // No copy
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
    
    // nodec::Formatterのベンチマーク
    {
        BenchmarkTimer timer("nodec::Formatter - Numeric formatting");
        for (int i = 0; i < iterations; ++i) {
            std::string result = nodec::Formatter() << "Number: " << i 
                                                   << ", Float: " << (i * 3.14) 
                                                   << ", Hex: " << std::hex << i;
            volatile auto len = result.length();
            (void)len;
        }
    }
    
    // nodec::optimized_ostringstream (FastFormatter)のベンチマーク
    {
        BenchmarkTimer timer("FastFormatter - Numeric formatting");
        for (int i = 0; i < iterations; ++i) {
            nodec::optimized_ostringstream oss;
            oss << "Number: " << i << ", Float: " << (i * 3.14) << ", Hex: " << std::hex << i;
            std::string result = oss.str();
            volatile auto len = result.length();
            (void)len;
        }
    }
    
    // FastFormatter view() - コピーなし
    {
        BenchmarkTimer timer("FastFormatter view() - Numeric formatting");
        for (int i = 0; i < iterations; ++i) {
            nodec::optimized_ostringstream oss;
            oss << "Number: " << i << ", Float: " << (i * 3.14) << ", Hex: " << std::hex << i;
            std::string_view result = oss.view(); // No copy
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
    
    // nodec::Formatterのベンチマーク - 長いチェーン
    {
        BenchmarkTimer timer("nodec::Formatter - Long chain");
        for (int i = 0; i < iterations; ++i) {
            std::string result = nodec::Formatter() << "Start: " << i
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
    
    // nodec::optimized_ostringstream (FastFormatter)のベンチマーク
    {
        BenchmarkTimer timer("FastFormatter - Long chain");
        for (int i = 0; i < iterations; ++i) {
            nodec::optimized_ostringstream oss;
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
    
    // FastFormatter view() - コピーなし
    {
        BenchmarkTimer timer("FastFormatter view() - Long chain");
        for (int i = 0; i < iterations; ++i) {
            nodec::optimized_ostringstream oss;
            oss << "Start: " << i
                << ", Value1: " << (i * 2)
                << ", Value2: " << (i * 3.5)
                << ", Value3: " << (i % 100)
                << ", Value4: " << std::hex << i
                << ", Value5: " << std::dec << (i + 1000)
                << ", End";
            std::string_view result = oss.view(); // No copy
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

TEST_CASE("Benchmark - Memory Usage") {
    const int iterations = 50000;
    
    std::cout << "\n--- Memory Usage Test ---" << std::endl;
    
    // 大量のFormatterオブジェクトを作成してメモリ使用量をテスト
    {
        BenchmarkTimer timer("nodec::Formatter - Multiple instances");
        std::vector<std::string> results;
        results.reserve(iterations);
        
        for (int i = 0; i < iterations; ++i) {
            results.push_back(nodec::Formatter() << "Test " << i << " with value " << (i * 2.5));
        }
        
        // 結果を使用して最適化を防ぐ
        volatile auto total_size = results.size();
        (void)total_size;
    }
    
    // nodec::optimized_ostringstream (FastFormatter)のベンチマーク
    {
        BenchmarkTimer timer("FastFormatter - Multiple instances");
        std::vector<std::string> results;
        results.reserve(iterations);
        
        for (int i = 0; i < iterations; ++i) {
            nodec::optimized_ostringstream oss;
            oss << "Test " << i << " with value " << (i * 2.5);
            results.push_back(oss.str());
        }
        
        // 結果を使用して最適化を防ぐ
        volatile auto total_size = results.size();
        (void)total_size;
    }
    
    // FastFormatter view() - string_viewのベクタ（参考用）
    {
        BenchmarkTimer timer("FastFormatter view() - Multiple instances (view only)");
        std::vector<nodec::optimized_ostringstream> streams;
        streams.reserve(iterations);
        
        for (int i = 0; i < iterations; ++i) {
            nodec::optimized_ostringstream oss;
            oss << "Test " << i << " with value " << (i * 2.5);
            streams.push_back(std::move(oss));
        }
        
        // string_viewとして結果を取得（実際の使用例）
        for (const auto& oss : streams) {
            std::string_view result = oss.view();
            volatile auto len = result.length();
            (void)len;
        }
        
        // 結果を使用して最適化を防ぐ
        volatile auto total_size = streams.size();
        (void)total_size;
    }

    {
        BenchmarkTimer timer("std::ostringstream - Multiple instances");
        std::vector<std::string> results;
        results.reserve(iterations);
        
        for (int i = 0; i < iterations; ++i) {
            std::ostringstream oss;
            oss << "Test " << i << " with value " << (i * 2.5);
            results.push_back(oss.str());
        }
        
        // 結果を使用して最適化を防ぐ
        volatile auto total_size = results.size();
        (void)total_size;
    }
    
    // テストが正常に実行されたことを確認
    CHECK(true);
}

// doctestが自動的にテストを実行するため、main関数は不要

TEST_CASE("Benchmark - Reserve Capacity") {
    const int iterations = 50000;
    
    std::cout << "\n--- Reserve Capacity Test ---" << std::endl;
    
    // FastFormatter with reserve
    {
        BenchmarkTimer timer("FastFormatter with reserve - Long text");
        for (int i = 0; i < iterations; ++i) {
            nodec::optimized_ostringstream oss(256); // Pre-reserve capacity
            oss << "This is a longer text example that will test the reserve functionality: "
                << i << ", value1: " << (i * 2) << ", value2: " << (i * 3.14)
                << ", value3: " << std::hex << i << ", value4: " << std::dec << (i + 100)
                << " - End of the long text";
            std::string result = oss.str();
            volatile auto len = result.length();
            (void)len;
        }
    }
    
    // FastFormatter with reserve + view()
    {
        BenchmarkTimer timer("FastFormatter view() with reserve - Long text");
        for (int i = 0; i < iterations; ++i) {
            nodec::optimized_ostringstream oss(256); // Pre-reserve capacity
            oss << "This is a longer text example that will test the reserve functionality: "
                << i << ", value1: " << (i * 2) << ", value2: " << (i * 3.14)
                << ", value3: " << std::hex << i << ", value4: " << std::dec << (i + 100)
                << " - End of the long text";
            std::string_view result = oss.view(); // No copy
            volatile auto len = result.length();
            (void)len;
        }
    }
    
    // FastFormatter without reserve (for comparison)
    {
        BenchmarkTimer timer("FastFormatter without reserve - Long text");
        for (int i = 0; i < iterations; ++i) {
            nodec::optimized_ostringstream oss; // No pre-reserve
            oss << "This is a longer text example that will test the reserve functionality: "
                << i << ", value1: " << (i * 2) << ", value2: " << (i * 3.14)
                << ", value3: " << std::hex << i << ", value4: " << std::dec << (i + 100)
                << " - End of the long text";
            std::string result = oss.str();
            volatile auto len = result.length();
            (void)len;
        }
    }
    
    // FastFormatter without reserve + view()
    {
        BenchmarkTimer timer("FastFormatter view() without reserve - Long text");
        for (int i = 0; i < iterations; ++i) {
            nodec::optimized_ostringstream oss; // No pre-reserve
            oss << "This is a longer text example that will test the reserve functionality: "
                << i << ", value1: " << (i * 2) << ", value2: " << (i * 3.14)
                << ", value3: " << std::hex << i << ", value4: " << std::dec << (i + 100)
                << " - End of the long text";
            std::string_view result = oss.view(); // No copy
            volatile auto len = result.length();
            (void)len;
        }
    }
    
    // std::ostringstream (for comparison)
    {
        BenchmarkTimer timer("std::ostringstream - Long text");
        for (int i = 0; i < iterations; ++i) {
            std::ostringstream oss;
            oss << "This is a longer text example that will test the reserve functionality: "
                << i << ", value1: " << (i * 2) << ", value2: " << (i * 3.14)
                << ", value3: " << std::hex << i << ", value4: " << std::dec << (i + 100)
                << " - End of the long text";
            std::string result = oss.str();
            volatile auto len = result.length();
            (void)len;
        }
    }

    // テストが正常に実行されたことを確認
    CHECK(true);
}

TEST_CASE("Benchmark - String View Performance") {
    const int iterations = 100000;
    
    std::cout << "\n--- String View Performance Test ---" << std::endl;
    
    // FastFormatter view() vs str() comparison
    {
        BenchmarkTimer timer("FastFormatter view() - No copy");
        for (int i = 0; i < iterations; ++i) {
            nodec::optimized_ostringstream oss;
            oss << "Test string " << i << " with some content";
            std::string_view result = oss.view(); // No copy
            // 結果を使用して最適化を防ぐ
            volatile auto len = result.length();
            (void)len;
        }
    }
    
    {
        BenchmarkTimer timer("FastFormatter str() - With copy");
        for (int i = 0; i < iterations; ++i) {
            nodec::optimized_ostringstream oss;
            oss << "Test string " << i << " with some content";
            std::string result = oss.str(); // Copy
            // 結果を使用して最適化を防ぐ
            volatile auto len = result.length();
            (void)len;
        }
    }
    
    // Long string performance comparison
    {
        BenchmarkTimer timer("FastFormatter view() - Long strings");
        for (int i = 0; i < iterations / 10; ++i) {
            nodec::optimized_ostringstream oss(1024);
            oss << "This is a much longer test string that contains more data to demonstrate "
                << "the performance benefits of using string_view instead of copying the entire string. "
                << "Iteration number: " << i << ", additional data: " << (i * 3.14159)
                << ", more content to make this string significantly longer and test memory efficiency.";
            std::string_view result = oss.view(); // No copy
            volatile auto len = result.length();
            (void)len;
        }
    }
    
    {
        BenchmarkTimer timer("FastFormatter str() - Long strings");
        for (int i = 0; i < iterations / 10; ++i) {
            nodec::optimized_ostringstream oss(1024);
            oss << "This is a much longer test string that contains more data to demonstrate "
                << "the performance benefits of using string_view instead of copying the entire string. "
                << "Iteration number: " << i << ", additional data: " << (i * 3.14159)
                << ", more content to make this string significantly longer and test memory efficiency.";
            std::string result = oss.str(); // Copy
            volatile auto len = result.length();
            (void)len;
        }
    }
    
    // テストが正常に実行されたことを確認
    CHECK(true);
}
