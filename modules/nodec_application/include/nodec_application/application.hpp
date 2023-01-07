#ifndef NODEC_APPLICATION__APPLICATION_HPP_
#define NODEC_APPLICATION__APPLICATION_HPP_

#include <nodec/macros.hpp>
#include <nodec/type_info.hpp>

#include <cassert>
#include <memory>
#include <vector>

namespace nodec_application {

/**
 * @brief Application interface.
 *
 */
class Application {
public:
    Application(std::vector<std::shared_ptr<void>> &services)
        : services_(services) {}
    virtual ~Application() {}

public:
    template<typename Service>
    decltype(auto) get_service() const {
        const auto index = nodec::type_seq_index<Service>::value();
        return *std::static_pointer_cast<Service>(services_.at(index));
    }

    template<typename Service>
    decltype(auto) add_service(std::shared_ptr<Service> service) {
        assert(static_cast<bool>(service) && "Null service cannot be assigned.");

        const auto index = nodec::type_seq_index<Service>::value();

        if (!(index < services_.size())) services_.resize(index + 1u);
        services_[index] = service;
        return *std::static_pointer_cast<Service>(service);
    }

    virtual void quit() noexcept = 0;

private:
    std::vector<std::shared_ptr<void>> &services_;

private:
    NODEC_DISABLE_COPY(Application)
};

/**
 * @brief Configuration function.
 *
 * Needs to be implemented in apps.
 *
 * @param application
 */
void on_configure(Application &application);

} // namespace nodec_application

#endif