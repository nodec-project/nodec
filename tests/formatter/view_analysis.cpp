#include <chrono>
#include <iostream>
#include <iomanip>
#include <nodec/optimized_ostringstream.hpp>

class PreciseBenchmark {
public:
    PreciseBenchmark(const std::string& name) : name_(name) {
        start_ = std::chrono::high_resolution_clock::now();
    }
    
    ~PreciseBenchmark() {
        auto end = std::chrono::high_resolution_clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_);
        std::cout << std::setw(40) << name_ << ": " << std::setw(10) << ns.count() << " ns" << std::endl;
    }

private:
    std::string name_;
    std::chrono::high_resolution_clock::time_point start_;
};

int main() {
    const int iterations = 1000000;
    
    std::cout << "=== View() vs str() Detailed Analysis ===" << std::endl;
    std::cout << std::setw(40) << "Test Case" << ": " << std::setw(10) << "Time (ns)" << std::endl;
    std::cout << std::string(55, '-') << std::endl;
    
    // 1. シンプルな文字列での比較
    {
        PreciseBenchmark bench("Simple str() - 1M iterations");
        for (int i = 0; i < iterations; ++i) {
            nodec::optimized_ostringstream oss;
            oss << "Test " << i;
            std::string result = oss.str();
            volatile auto len = result.length();
            (void)len;
        }
    }
    
    {
        PreciseBenchmark bench("Simple view() - 1M iterations");
        for (int i = 0; i < iterations; ++i) {
            nodec::optimized_ostringstream oss;
            oss << "Test " << i;
            std::string_view result = oss.view();
            volatile auto len = result.length();
            (void)len;
        }
    }
    
    // 2. 単一操作での比較
    std::cout << std::endl;
    nodec::optimized_ostringstream test_oss;
    test_oss << "This is a test string with some content: " << 12345 << ", float: " << 3.14159;
    
    {
        PreciseBenchmark bench("Single str() call - 1M times");
        for (int i = 0; i < iterations; ++i) {
            std::string result = test_oss.str();
            volatile auto len = result.length();
            (void)len;
        }
    }
    
    {
        PreciseBenchmark bench("Single view() call - 1M times");
        for (int i = 0; i < iterations; ++i) {
            std::string_view result = test_oss.view();
            volatile auto len = result.length();
            (void)len;
        }
    }
    
    // 3. コンストラクション コストの比較
    std::cout << std::endl;
    {
        PreciseBenchmark bench("string construction only");
        for (int i = 0; i < iterations; ++i) {
            std::string result("Test string content for analysis");
            volatile auto len = result.length();
            (void)len;
        }
    }
    
    {
        PreciseBenchmark bench("string_view construction only");
        std::string source("Test string content for analysis");
        for (int i = 0; i < iterations; ++i) {
            std::string_view result(source);
            volatile auto len = result.length();
            (void)len;
        }
    }
    
    return 0;
}
