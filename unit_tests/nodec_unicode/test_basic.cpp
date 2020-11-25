#include <string>
#include <iostream>

int main()
{

    std::string str = "あ";
    auto* begin = reinterpret_cast<const uint8_t*>(str.data());
    std::cout << str.size() << std::endl;
    std::cout << (int)begin[0] << std::endl;
    std::cout << (int)begin[1] << std::endl;
    std::cout << (int)begin[2] << std::endl;
    std::cout << (int)*begin << std::endl;
    uint32_t c = 0;
    c += *begin++;
    c <<= 6;
    std::cout << c << std::endl;

    return 0;
}