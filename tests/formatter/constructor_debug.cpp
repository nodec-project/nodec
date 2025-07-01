#include <nodec/optimized_ostringstream.hpp>
#include <iostream>

int main() {
    std::cout << "=== Constructor Debug Test ===" << std::endl;
    
    // Case 1: explicit size_type コンストラクタが呼ばれるべき
    std::cout << "Case 1: nodec::optimized_ostringstream oss(1024);" << std::endl;
    nodec::optimized_ostringstream oss1(1024);
    std::cout << "Capacity: " << oss1.capacity() << std::endl;
    
    // Case 2: size_typeを明示的にキャスト
    std::cout << "\nCase 2: nodec::optimized_ostringstream oss(static_cast<size_t>(1024));" << std::endl;
    nodec::optimized_ostringstream oss2(static_cast<std::size_t>(1024));
    std::cout << "Capacity: " << oss2.capacity() << std::endl;
    
    // Case 3: openmodeコンストラクタを明示的に呼ぶ
    std::cout << "\nCase 3: nodec::optimized_ostringstream oss(std::ios_base::out);" << std::endl;
    nodec::optimized_ostringstream oss3(std::ios_base::out);
    std::cout << "Capacity: " << oss3.capacity() << std::endl;
    
    // Case 4: どの型が1024として解釈されているかチェック
    std::cout << "\n=== Type Analysis ===" << std::endl;
    std::cout << "1024 as int: " << static_cast<int>(1024) << std::endl;
    std::cout << "1024 as size_t: " << static_cast<std::size_t>(1024) << std::endl;
    std::cout << "1024 as openmode: " << static_cast<std::ios_base::openmode>(1024) << std::endl;
    
    return 0;
}
