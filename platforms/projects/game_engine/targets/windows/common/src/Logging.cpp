#pragma once

#include "Logging.hpp"

#include <nodec/error_formatter.hpp>

#include <fstream>
#include <sstream>

namespace {

std::ofstream logFile;
bool initialized = false;

void RecordToFileHandler(const nodec::logging::LogRecord& record) {
    logFile << record << "\n";
}

}


void InitLogging(nodec::logging::Level level) {
    if (initialized) return;

    logFile.open("output.log", std::ios::binary);
    if (!logFile) {
        throw std::runtime_error(nodec::error_fomatter::with_type_file_line<std::runtime_error>(
            "Logging initialize failed. cannot open the log file.",
            __FILE__, __LINE__
            ));
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