
/****************************************************************

   This file was created automatically by `FlexCat 2.18'
   from "//Locale/Catalogs/Switcher.cd".

   Do NOT edit by hand!

****************************************************************/

#include <exec/memory.h>
#include <libraries/iffparse.h>

#include <proto/locale.h>

#include "switcher_cat.h"

struct FC_String switcher_Strings[5] = {
    { (STRPTR) "Window and screen switcher for DockBot", 0 },
    { (STRPTR) "Dock Window", 1 },
    { (STRPTR) "Screen Centre", 2 },
    { (STRPTR) "Position", 3 },
    { (STRPTR) "Key", 4 }
};

VOID InitCatalog(struct Catalog *catalog)
{
    struct FC_String *fc;
	UWORD i;

    for (i = 0, fc = switcher_Strings;  i < 5;  i++, fc++) {
	
		fc->msg = GetCatalogStr(catalog, fc->id, (STRPTR) fc->msg);
    }
}

