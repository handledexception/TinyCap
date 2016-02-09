#include "util.h"
#include <cstdarg>

void DebugOut(const char *format, ...)
{	
	char out[256];
	va_list ap = { 0 };
	
	va_start(ap, format);
	vsprintf_s(out, format, ap);
	va_end(ap);
	OutputDebugStringA(out);	
};

void DebugOutW(const wchar_t *format, ...)
{
	wchar_t out[256];
	va_list ap = { 0 };

	va_start(ap, format);
	wvsprintfW(out, format, ap);	
	va_end(ap);
	OutputDebugStringW(out);
}