#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <nodec/optimized_ostringstream.hpp>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

// デバッグ用のstreambuf - 関数呼び出しを追跡
template<typename CharT = char, typename Traits = std::char_traits<CharT>>
class debug_stringbuf : public nodec::basic_reserved_stringbuf<CharT, Traits> {
public:
    using base_type = nodec::basic_reserved_stringbuf<CharT, Traits>;
    using int_type = typename Traits::int_type;
    using char_type = CharT;
    using string_type = std::basic_string<CharT, Traits>;
    using size_type = typename string_type::size_type;

    debug_stringbuf() = default;
    explicit debug_stringbuf(size_type reserve_size) : base_type(reserve_size) {}

    // overflow()呼び出しを追跡
    int_type overflow(int_type c = Traits::eof()) override {
        std::cout << "📞 overflow() called with char: ";
        if (!Traits::eq_int_type(c, Traits::eof())) {
            char ch = Traits::to_char_type(c);
            if (ch >= 32 && ch <= 126) {  // 印刷可能文字
                std::cout << "'" << ch << "'";
            } else {
                std::cout << "(non-printable:" << static_cast<int>(ch) << ")";
            }
        } else {
            std::cout << "EOF";
        }
        std::cout << " | Buffer size: " << this->size() 
                  << " | Capacity: " << this->capacity() << std::endl;
        
        return base_type::overflow(c);
    }

    // xsputn()呼び出しを追跡
    std::streamsize xsputn(const char_type *s, std::streamsize count) override {
        std::cout << "📞 xsputn() called with " << count << " chars: \"";
        // 最初の20文字まで表示
        for (std::streamsize i = 0; i < std::min(count, std::streamsize(20)); ++i) {
            char ch = s[i];
            if (ch >= 32 && ch <= 126) {
                std::cout << ch;
            } else {
                std::cout << "\\x" << std::hex << static_cast<int>(ch) << std::dec;
            }
        }
        if (count > 20) std::cout << "...";
        std::cout << "\" | Buffer size before: " << this->size() 
                  << " | Capacity: " << this->capacity() << std::endl;
        
        auto result = base_type::xsputn(s, count);
        
        std::cout << "   → After xsputn: Buffer size: " << this->size() 
                  << " | Capacity: " << this->capacity() << std::endl;
        
        return result;
    }

    // sync()呼び出しを追跡
    int sync() override {
        std::cout << "📞 sync() called | Buffer size: " << this->size() << std::endl;
        return base_type::sync();
    }
};

// デバッグ用のostringstream
template<typename CharT = char, typename Traits = std::char_traits<CharT>>
class debug_ostringstream : public std::basic_ostream<CharT, Traits> {
public:
    using base_type = std::basic_ostream<CharT, Traits>;
    using stringbuf_type = debug_stringbuf<CharT, Traits>;
    using string_type = std::basic_string<CharT, Traits>;
    using size_type = typename string_type::size_type;

    debug_ostringstream() : base_type(&buffer_), buffer_() {}
    explicit debug_ostringstream(size_type reserve_size) 
        : base_type(&buffer_), buffer_(reserve_size) {}

    stringbuf_type* rdbuf() const {
        return const_cast<stringbuf_type*>(&buffer_);
    }

    string_type str() const {
        return buffer_.str();
    }

private:
    stringbuf_type buffer_;
};

TEST_CASE("overflow() と xsputn() の呼び出しタイミング解説") {
    std::cout << "\n=== overflow() と xsputn() 呼び出しタイミングの詳細解説 ===" << std::endl;
    
    std::cout << "\n【1. 単一文字の書き込み - overflow()が呼ばれる】" << std::endl;
    {
        debug_ostringstream dss;
        std::cout << "初期状態 - Buffer size: " << dss.rdbuf()->size() 
                  << ", Capacity: " << dss.rdbuf()->capacity() << std::endl;
        
        std::cout << "\n単一文字 'A' を書き込み:" << std::endl;
        dss << 'A';  // 単一文字 → overflow()呼び出し
        
        std::cout << "\n単一文字 'B' を書き込み:" << std::endl;
        dss << 'B';  // 単一文字 → overflow()呼び出し
    }
    
    std::cout << "\n【2. 文字列の書き込み - xsputn()が呼ばれる】" << std::endl;
    {
        debug_ostringstream dss;
        std::cout << "\n文字列 \"Hello\" を書き込み:" << std::endl;
        dss << "Hello";  // 文字列 → xsputn()呼び出し
        
        std::cout << "\n文字列 \" World\" を書き込み:" << std::endl;
        dss << " World";  // 文字列 → xsputn()呼び出し
    }
    
    std::cout << "\n【3. 数値の書き込み - 内部で文字列変換後にxsputn()】" << std::endl;
    {
        debug_ostringstream dss;
        std::cout << "\n整数 123 を書き込み:" << std::endl;
        dss << 123;  // 数値 → 内部でstr変換 → xsputn()呼び出し
        
        std::cout << "\n浮動小数点 3.14 を書き込み:" << std::endl;
        dss << 3.14;  // 浮動小数点 → 内部でstr変換 → xsputn()呼び出し
    }
    
    std::cout << "\n【4. マニピュレータとの組み合わせ】" << std::endl;
    {
        debug_ostringstream dss;
        std::cout << "\nstd::hex マニピュレータと数値:" << std::endl;
        dss << std::hex << 255;  // マニピュレータ設定後、数値変換 → xsputn()
        
        std::cout << "\nstd::setw(10) と文字列:" << std::endl;
        dss << std::setw(10) << "test";  // 幅設定後、文字列 → xsputn()
    }
    
    std::cout << "\n【5. バッファ容量不足時の動作】" << std::endl;
    {
        debug_ostringstream dss(5);  // 小さな初期容量
        std::cout << "\n初期容量5で開始、長い文字列を書き込み:" << std::endl;
        dss << "This is a very long string that exceeds initial capacity";
        // 容量不足 → xsputn()内でreserve()が呼ばれる
    }
    
    std::cout << "\n【6. flush() と sync() の関係】" << std::endl;
    {
        debug_ostringstream dss;
        dss << "Some content";
        std::cout << "\nflush() を明示的に呼び出し:" << std::endl;
        dss.flush();  // flush() → sync()呼び出し
    }
    
    REQUIRE(true);
}

TEST_CASE("std::ostringstreamとの比較") {
    std::cout << "\n=== std::ostringstream vs debug_ostringstream 動作比較 ===" << std::endl;
    
    std::cout << "\n【通常のstd::ostringstream】" << std::endl;
    {
        std::ostringstream oss;
        oss << "Hello" << " " << 123 << " " << 'X';
        std::cout << "結果: \"" << oss.str() << "\"" << std::endl;
        std::cout << "※内部関数呼び出しは見えない" << std::endl;
    }
    
    std::cout << "\n【debug_ostringstream（関数呼び出し可視化）】" << std::endl;
    {
        debug_ostringstream dss;
        dss << "Hello" << " " << 123 << " " << 'X';
        std::cout << "結果: \"" << dss.str() << "\"" << std::endl;
        std::cout << "※上記のように内部関数呼び出しが見える" << std::endl;
    }
    
    REQUIRE(true);
}

TEST_CASE("パフォーマンス影響の実演") {
    std::cout << "\n=== パフォーマンス影響の実演 ===" << std::endl;
    
    const int iterations = 1000;
    
    std::cout << "\n【文字を1個ずつ書き込み - overflow()多発】" << std::endl;
    {
        debug_ostringstream dss;
        std::cout << "最初の5回の文字書き込み:" << std::endl;
        for (int i = 0; i < 5; ++i) {
            dss << static_cast<char>('A' + i);
        }
        std::cout << "... (残り" << (iterations - 5) << "回は省略)" << std::endl;
        
        // 残りは静かに実行
        for (int i = 5; i < iterations; ++i) {
            debug_ostringstream quiet_dss;
            quiet_dss << static_cast<char>('A' + (i % 26));
        }
    }
    
    std::cout << "\n【文字列をまとめて書き込み - xsputn()効率的】" << std::endl;
    {
        debug_ostringstream dss;
        std::cout << "1回の文字列書き込み:" << std::endl;
        std::string long_str;
        for (int i = 0; i < iterations; ++i) {
            long_str += static_cast<char>('A' + (i % 26));
        }
        dss << long_str;  // 一度にまとめて書き込み
    }
    
    REQUIRE(true);
}
