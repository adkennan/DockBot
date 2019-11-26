
/****************************************************************

   This file was created automatically by `FlexCat 2.18'
   from "/Locale/Catalogs/DockBot.cd".

   Do NOT edit by hand!

****************************************************************/

#ifndef DockBot_CAT_H
#define DockBot_CAT_H

#include <exec/types.h>
#include <libraries/locale.h>

VOID open_catalog(VOID);

VOID close_catalog(VOID);

struct FC_String {
    const UBYTE *msg;
    LONG id;
};

extern struct FC_String DockBot_Strings[15];

#define MSG_OK (DockBot_Strings[0].msg)
#define _MSG_OK (DockBot_Strings+0)
#define MSG_ERR_OpenLibrary (DockBot_Strings[1].msg)
#define _MSG_ERR_OpenLibrary (DockBot_Strings+1)
#define MSG_MENU_Project (DockBot_Strings[2].msg)
#define _MSG_MENU_Project (DockBot_Strings+2)
#define MSG_MENU_Settings (DockBot_Strings[3].msg)
#define _MSG_MENU_Settings (DockBot_Strings+3)
#define MSG_KEY_Settings (DockBot_Strings[4].msg)
#define _MSG_KEY_Settings (DockBot_Strings+4)
#define MSG_MENU_About (DockBot_Strings[5].msg)
#define _MSG_MENU_About (DockBot_Strings+5)
#define MSG_KEY_About (DockBot_Strings[6].msg)
#define _MSG_KEY_About (DockBot_Strings+6)
#define MSG_MENU_Help (DockBot_Strings[7].msg)
#define _MSG_MENU_Help (DockBot_Strings+7)
#define MSG_MENU_Iconify (DockBot_Strings[8].msg)
#define _MSG_MENU_Iconify (DockBot_Strings+8)
#define MSG_KEY_Iconify (DockBot_Strings[9].msg)
#define _MSG_KEY_Iconify (DockBot_Strings+9)
#define MSG_MENU_Quit (DockBot_Strings[10].msg)
#define _MSG_MENU_Quit (DockBot_Strings+10)
#define MSG_KEY_Quit (DockBot_Strings[11].msg)
#define _MSG_KEY_Quit (DockBot_Strings+11)
#define MSG_ABOUT_Title (DockBot_Strings[12].msg)
#define _MSG_ABOUT_Title (DockBot_Strings+12)
#define MSG_ABOUT_Text (DockBot_Strings[13].msg)
#define _MSG_ABOUT_Text (DockBot_Strings+13)
#define MSG_CX_Description (DockBot_Strings[14].msg)
#define _MSG_CX_Description (DockBot_Strings+14)

#endif
