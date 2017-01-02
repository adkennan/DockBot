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

#define CLASS_NAME      "DockButton"
#define CLASS_VERSION   1
#define CLASS_REVISION  0
#define CLASS_VER_STR   "1.0 (25/04/2016)"
#define CLASS_DESC		"A button for DockBot"
#define CLASS_COPYRIGHT "© 2016 Andrew Kennan"
#define PRIVATE_DATA    ButtonGadgetData
#define PARENT_CLASS    DB_ROOT_CLASS

#define GADGET_LIB_DATA     ButtonLibData

#define GADGET_LIB_INIT     button_lib_init
#define GADGET_LIB_EXPUNGE  button_lib_expunge

#undef METHOD_NEW          
#define METHOD_DISPOSE      button_dispose

#define METHOD_TICK         button_tick
#define METHOD_CLICK        button_click
#define METHOD_DROP         button_drop
#define METHOD_HOTKEY		button_click

#define METHOD_GETSIZE      button_get_size
#define METHOD_DRAW         button_draw

#define METHOD_READCONFIG   button_read_config
#define METHOD_WRITECONFIG  button_write_config
#define METHOD_GETEDITOR	button_get_editor
#define METHOD_EDITOREVENT	button_editor_event
//#define METHOD_EDITORUPDATE button_editor_update
  
#define METHOD_GETHOTKEY    button_get_hotkey
#define METHOD_GETLABEL     button_get_label

struct ButtonLibData
{
	struct Library *iconBase;
	struct Library *dosBase;
  
	BOOL tritonOpen;
};

struct ButtonGadgetData
{
    STRPTR name;
    STRPTR path;
    STRPTR args;
    STRPTR con;
    STRPTR hotKey;
    UWORD imageW;
    UWORD imageH;
    struct DiskObject *diskObj;
    UWORD startType;
    UWORD counter;
    UWORD iconState;
};

#endif
