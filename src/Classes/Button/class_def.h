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
#include "dockbutton_cat.h"

#define CLASS_NAME      "DockButton"
#define CLASS_VERSION   1
#define CLASS_REVISION  3
#define CLASS_VER_STR   "1.3 (" DS ")"
#define CLASS_COPYRIGHT "© 2020 Andrew Kennan"
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
#define METHOD_LAUNCHED     button_launched

#define METHOD_GETSIZE      button_get_size
#define METHOD_DRAW         button_draw
#define METHOD_REMAP        button_remap

#define METHOD_READCONFIG   button_read_config
#define METHOD_WRITECONFIG  button_write_config
#define METHOD_INITEDIT     button_init_edit
#define METHOD_GETEDITOR	button_get_editor
#define METHOD_EDITOREVENT  button_editor_event
#define METHOD_EDITORUPDATE button_editor_update
#define METHOD_CANEDIT		button_can_edit
  
#define METHOD_GETHOTKEY    button_get_hotkey
#define METHOD_GETLABEL     button_get_label

#define METHOD_INITBUTTON   button_init

struct ButtonLibData
{
	struct Library *iconBase;
	struct Library *dosBase;
    struct Library *gfxBase;
};

struct ButtonGadgetData
{
    STRPTR name;
    STRPTR path;
    STRPTR args;
    STRPTR con;
    STRPTR hotKey;
    STRPTR brushPath;
    UWORD imageW;
    UWORD imageH;
    struct DiskObject *diskObj;
    APTR brushImg;
    UWORD startType;
    UWORD counter;
    UWORD iconState;
    STRPTR selectedPath;
};

#endif
