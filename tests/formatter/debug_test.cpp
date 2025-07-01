#include <nodec/optimized_ostringstream.hpp>
#include <iostream>

int main() {
    // デバッグ用テスト
    std::cout << "Creating ostringstream with reserve 1024..." << std::endl;
    nodec::optimized_ostringstream oss(1024);
    
    std::cout << "Initial capacity: " << oss.capacity() << std::endl;
    std::cout << "Initial size: " << oss.size() << std::endl;
    
    // 手動でリザーブを試行
    std::cout << "Calling reserve(2048)..." << std::endl;
    oss.reserve(2048);
    std::cout << "After manual reserve capacity: " << oss.capacity() << std::endl;
    
    oss << "Test";
    
    std::cout << "After write capacity: " << oss.capacity() << std::endl;
    std::cout << "After write size: " << oss.size() << std::endl;
    std::cout << "Content: " << oss.str() << std::endl;
    
    // 標準のstd::stringでの比較
    std::string test_string;
    test_string.reserve(1024);
    std::cout << "Standard string reserve(1024) capacity: " << test_string.capacity() << std::endl;
    
    return 0;
}
