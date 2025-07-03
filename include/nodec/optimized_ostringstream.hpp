#ifndef NODEC__OPTIMIZED_OSTRINGSTREAM_HPP_
#define NODEC__OPTIMIZED_OSTRINGSTREAM_HPP_

#include <sstream>
#include <string>
#include <string_view>

namespace nodec {

/**
 * 高度に最適化されたstreambuf - 確実な事前リザーブ機能付き
 * std::stringbufとは異なり、真の高性能を追求したシンプルな実装
 */
template<typename CharT = char, typename Traits = std::char_traits<CharT>>
class basic_reserved_stringbuf : public std::basic_streambuf<CharT, Traits> {
public:
    using base_type = std::basic_streambuf<CharT, Traits>;
    using string_type = std::basic_string<CharT, Traits>;
    using string_view_type = std::basic_string_view<CharT, Traits>;
    using size_type = typename string_type::size_type;
    using int_type = typename Traits::int_type;
    using char_type = CharT;
    using traits_type = Traits;
    using pos_type = typename Traits::pos_type;
    using off_type = typename Traits::off_type;

    // デフォルトコンストラクタ
    basic_reserved_stringbuf() = default;

    // 事前リザーブ付きコンストラクタ
    explicit basic_reserved_stringbuf(size_type reserve_size) {
        if (reserve_size > 0) {
            ensure_capacity(reserve_size);
        }
    }

    // 初期文字列 + 事前リザーブ付きコンストラクタ
    basic_reserved_stringbuf(const string_type &initial_str, size_type reserve_size)
        : buffer_(initial_str) {
        if (reserve_size > initial_str.size()) {
            ensure_capacity(reserve_size);
        }
    }

    // ムーブコンストラクタ
    basic_reserved_stringbuf(basic_reserved_stringbuf &&other) noexcept
        : base_type(std::move(other)), buffer_(std::move(other.buffer_)) {}

    // ムーブ代入演算子
    basic_reserved_stringbuf &operator=(basic_reserved_stringbuf &&other) noexcept {
        if (this != &other) {
            base_type::operator=(std::move(other));
            buffer_ = std::move(other.buffer_);
        }
        return *this;
    }

    // デストラクタ
    ~basic_reserved_stringbuf() = default;

    // 文字列取得
    string_type str() const {
        return buffer_;
    }

    // 文字列ビュー取得（コピーなし）
    string_view_type view() const {
        return string_view_type(buffer_.data(), buffer_.size());
    }

    // 文字列設定
    void str(const string_type &s) {
        buffer_ = s;
    }

    void str(string_type &&s) {
        buffer_ = std::move(s);
    }

    // 事前リザーブ機能
    void reserve(size_type new_capacity) {
        ensure_capacity(new_capacity);
    }

    // 内容をクリア（容量は保持）
    void clear() {
        buffer_.clear();
    }

    // 容量・サイズ情報
    size_type capacity() const {
        return buffer_.capacity();
    }

    size_type size() const {
        return buffer_.size();
    }

protected:
    // より効率的なオーバーフロー処理
    int_type overflow(int_type c = Traits::eof()) override {
        if (!Traits::eq_int_type(c, Traits::eof())) {
            // バッファに余裕があるかチェック
            if (buffer_.size() < buffer_.capacity()) {
                buffer_.push_back(Traits::to_char_type(c));
            } else {
                // 容量不足時は少し多めに確保
                buffer_.reserve(buffer_.capacity() == 0 ? 16 : buffer_.capacity() * 2);
                buffer_.push_back(Traits::to_char_type(c));
            }
            return c;
        }
        return Traits::not_eof(c);
    }

    // バルク書き込み最適化（より積極的な事前確保）
    std::streamsize xsputn(const char_type *s, std::streamsize count) override {
        if (count <= 0) {
            return 0;
        }
        
        // 必要な容量を事前に確保
        size_type needed = buffer_.size() + static_cast<size_type>(count);
        if (needed > buffer_.capacity()) {
            buffer_.reserve(needed);
        }
        
        buffer_.append(s, static_cast<size_type>(count));
        return count;
    }

    // 同期処理（何もしない - 直接書き込み済み）
    int sync() override {
        return 0;
    }

private:
    string_type buffer_;

    // 効率的な容量確保（シンプル版）
    void ensure_capacity(size_type capacity) {
        if (capacity > buffer_.capacity()) {
            buffer_.reserve(capacity);
        }
    }

    // コピーは禁止、ムーブのみ許可
    basic_reserved_stringbuf(const basic_reserved_stringbuf &) = delete;
    basic_reserved_stringbuf &operator=(const basic_reserved_stringbuf &) = delete;
};

using reserved_stringbuf = basic_reserved_stringbuf<char>;
using reserved_wstringbuf = basic_reserved_stringbuf<wchar_t>;

/**
 * 事前リザーブ機能付きostringstream
 * 完全なstd::ostringstreamとの互換性を保ちつつ、高性能を実現
 */
template<typename CharT = char, typename Traits = std::char_traits<CharT>>
class basic_optimized_ostringstream : public std::basic_ostream<CharT, Traits> {
public:
    using base_type = std::basic_ostream<CharT, Traits>;
    using string_type = std::basic_string<CharT, Traits>;
    using string_view_type = std::basic_string_view<CharT, Traits>;
    using size_type = typename string_type::size_type;
    using stringbuf_type = basic_reserved_stringbuf<CharT, Traits>;

    // デフォルトコンストラクタ
    basic_optimized_ostringstream()
        : base_type(&buffer_), buffer_() {}

    // 事前リザーブ付きコンストラクタ（曖昧さ回避のため型を明確化）
    explicit basic_optimized_ostringstream(size_type reserve_size)
        : base_type(&buffer_), buffer_(reserve_size) {}

    // 初期文字列付きコンストラクタ
    explicit basic_optimized_ostringstream(const string_type &initial_str)
        : base_type(&buffer_), buffer_(initial_str, 0) {}

    // 初期文字列 + 事前リザーブ付きコンストラクタ
    basic_optimized_ostringstream(const string_type &initial_str, size_type reserve_size)
        : base_type(&buffer_), buffer_(initial_str, reserve_size) {}

    // ムーブコンストラクタ
    basic_optimized_ostringstream(basic_optimized_ostringstream &&other) noexcept
        : base_type(std::move(other)), buffer_(std::move(other.buffer_)) {
        base_type::set_rdbuf(&buffer_);
    }

    // ムーブ代入演算子
    basic_optimized_ostringstream &operator=(basic_optimized_ostringstream &&other) noexcept {
        if (this != &other) {
            base_type::operator=(std::move(other));
            buffer_ = std::move(other.buffer_);
            base_type::set_rdbuf(&buffer_);
        }
        return *this;
    }

    // デストラクタ
    ~basic_optimized_ostringstream() = default;

    // rdbufアクセス
    stringbuf_type *rdbuf() const {
        return const_cast<stringbuf_type *>(&buffer_);
    }

    // 文字列取得
    string_type str() const {
        return buffer_.str();
    }

    // 文字列ビュー取得（コピーなし - 高性能）
    string_view_type view() const {
        return buffer_.view();
    }

    // 文字列設定
    void str(const string_type &s) {
        buffer_.str(s);
    }

    void str(string_type &&s) {
        buffer_.str(std::move(s));
    }

    // 事前リザーブ機能
    void reserve(size_type new_capacity) {
        buffer_.reserve(new_capacity);
    }

    // 容量・サイズ情報
    size_type capacity() const {
        return buffer_.capacity();
    }

    size_type size() const {
        return buffer_.size();
    }

    // clear機能（内容をクリアして再利用 - 容量は保持）
    void clear_content() {
        buffer_.clear(); // stringbufの内容をクリア（容量保持）
        base_type::clear(); // ストリームの状態もクリア
    }

    // 文字列変換演算子（利便性のため）
    operator string_type() const {
        return str();
    }

private:
    stringbuf_type buffer_;

    // コピーは禁止、ムーブのみ許可
    basic_optimized_ostringstream(const basic_optimized_ostringstream &) = delete;
    basic_optimized_ostringstream &operator=(const basic_optimized_ostringstream &) = delete;
};

// 型エイリアス
using optimized_ostringstream = basic_optimized_ostringstream<char>;
using optimized_wostringstream = basic_optimized_ostringstream<wchar_t>;

// 利便性のための名前空間エイリアス（既存のFormatterとの互換性）
using FastFormatter = optimized_ostringstream;

} // namespace nodec

#endif // NODEC__OPTIMIZED_OSTRINGSTREAM_HPP_
