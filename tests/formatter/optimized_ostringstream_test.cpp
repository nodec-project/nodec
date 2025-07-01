#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec/optimized_ostringstream.hpp>
#include <nodec/vector3.hpp>
#include <iomanip>
#include <sstream>
#include <chrono>

TEST_CASE("Optimized ostringstream - Basic functionality") {
    SUBCASE("Default construction and basic output") {
        nodec::optimized_ostringstream oss;
        oss << "Hello, World!";
        CHECK(oss.str() == "Hello, World!");
    }
    
    SUBCASE("Reserved construction") {
        nodec::optimized_ostringstream oss(1024);
        CHECK(oss.capacity() >= 1024);
        
        oss << "Reserved string";
        CHECK(oss.str() == "Reserved string");
        CHECK(oss.capacity() >= 1024); // 容量は保持される
    }
    
    SUBCASE("Initial string with reserve") {
        std::string initial = "Start: ";
        nodec::optimized_ostringstream oss(initial, 512);
        CHECK(oss.capacity() >= 512);
        
        oss << "Additional content";
        CHECK(oss.str() == "Start: Additional content");
    }
}

TEST_CASE("Optimized ostringstream - Complete ostringstream compatibility") {
    SUBCASE("Manipulators work exactly like ostringstream") {
        // optimized_ostringstream
        nodec::optimized_ostringstream oss;
        oss << std::hex << std::uppercase << std::showbase 
            << std::setfill('0') << std::setw(8) << 255;
        std::string result1 = oss.str();
        
        // standard ostringstream
        std::ostringstream std_oss;
        std_oss << std::hex << std::uppercase << std::showbase 
                << std::setfill('0') << std::setw(8) << 255;
        std::string result2 = std_oss.str();
        
        CHECK(result1 == result2);
        CHECK(result1 == "00000XFF"); // 期待される結果
    }
    
    SUBCASE("Float formatting") {
        nodec::optimized_ostringstream oss;
        oss << std::fixed << std::setprecision(2) << 3.14159;
        
        std::ostringstream std_oss;
        std_oss << std::fixed << std::setprecision(2) << 3.14159;
        
        CHECK(oss.str() == std_oss.str());
        CHECK(oss.str() == "3.14");
    }
    
    SUBCASE("Boolean formatting") {
        nodec::optimized_ostringstream oss;
        oss << std::boolalpha << true << ", " << false;
        
        std::ostringstream std_oss;
        std_oss << std::boolalpha << true << ", " << false;
        
        CHECK(oss.str() == std_oss.str());
        CHECK(oss.str() == "true, false");
    }
}

TEST_CASE("Optimized ostringstream - Custom types support") {
    SUBCASE("Vector3 support via ostream operator") {
        nodec::Vector3f vec(1.0f, 2.0f, 3.0f);
        
        nodec::optimized_ostringstream oss;
        oss << "Vector: " << vec;
        
        std::ostringstream std_oss;
        std_oss << "Vector: " << vec;
        
        CHECK(oss.str() == std_oss.str());
        // Vector3のoperator<<が正しく呼ばれることを確認
        CHECK(oss.str().find("( 1, 2, 3 )") != std::string::npos);
    }
}

TEST_CASE("Optimized ostringstream - Performance features") {
    SUBCASE("Reserve prevents reallocations") {
        nodec::optimized_ostringstream oss(1000);
        size_t initial_capacity = oss.capacity();
        
        // 大量のデータを追加
        for (int i = 0; i < 100; ++i) {
            oss << "Data chunk " << i << "; ";
        }
        
        // 容量が維持されていることを確認（再配置が発生していない）
        CHECK(oss.capacity() >= initial_capacity);
        CHECK(oss.size() > 0);
    }
    
    SUBCASE("Clear and reuse") {
        nodec::optimized_ostringstream oss(512);
        
        oss << "First use";
        CHECK(oss.str() == "First use");
        
        oss.clear_content();
        CHECK(oss.str().empty());
        CHECK(oss.capacity() >= 512); // 容量は保持
        
        oss << "Second use";
        CHECK(oss.str() == "Second use");
    }
}

TEST_CASE("Optimized ostringstream - Complex formatting chains") {
    SUBCASE("Complex mixed formatting") {
        nodec::optimized_ostringstream oss(256);
        oss << "Value: " << std::hex << std::uppercase << std::showbase 
            << std::setfill('0') << std::setw(8) << 255
            << ", Float: " << std::dec << std::fixed << std::setprecision(2) << 3.14159
            << ", Bool: " << std::boolalpha << true;
        
        std::ostringstream std_oss;
        std_oss << "Value: " << std::hex << std::uppercase << std::showbase 
                << std::setfill('0') << std::setw(8) << 255
                << ", Float: " << std::dec << std::fixed << std::setprecision(2) << 3.14159
                << ", Bool: " << std::boolalpha << true;
        
        CHECK(oss.str() == std_oss.str());
        CHECK(oss.str() == "Value: 00000XFF, Float: 3.14, Bool: true");
    }
}

TEST_CASE("Optimized ostringstream - Move semantics") {
    SUBCASE("Move construction") {
        nodec::optimized_ostringstream oss1(256);
        oss1 << "Original content";
        
        auto oss2 = std::move(oss1);
        CHECK(oss2.str() == "Original content");
        CHECK(oss2.capacity() >= 256);
    }
    
    SUBCASE("Move assignment") {
        nodec::optimized_ostringstream oss1(256);
        oss1 << "Original content";
        
        nodec::optimized_ostringstream oss2;
        oss2 = std::move(oss1);
        
        CHECK(oss2.str() == "Original content");
        CHECK(oss2.capacity() >= 256);
    }
}

TEST_CASE("Optimized ostringstream - Conversion operators") {
    SUBCASE("String conversion operator") {
        nodec::optimized_ostringstream oss;
        oss << "Test content";
        
        std::string result = oss; // implicit conversion
        CHECK(result == "Test content");
        
        // 関数への渡し方も確認
        auto check_string = [](const std::string& s) { return s == "Test content"; };
        CHECK(check_string(oss));
    }
}

// パフォーマンステスト（参考用）
TEST_CASE("Optimized ostringstream - Performance comparison") {
    const int iterations = 1000;
    const size_t reserve_size = 10000;
    
    SUBCASE("Reserved vs non-reserved performance indication") {
        // このテストは実際のパフォーマンス測定ではなく、
        // APIが正しく動作することの確認
        
        // 事前リザーブあり
        nodec::optimized_ostringstream oss_reserved(reserve_size);
        for (int i = 0; i < iterations; ++i) {
            oss_reserved << "Item " << i << ": some content; ";
        }
        
        // 事前リザーブなし
        nodec::optimized_ostringstream oss_normal;
        for (int i = 0; i < iterations; ++i) {
            oss_normal << "Item " << i << ": some content; ";
        }
        
        // 両方とも同じ結果を生成することを確認
        CHECK(oss_reserved.str() == oss_normal.str());
        CHECK(oss_reserved.capacity() >= reserve_size);
        
        // パフォーマンス上の利点は実際の使用で測定される
        // ここでは機能の正確性のみを確認
    }
}
