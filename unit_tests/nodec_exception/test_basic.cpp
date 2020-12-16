#include <iostream>
#include <sstream>

#include <nodec/nodec_exception.hpp>

using namespace nodec;

class TestNodecException : public NodecException
{
public:
    TestNodecException(int error_code, const char* file, size_t line) noexcept
        :
        NodecException(file, line),
        error_code(error_code)
    {
        std::ostringstream oss;
        oss << "Are you OK? ですか?" << std::endl
            << "error code: " << error_code << std::endl;
        message = oss.str();
    }

    //const char* type() const noexcept override { return "TestNodecException"; }

private:
    int error_code;
};
int main()
{
    try
    {
        throw TestNodecException(1, __FILE__, __LINE__);
        throw NodecException("test", __FILE__, __LINE__);
        throw std::runtime_error("Test Runtime Exception");
        throw 1;
    }
    catch (const NodecException& e)
    {
        std::wcout << e.what() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "Standard Exception: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "No detail available" << std::endl;
    }
    return 0;
}