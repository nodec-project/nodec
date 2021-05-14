#include "Graphics/DxgiInfoLogger.hpp"
#include "Graphics/Graphics.hpp"

#include <nodec/error_formatter.hpp>
#include <nodec/logging.hpp>

#include <dxgidebug.h>

#include <stdexcept>
#include <sstream>

#pragma comment(lib, "dxguid.lib")

DxgiInfoLogger::DxgiInfoLogger() {
    // define function signature of DXGIGetDebugInterface
    typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

    // load the dll that contains the function DXGIGetDebugInterface
    const auto hModDxgiDebug = LoadLibraryEx(L"dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

    if (hModDxgiDebug == nullptr) {
        throw std::runtime_error(nodec::error_fomatter::with_type_file_line<std::runtime_error>(
            "Failed to load the dll 'dxgidebug.dll'",
            __FILE__, __LINE__
            ));
    }

    // get address of DXGIGetDebugInterface in dll
    const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
        reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface"))
        );
    if (DxgiGetDebugInterface == nullptr) {
        throw std::runtime_error(nodec::error_fomatter::with_type_file_line<std::runtime_error>(
            "Failed to get address of DXGIGetDebugInterface in dll 'dxgidebug.dll'",
            __FILE__, __LINE__
            ));
    }

    HRESULT hr;
    if (FAILED(hr = DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), &pDxgiInfoQueue))) {
        throw Graphics::HrException(hr, __FILE__, __LINE__);
    }

    nodec::logging::InfoStream(__FILE__, __LINE__) << "[DxgiInfoLogger] >>> Successfully initialized." << std::flush;
}

void DxgiInfoLogger::SetLatest() noexcept {
    // set the index (next) so that the next all to 
    // Dump() will only get errors generated after this call.
    next = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

bool DxgiInfoLogger::DumpIfAny(nodec::logging::Level logLevel) {
    std::ostringstream oss;

    oss << "[DxgiInfoLogger] >>> Dump the messsages.\n"
        << "=== Messages ===\n";
    if (!GetMessages(oss)) {
        return false;
    }
    oss << "END Messages ===\n";

    nodec::logging::log(logLevel, oss.str(), __FILE__, __LINE__);

    return true;
}

void DxgiInfoLogger::Dump(nodec::logging::Level logLevel) {
    std::ostringstream oss;

    oss << "[DxgiInfoLogger] >>> Dump the messsages.\n"
        << "=== Messages ===\n";
    GetMessages(oss);
    oss << "END Messages ===\n";

    nodec::logging::log(logLevel, oss.str(), __FILE__, __LINE__);
}

bool DxgiInfoLogger::GetMessages(std::ostringstream& outMessagesStream) {
    const auto end = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);

    if (next == end) {
        return false;
    }

    for (; next < end; ++next) {
        HRESULT hr;
        SIZE_T messageLength;

        // get the size of message in bytes
        if (FAILED(hr = pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, next, nullptr, &messageLength))) {
            outMessagesStream << "[ERROR] cannnot get the size of message '" << next << "': hr=0x"
                << std::hex << std::uppercase << hr << std::dec << "\n";
            continue;
        }
        // allocate memory for message
        auto bytes = std::make_unique<byte[]>(messageLength);
        auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());
        // get the message and log its description
        if (FAILED(hr = pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, next, pMessage, &messageLength))) {
            outMessagesStream << "[ERROR] cannnot get the size of message '" << next << "': hr=0x"
                << std::hex << std::uppercase << hr << std::dec << "\n";
            continue;
        }
        outMessagesStream << pMessage->pDescription << "\n";
    }
    return true;
}