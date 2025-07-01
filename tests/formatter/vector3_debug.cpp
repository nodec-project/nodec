#include <nodec/optimized_ostringstream.hpp>
#include <nodec/vector3.hpp>
#include <sstream>
#include <iostream>

int main() {
    nodec::Vector3f vec(1.0f, 2.0f, 3.0f);
    
    std::cout << "=== Vector3 Output Test ===" << std::endl;
    
    // optimized_ostringstream
    nodec::optimized_ostringstream oss;
    oss << "Vector: " << vec;
    std::cout << "optimized_ostringstream: '" << oss.str() << "'" << std::endl;
    
    // standard ostringstream
    std::ostringstream std_oss;
    std_oss << "Vector: " << vec;
    std::cout << "standard ostringstream: '" << std_oss.str() << "'" << std::endl;
    
    // 直接Vector3出力
    std::cout << "Direct output: " << vec << std::endl;
    
    return 0;
}
