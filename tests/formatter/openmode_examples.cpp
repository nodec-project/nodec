#include <sstream>
#include <iostream>

int main() {
    std::cout << "=== openmode の実用例 ===" << std::endl;
    
    // 1. 通常の出力（デフォルト）
    std::ostringstream oss1;
    oss1 << "Hello";
    oss1 << " World";
    std::cout << "通常: " << oss1.str() << std::endl; // "Hello World"
    
    // 2. 追記モード  
    std::ostringstream oss2("Initial: ", std::ios_base::app);
    oss2 << "Added";
    std::cout << "追記: " << oss2.str() << std::endl; // "Initial: Added"
    
    // 3. 初期文字列を削除して開始
    std::ostringstream oss3("ToBeDeleted", std::ios_base::out | std::ios_base::trunc);
    oss3 << "New content";
    std::cout << "削除後: " << oss3.str() << std::endl; // "New content"
    
    // 4. 初期文字列を保持（デフォルト動作）
    std::ostringstream oss4("Keep: ");
    oss4 << "Added";
    std::cout << "保持: " << oss4.str() << std::endl; // "Keep: Added"
    
    return 0;
}
