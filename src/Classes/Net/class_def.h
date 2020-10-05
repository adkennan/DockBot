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
#include "docknet_cat.h"

#define CLASS_NAME      "DockNet"
#define CLASS_VERSION   1
#define CLASS_REVISION  0
#define CLASS_VER_STR   "1.0 (" DS ")"
#define CLASS_COPYRIGHT "© 2020 flype"

#define PRIVATE_DATA        NetGadgetData

#define GADGET_LIB_DATA     NetLibData
#define GADGET_LIB_INIT     net_lib_init
#define GADGET_LIB_EXPUNGE  net_lib_expunge

#define METHOD_NEW			net_new
#undef METHOD_DISPOSE
#undef METHOD_ADDED
#undef METHOD_REMOVED

#define METHOD_TICK         net_tick
#define METHOD_CLICK        net_click
#define METHOD_LAUNCHED     net_launched

#undef METHOD_DROP
#undef METHOD_HOTKEY

#define METHOD_GETSIZE      net_get_size
#define METHOD_DRAW         net_draw

#undef METHOD_READCONFIG
#undef METHOD_WRITECONFIG
#undef METHOD_GETSETTINGS

#undef METHOD_GETHOTKEY
#undef METHOD_GETLABEL

struct NetLibData
{
    struct Library *graphicsLib;
    struct Library *socketLib;
};

struct NetGadgetData 
{
    ULONG counter;
    BOOL clicked;
};

#endif
