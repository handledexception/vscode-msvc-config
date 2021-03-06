#include "PLogger.h"

#include <codecvt>
#include <locale>


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

void PLogger::log(int level, const wchar_t* format, ...)
{
	va_list args;
	wchar_t tmp[4096];
	size_t fmt_len = wcslen(format);

	va_start(args, format);
	std::vswprintf(tmp, fmt_len + 4096, format, args);
	OutputDebugString(tmp);
	va_end(args);
}
