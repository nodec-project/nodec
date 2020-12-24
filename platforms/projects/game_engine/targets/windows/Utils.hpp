#pragma once

#include <Windows.h>

#include <sstream>
#include <string>

namespace Utils
{


class WideCharacterConvertException
{
public:
    WideCharacterConvertException(int64_t convertResult)
        :
        convertResult(convertResult)
    {
        std::wostringstream oss;
        oss << type() << L": " << L"Exception occurred: Failure to convert its message text using MultiByteToWideChar: convertResult="
            << convertResult
            << L"; GetLastError()="
            << GetLastError();
        whatBuffer = oss.str();
    }
    const wchar_t* type() const { return L"WideCharacterConvertException"; }
    const wchar_t* what() const { return whatBuffer.c_str(); }
private:
    const int64_t convertResult;
    mutable std::wstring whatBuffer;
};

/**
* <https://docs.microsoft.com/ja-jp/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar?redirectedfrom=MSDN>
*/
inline std::wstring TryMultiByteToWideChar(const char* multiByteStr)
{
    std::wstring wideCharStr;
    int64_t convertResult = MultiByteToWideChar(CP_UTF8, 0, multiByteStr, (int)strlen(multiByteStr), nullptr, 0);
    if (convertResult <= 0)
    {
        throw WideCharacterConvertException(convertResult);
    }

    wideCharStr.resize(convertResult + 10);
    convertResult = MultiByteToWideChar(CP_UTF8, 0, multiByteStr, (int)strlen(multiByteStr), &wideCharStr[0], (int)wideCharStr.size());
    if (convertResult <= 0)
    {
        throw WideCharacterConvertException(convertResult);
    }
    return wideCharStr;
}


}