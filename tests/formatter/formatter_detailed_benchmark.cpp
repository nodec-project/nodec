#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <vector>

#include <nodec/formatter.hpp>

TEST_CASE("Detailed Benchmark - Construction Overhead") {
    std::cout << "\n--- Construction Overhead Test ---" << std::endl;
    const int iterations = 1000000;
    
    // Formatterオブジェクトの構築コスト
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        nodec::Formatter formatter;
        volatile auto* ptr = &formatter;
        (void)ptr;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Formatter construction: " << duration.count() << " microseconds" << std::endl;
    
    // ostringstream構築コスト（比較用）
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        std::ostringstream oss;
        volatile auto* ptr = &oss;
        (void)ptr;
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "ostringstream construction: " << duration.count() << " microseconds" << std::endl;
    
    CHECK(true);
}

TEST_CASE("Detailed Benchmark - String Types") {
    std::cout << "\n--- String Types Test ---" << std::endl;
    const int iterations = 100000;
    
    // 異なる文字列型での性能テスト
    std::string std_string = "Hello World";
    const char* c_string = "Hello World";
    
    // std::string
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        std::string result = nodec::Formatter() << std_string << i;
        volatile auto len = result.length();
        (void)len;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "std::string input: " << duration.count() << " microseconds" << std::endl;
    
    // const char*
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        std::string result = nodec::Formatter() << c_string << i;
        volatile auto len = result.length();
        (void)len;
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "const char* input: " << duration.count() << " microseconds" << std::endl;
    
    CHECK(true);
}

TEST_CASE("Detailed Benchmark - Error Formatter") {
    std::cout << "\n--- Error Formatter Test ---" << std::endl;
    const int iterations = 50000;
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        std::string result = nodec::ErrorFormatter<std::runtime_error>(__FILE__, __LINE__) 
                           << "Error occurred with value: " << i;
        volatile auto len = result.length();
        (void)len;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "ErrorFormatter: " << duration.count() << " microseconds" << std::endl;
    
    CHECK(true);
}

TEST_CASE("Detailed Benchmark - Multiple Instances") {
    std::cout << "\n--- Multiple Instances Test ---" << std::endl;
    // 注意: このテストは実際の並行性テストではなく、
    // 単一スレッドでの複数インスタンス作成テストです
    
    const int iterations = 10000;
    std::vector<std::string> results;
    results.reserve(iterations * 3);
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        // 複数のFormatterを同時に作成
        nodec::Formatter f1, f2, f3;
        f1 << "Thread1: " << i;
        f2 << "Thread2: " << (i * 2);
        f3 << "Thread3: " << (i * 3);
        
        results.push_back(f1);
        results.push_back(f2);
        results.push_back(f3);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Multiple instances: " << duration.count() << " microseconds" << std::endl;
    
    // 結果を使用して最適化を防ぐ
    volatile auto total_size = results.size();
    (void)total_size;
    
    CHECK(true);
}
