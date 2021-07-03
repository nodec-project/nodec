#include <nodec/flags/flags.hpp>
#include <nodec/logging.hpp>
#include <nodec/formatter.hpp>

enum class CustomFlags {
    None = 0,
    FlagA = 0x01,
    FlagB = 0x02,
    FlagC = 0x04,
};


std::ostream& operator<<(std::ostream& stream, const CustomFlags& flags) {
    switch (flags) {
    case CustomFlags::None:
        return stream << "None";
    case CustomFlags::FlagA:
        return stream << "FlagA";
    case CustomFlags::FlagB:
        return stream << "FlagB";
    case CustomFlags::FlagC:
        return stream << "FlagC";
    }
    return stream << "Unknown";
}


template <class E>
auto print(std::ostream& stream, const E& e)
    -> typename std::enable_if<
    nodec::is_printable<E>::value, std::ostream&
    >::type {
    return stream << "printable. " << e;
}

template <class E>
auto print(std::ostream& stream, const E& e)
-> typename std::enable_if<
    !nodec::is_printable<E>::value, std::ostream&
>::type {
    return stream << "not printable. ";
}

NODEC_ALLOW_FLAGS_FOR_ENUM(CustomFlags)

using namespace nodec;

int main() {
    logging::record_handlers().connect(logging::record_to_stdout_handler);
    
    //nodec::flags::Flags<CustomFlags> flag;
    auto flags = CustomFlags::FlagA | CustomFlags::FlagC;
    
    logging::InfoStream(__FILE__, __LINE__) << flags;

    //auto iter = mask1.begin();
    //for (auto iter = flags.begin(); iter != flags.end(); ++iter) {
    //    nodec::logging::InfoStream(__FILE__, __LINE__) << *iter;
    //}

    //for (auto flag : flags) {
    //    nodec::logging::InfoStream(__FILE__, __LINE__) << flag;
    //}

    //std::ostringstream oss;
    ////print(oss, CustomFlags::FlagA);
    ////print(oss, mask1);
    //oss << flags;
    //logging::InfoStream(__FILE__, __LINE__) << oss.str();
    std::string str = (Formatter() << flags);
    logging::InfoStream(__FILE__, __LINE__) << str;

    logging::InfoStream(__FILE__, __LINE__) << std::boolalpha << (bool)(flags & CustomFlags::FlagB);


    flags |= CustomFlags::FlagB;
    flags &= ~CustomFlags::FlagA;

    logging::InfoStream(__FILE__, __LINE__) << flags;


    logging::InfoStream(__FILE__, __LINE__) << std::boolalpha << (bool)(flags & CustomFlags::FlagB);

    //mask1 = CustomFlags::None;
    //std::vector<int> vec;
    

    return 0;
}