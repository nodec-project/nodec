#include <iostream>
#include <sstream>
#include <iomanip>

int main() {
    std::ostringstream oss;
    oss << std::hex << std::uppercase << std::showbase << std::setfill('0') << std::setw(8) << 255;
    std::cout << "ostringstream result: " << oss.str() << std::endl;
    return 0;
}
