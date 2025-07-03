#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <nodec/optimized_ostringstream.hpp>
#include <chrono>
#include <vector>
#include <string>
#include <iostream>

using namespace std::chrono;

// 改良版: 文字列長に応じて自動選択するバージョン
template<typename CharT = char, typename Traits = std::char_traits<CharT>>
class smart_optimized_ostringstream : public nodec::basic_optimized_ostringstream<CharT, Traits> {
public:
    using base_type = nodec::basic_optimized_ostringstream<CharT, Traits>;
    using string_type = typename base_type::string_type;
    using string_view_type = typename base_type::string_view_type;
    
    // 基底クラスのコンストラクタを継承
    using base_type::base_type;
    
    // 文字列長に応じて最適な方法を自動選択
    auto get_optimal() const {
        const auto size = this->rdbuf()->view().size();
        if (size > 100) {  // 閾値: 100文字
            return this->view();
        } else {
            return string_view_type(this->str());  // str()をstring_viewでラップ
        }
    }
    
    // コンパイル時選択版
    template<bool PreferView = false>
    auto get_string() const {
        if constexpr (PreferView) {
            return this->view();
        } else {
            return this->str();
        }
    }
};

using SmartFormatter = smart_optimized_ostringstream<>;

TEST_CASE("Smart Formatter Performance Test") {
    const int iterations = 100000;
    
    std::cout << "=== Smart Formatter Analysis ===" << std::endl;
    
    // 短い文字列テスト
    {
        auto start = high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            SmartFormatter sf;
            sf << "Test" << i;
            auto result = sf.get_optimal();  // 自動選択
            (void)result.size(); // 使用することで最適化を防ぐ
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        std::cout << "Smart Formatter (short strings): " << duration.count() << " microseconds" << std::endl;
    }
    
    // 長い文字列テスト
    {
        auto start = high_resolution_clock::now();
        for (int i = 0; i < iterations / 10; ++i) {  // 回数を減らして調整
            SmartFormatter sf;
            sf << "This is a very long string that exceeds the threshold of 100 characters. ";
            sf << "It contains multiple parts to make it definitely longer than the threshold. ";
            sf << "Test number: " << i << " with additional padding to ensure length.";
            auto result = sf.get_optimal();  // 自動選択
            (void)result.size();
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        std::cout << "Smart Formatter (long strings): " << duration.count() << " microseconds" << std::endl;
    }
    
    // 比較: 従来のview()固定
    {
        auto start = high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            nodec::optimized_ostringstream sf;
            sf << "Test" << i;
            auto result = sf.view();
            (void)result.size();
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        std::cout << "Regular view() (short strings): " << duration.count() << " microseconds" << std::endl;
    }
    
    // 比較: 従来のstr()固定
    {
        auto start = high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            nodec::optimized_ostringstream sf;
            sf << "Test" << i;
            auto result = sf.str();
            (void)result.size();
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        std::cout << "Regular str() (short strings): " << duration.count() << " microseconds" << std::endl;
    }
    
    std::cout << std::endl;
    
    // テンプレート版のテスト
    std::cout << "=== Template-based Selection ===" << std::endl;
    
    {
        auto start = high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            SmartFormatter sf;
            sf << "Test" << i;
            auto result = sf.get_string<false>();  // str() 相当
            (void)result.size();
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        std::cout << "Template-based str(): " << duration.count() << " microseconds" << std::endl;
    }
    
    {
        auto start = high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            SmartFormatter sf;
            sf << "Test" << i;
            auto result = sf.get_string<true>();   // view() 相当
            (void)result.size();
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        std::cout << "Template-based view(): " << duration.count() << " microseconds" << std::endl;
    }
    
    REQUIRE(true); // テストが正常に実行されたことを示す
}

TEST_CASE("Memory Overhead Analysis") {
    std::cout << "=== Memory Overhead Analysis ===" << std::endl;
    
    // メモリ使用量の比較
    std::cout << "sizeof(std::string): " << sizeof(std::string) << " bytes" << std::endl;
    std::cout << "sizeof(std::string_view): " << sizeof(std::string_view) << " bytes" << std::endl;
    std::cout << "sizeof(nodec::optimized_ostringstream): " << sizeof(nodec::optimized_ostringstream) << " bytes" << std::endl;
    std::cout << "sizeof(SmartFormatter): " << sizeof(SmartFormatter) << " bytes" << std::endl;
    
    // 実際の文字列でのメモリ使用量テスト
    {
        std::string short_str = "Test123";
        std::string_view short_view = short_str;
        
        std::cout << "Short string - std::string size: " << short_str.size() 
                  << ", capacity: " << short_str.capacity() << std::endl;
        std::cout << "Short string - std::string_view size: " << short_view.size() << std::endl;
    }
    
    {
        std::string long_str(200, 'A');  // 200文字の長い文字列
        std::string_view long_view = long_str;
        
        std::cout << "Long string - std::string size: " << long_str.size() 
                  << ", capacity: " << long_str.capacity() << std::endl;
        std::cout << "Long string - std::string_view size: " << long_view.size() << std::endl;
    }
    
    REQUIRE(true);
}

TEST_CASE("Function Call Overhead Analysis") {
    std::cout << "=== Function Call Overhead Analysis ===" << std::endl;
    
    const int iterations = 1000000;
    nodec::optimized_ostringstream ss;
    ss << "Test string for overhead analysis";
    
    // str() 呼び出しオーバーヘッド
    {
        auto start = high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            auto result = ss.str();
            volatile auto len = result.size();
            (void)len;
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<nanoseconds>(end - start);
        std::cout << "str() call overhead: " << duration.count() / iterations << " ns per call" << std::endl;
    }
    
    // view() 呼び出しオーバーヘッド
    {
        auto start = high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            auto result = ss.view();
            volatile auto len = result.size();
            (void)len;
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<nanoseconds>(end - start);
        std::cout << "view() call overhead: " << duration.count() / iterations << " ns per call" << std::endl;
    }
    
    // 直接バッファアクセス（参考）
    {
        auto start = high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            auto buffer = ss.rdbuf()->str();
            volatile auto len = buffer.size();
            (void)len;
        }
        auto end = high_resolution_clock::now();  
        auto duration = duration_cast<nanoseconds>(end - start);
        std::cout << "Direct buffer access: " << duration.count() / iterations << " ns per call" << std::endl;
    }
    
    REQUIRE(true);
}
