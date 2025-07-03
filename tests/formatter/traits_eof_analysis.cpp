#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <nodec/optimized_ostringstream.hpp>
#include <iostream>
#include <type_traits>

// Traitsの動作を詳しく調べるテスト
TEST_CASE("Traits::eq_int_type と EOF 処理の詳細解説") {
    std::cout << "\n=== Traits と EOF 処理の詳細分析 ===" << std::endl;
    
    using Traits = std::char_traits<char>;
    using int_type = Traits::int_type;
    
    // EOFの値を確認
    int_type eof_value = Traits::eof();
    std::cout << "EOF値: " << eof_value << std::endl;
    std::cout << "EOF値 (16進): 0x" << std::hex << eof_value << std::dec << std::endl;
    
    // 通常の文字値との比較
    char normal_char = 'A';
    int_type normal_int_type = Traits::to_int_type(normal_char);
    
    std::cout << "\n--- 通常文字の処理 ---" << std::endl;
    std::cout << "文字 'A' の int_type値: " << normal_int_type << std::endl;
    std::cout << "文字 'A' (16進): 0x" << std::hex << normal_int_type << std::dec << std::endl;
    
    // eq_int_type による比較
    bool is_eof_normal = Traits::eq_int_type(normal_int_type, eof_value);
    bool is_not_eof_normal = !Traits::eq_int_type(normal_int_type, eof_value);
    
    std::cout << "Traits::eq_int_type('A', EOF): " << std::boolalpha << is_eof_normal << std::endl;
    std::cout << "!Traits::eq_int_type('A', EOF): " << std::boolalpha << is_not_eof_normal << std::endl;
    
    // EOFとの比較
    bool is_eof_eof = Traits::eq_int_type(eof_value, eof_value);
    bool is_not_eof_eof = !Traits::eq_int_type(eof_value, eof_value);
    
    std::cout << "\n--- EOF値の処理 ---" << std::endl;
    std::cout << "Traits::eq_int_type(EOF, EOF): " << std::boolalpha << is_eof_eof << std::endl;
    std::cout << "!Traits::eq_int_type(EOF, EOF): " << std::boolalpha << is_not_eof_eof << std::endl;
    
    // 実際の overflow() 内での処理シミュレーション
    std::cout << "\n--- overflow() 内での処理シミュレーション ---" << std::endl;
    
    auto simulate_overflow = [](int_type c) {
        std::cout << "入力値: " << c << " (0x" << std::hex << c << std::dec << ")" << std::endl;
        
        if (!Traits::eq_int_type(c, Traits::eof())) {
            std::cout << "→ 有効な文字として処理される" << std::endl;
            char actual_char = Traits::to_char_type(c);
            std::cout << "→ 変換された文字: '" << actual_char << "'" << std::endl;
            return c;  // 成功
        } else {
            std::cout << "→ EOF として認識され、処理をスキップ" << std::endl;
            return Traits::not_eof(c);  // EOFでない値を返す
        }
    };
    
    std::cout << "\n1. 通常文字 'B' の場合:" << std::endl;
    simulate_overflow(Traits::to_int_type('B'));
    
    std::cout << "\n2. EOF値の場合:" << std::endl;
    simulate_overflow(Traits::eof());
    
    std::cout << "\n3. 数値 255 (0xFF) の場合:" << std::endl;
    simulate_overflow(255);
    
    REQUIRE(true);
}

TEST_CASE("型安全性の重要性") {
    std::cout << "\n=== 型安全性の重要性 ===" << std::endl;
    
    using Traits = std::char_traits<char>;
    
    // 危険な比較 vs 安全な比較
    int_type c = Traits::to_int_type('A');
    int_type eof_val = Traits::eof();
    
    std::cout << "文字 'A' の値: " << c << std::endl;
    std::cout << "EOF の値: " << eof_val << std::endl;
    
    // 直接比較（推奨されない）
    bool unsafe_comparison = (c == eof_val);
    std::cout << "直接比較 (c == eof): " << std::boolalpha << unsafe_comparison << std::endl;
    
    // 安全な比較（推奨）
    bool safe_comparison = Traits::eq_int_type(c, eof_val);
    std::cout << "安全な比較 Traits::eq_int_type(c, eof): " << std::boolalpha << safe_comparison << std::endl;
    
    // なぜ安全な比較が重要か
    std::cout << "\n--- 型変換の問題 ---" << std::endl;
    
    // 文字型の範囲
    std::cout << "char の最小値: " << static_cast<int>(std::numeric_limits<char>::min()) << std::endl;
    std::cout << "char の最大値: " << static_cast<int>(std::numeric_limits<char>::max()) << std::endl;
    std::cout << "int_type のサイズ: " << sizeof(Traits::int_type) << " bytes" << std::endl;
    std::cout << "char のサイズ: " << sizeof(char) << " bytes" << std::endl;
    
    REQUIRE(true);
}

TEST_CASE("実際のoverflow()動作確認") {
    std::cout << "\n=== 実際の overflow() 動作確認 ===" << std::endl;
    
    // カスタムデバッグstreambuf
    class debug_stringbuf : public nodec::basic_reserved_stringbuf<char> {
    public:
        using base_type = nodec::basic_reserved_stringbuf<char>;
        using int_type = std::char_traits<char>::int_type;
        
        int_type overflow(int_type c) override {
            std::cout << "overflow() called with: " << c;
            
            if (!std::char_traits<char>::eq_int_type(c, std::char_traits<char>::eof())) {
                std::cout << " → 有効な文字 '" << std::char_traits<char>::to_char_type(c) << "'" << std::endl;
                return base_type::overflow(c);
            } else {
                std::cout << " → EOF（処理スキップ）" << std::endl;
                return std::char_traits<char>::not_eof(c);
            }
        }
    };
    
    debug_stringbuf buf;
    std::ostream stream(&buf);
    
    std::cout << "文字 'X' を書き込み:" << std::endl;
    stream << 'X';
    
    std::cout << "文字 'Y' を書き込み:" << std::endl;
    stream << 'Y';
    
    std::cout << "結果: \"" << buf.str() << "\"" << std::endl;
    
    REQUIRE(true);
}
