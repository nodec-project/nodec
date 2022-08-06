#ifndef NODEC__APPLICATION_HPP_
#define NODEC__APPLICATION_HPP_

#include <nodec/macros.hpp>

namespace nodec {

class Application {
public:
    Application(){};
    virtual ~Application(){};

    int run() {
        try {
            return main();
        } catch (...) {
            return on_error_exit();
        }
    }

protected:
    virtual int main() = 0;
    virtual int on_error_exit() = 0;

private:
    NODEC_DISABLE_COPY(Application);
};

} // namespace nodec
#endif