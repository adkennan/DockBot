
/****************************************************************

   This file was created automatically by `FlexCat 2.18'
   from "//Locale/Catalogs/DockClock.cd".

   Do NOT edit by hand!

****************************************************************/

#ifndef DockClock_CAT_H
#define DockClock_CAT_H

#include <exec/types.h>
#include <libraries/locale.h>

VOID InitCatalog(struct Catalog* catalog);

struct FC_String {
    const UBYTE *msg;
    LONG id;
};

extern struct FC_String DockClock_Strings[2];

#define MSG_Description (DockClock_Strings[0].msg)
#define _MSG_Description (DockClock_Strings+0)
#define MSG_UI_Format (DockClock_Strings[1].msg)
#define _MSG_UI_Format (DockClock_Strings+1)

#endif
