
/****************************************************************

   This file was created automatically by `FlexCat 2.18'
   from "//Locale/Catalogs/DockNet.cd".

   Do NOT edit by hand!

****************************************************************/

#ifndef DockNet_CAT_H
#define DockNet_CAT_H

#include <exec/types.h>
#include <libraries/locale.h>

VOID InitCatalog(struct Catalog* catalog);

struct FC_String {
    const UBYTE *msg;
    LONG id;
};

extern struct FC_String DockNet_Strings[3];

#define MSG_Description (DockNet_Strings[0].msg)
#define _MSG_Description (DockNet_Strings+0)
#define MSG_LBL_RECV (DockNet_Strings[1].msg)
#define _MSG_LBL_RECV (DockNet_Strings+1)
#define MSG_LBL_SENT (DockNet_Strings[2].msg)
#define _MSG_LBL_SENT (DockNet_Strings+2)

#endif
