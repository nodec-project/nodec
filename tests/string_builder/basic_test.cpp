#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec/string_builder.hpp>
#include <nodec/vector3.hpp>

TEST_CASE("Testing StringBuilder - Basic Types") {
    // 文字列連結
    {
        std::string result;
        nodec::StringBuilder(result) << "Hello" << " " << "World";
        CHECK(result == "Hello World");
    }

    // 整数
    {
        std::string result;
        nodec::StringBuilder(result) << "Number: " << 42;
        CHECK(result == "Number: 42");
    }

    // 浮動小数点
    {
        std::string result;
        nodec::StringBuilder(result) << "Float: " << 3.14;
        CHECK(result == "Float: 3.14");
    }

    // 文字
    {
        std::string result;
        nodec::StringBuilder(result) << 'A' << 'B' << 'C';
        CHECK(result == "ABC");
    }
}

// TEST_CASE("Testing Formatter - Numeric Types") {
//     // 様々な整数型
//     std::string result1 = nodec::Formatter() << static_cast<int>(123);
//     CHECK(result1 == "123");

//     std::string result2 = nodec::Formatter() << static_cast<long>(456L);
//     CHECK(result2 == "456");

//     std::string result3 = nodec::Formatter() << static_cast<long long>(789LL);
//     CHECK(result3 == "789");

//     // 負の数
//     std::string result4 = nodec::Formatter() << -42;
//     CHECK(result4 == "-42");

//     // ゼロ
//     std::string result5 = nodec::Formatter() << 0;
//     CHECK(result5 == "0");
// }

// TEST_CASE("Testing Formatter - Hex Mode") {
//     // 16進数モード
//     std::string result1 = nodec::Formatter() << "Hex: " << std::hex << 255;
//     CHECK(result1 == "Hex: ff");

//     // 10進数に戻す
//     std::string result2 = nodec::Formatter() << std::hex << 16 << ", " << std::dec << 16;
//     CHECK(result2 == "10, 16");

//     // 複数の16進数
//     std::string result3 = nodec::Formatter() << std::hex << 10 << " " << 15 << " " << 255;
//     CHECK(result3 == "a f ff");
// }

// // TEST_CASE("Testing Formatter - Reserve Functionality") {
// //     // 事前リザーブ
// //     nodec::Formatter formatter(100);
// //     formatter << "This is a test string that might be long";
// //     std::string result = formatter.str();
// //     CHECK(result == "This is a test string that might be long");

// //     // クリア機能
// //     formatter.clear();
// //     formatter << "New content";
// //     CHECK(formatter.str() == "New content");
// // }

// // TEST_CASE("Testing Formatter - Utility Methods") {
// //     nodec::Formatter formatter;
// //     formatter << "Hello";

// //     // サイズチェック
// //     CHECK(formatter.size() == 5);

// //     // 追加リザーブ
// //     formatter.reserve(100);
// //     formatter << " World";
// //     CHECK(formatter.str() == "Hello World");

// //     // クリア
// //     formatter.clear();
// //     CHECK(formatter.size() == 0);
// //     CHECK(formatter.str() == "");
// // }

// TEST_CASE("Testing Formatter - Vector3 Fallback") {
//     // Vector3の文字列化テスト（ostream演算子フォールバック）
//     nodec::Vector3f vec3f(1.5f, 2.7f, 3.9f);
//     std::string result1 = nodec::Formatter() << "Vector3f: " << vec3f;
//     CHECK(result1 == "Vector3f: ( 1.5, 2.7, 3.9 )");

//     // Vector3iの文字列化テスト
//     nodec::Vector3i vec3i(10, 20, 30);
//     std::string result2 = nodec::Formatter() << "Position: " << vec3i << ", End";
//     CHECK(result2 == "Position: ( 10, 20, 30 ), End");

//     // Vector3dの文字列化テスト
//     nodec::Vector3d vec3d(1.23456, 7.89012, 3.45678);
//     std::string result3 = nodec::Formatter() << vec3d;
//     CHECK(result3 == "( 1.23456, 7.89012, 3.45678 )");

//     // 複数のVector3を連結
//     nodec::Vector3f pos(1.0f, 2.0f, 3.0f);
//     nodec::Vector3f dir(0.0f, 1.0f, 0.0f);
//     std::string result4 = nodec::Formatter() << "Pos: " << pos << ", Dir: " << dir;
//     CHECK(result4 == "Pos: ( 1, 2, 3 ), Dir: ( 0, 1, 0 )");

//     // ostringstream との比較確認
//     std::ostringstream oss;
//     oss << "Vector3f: " << vec3f;
//     std::string oss_result = oss.str();
//     CHECK(result1 == oss_result);  // 同じ結果であることを確認
// }