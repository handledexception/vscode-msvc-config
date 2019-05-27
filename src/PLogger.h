#pragma once

#include <cstdarg>
#include <string>
#include <codecvt>
#include <locale>

#include <windows.h>

enum LOG_LEVEL {
    Info = 100,
    Debug = 200,
    Warning = 300,
    Error = 400
};

std::wstring str2wstr(const std::string& str)
{
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;
    return converter.from_bytes(str);
}

std::string wstr2str(const std::wstring& wstr)
{
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;
    return converter.to_bytes(wstr);
}

class PLogger {

public:
    static void log(int level, const wchar_t* format, ...);
};

void PLogger::log(int level, const wchar_t* format, ...)
{
    va_list args;
    wchar_t tmp[4096];

    va_start(args, format);
    std::vswprintf(tmp, sizeof(tmp), format, args);
    OutputDebugString(tmp);
    va_end(args);
}
