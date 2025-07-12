#ifndef NODEC__FORMATTER_HPP_
#define NODEC__FORMATTER_HPP_

#include <nodec/macros.hpp>

#include <iomanip>
#include <locale>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <typeinfo>

namespace nodec {

class FormatterLegacy {
public:
    FormatterLegacy() noexcept {}
    ~FormatterLegacy() noexcept {}

    template<typename T>
    FormatterLegacy &operator<<(const T &value) noexcept {
        stream_ << value;
        return *this;
    }

    operator std::string() const noexcept {
        return stream_.str();
    }

private:
    std::ostringstream stream_;

    NODEC_DISABLE_COPY(FormatterLegacy)
};

template<typename CharT = char, typename Traits = std::char_traits<CharT>>
class BasicStringBuilderStream {
private:
    class BasicStringBuilderStreamBuf : public std::basic_streambuf<CharT, Traits> {
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

        BasicStringBuilderStreamBuf(string_type &dest): dest_(dest) {}

    protected:
        /**
         * @note This function is called when sputc()
         */
        int_type overflow(int_type c = Traits::eof()) override {
            if (Traits::eq_int_type(c, Traits::eof())) {
                return Traits::not_eof(c);
            }
            dest_.push_back(Traits::to_char_type(c));
            return c;
        }

        std::streamsize xsputn(const char_type *s, std::streamsize count) override {
            if (count <= 0) {
                return 0;
            }

            dest_.append(s, static_cast<size_type>(count));
            return count;
        }

    private:
        string_type &dest_;
    };

public:
    using string_type = std::basic_string<CharT, Traits>;
    using string_view_type = std::basic_string_view<CharT, Traits>;
    using size_type = typename string_type::size_type;
    using stringbuf_type = BasicStringBuilderStreamBuf;

    BasicStringBuilderStream(string_type &dest): base_stream_(&buffer_), buffer_(dest) {}

    string_type str() const {
        return buffer_.str();
    }

    BasicStringBuilderStream &operator<<(const char *str) {
        if (str == nullptr) {
            return *this;
        }

        // 文字列の長さを取得
        std::streamsize count = static_cast<std::streamsize>(Traits::length(str));

        // パディングの計算
        std::streamsize pad = base_stream_.width() <= 0 || base_stream_.width() <= count ? 0 : base_stream_.width() - count;

        // 左詰め以外の場合は左側にパディング
        if ((base_stream_.flags() & std::ios_base::adjustfield) != std::ios_base::left) {
            for (; 0 < pad; --pad) {
                buffer_.sputc(base_stream_.fill());
            }
        }

        // 文字列本体を出力
        buffer_.sputn(str, count);

        // 右側にパディング（左詰めの場合）
        for (; 0 < pad; --pad) {
            buffer_.sputc(base_stream_.fill());
        }

        // width をリセット
        base_stream_.width(0);

        return *this;
    }

    template<typename T>
    BasicStringBuilderStream &operator<<(const T &value) {
        base_stream_ << value;
        return *this;
    }

private:
    stringbuf_type buffer_;
    std::basic_ostream<CharT, Traits> base_stream_;
};

using StringBuilderStream = BasicStringBuilderStream<char>;

template<typename ErrorT>
class ErrorFormatter {
public:
    ErrorFormatter(const char *file, const size_t line) noexcept
        : type_{typeid(ErrorT).name()}, file_{file}, line_{line} {
        stream_ << type_ << ": ";
    }

    template<typename T>
    ErrorFormatter &operator<<(const T &value) noexcept {
        stream_ << value;
        return *this;
    }

    operator std::string() noexcept {
        stream_ << "\n"
                << std::dec
                << "[File] " << file_ << "\n"
                << "[Line] " << line_;
        return stream_.str();
    }

private:
    std::ostringstream stream_;

    const char *type_;
    const char *file_;
    const size_t line_;

    NODEC_DISABLE_COPY(ErrorFormatter)
};

} // namespace nodec

#endif