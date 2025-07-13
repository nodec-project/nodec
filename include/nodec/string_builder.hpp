#ifndef NODEC__STRING_BUILDER_HPP_
#define NODEC__STRING_BUILDER_HPP_

#include <string>

namespace nodec {

template<typename CharT = char, typename Traits = std::char_traits<CharT>>
class BasicStringBuilder {
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

    BasicStringBuilder(string_type &dest): base_stream_(&buffer_), buffer_(dest) {}

    string_type str() const {
        return buffer_.str();
    }

    BasicStringBuilder &operator<<(const string_view_type str) {
        using size_type = typename string_view_type::size_type;

        const size_type str_size = str.size();
        size_type pad;

        if (base_stream_.width() <= 0 || static_cast<size_type>(base_stream_.width()) <= str_size) {
            pad = 0;
        } else {
            pad = static_cast<size_type>(base_stream_.width()) - str_size;
        }

        // 左詰め以外の場合は左側にパディング
        if ((base_stream_.flags() & std::ios_base::adjustfield) != std::ios_base::left) {
            for (; 0 < pad; --pad) {
                buffer_.sputc(base_stream_.fill());
            }
        }

        // 文字列本体を出力
        buffer_.sputn(str.data(), static_cast<std::streamsize>(str_size));

        // 右側にパディング（左詰めの場合）
        for (; 0 < pad; --pad) {
            buffer_.sputc(base_stream_.fill());
        }

        // width をリセット
        base_stream_.width(0);

        return *this;
    }

    /**
     * @brief Inserts a null-terminated character string into the stream
     *
     * This operator performs formatted output of a C-style string with support for
     * width, fill character, and alignment manipulators (setw, setfill, left, right, internal).
     * The behavior is consistent with std::ostream::operator<<(const char*).
     *
     * @param str Pointer to a null-terminated character string
     * @return Reference to this BasicStringBuilder object for method chaining
     *
     * @pre str must be a valid pointer to a null-terminated string (str != nullptr)
     * @warning Passing nullptr results in undefined behavior
     *
     * @note The width setting is automatically reset to 0 after the operation,
     *       following standard stream behavior
     *
     * @example
     * @code
     * std::string buffer;
     * StringBuilder builder(buffer);
     * builder << std::setw(10) << std::setfill('*') << "Hello";
     * // buffer contains "*****Hello"
     * @endcode
     */
    BasicStringBuilder &operator<<(const char *str) {
        std::streamsize count = static_cast<std::streamsize>(Traits::length(str));

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
    BasicStringBuilder &operator<<(const T &value) {
        base_stream_ << value;
        return *this;
    }

private:
    stringbuf_type buffer_;
    std::basic_ostream<CharT, Traits> base_stream_;
};

using StringBuilder = BasicStringBuilder<char>;

} // namespace nodec

#endif