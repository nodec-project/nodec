#ifndef NODEC__APPLICATION_HPP_
#define NODEC__APPLICATION_HPP_

#include <nodec/macros.hpp>

namespace nodec
{

class Application
{
private:
    NODEC_DISABLE_COPY(Application);

public:
    Application() {};
    virtual ~Application() {};

    int run();

protected:
    virtual int main() = 0;
    virtual int on_error_exit() = 0;

};

}
#endif