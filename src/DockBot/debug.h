/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <exec/types.h>

#ifndef DEBUG_MEM
#define LOG_MEMORY
#define LOG_MEMORY_TIMED
#else

VOID log_memory(VOID);
VOID log_memory_timed(VOID);

#define LOG_MEMORY 		 log_memory();
#define LOG_MEMORY_TIMED log_memory_timed();

#endif

#endif

