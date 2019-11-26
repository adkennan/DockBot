
/****************************************************************

   This file was created automatically by `FlexCat 2.18'
   from "//Locale/Catalogs/DockMem.cd".

   Do NOT edit by hand!

****************************************************************/

#ifndef DockMem_CAT_H
#define DockMem_CAT_H

#include <exec/types.h>
#include <libraries/locale.h>

VOID InitCatalog(struct Catalog* catalog);

struct FC_String {
    const UBYTE *msg;
    LONG id;
};

extern struct FC_String DockMem_Strings[3];

#define MSG_Description (DockMem_Strings[0].msg)
#define _MSG_Description (DockMem_Strings+0)
#define MSG_LBL_Chip (DockMem_Strings[1].msg)
#define _MSG_LBL_Chip (DockMem_Strings+1)
#define MSG_LBL_Fast (DockMem_Strings[2].msg)
#define _MSG_LBL_Fast (DockMem_Strings+2)

#endif
