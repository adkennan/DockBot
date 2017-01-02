/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#ifndef __CLASS_DEF_H__
#define __CLASS_DEF_H__

#include "gadget_class.h"

#define CLASS_NAME      "DockClock"
#define CLASS_VERSION   1
#define CLASS_REVISION  0
#define CLASS_VER_STR   "1.0 (25/04/2016)"
#define CLASS_DESC		"A clock for DockBot"
#define CLASS_COPYRIGHT "© 2016 Andrew Kennan"
#define PRIVATE_DATA    ClockGadgetData

#define GADGET_LIB_DATA     ClockLibData

#define GADGET_LIB_INIT     clock_lib_init
#define GADGET_LIB_EXPUNGE  clock_lib_expunge

#define METHOD_NEW          clock_new
#define METHOD_DISPOSE      clock_dispose

#define METHOD_TICK         clock_tick
#undef METHOD_CLICK        
#undef METHOD_DROP         
#undef METHOD_HOTKEY	    

#define METHOD_GETSIZE      clock_get_size
#define METHOD_DRAW         clock_draw

#define METHOD_READCONFIG   clock_read_config
#define METHOD_WRITECONFIG  clock_write_config
#undef METHOD_GETSETTINGS

#undef METHOD_GETHOTKEY   
#undef METHOD_GETLABEL     


#define MAX_DISPLAY_LENGTH 20

struct ClockLibData
{
  struct Library *localeBase;
  struct Library *dosBase;
};

struct ClockGadgetData 
{
  struct Locale *locale;
  STRPTR format;
  ULONG counter;
  UWORD minutes;
  UWORD pos;  
  UWORD formatLen;
  char time[MAX_DISPLAY_LENGTH];
};

#endif
