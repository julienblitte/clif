#include <stdio.h>
#include <stdarg.h>

#include "debug.h"

#ifndef NDEBUG
void dbgprintf(int level, char* format, ...)
{
	va_list args;

	if (level > DEBUG_LEVEL)
			return;

	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
}
#endif
