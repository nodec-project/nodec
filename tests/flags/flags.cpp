#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <algorithm>
#include <vector>

#include <nodec/flags.hpp>
#include <nodec/flags_iterator.hpp>

enum class FlagsWithRegistered {
    None = 0,
    FlagA = 0x01,
    FlagB = 0x02,
    FlagC = 0x04,
};

NODEC_FLAGS_ENABLE(FlagsWithRegistered)

enum class NotFlags {
    None = 0,
    FlagA = 0x01,
    FlagB = 0x02,
    FlagC = 0x04,
};

enum class FlagsWithAutoDetected {
    None = 0,
    FlagA = 0x01,
    FlagB = 0x02,
    FlagC = 0x04,
    _nodec_flags_enable
};

enum class FlagsWithInt : int {
    None = 0,
    FlagA = 0x01,
    FlagB = 0x02,
    FlagC = 0x04,
    _nodec_flags_enable
};

TEST_CASE("Testing to enable flags") {
    static_assert(nodec::is_flags<FlagsWithRegistered>::value, "Failed");
    static_assert(!nodec::is_flags<NotFlags>::value, "Failed");
    static_assert(nodec::is_flags<FlagsWithAutoDetected>::value, "Failed");
}

TEST_CASE("Testing traits") {
    using namespace nodec;

    static_assert(std::is_same<Flags<FlagsWithInt>::enum_type, FlagsWithInt>::value, "Failed");
    static_assert(std::is_same<Flags<FlagsWithInt>::underlying_type, int>::value, "Failed");
}

TEST_CASE("Testing bitwise operations") {
    using namespace nodec;

    SUBCASE("flags = enum | enum") {
        auto flags = FlagsWithRegistered::FlagA | FlagsWithRegistered::FlagB;

        CHECK_EQ(flags.underlying_value(), 0x03);
    }

    SUBCASE("bool = flags & enum") {
        auto flags = FlagsWithRegistered::FlagA | FlagsWithRegistered::FlagB;

        CHECK_EQ(static_cast<bool>(flags & FlagsWithRegistered::FlagA), true);
        CHECK_EQ(static_cast<bool>(flags & FlagsWithRegistered::FlagC), false);
    }
}

TEST_CASE("Testing FlagsIterator") {
    using namespace nodec;

    Flags<FlagsWithAutoDetected> flags(FlagsWithAutoDetected::FlagA | FlagsWithAutoDetected::FlagC);

    std::vector<FlagsWithAutoDetected> result;
    std::copy(flags.begin(), flags.end(), std::back_inserter(result));

    CHECK_EQ(result.size(), 2);
    CHECK_EQ(result[0], FlagsWithAutoDetected::FlagA);
    CHECK_EQ(result[1], FlagsWithAutoDetected::FlagC);
}

// #include <nodec/flags/flags.hpp>
// #include <nodec/logging.hpp>
// #include <nodec/formatter.hpp>

// enum class CustomFlags {
//     None = 0,
//     FlagA = 0x01,
//     FlagB = 0x02,
//     FlagC = 0x04,
// };

// std::ostream& operator<<(std::ostream& stream, const CustomFlags& flags) {
//     switch (flags) {
//     case CustomFlags::None:
//         return stream << "None";
//     case CustomFlags::FlagA:
//         return stream << "FlagA";
//     case CustomFlags::FlagB:
//         return stream << "FlagB";
//     case CustomFlags::FlagC:
//         return stream << "FlagC";
//     }
//     return stream << "Unknown";
// }

// template <class E>
// auto print(std::ostream& stream, const E& e)
//     -> typename std::enable_if<
//     nodec::is_printable<E>::value, std::ostream&
//     >::type {
//     return stream << "printable. " << e;
// }

// template <class E>
// auto print(std::ostream& stream, const E& e)
// -> typename std::enable_if<
//     !nodec::is_printable<E>::value, std::ostream&
// >::type {
//     return stream << "not printable. ";
// }

// NODEC_ALLOW_FLAGS_FOR_ENUM(CustomFlags)

// using namespace nodec;

// int main() {
//     logging::record_handlers().connect(logging::record_to_stdout_handler);

//     //nodec::flags::Flags<CustomFlags> flag;
//     auto flags = CustomFlags::FlagA | CustomFlags::FlagC;

//     logging::InfoStream(__FILE__, __LINE__) << flags;

//     //auto iter = mask1.begin();
//     //for (auto iter = flags.begin(); iter != flags.end(); ++iter) {
//     //    nodec::logging::InfoStream(__FILE__, __LINE__) << *iter;
//     //}

//     //for (auto flag : flags) {
//     //    nodec::logging::InfoStream(__FILE__, __LINE__) << flag;
//     //}

//     //std::ostringstream oss;
//     ////print(oss, CustomFlags::FlagA);
//     ////print(oss, mask1);
//     //oss << flags;
//     //logging::InfoStream(__FILE__, __LINE__) << oss.str();
//     std::string str = (Formatter() << flags);
//     logging::InfoStream(__FILE__, __LINE__) << str;

//     logging::InfoStream(__FILE__, __LINE__) << std::boolalpha << (bool)(flags & CustomFlags::FlagB);

//     flags |= CustomFlags::FlagB;
//     flags &= ~CustomFlags::FlagA;

//     logging::InfoStream(__FILE__, __LINE__) << flags;

//     logging::InfoStream(__FILE__, __LINE__) << std::boolalpha << (bool)(flags & CustomFlags::FlagB);

//     //mask1 = CustomFlags::None;
//     //std::vector<int> vec;

//     return 0;
// }