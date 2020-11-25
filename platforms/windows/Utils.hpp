#pragma onece

#include <Windows.h>

#include <nodec/nodec_exception.hpp>


bool TryMultiByteToWideChar(const char* multiByteStr, std::wstring& wideCharStr)
{
    int convertResult = MultiByteToWideChar(CP_UTF8, 0, multiByteStr, (int)strlen(multiByteStr), nullptr, 0);
    if (convertResult <= 0)
    {
        return false;
    }
}