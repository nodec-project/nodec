#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <nodec/logging/logging.hpp>

TEST_CASE("Testing add_handler") {
    using namespace nodec::logging;

    auto logger = get_logger();
    int count = 0;

    // Check the handler is called.
    {
        auto conn = logger->add_handler([&](const LogRecord &record) {
            count++;
        });

        logger->debug("hello", __FILE__, __LINE__);
        CHECK(count == 1);
    }

    // Check the handler is disconnected.
    {
        count = 0;
        logger->debug("hello", __FILE__, __LINE__);
        CHECK(count == 0);
    }

    // Check the parent handler is called.
    {
        auto conn1 = logger->add_handler([&](const LogRecord &record) {
            count++;
        });

        auto sub_logger = get_logger("sub");
        auto conn2 = sub_logger->add_handler([&](const LogRecord &record) {
            count++;
        });
        sub_logger->debug("hello", __FILE__, __LINE__);

        CHECK(count == 2);
    }
}

TEST_CASE("Testing LogStream") {
    using namespace nodec::logging;

    auto logger = get_logger();
    {
        std::string result;
        auto conn = logger->add_handler([&](const LogRecord &record) {
            result = record.message;
        });
        logger->debug(__FILE__, __LINE__) << "Hello" << 100;

        CHECK(result == "Hello100");
    }
}
