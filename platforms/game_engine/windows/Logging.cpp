#pragma once

#include "Logging.hpp"

#include <nodec/nodec_exception.hpp>
#include <fstream>
#include <sstream>

static std::ofstream logFile;
static bool initialized = false;

static void RecordToFileHandler(const nodec::logging::LogRecord& record)
{
    logFile << record << "\n";
}

static auto handler = nodec::logging::StaticRecordHandler::make_shared(RecordToFileHandler);

void InitLogging(nodec::logging::Level level)
{
    if (initialized) return;

    logFile.open("output.log", std::ios::binary);
    if (!logFile)
    {
        throw nodec::NodecException("Logging initialize failed. cannot open the log file.", __FILE__, __LINE__);
    }

    nodec::logging::set_level(level);
    nodec::logging::record_handlers += handler;

    nodec::logging::InfoStream info_stream(__FILE__, __LINE__);
    info_stream << "[Logging] >>>\n"
        << "Logging successfully initiallized.\n"
        << "log_level: " << level
        << std::flush;

    initialized = true;
}