#ifndef NODEC__FORMATTER_HPP_
#define NODEC__FORMATTER_HPP_

#include <nodec/macros.hpp>

#include <sstream>

namespace nodec {

/*
*   Note:
*       * <https://stackoverflow.com/questions/12261915/how-to-throw-stdexceptions-with-variable-messages>
*/

class Formatter {
public:
    Formatter() noexcept {}
    ~Formatter() noexcept {}

    template<typename T>
    Formatter& operator<<(const T &value) noexcept {
        stream_ << value;
        return *this;
    }

    operator std::string() const noexcept {
        return stream_.str();
    }

private:
    std::ostringstream stream_;

    NODEC_DISABLE_COPY(Formatter);
};

}


#endif