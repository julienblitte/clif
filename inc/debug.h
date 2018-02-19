#ifndef DEBUG_H
#define DEBUG_H

#include "clif.h"
#include <stdarg.h>

#define DCRITICAL	0
#define DERROR		1
#define DWARNING	2
#define DINFO		3
#define DDEBUG		4
#define DVERBOSE	5
#define DPEDANTIC	6

#ifndef DEBUG_LEVEL
	#define DEBUG_LEVEL		DDEBUG
#endif

#ifdef NDEBUG
	#define dbgprintf(l,f,...)	{}
#else
	void dbgprintf(int level, char* format, ...);
#endif

#endif // DEBUG_H
