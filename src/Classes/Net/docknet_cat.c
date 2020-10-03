
/****************************************************************

   This file was created automatically by `FlexCat 2.18'
   from "//Locale/Catalogs/DockNet.cd".

   Do NOT edit by hand!

****************************************************************/

#include <exec/memory.h>
#include <libraries/iffparse.h>

#include <proto/locale.h>

#include "DockNet_cat.h"

struct FC_String DockNet_Strings[3] = {
    { (STRPTR) "A network meter for DockBot", 0 },
    { (STRPTR) "D: %s", 1 },
    { (STRPTR) "U: %s", 2 }
};

VOID InitCatalog(struct Catalog *catalog)
{
    struct FC_String *fc;
	UWORD i;

    for (i = 0, fc = DockNet_Strings;  i < 3;  i++, fc++) {
	
		fc->msg = GetCatalogStr(catalog, fc->id, (STRPTR) fc->msg);
    }
}

