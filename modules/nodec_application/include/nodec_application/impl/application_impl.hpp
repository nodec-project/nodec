#ifndef NODEC_APPLICATION__IMPL__APPLICATION_IMPL_HPP_
#define NODEC_APPLICATION__IMPL__APPLICATION_IMPL_HPP_

#include "../application.hpp"

#include <nodec/logging.hpp>
#include <nodec/signals/signal.hpp>

#include <cassert>
#include <memory>
#include <vector>

namespace nodec_application {
namespace impl {

class ApplicationImpl : public Application {
public:
    ApplicationImpl()
        : Application(services_) {}

    virtual ~ApplicationImpl() {}

    void configure() {
        assert(state_ == State::Unconfigured);

        on_configure(*this);

        state_ = State::Active;
    }

    void release_all_services() {
        services_.clear();
    }

private:
    std::vector<std::shared_ptr<void>> services_;

    enum class State {
        Unconfigured,
        Active
    };

    State state_{State::Unconfigured};
};

} // namespace impl
} // namespace nodec_application

#endif