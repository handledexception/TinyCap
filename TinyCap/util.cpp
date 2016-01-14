#include "util.h"
#include <cstdarg>
void DebugOut(const char *format, ...)
{	
	char out[256];
	va_list ap;
	
	va_start(ap, format);
	wvsprintfA(out, format, ap);	
	OutputDebugStringA(out);
	va_end(ap);	
};

void DebugOutW(const wchar_t *format, ...)
{
	wchar_t out[256];
	va_list ap;

	va_start(ap, format);
	wvsprintfW(out, format, ap);
	OutputDebugStringW(out);
	va_end(ap);
}