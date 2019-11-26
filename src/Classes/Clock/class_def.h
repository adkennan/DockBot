/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#ifndef __CLASS_DEF_H__
#define __CLASS_DEF_H__

#include "gadget_class.h"
#include "dockclock_cat.h"

#define CLASS_NAME      "DockClock"
#define CLASS_VERSION   1
#define CLASS_REVISION  2
#define CLASS_VER_STR   "1.2 (" DS ")"
#define CLASS_COPYRIGHT "© 2019 Andrew Kennan"
#define PRIVATE_DATA    ClockGadgetData

#define GADGET_LIB_DATA     ClockLibData

#define GADGET_LIB_INIT     clock_lib_init
#define GADGET_LIB_EXPUNGE  clock_lib_expunge

#define METHOD_NEW          clock_new
#define METHOD_DISPOSE      clock_dispose

#define METHOD_TICK         clock_tick

#define METHOD_GETSIZE      clock_get_size
#define METHOD_DRAW         clock_draw

#define METHOD_READCONFIG   clock_read_config
#define METHOD_WRITECONFIG  clock_write_config

#define METHOD_GETEDITOR    clock_get_editor
#define METHOD_EDITORUPDATE clock_editor_update
#define METHOD_CANEDIT		clock_can_edit


#define MAX_DISPLAY_LENGTH 20

struct ClockLibData
{
  struct Library *dosBase;
  struct Library *utilityBase;
  struct Library *gfxBase;
};

struct ClockGadgetData 
{
    struct Locale *locale;
    struct MinList lines; 
    STRPTR format;
    STRPTR splitFormat;   
    ULONG counter;
};

#endif
