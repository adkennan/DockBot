
/****************************************************************

   This file was created automatically by `FlexCat 2.18'
   from "//Locale/Catalogs/DockMem.cd".

   Do NOT edit by hand!

****************************************************************/

#include <exec/memory.h>
#include <libraries/iffparse.h>

#include <proto/locale.h>

#include "DockMem_cat.h"

struct FC_String DockMem_Strings[3] = {
    { (STRPTR) "A memory meter for DockBot", 0 },
    { (STRPTR) "Chip %d%%", 1 },
    { (STRPTR) "Fast %d%%", 2 }
};

VOID InitCatalog(struct Catalog *catalog)
{
    struct FC_String *fc;
	UWORD i;

    for (i = 0, fc = DockMem_Strings;  i < 3;  i++, fc++) {
	
		fc->msg = GetCatalogStr(catalog, fc->id, (STRPTR) fc->msg);
    }
}

