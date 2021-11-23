#pragma once

#include <nodec/macros.hpp>
#include <nodec/logging.hpp>

// Prevent to define min/max macro in windows api.
#define NOMINMAX
#include <wrl.h>
#include <dxgidebug.h>

#include <mutex>


class DxgiInfoLogger
{
public:
    DxgiInfoLogger();
    ~DxgiInfoLogger() = default;

    //void SetLatest() noexcept;
    void Dump(nodec::logging::Level logLevel);
    bool DumpIfAny(nodec::logging::Level logLevel);

private:
    bool GetMessages(std::ostringstream& outMessagesStream);

private:
    Microsoft::WRL::ComPtr<IDXGIInfoQueue> mpDxgiInfoQueue;
    unsigned long long mNext = 0;

private:
    NODEC_DISABLE_COPY(DxgiInfoLogger);
};