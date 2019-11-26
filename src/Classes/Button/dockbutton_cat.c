
/****************************************************************

   This file was created automatically by `FlexCat 2.18'
   from "//Locale/Catalogs/DockButton.cd".

   Do NOT edit by hand!

****************************************************************/

#include <exec/memory.h>
#include <libraries/iffparse.h>

#include <proto/locale.h>

#include "DockButton_cat.h"

struct FC_String DockButton_Strings[11] = {
    { (STRPTR) "A button for DockBot", 0 },
    { (STRPTR) "Workbench", 1 },
    { (STRPTR) "Shell", 2 },
    { (STRPTR) "Choose an application", 3 },
    { (STRPTR) "Select", 4 },
    { (STRPTR) "Cancel", 5 },
    { (STRPTR) "Name", 6 },
    { (STRPTR) "Arguments", 7 },
    { (STRPTR) "Key", 8 },
    { (STRPTR) "Start Type", 9 },
    { (STRPTR) "Console", 10 }
};

VOID InitCatalog(struct Catalog *catalog)
{
    struct FC_String *fc;
	UWORD i;

    for (i = 0, fc = DockButton_Strings;  i < 11;  i++, fc++) {
	
		fc->msg = GetCatalogStr(catalog, fc->id, (STRPTR) fc->msg);
    }
}

