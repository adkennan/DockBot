
/****************************************************************

   This file was created automatically by `FlexCat 2.18'
   from "//Locale/Catalogs/Eyes.cd".

   Do NOT edit by hand!

****************************************************************/

#include <exec/memory.h>
#include <libraries/iffparse.h>

#include <proto/locale.h>

#include "Eyes_cat.h"

struct FC_String Eyes_Strings[2] = {
    { (STRPTR) "Eyes that follow your mouse pointer", 0 },
    { (STRPTR) "How many eyes?", 1 }
};

VOID InitCatalog(struct Catalog *catalog)
{
    struct FC_String *fc;
	UWORD i;

    for (i = 0, fc = Eyes_Strings;  i < 2;  i++, fc++) {
	
		fc->msg = GetCatalogStr(catalog, fc->id, (STRPTR) fc->msg);
    }
}

