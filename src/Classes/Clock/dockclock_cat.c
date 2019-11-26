
/****************************************************************

   This file was created automatically by `FlexCat 2.18'
   from "//Locale/Catalogs/DockClock.cd".

   Do NOT edit by hand!

****************************************************************/

#include <exec/memory.h>
#include <libraries/iffparse.h>

#include <proto/locale.h>

#include "DockClock_cat.h"

struct FC_String DockClock_Strings[2] = {
    { (STRPTR) "A clock gadget for DockBot", 0 },
    { (STRPTR) "Format", 1 }
};

VOID InitCatalog(struct Catalog *catalog)
{
    struct FC_String *fc;
	UWORD i;

    for (i = 0, fc = DockClock_Strings;  i < 2;  i++, fc++) {
	
		fc->msg = GetCatalogStr(catalog, fc->id, (STRPTR) fc->msg);
    }
}

