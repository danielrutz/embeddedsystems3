#ifndef PRINTF_H
#define PRINTF_H

#include <cstdarg>

char* Printf(char* dst, const void* end, const char* fmt, ...);
char* Printf_args(char* dst, const void* end, const char* fmt, va_list args);

#endif
