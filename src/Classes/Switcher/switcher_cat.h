
/****************************************************************

   This file was created automatically by `FlexCat 2.18'
   from "//Locale/Catalogs/Switcher.cd".

   Do NOT edit by hand!

****************************************************************/

#ifndef switcher_CAT_H
#define switcher_CAT_H

#include <exec/types.h>
#include <libraries/locale.h>

VOID InitCatalog(struct Catalog* catalog);

struct FC_String {
    const UBYTE *msg;
    LONG id;
};

extern struct FC_String switcher_Strings[5];

#define MSG_Description (switcher_Strings[0].msg)
#define _MSG_Description (switcher_Strings+0)
#define MSG_POS_Dock (switcher_Strings[1].msg)
#define _MSG_POS_Dock (switcher_Strings+1)
#define MSG_POS_Screen (switcher_Strings[2].msg)
#define _MSG_POS_Screen (switcher_Strings+2)
#define MSG_UI_Position (switcher_Strings[3].msg)
#define _MSG_UI_Position (switcher_Strings+3)
#define MSG_UI_Key (switcher_Strings[4].msg)
#define _MSG_UI_Key (switcher_Strings+4)

#endif
