#pragma once

#include <nodec/macros.hpp>
#include <nodec/logging.hpp>

#include <wrl.h>
#include <dxgidebug.h>

class DxgiInfoLogger
{
public:
    DxgiInfoLogger();
    ~DxgiInfoLogger() = default;

    void SetLatest() noexcept;
    void Dump(nodec::logging::Level log_level);

private:
    Microsoft::WRL::ComPtr<IDXGIInfoQueue> pDxgiInfoQueue;
    unsigned long long next = 0;

private:
    NODEC_DISABLE_COPY(DxgiInfoLogger);
};