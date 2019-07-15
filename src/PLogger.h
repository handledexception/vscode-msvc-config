#pragma once

#include <cstdarg>
#include <string>
#include <codecvt>
#include <locale>

#include <Windows.h>

enum LOG_LEVEL {
	Info = 100,
	Debug = 200,
	Warning = 300,
	Error = 400
};

static std::wstring str2wstr(const std::string& str);
static  std::string wstr2str(const std::wstring& wstr);

class PLogger {
public:
	PLogger() {};
	~PLogger() {};
	static void log(int level, const wchar_t* format, ...);
};
