
#ifndef __CLASS_DEF_H__
#define __CLASS_DEF_H__

#include "gadget_class.h"

#define CLASS_NAME      "DockMem"
#define CLASS_VERSION   1
#define CLASS_REVISION  0
#define CLASS_VER_STR   "1.0 (06/05/2016)"
#define CLASS_DESC		"A memory meter for DockBot"
#define CLASS_COPYRIGHT "© 2016 Andrew Kennan"

#define PRIVATE_DATA    MemGadgetData

#define GADGET_LIB_DATA     MemLibData

#define GADGET_LIB_INIT     mem_lib_init
#define GADGET_LIB_EXPUNGE  mem_lib_expunge

#undef METHOD_NEW          
#undef METHOD_DISPOSE      

#undef METHOD_ADDED
#undef METHOD_REMOVED
#define METHOD_TICK         mem_tick
#undef METHOD_CLICK        
#undef METHOD_DROP     
#undef METHOD_HOTKEY		

#define METHOD_GETSIZE      mem_get_size   
#define METHOD_DRAW         mem_draw

#undef METHOD_READCONFIG   
#undef METHOD_WRITECONFIG
#undef METHOD_GETSETTINGS
  
#undef METHOD_GETHOTKEY
#undef METHOD_GETLABEL

struct MemLibData
{
  struct Library *graphicsLib;
};

struct MemGadgetData 
{
  ULONG counter;
};

#endif
