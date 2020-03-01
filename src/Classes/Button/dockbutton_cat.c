
/****************************************************************

   This file was created automatically by `FlexCat 2.18'
   from "//Locale/Catalogs/DockButton.cd".

   Do NOT edit by hand!

****************************************************************/

#include <exec/memory.h>
#include <libraries/iffparse.h>

#include <proto/locale.h>

#include "DockButton_cat.h"

struct FC_String DockButton_Strings[13] = {
    { (STRPTR) "A button for DockBot", 0 },
    { (STRPTR) "Workbench", 1 },
    { (STRPTR) "Shell", 2 },
    { (STRPTR) "Choose an application", 3 },
    { (STRPTR) "Choose an image", 4 },
    { (STRPTR) "Select", 5 },
    { (STRPTR) "Cancel", 6 },
    { (STRPTR) "Name", 7 },
    { (STRPTR) "Image", 8 },
    { (STRPTR) "Arguments", 9 },
    { (STRPTR) "Key", 10 },
    { (STRPTR) "Start Type", 11 },
    { (STRPTR) "Console", 12 }
};

VOID InitCatalog(struct Catalog *catalog)
{
    struct FC_String *fc;
	UWORD i;

    for (i = 0, fc = DockButton_Strings;  i < 13;  i++, fc++) {
	
		fc->msg = GetCatalogStr(catalog, fc->id, (STRPTR) fc->msg);
    }
}

