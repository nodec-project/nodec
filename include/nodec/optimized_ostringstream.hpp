#ifndef NODEC__OPTIMIZED_OSTRINGSTREAM_HPP_
#define NODEC__OPTIMIZED_OSTRINGSTREAM_HPP_

#include <memory>
#include <nodec/macros.hpp>
#include <sstream>
#include <string>

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
    using size_type = typename string_type::size_type;
    using int_type = typename Traits::int_type;
    using char_type = CharT;
    using traits_type = Traits;
    using pos_type = typename Traits::pos_type;
    using off_type = typename Traits::off_type;

    // デフォルトコンストラクタ
    basic_reserved_stringbuf()
        : mode_(std::ios_base::out), buffer_(), reserved_capacity_(0) {
        init_put_area();
    }

    // 事前リザーブ付きコンストラクタ
    explicit basic_reserved_stringbuf(size_type reserve_size)
        : mode_(std::ios_base::out), buffer_(), reserved_capacity_(reserve_size) {
        buffer_.reserve(reserve_size);
        init_put_area();
    }

    // 初期文字列 + 事前リザーブ付きコンストラクタ
    basic_reserved_stringbuf(const string_type &initial_str, size_type reserve_size)
        : mode_(std::ios_base::out), buffer_(initial_str), reserved_capacity_(reserve_size) {
        ensure_capacity(std::max(reserve_size, initial_str.size()));
        init_put_area();
    }

    // ムーブコンストラクタ
    basic_reserved_stringbuf(basic_reserved_stringbuf &&other) noexcept
        : base_type(std::move(other)), mode_(other.mode_), buffer_(std::move(other.buffer_)),
          reserved_capacity_(other.reserved_capacity_) {
        init_put_area();
    }

    // ムーブ代入演算子
    basic_reserved_stringbuf &operator=(basic_reserved_stringbuf &&other) noexcept {
        if (this != &other) {
            base_type::operator=(std::move(other));
            mode_ = other.mode_;
            buffer_ = std::move(other.buffer_);
            reserved_capacity_ = other.reserved_capacity_;
            init_put_area();
        }
        return *this;
    }

    // デストラクタ
    ~basic_reserved_stringbuf() = default;

    // 文字列取得
    string_type str() const {
        return buffer_;
    }

    // 文字列設定
    void str(const string_type &s) {
        buffer_ = s;
        init_put_area();
    }

    void str(string_type &&s) {
        buffer_ = std::move(s);
        init_put_area();
    }

    // 事前リザーブ機能
    void reserve(size_type new_capacity) {
        reserved_capacity_ = new_capacity;
        ensure_capacity(new_capacity);
    }

    // 容量・サイズ情報（リザーブされた容量も考慮）
    size_type capacity() const {
        return std::max(buffer_.capacity(), reserved_capacity_);
    }

    size_type size() const {
        return buffer_.size();
    }

protected:
    // オーバーフロー処理（高度に最適化）
    int_type overflow(int_type c = Traits::eof()) override {
        if (!(mode_ & std::ios_base::out)) {
            return Traits::eof();
        }

        if (!Traits::eq_int_type(c, Traits::eof())) {
            // 文字を直接buffer_に追加
            try {
                buffer_.push_back(Traits::to_char_type(c));
                return c;
            } catch (...) {
                return Traits::eof();
            }
        }
        return Traits::not_eof(c);
    }

    // バルク書き込み最適化
    std::streamsize xsputn(const char_type *s, std::streamsize count) override {
        if (!(mode_ & std::ios_base::out) || count <= 0) {
            return 0;
        }

        try {
            buffer_.append(s, static_cast<size_type>(count));
            return count;
        } catch (...) {
            return 0;
        }
    }

    // 同期処理
    int sync() override {
        // 既にbuffer_に直接書き込んでいるので何もしない
        return 0;
    }

    // シーク処理
    pos_type seekoff(off_type off, std::ios_base::seekdir way,
                     std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override {
        if (!(mode_ & which)) {
            return pos_type(off_type(-1));
        }

        off_type newoff;
        switch (way) {
        case std::ios_base::beg:
            newoff = off;
            break;
        case std::ios_base::cur:
            newoff = static_cast<off_type>(buffer_.size()) + off;
            break;
        case std::ios_base::end:
            newoff = static_cast<off_type>(buffer_.size()) + off;
            break;
        default:
            return pos_type(off_type(-1));
        }

        if (newoff < 0 || static_cast<size_type>(newoff) > buffer_.size()) {
            return pos_type(off_type(-1));
        }

        buffer_.resize(static_cast<size_type>(newoff));
        return pos_type(newoff);
    }

    pos_type seekpos(pos_type pos, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override {
        return seekoff(off_type(pos), std::ios_base::beg, which);
    }

private:
    std::ios_base::openmode mode_;
    string_type buffer_;
    size_type reserved_capacity_;

    // 容量確保の内部メソッド（SSOを考慮）
    void ensure_capacity(size_type capacity) {
        if (capacity > 0) {
            // 実際の容量を確保（SSOを回避）
            if (capacity > 15) { // SSOの閾値を超える場合
                string_type temp(capacity, CharT{});
                temp.clear();
                if (temp.capacity() >= capacity) {
                    // 既存のコンテンツをコピー
                    temp = buffer_;
                    buffer_ = std::move(temp);
                }
            }
        }
    }

    // Put areaの初期化（ダミー - 実際の書き込みはoverflow/xsputnで処理）
    void init_put_area() {
        // streambufの要求を満たすためのダミー設定
        // 実際の書き込みはoverflowとxsputnで直接buffer_に行う
        char_type dummy{};
        base_type::setp(&dummy, &dummy);
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

    // clear機能（内容をクリアして再利用）
    void clear_content() {
        buffer_.str(string_type());
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
