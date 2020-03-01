
/****************************************************************

   This file was created automatically by `FlexCat 2.18'
   from "//Locale/Catalogs/DockButton.cd".

   Do NOT edit by hand!

****************************************************************/

#ifndef DockButton_CAT_H
#define DockButton_CAT_H

#include <exec/types.h>
#include <libraries/locale.h>

VOID InitCatalog(struct Catalog* catalog);

struct FC_String {
    const UBYTE *msg;
    LONG id;
};

extern struct FC_String DockButton_Strings[13];

#define MSG_Description (DockButton_Strings[0].msg)
#define _MSG_Description (DockButton_Strings+0)
#define MSG_ST_Workbench (DockButton_Strings[1].msg)
#define _MSG_ST_Workbench (DockButton_Strings+1)
#define MSG_ST_Shell (DockButton_Strings[2].msg)
#define _MSG_ST_Shell (DockButton_Strings+2)
#define MSG_FR_ChooseApplication (DockButton_Strings[3].msg)
#define _MSG_FR_ChooseApplication (DockButton_Strings+3)
#define MSG_FR_ChooseBrush (DockButton_Strings[4].msg)
#define _MSG_FR_ChooseBrush (DockButton_Strings+4)
#define MSG_FR_OkText (DockButton_Strings[5].msg)
#define _MSG_FR_OkText (DockButton_Strings+5)
#define MSG_FR_CancelText (DockButton_Strings[6].msg)
#define _MSG_FR_CancelText (DockButton_Strings+6)
#define MSG_UI_Name (DockButton_Strings[7].msg)
#define _MSG_UI_Name (DockButton_Strings+7)
#define MSG_UI_Brush (DockButton_Strings[8].msg)
#define _MSG_UI_Brush (DockButton_Strings+8)
#define MSG_UI_Args (DockButton_Strings[9].msg)
#define _MSG_UI_Args (DockButton_Strings+9)
#define MSG_UI_Key (DockButton_Strings[10].msg)
#define _MSG_UI_Key (DockButton_Strings+10)
#define MSG_UI_StartType (DockButton_Strings[11].msg)
#define _MSG_UI_StartType (DockButton_Strings+11)
#define MSG_UI_Console (DockButton_Strings[12].msg)
#define _MSG_UI_Console (DockButton_Strings+12)

#endif
