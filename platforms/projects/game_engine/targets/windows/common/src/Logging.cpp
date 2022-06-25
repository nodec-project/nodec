#pragma once

#include "Logging.hpp"

#include <nodec/formatter.hpp>

#include <fstream>
#include <sstream>

namespace {

std::ofstream logFile;
bool initialized = false;

void RecordToFileHandler(const nodec::logging::LogRecord& record) {
    try {
        logFile << record << "\n";
        logFile.flush();
    }
    catch (...) {
        // It is not possible to log exceptions that occur during log writing, so ignore them.
    }
}

}


void InitLogging(nodec::logging::Level level) {
    if (initialized) return;

    logFile.open("output.log", std::ios::binary);
    if (!logFile) {
        throw std::runtime_error(nodec::ErrorFormatter<std::runtime_error>(__FILE__, __LINE__)
            << "Logging initialize failed. cannot open the log file."
        );
    }

    nodec::logging::set_level(level);
    nodec::logging::record_handlers().connect(RecordToFileHandler);

    nodec::logging::InfoStream info_stream(__FILE__, __LINE__);
    info_stream
        << "[Logging] >>> Logging successfully initiallized.\n"
        << "log_level: " << level
        << std::flush;

    initialized = true;
}