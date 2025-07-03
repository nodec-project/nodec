#ifndef NODEC__FORMATTER_HPP_
#define NODEC__FORMATTER_HPP_

#include <nodec/macros.hpp>

#include <iomanip>
#include <locale>
#include <sstream>
#include <type_traits>
#include <typeinfo>

namespace nodec {

class Formatter {
public:
    Formatter() noexcept {}
    ~Formatter() noexcept {}

    template<typename T>
    Formatter &operator<<(const T &value) noexcept {
        stream_ << value;
        return *this;
    }

    operator std::string() const noexcept {
        return stream_.str();
    }

private:
    std::ostringstream stream_;

    NODEC_DISABLE_COPY(Formatter)
};

// // カスタム操作子の定義
// struct SetWidth {
//     int width;
//     explicit SetWidth(int w) : width(w) {}
// };

// struct SetFill {
//     char fill_char;
//     explicit SetFill(char c) : fill_char(c) {}
// };

// struct SetPrecision {
//     int precision;
//     explicit SetPrecision(int p) : precision(p) {}
// };

// // カスタム操作子のヘルパー関数
// inline SetWidth setw(int width) { return SetWidth(width); }
// inline SetFill setfill(char fill_char) { return SetFill(fill_char); }
// inline SetPrecision setprecision(int precision) { return SetPrecision(precision); }

// /*
//  * The Text Formatter
//  *
//  * if you use C++20, consider to use std::format.
//  *   * <https://en.cppreference.com/w/cpp/utility/format>
//  *
//  * It is designed for easy use of text format even though under C++20.
//  * ostringstream compatible formatter with high performance.
//  *
//  * Implementation refs:
//  *   * <https://stackoverflow.com/questions/12261915/how-to-throw-stdexceptions-with-variable-messages>
//  *
//  */

// class Formatter {
// public:
//     enum class Alignment {
//         Left,
//         Right,
//         Internal
//     };

//     enum class FloatFormat {
//         Default,
//         Fixed,
//         Scientific
//     };

//     Formatter() noexcept = default;
//     ~Formatter() noexcept = default;

//     // 事前リザーブ機能付きコンストラクタ
//     explicit Formatter(size_t reserve_size) noexcept {
//         buffer_.reserve(reserve_size);
//     }

//     // 基本型の効率的な処理
//     Formatter &operator<<(const char* value) noexcept {
//         if (value) {
//             append_string_with_format(value);
//         }
//         return *this;
//     }

//     Formatter &operator<<(const std::string& value) noexcept {
//         append_string_with_format(value);
//         return *this;
//     }

//     Formatter &operator<<(char value) noexcept {
//         append_char_with_format(value);
//         return *this;
//     }

//     // 整数型の効率的な処理
//     Formatter &operator<<(int value) noexcept {
//         append_integer_with_format(value);
//         return *this;
//     }

//     Formatter &operator<<(long value) noexcept {
//         append_integer_with_format(value);
//         return *this;
//     }

//     Formatter &operator<<(long long value) noexcept {
//         append_integer_with_format(value);
//         return *this;
//     }

//     Formatter &operator<<(unsigned int value) noexcept {
//         append_unsigned_integer_with_format(value);
//         return *this;
//     }

//     Formatter &operator<<(unsigned long value) noexcept {
//         append_unsigned_integer_with_format(value);
//         return *this;
//     }

//     Formatter &operator<<(unsigned long long value) noexcept {
//         append_unsigned_integer_with_format(value);
//         return *this;
//     }

//     // 浮動小数点型の処理
//     Formatter &operator<<(float value) noexcept {
//         append_float_with_format(static_cast<double>(value));
//         return *this;
//     }

//     Formatter &operator<<(double value) noexcept {
//         append_float_with_format(value);
//         return *this;
//     }

//     // ostream操作子のサポート（hex, dec等）
//     Formatter &operator<<(std::ios_base& (*manipulator)(std::ios_base&)) noexcept {
//         if (manipulator == std::hex) {
//             base_mode_ = 16;
//         } else if (manipulator == std::dec) {
//             base_mode_ = 10;
//         } else if (manipulator == std::oct) {
//             base_mode_ = 8;
//         } else if (manipulator == std::fixed) {
//             float_mode_ = FloatMode::Fixed;
//         } else if (manipulator == std::scientific) {
//             float_mode_ = FloatMode::Scientific;
//         } else if (manipulator == std::uppercase) {
//             uppercase_ = true;
//         } else if (manipulator == std::nouppercase) {
//             uppercase_ = false;
//         } else if (manipulator == std::showbase) {
//             showbase_ = true;
//         } else if (manipulator == std::noshowbase) {
//             showbase_ = false;
//         } else if (manipulator == std::boolalpha) {
//             boolalpha_ = true;
//         } else if (manipulator == std::noboolalpha) {
//             boolalpha_ = false;
//         } else if (manipulator == std::left) {
//             align_mode_ = AlignMode::Left;
//         } else if (manipulator == std::right) {
//             align_mode_ = AlignMode::Right;
//         } else if (manipulator == std::internal) {
//             align_mode_ = AlignMode::Internal;
//         }
//         return *this;
//     }

//     // カスタム操作子のサポート
//     Formatter &operator<<(const SetWidth& sw) noexcept {
//         width_ = sw.width;
//         return *this;
//     }

//     Formatter &operator<<(const SetFill& sf) noexcept {
//         fill_char_ = sf.fill_char;
//         return *this;
//     }

//     Formatter &operator<<(const SetPrecision& sp) noexcept {
//         precision_ = sp.precision;
//         return *this;
//     }

//     // bool型の特別処理
//     Formatter &operator<<(bool value) noexcept {
//         if (boolalpha_) {
//             buffer_ += (value ? "true" : "false");
//         } else {
//             buffer_ += (value ? '1' : '0');
//         }
//         return *this;
//     }

//     // その他の型（フォールバック）- ただし、専用マニピュレータは除外
//     template<typename T>
//     typename std::enable_if<
//         !std::is_same<T, SetWidth>::value &&
//         !std::is_same<T, SetFill>::value &&
//         !std::is_same<T, SetPrecision>::value,
//         Formatter&
//     >::type operator<<(const T &value) noexcept {
//         // 複雑な型はostringstreamにフォールバック
//         std::ostringstream oss;
//         oss << value;
//         buffer_ += oss.str();
//         return *this;
//     }

//     operator std::string() const noexcept {
//         return buffer_;
//     }

//     // 追加のユーティリティメソッド
//     void reserve(size_t size) noexcept {
//         buffer_.reserve(size);
//     }

//     void clear() noexcept {
//         buffer_.clear();
//         // フォーマット状態をリセット
//         base_mode_ = 10;
//         float_mode_ = FloatMode::Default;
//         precision_ = 6;
//         width_ = 0;
//         fill_char_ = ' ';
//         uppercase_ = false;
//         showbase_ = false;
//         boolalpha_ = false;
//         align_mode_ = AlignMode::Right;
//     }

//     size_t size() const noexcept {
//         return buffer_.size();
//     }

//     const std::string& str() const noexcept {
//         return buffer_;
//     }

//     // マニピュレータ用のpublicメソッド
//     void apply_setw(int w) noexcept {
//         width_ = w;
//     }

//     void apply_setfill(char c) noexcept {
//         fill_char_ = c;
//     }

//     void apply_setprecision(int p) noexcept {
//         precision_ = p;
//     }

// private:
//     enum class FloatMode {
//         Default,
//         Fixed,
//         Scientific
//     };

//     enum class AlignMode {
//         Left,
//         Right,
//         Internal
//     };

//     std::string buffer_;

//     // フォーマット状態
//     int base_mode_ = 10;           // 10進数がデフォルト
//     FloatMode float_mode_ = FloatMode::Default;
//     int precision_ = 6;            // デフォルト精度
//     int width_ = 0;                // フィールド幅
//     char fill_char_ = ' ';         // 埋め文字
//     bool uppercase_ = false;       // 大文字表示
//     bool showbase_ = false;        // プレフィックス表示
//     bool boolalpha_ = false;       // bool値をtrue/falseで表示
//     AlignMode align_mode_ = AlignMode::Right;  // 右揃えがデフォルト

//     // マニピュレータ適用
//     template<typename T>
//     void apply_manipulator(const T& manipulator) noexcept {
//         // SFINAE を使ってsetw, setfill, setprecisionを識別
//         // 実際の実装では型特性を使用するが、簡略化のため条件分岐で対応
//         // この部分は実装が複雑になるため、後で詳細化
//     }

//     // 整数を効率的に文字列に変換
//     template<typename IntType>
//     void append_integer(IntType value) noexcept {
//         if (base_mode_ == 16) {
//             if (value < 0) {
//                 buffer_ += '-';
//                 append_hex_with_format(static_cast<unsigned long long>(-value));
//             } else {
//                 append_hex_with_format(static_cast<unsigned long long>(value));
//             }
//         } else if (base_mode_ == 8) {
//             if (value < 0) {
//                 buffer_ += '-';
//                 append_oct_with_format(static_cast<unsigned long long>(-value));
//             } else {
//                 append_oct_with_format(static_cast<unsigned long long>(value));
//             }
//         } else {
//             append_decimal_with_format(value);
//         }
//     }

//     template<typename UIntType>
//     void append_unsigned_integer(UIntType value) noexcept {
//         if (base_mode_ == 16) {
//             append_hex_with_format(value);
//         } else if (base_mode_ == 8) {
//             append_oct_with_format(value);
//         } else {
//             append_decimal_with_format(value);
//         }
//     }

//     // 10進数フォーマット
//     template<typename IntType>
//     void append_decimal_with_format(IntType value) noexcept {
//         std::string result;

//         if (value == 0) {
//             result = "0";
//         } else {
//             bool negative = value < 0;
//             if (negative) {
//                 value = -value;
//             }

//             char temp[32];
//             int pos = 0;
//             while (value > 0) {
//                 temp[pos++] = '0' + (value % 10);
//                 value /= 10;
//             }

//             if (negative) {
//                 result += '-';
//             }

//             for (int i = pos - 1; i >= 0; --i) {
//                 result += temp[i];
//             }
//         }

//         apply_field_formatting(result);
//     }

//     // 16進数フォーマット
//     void append_hex_with_format(unsigned long long value) noexcept {
//         std::string result;

//         if (showbase_ && value != 0) {
//             result += "0x";
//         }

//         if (value == 0) {
//             result += '0';
//         } else {
//             char temp[32];
//             int pos = 0;
//             const char* hex_chars = uppercase_ ? "0123456789ABCDEF" : "0123456789abcdef";

//             while (value > 0) {
//                 temp[pos++] = hex_chars[value % 16];
//                 value /= 16;
//             }

//             for (int i = pos - 1; i >= 0; --i) {
//                 result += temp[i];
//             }
//         }

//         apply_field_formatting(result);
//     }

//     // 8進数フォーマット
//     void append_oct_with_format(unsigned long long value) noexcept {
//         std::string result;

//         if (showbase_ && value != 0) {
//             result += '0';
//         }

//         if (value == 0) {
//             result += '0';
//         } else {
//             char temp[32];
//             int pos = 0;

//             while (value > 0) {
//                 temp[pos++] = '0' + (value % 8);
//                 value /= 8;
//             }

//             for (int i = pos - 1; i >= 0; --i) {
//                 result += temp[i];
//             }
//         }

//         apply_field_formatting(result);
//     }

//     // フィールドフォーマット適用
//     void apply_field_formatting(const std::string& content) noexcept {
//         if (width_ <= static_cast<int>(content.length())) {
//             // 幅指定なし、または内容が幅以上
//             buffer_ += content;
//             width_ = 0;  // 一度使用したらリセット
//             return;
//         }

//         int padding = width_ - static_cast<int>(content.length());

//         switch (align_mode_) {
//             case AlignMode::Left:
//                 buffer_ += content;
//                 buffer_.append(padding, fill_char_);
//                 break;

//             case AlignMode::Right:
//                 buffer_.append(padding, fill_char_);
//                 buffer_ += content;
//                 break;

//             case AlignMode::Internal:
//                 // 符号と数値の間に埋め文字を挿入
//                 if (!content.empty() && (content[0] == '-' || content[0] == '+')) {
//                     buffer_ += content[0];
//                     buffer_.append(padding, fill_char_);
//                     buffer_ += content.substr(1);
//                 } else if (content.length() >= 2 && content[0] == '0' && (content[1] == 'x' || content[1] == 'X')) {
//                     buffer_ += content.substr(0, 2);
//                     buffer_.append(padding, fill_char_);
//                     buffer_ += content.substr(2);
//                 } else {
//                     buffer_.append(padding, fill_char_);
//                     buffer_ += content;
//                 }
//                 break;
//         }

//         width_ = 0;  // 一度使用したらリセット
//     }

//     // フォーマット付きの文字列・数値処理メソッド
//     void append_string_with_format(const std::string& str) noexcept {
//         if (width_ > 0 && static_cast<int>(str.length()) < width_) {
//             apply_field_formatting(str);
//         } else {
//             buffer_ += str;
//         }
//         width_ = 0; // 使用後リセット
//     }

//     void append_char_with_format(char c) noexcept {
//         if (width_ > 0 && width_ > 1) {
//             std::string str(1, c);
//             apply_field_formatting(str);
//         } else {
//             buffer_ += c;
//         }
//         width_ = 0; // 使用後リセット
//     }

//     void append_integer_with_format(long long value) noexcept {
//         std::string num_str = format_integer(value);
//         if (width_ > 0 && static_cast<int>(num_str.length()) < width_) {
//             apply_field_formatting(num_str);
//         } else {
//             buffer_ += num_str;
//         }
//         width_ = 0; // 使用後リセット
//     }

//     void append_unsigned_integer_with_format(unsigned long long value) noexcept {
//         std::string num_str = format_unsigned_integer(value);
//         if (width_ > 0 && static_cast<int>(num_str.length()) < width_) {
//             apply_field_formatting(num_str);
//         } else {
//             buffer_ += num_str;
//         }
//         width_ = 0; // 使用後リセット
//     }

//     void append_float_with_format(double value) noexcept {
//         std::ostringstream oss;
//         oss.precision(precision_);

//         // フロートモード設定
//         switch (float_mode_) {
//             case FloatMode::Fixed:
//                 oss << std::fixed;
//                 break;
//             case FloatMode::Scientific:
//                 oss << std::scientific;
//                 break;
//             case FloatMode::Default:
//                 // デフォルトフォーマット
//                 break;
//         }

//         // 大文字設定
//         if (uppercase_) {
//             oss << std::uppercase;
//         }

//         oss << value;
//         std::string result = oss.str();

//         if (width_ > 0 && static_cast<int>(result.length()) < width_) {
//             apply_field_formatting(result);
//         } else {
//             buffer_ += result;
//         }
//         width_ = 0; // 使用後リセット
//     }

//     // 数値フォーマット関数
//     std::string format_integer(long long value) noexcept {
//         if (base_mode_ == 16) {
//             return format_hex_signed(value);
//         } else if (base_mode_ == 8) {
//             return format_oct_signed(value);
//         } else {
//             return format_decimal(value);
//         }
//     }

//     std::string format_unsigned_integer(unsigned long long value) noexcept {
//         if (base_mode_ == 16) {
//             return format_hex(value);
//         } else if (base_mode_ == 8) {
//             return format_oct(value);
//         } else {
//             return format_unsigned_decimal(value);
//         }
//     }

//     std::string format_hex_signed(long long value) noexcept {
//         if (value < 0) {
//             return "-" + format_hex(static_cast<unsigned long long>(-value));
//         } else {
//             return format_hex(static_cast<unsigned long long>(value));
//         }
//     }

//     std::string format_oct_signed(long long value) noexcept {
//         if (value < 0) {
//             return "-" + format_oct(static_cast<unsigned long long>(-value));
//         } else {
//             return format_oct(static_cast<unsigned long long>(value));
//         }
//     }

//     std::string format_decimal(long long value) noexcept {
//         if (value == 0) return "0";

//         std::string result;
//         bool negative = value < 0;
//         if (negative) value = -value;

//         while (value > 0) {
//             result = char('0' + value % 10) + result;
//             value /= 10;
//         }

//         if (negative) result = "-" + result;
//         return result;
//     }

//     std::string format_unsigned_decimal(unsigned long long value) noexcept {
//         if (value == 0) return "0";

//         std::string result;
//         while (value > 0) {
//             result = char('0' + value % 10) + result;
//             value /= 10;
//         }
//         return result;
//     }

//     std::string format_hex(unsigned long long value) noexcept {
//         if (value == 0) {
//             std::string result = "0";
//             if (showbase_) result = "0x" + result;
//             return result;
//         }

//         std::string result;
//         const char* hex_chars = uppercase_ ? "0123456789ABCDEF" : "0123456789abcdef";

//         while (value > 0) {
//             result = hex_chars[value % 16] + result;
//             value /= 16;
//         }

//         if (showbase_) {
//             result = (uppercase_ ? "0X" : "0x") + result;
//         }

//         return result;
//     }

//     std::string format_oct(unsigned long long value) noexcept {
//         if (value == 0) {
//             std::string result = "0";
//             if (showbase_) result = "0" + result;
//             return result;
//         }

//         std::string result;
//         while (value > 0) {
//             result = char('0' + value % 8) + result;
//             value /= 8;
//         }

//         if (showbase_) {
//             result = "0" + result;
//         }

//         return result;
//     }

//     void append_float(double value) noexcept {
//         std::ostringstream oss;

//         // 精度設定
//         oss.precision(precision_);

//         // フロートモード設定
//         switch (float_mode_) {
//             case FloatMode::Fixed:
//                 oss << std::fixed;
//                 break;
//             case FloatMode::Scientific:
//                 oss << std::scientific;
//                 break;
//             case FloatMode::Default:
//                 // デフォルトフォーマット
//                 break;
//         }

//         // 大文字設定
//         if (uppercase_) {
//             oss << std::uppercase;
//         }

//         oss << value;
//         std::string result = oss.str();
//         apply_field_formatting(result);
//     }

//     NODEC_DISABLE_COPY(Formatter)
// };

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