#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <nodec/optimized_ostringstream.hpp>
#include <iostream>
#include <sstream>
#include <chrono>

// EOFチェックを省略したバージョン
template<typename CharT = char, typename Traits = std::char_traits<CharT>>
class no_eof_check_stringbuf : public std::basic_streambuf<CharT, Traits> {
public:
    using base_type = std::basic_streambuf<CharT, Traits>;
    using string_type = std::basic_string<CharT, Traits>;
    using size_type = typename string_type::size_type;
    using int_type = typename Traits::int_type;
    using char_type = CharT;

    // EOFチェックを完全に省略したoverflow()
    int_type overflow(int_type c = Traits::eof()) override {
        // 警告：EOFチェックなし！
        if (buffer_.size() < buffer_.capacity()) {
            buffer_.push_back(Traits::to_char_type(c));
        } else {
            buffer_.reserve(buffer_.capacity() == 0 ? 16 : buffer_.capacity() * 2);
            buffer_.push_back(Traits::to_char_type(c));
        }
        return c;
    }

    std::streamsize xsputn(const char_type *s, std::streamsize count) override {
        if (count <= 0) return 0;
        size_type needed = buffer_.size() + static_cast<size_type>(count);
        if (needed > buffer_.capacity()) {
            buffer_.reserve(needed);
        }
        buffer_.append(s, static_cast<size_type>(count));
        return count;
    }

    string_type str() const { return buffer_; }
    void clear() { buffer_.clear(); }

private:
    string_type buffer_;
};

// EOFチェックありとなしの統計を取るバージョン
template<typename CharT = char, typename Traits = std::char_traits<CharT>>
class statistics_stringbuf : public std::basic_streambuf<CharT, Traits> {
public:
    using base_type = std::basic_streambuf<CharT, Traits>;
    using string_type = std::basic_string<CharT, Traits>;
    using size_type = typename string_type::size_type;
    using int_type = typename Traits::int_type;
    using char_type = CharT;

    int_type overflow(int_type c = Traits::eof()) override {
        overflow_calls_++;
        
        if (!Traits::eq_int_type(c, Traits::eof())) {
            valid_char_calls_++;
            if (buffer_.size() < buffer_.capacity()) {
                buffer_.push_back(Traits::to_char_type(c));
            } else {
                buffer_.reserve(buffer_.capacity() == 0 ? 16 : buffer_.capacity() * 2);
                buffer_.push_back(Traits::to_char_type(c));
            }
            return c;
        } else {
            eof_calls_++;
            return Traits::not_eof(c);
        }
    }

    std::streamsize xsputn(const char_type *s, std::streamsize count) override {
        xsputn_calls_++;
        if (count <= 0) return 0;
        size_type needed = buffer_.size() + static_cast<size_type>(count);
        if (needed > buffer_.capacity()) {
            buffer_.reserve(needed);
        }
        buffer_.append(s, static_cast<size_type>(count));
        return count;
    }

    string_type str() const { return buffer_; }
    
    void print_statistics() const {
        std::cout << "=== 統計情報 ===" << std::endl;
        std::cout << "overflow() 総呼び出し回数: " << overflow_calls_ << std::endl;
        std::cout << "  - 有効な文字: " << valid_char_calls_ << std::endl;
        std::cout << "  - EOF: " << eof_calls_ << std::endl;
        std::cout << "xsputn() 呼び出し回数: " << xsputn_calls_ << std::endl;
    }
    
    void reset_statistics() {
        overflow_calls_ = valid_char_calls_ = eof_calls_ = xsputn_calls_ = 0;
    }

private:
    string_type buffer_;
    mutable size_t overflow_calls_ = 0;
    mutable size_t valid_char_calls_ = 0;
    mutable size_t eof_calls_ = 0;
    mutable size_t xsputn_calls_ = 0;
};

TEST_CASE("EOFチェックの必要性を検証") {
    std::cout << "\n=== EOFチェックの必要性検証 ===" << std::endl;
    
    statistics_stringbuf<char> stats_buf;
    std::ostream stats_stream(&stats_buf);
    
    std::cout << "\n1. 通常の文字列操作テスト:" << std::endl;
    stats_buf.reset_statistics();
    
    stats_stream << "Hello";
    stats_stream << " ";
    stats_stream << "World";
    stats_stream << 123;
    stats_stream << 3.14;
    
    stats_buf.print_statistics();
    std::cout << "結果: \"" << stats_buf.str() << "\"" << std::endl;
    
    std::cout << "\n2. 大量文字処理テスト:" << std::endl;
    stats_buf.reset_statistics();
    
    for (int i = 0; i < 1000; ++i) {
        stats_stream << static_cast<char>('A' + (i % 26));
    }
    
    stats_buf.print_statistics();
    
    std::cout << "\n3. 混合処理テスト:" << std::endl;
    stats_buf.reset_statistics();
    
    for (int i = 0; i < 100; ++i) {
        stats_stream << "Item" << i << ": " << (i * 1.5) << " ";
    }
    
    stats_buf.print_statistics();
}

TEST_CASE("EOFチェックなしでの動作確認") {
    std::cout << "\n=== EOFチェック省略版の動作確認 ===" << std::endl;
    
    try {
        no_eof_check_stringbuf<char> no_check_buf;
        std::ostream no_check_stream(&no_check_buf);
        
        std::cout << "EOFチェックなしでの書き込みテスト:" << std::endl;
        
        no_check_stream << "Test";
        no_check_stream << 123;
        no_check_stream << " works!";
        
        std::cout << "結果: \"" << no_check_buf.str() << "\"" << std::endl;
        std::cout << "→ 正常に動作（EOFは渡されていない）" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "例外発生: " << e.what() << std::endl;
    }
}

TEST_CASE("手動でEOFを渡した場合の動作") {
    std::cout << "\n=== 手動EOF送信テスト ===" << std::endl;
    
    statistics_stringbuf<char> stats_buf;
    
    std::cout << "1. EOFチェックありの場合:" << std::endl;
    auto eof_val = std::char_traits<char>::eof();
    auto result_with_check = stats_buf.overflow(eof_val);
    stats_buf.print_statistics();
    std::cout << "EOF処理結果: " << result_with_check << std::endl;
    
    std::cout << "\n2. 通常文字との比較:" << std::endl;
    stats_buf.reset_statistics();
    auto normal_result = stats_buf.overflow(std::char_traits<char>::to_int_type('A'));
    stats_buf.print_statistics();
    std::cout << "通常文字処理結果: " << normal_result << std::endl;
    std::cout << "バッファ内容: \"" << stats_buf.str() << "\"" << std::endl;
}

TEST_CASE("C++標準準拠とパフォーマンスの比較") {
    std::cout << "\n=== 標準準拠 vs パフォーマンス ===" << std::endl;
    
    const int iterations = 100000;
    
    // EOFチェックありのバージョン
    {
        auto start = std::chrono::high_resolution_clock::now();
        
        nodec::optimized_ostringstream oss_with_check;
        for (int i = 0; i < iterations; ++i) {
            oss_with_check << static_cast<char>('A' + (i % 26));
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "EOFチェックあり: " << duration.count() << " μs" << std::endl;
        std::cout << "結果長: " << oss_with_check.size() << std::endl;
    }
    
    // EOFチェックなしのバージョン
    {
        auto start = std::chrono::high_resolution_clock::now();
        
        no_eof_check_stringbuf<char> buf_no_check;
        std::ostream stream_no_check(&buf_no_check);
        
        for (int i = 0; i < iterations; ++i) {
            stream_no_check << static_cast<char>('A' + (i % 26));
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "EOFチェックなし: " << duration.count() << " μs" << std::endl;
        std::cout << "結果長: " << buf_no_check.str().size() << std::endl;
    }
}

TEST_CASE("実用的な推奨事項") {
    std::cout << "\n=== 実用的な推奨事項 ===" << std::endl;
    
    std::cout << "1. C++標準への準拠:" << std::endl;
    std::cout << "   - overflow()の標準的な実装ではEOFチェックが期待される" << std::endl;
    std::cout << "   - 他のstreambuf実装との互換性確保" << std::endl;
    
    std::cout << "\n2. 防御的プログラミング:" << std::endl;
    std::cout << "   - 予期しないEOF値から保護" << std::endl;
    std::cout << "   - 将来の仕様変更への対応" << std::endl;
    
    std::cout << "\n3. パフォーマンスへの影響:" << std::endl;
    std::cout << "   - 1回の比較操作による影響は微小" << std::endl;
    std::cout << "   - 現代のCPUでは分岐予測により最適化される" << std::endl;
    
    std::cout << "\n4. 推奨:" << std::endl;
    std::cout << "   EOFチェックは『保持する』ことを推奨" << std::endl;
    std::cout << "   理由: 安全性 > 微小なパフォーマンス差" << std::endl;
    
    REQUIRE(true);
}
