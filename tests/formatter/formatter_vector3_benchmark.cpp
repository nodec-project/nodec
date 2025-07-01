#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <nodec/formatter.hpp>
#include <nodec/vector3.hpp>

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

TEST_CASE("Benchmark - Vector3 Fallback Performance") {
    const int iterations = 50000;
    
    std::cout << "\n--- Vector3 Fallback Performance Test ---" << std::endl;
    
    // テスト用のVector3データ
    std::vector<nodec::Vector3f> vectors;
    vectors.reserve(iterations);
    for (int i = 0; i < iterations; ++i) {
        vectors.emplace_back(static_cast<float>(i), static_cast<float>(i * 2), static_cast<float>(i * 3));
    }
    
    // nodec::Formatter（フォールバック使用）
    {
        BenchmarkTimer timer("nodec::Formatter - Vector3f fallback");
        std::vector<std::string> results;
        results.reserve(iterations);
        
        for (int i = 0; i < iterations; ++i) {
            results.push_back(nodec::Formatter() << "Vec: " << vectors[i] << ", Index: " << i);
        }
        
        // 結果を使用して最適化を防ぐ
        volatile auto total_size = results.size();
        (void)total_size;
    }
    
    // std::ostringstream（比較用）
    {
        BenchmarkTimer timer("std::ostringstream - Vector3f");
        std::vector<std::string> results;
        results.reserve(iterations);
        
        for (int i = 0; i < iterations; ++i) {
            std::ostringstream oss;
            oss << "Vec: " << vectors[i] << ", Index: " << i;
            results.push_back(oss.str());
        }
        
        // 結果を使用して最適化を防ぐ
        volatile auto total_size = results.size();
        (void)total_size;
    }
    
    // std::string + 演算子（手動でVector3を文字列化）
    {
        BenchmarkTimer timer("std::string + operator - Manual Vector3f");
        std::vector<std::string> results;
        results.reserve(iterations);
        
        for (int i = 0; i < iterations; ++i) {
            std::string result = std::string("Vec: ( ") 
                               + std::to_string(vectors[i].x) + ", "
                               + std::to_string(vectors[i].y) + ", "
                               + std::to_string(vectors[i].z) + " ), Index: "
                               + std::to_string(i);
            results.push_back(result);
        }
        
        // 結果を使用して最適化を防ぐ
        volatile auto total_size = results.size();
        (void)total_size;
    }
    
    CHECK(true);
}

TEST_CASE("Benchmark - Vector3 Types Comparison") {
    const int iterations = 30000;
    
    std::cout << "\n--- Vector3 Types Performance Comparison ---" << std::endl;
    
    // Vector3f のベンチマーク
    {
        BenchmarkTimer timer("nodec::Formatter - Vector3f");
        nodec::Vector3f vec(1.5f, 2.7f, 3.9f);
        
        for (int i = 0; i < iterations; ++i) {
            std::string result = nodec::Formatter() << "Float Vec: " << vec << " [" << i << "]";
            volatile auto len = result.length();
            (void)len;
        }
    }
    
    // Vector3i のベンチマーク
    {
        BenchmarkTimer timer("nodec::Formatter - Vector3i");
        nodec::Vector3i vec(15, 27, 39);
        
        for (int i = 0; i < iterations; ++i) {
            std::string result = nodec::Formatter() << "Int Vec: " << vec << " [" << i << "]";
            volatile auto len = result.length();
            (void)len;
        }
    }
    
    // Vector3d のベンチマーク
    {
        BenchmarkTimer timer("nodec::Formatter - Vector3d");
        nodec::Vector3d vec(1.5, 2.7, 3.9);
        
        for (int i = 0; i < iterations; ++i) {
            std::string result = nodec::Formatter() << "Double Vec: " << vec << " [" << i << "]";
            volatile auto len = result.length();
            (void)len;
        }
    }
    
    CHECK(true);
}

TEST_CASE("Benchmark - Complex Vector3 Chains") {
    const int iterations = 10000;
    
    std::cout << "\n--- Complex Vector3 Chain Test ---" << std::endl;
    
    // 複雑なVector3チェーンのベンチマーク
    {
        BenchmarkTimer timer("nodec::Formatter - Complex Vector3 chain");
        
        for (int i = 0; i < iterations; ++i) {
            nodec::Vector3f pos(static_cast<float>(i), static_cast<float>(i * 2), static_cast<float>(i * 3));
            nodec::Vector3f vel(1.0f, 0.5f, -1.0f);
            nodec::Vector3i id(i, i + 1, i + 2);
            
            std::string result = nodec::Formatter() 
                               << "Entity[" << i << "] "
                               << "Pos: " << pos << ", "
                               << "Vel: " << vel << ", "
                               << "ID: " << id << ", "
                               << "Time: " << (i * 0.016f);
            
            volatile auto len = result.length();
            (void)len;
        }
    }
    
    // std::ostringstream での同等処理（比較用）
    {
        BenchmarkTimer timer("std::ostringstream - Complex Vector3 chain");
        
        for (int i = 0; i < iterations; ++i) {
            nodec::Vector3f pos(static_cast<float>(i), static_cast<float>(i * 2), static_cast<float>(i * 3));
            nodec::Vector3f vel(1.0f, 0.5f, -1.0f);
            nodec::Vector3i id(i, i + 1, i + 2);
            
            std::ostringstream oss;
            oss << "Entity[" << i << "] "
                << "Pos: " << pos << ", "
                << "Vel: " << vel << ", "
                << "ID: " << id << ", "
                << "Time: " << (i * 0.016f);
            
            std::string result = oss.str();
            volatile auto len = result.length();
            (void)len;
        }
    }
    
    CHECK(true);
}

TEST_CASE("Benchmark - Vector3 Memory Efficiency") {
    const int iterations = 20000;
    
    std::cout << "\n--- Vector3 Memory Efficiency Test ---" << std::endl;
    
    // 大量のVector3文字列化でのメモリ効率テスト
    {
        BenchmarkTimer timer("nodec::Formatter - Vector3 memory test");
        std::vector<std::string> results;
        results.reserve(iterations);
        
        for (int i = 0; i < iterations; ++i) {
            // 事前リザーブを使用
            nodec::Formatter formatter(64);  // Vector3文字列は通常30-40文字程度
            
            nodec::Vector3f vec(
                static_cast<float>(i * 1.1f), 
                static_cast<float>(i * 2.2f), 
                static_cast<float>(i * 3.3f)
            );
            
            formatter << "Vector[" << i << "]: " << vec << " magnitude";
            results.push_back(formatter.str());
        }
        
        volatile auto total_size = results.size();
        (void)total_size;
    }
    
    // 事前リザーブなしでの比較
    {
        BenchmarkTimer timer("nodec::Formatter - Vector3 without reserve");
        std::vector<std::string> results;
        results.reserve(iterations);
        
        for (int i = 0; i < iterations; ++i) {
            nodec::Vector3f vec(
                static_cast<float>(i * 1.1f), 
                static_cast<float>(i * 2.2f), 
                static_cast<float>(i * 3.3f)
            );
            
            results.push_back(nodec::Formatter() << "Vector[" << i << "]: " << vec << " magnitude");
        }
        
        volatile auto total_size = results.size();
        (void)total_size;
    }
    
    CHECK(true);
}
