#ifndef NODEC__LOGGING__LOGGER_REPOSITORY_HPP_
#define NODEC__LOGGING__LOGGER_REPOSITORY_HPP_

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "logger.hpp"

namespace nodec {
namespace logging {

class LoggerRepository {
public:
    LoggerRepository()
        : root_logger_(get_logger("")) {
    }

    std::shared_ptr<Logger> get_logger(const std::string &name) {
        std::lock_guard<std::recursive_mutex> lock(loggers_mutex_);
        auto &logger = loggers_[name];
        if (logger) return logger;

        if (name.empty()) {
            logger = std::make_shared<Logger>("", nullptr);
            return logger;
        }

        std::string parent_name;
        {
            auto index = name.find_last_of('.');
            if (index != std::string::npos) {
                parent_name = name.substr(0, index);
            }
        }
        logger = std::make_shared<Logger>(name, get_logger(parent_name));
        return logger;
    }

    std::shared_ptr<Logger> root_logger() const noexcept {
        return root_logger_;
    }

private:
    std::recursive_mutex loggers_mutex_;
    std::unordered_map<std::string, std::shared_ptr<Logger>> loggers_;
    std::shared_ptr<Logger> root_logger_;
};

class LoggerRepositoryLocater {
public:
    static LoggerRepository &get() {
        static LoggerRepository instance;
        return instance;
    }
};
} // namespace logging
} // namespace nodec
#endif