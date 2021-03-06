#pragma once

#include <cstdarg>
#include <string>

#include <Windows.h>

#define UTILS_EXPORT __declspec( dllexport )

enum LOG_LEVEL {
	Info = 100,
	Debug = 200,
	Warning = 300,
	Error = 400
};

static std::wstring str2wstr(const std::string& str);
static  std::string wstr2str(const std::wstring& wstr);

class UTILS_EXPORT PLogger {
public:
	PLogger() {};
	~PLogger() {};
	static void log(int level, const wchar_t* format, ...);
};
