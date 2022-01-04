#include <Audio/AudioIntegration.hpp>

#include <nodec/logging.hpp>

int main() {
    using namespace nodec;

    logging::record_handlers().connect(logging::record_to_stdout_handler);
    try {
        AudioIntegration audioIntegration;
    }
    catch (std::exception& e) {
        logging::ErrorStream(__FILE__, __LINE__) << e.what();
    }

    return 0;
}