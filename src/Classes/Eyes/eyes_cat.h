
/****************************************************************

   This file was created automatically by `FlexCat 2.18'
   from "//Locale/Catalogs/Eyes.cd".

   Do NOT edit by hand!

****************************************************************/

#ifndef Eyes_CAT_H
#define Eyes_CAT_H

#include <exec/types.h>
#include <libraries/locale.h>

VOID InitCatalog(struct Catalog* catalog);

struct FC_String {
    const UBYTE *msg;
    LONG id;
};

extern struct FC_String Eyes_Strings[2];

#define MSG_Description (Eyes_Strings[0].msg)
#define _MSG_Description (Eyes_Strings+0)
#define MSG_UI_NumEyes (Eyes_Strings[1].msg)
#define _MSG_UI_NumEyes (Eyes_Strings+1)

#endif
