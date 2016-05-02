
#ifndef __CLASS_DEF_H__
#define __CLASS_DEF_H__

#define CLASS_NAME      "clock.dg"
#define CLASS_VERSION   1
#define CLASS_REVISION  0
#define CLASS_VER_STR   " 1.0 (25/04/2016)"
#define PRIVATE_DATA    ClockGadgetData
#define PARENT_CLASS    DB_ROOT_CLASS
#define SETTINGS_CLASS  "clockdg.mcc"

#undef GADGET_LIB_DATA
#undef GADGET_LIB_INIT

#undef METHOD_NEW
#undef METHOD_DISPOSE
#define METHOD_DRAW         clock_draw
#define METHOD_TICK         clock_tick
#undef METHOD_CLICK       
#undef METHOD_DROP        
#define METHOD_GETSIZE      clock_get_size
#undef METHOD_READCONFIG   

struct ClockGadgetData 
{
  ULONG counter;
  char time[10];
};

#endif
