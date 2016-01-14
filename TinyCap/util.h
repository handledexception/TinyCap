#ifndef _H_UTIL
#define _H_UTIL

#include <Windows.h>
#include <comdef.h>

typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

const float PI			= 3.141592654f;
const float TWOPI		= 6.283185307f;

void DebugOut(const char *format, ...);
void DebugOutW(const wchar_t *format, ...);

#endif