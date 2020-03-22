
/****************************************************************

   This file was created automatically by `FlexCat 2.18'
   from "/Locale/Catalogs/DockBotPrefs.cd".

   Do NOT edit by hand!

****************************************************************/

#include <exec/memory.h>
#include <libraries/iffparse.h>

#include <proto/locale.h>

#include "dockbot.h"

#include "DockBotPrefs_cat.h"

#include <stdio.h>

struct FC_String DockBotPrefs_Strings[43] = {
    { (STRPTR) "DockBot Preferences", 0 },
    { (STRPTR) "Preferences editor for DockBot", 1 },
    { (STRPTR) "OK", 2 },
    { (STRPTR) "Cancel", 3 },
    { (STRPTR) "Couldn't load class list.", 4 },
    { (STRPTR) "Couldn't load config.", 5 },
    { (STRPTR) "Couldn't open window.", 6 },
    { (STRPTR) "Unable to open version %d of %s.", 7 },
    { (STRPTR) "Unable to display editor dialogue.", 8 },
    { (STRPTR) "Sorry, directories cannot be added to the dock.", 9 },
    { (STRPTR) "Left", 10 },
    { (STRPTR) "Right", 11 },
    { (STRPTR) "Top", 12 },
    { (STRPTR) "Bottom", 13 },
    { (STRPTR) "Top/Left", 14 },
    { (STRPTR) "Center", 15 },
    { (STRPTR) "Bottom/Right", 16 },
    { (STRPTR) "New Dock Gadget", 17 },
    { (STRPTR) "Choose Gadget", 18 },
    { (STRPTR) "About", 19 },
    { (STRPTR) "Project", 20 },
    { (STRPTR) "?_About", 21 },
    { (STRPTR) "Q_Quit", 22 },
    { (STRPTR) "DockBot Preferences", 23 },
    { (STRPTR) "Dock Settings", 24 },
    { (STRPTR) "Position", 25 },
    { (STRPTR) "Alignment", 26 },
    { (STRPTR) "Show Labels", 27 },
    { (STRPTR) "Show Borders", 28 },
    { (STRPTR) "Background Image", 29 },
    { (STRPTR) "Gadgets", 30 },
    { (STRPTR) "_New...", 31 },
    { (STRPTR) "_Edit...", 32 },
    { (STRPTR) "Delete", 33 },
    { (STRPTR) "_Up", 34 },
    { (STRPTR) "_Down", 35 },
    { (STRPTR) "Save", 36 },
    { (STRPTR) "Test", 37 },
    { (STRPTR) "Use", 38 },
    { (STRPTR) "Delete Gadget", 39 },
    { (STRPTR) "Are you sure you want to delete this gadget?", 40 },
    { (STRPTR) "_OK|Cancel", 41 },
    { (STRPTR) "Choose a background", 42 }
};

STATIC struct Catalog *DockBotPrefsCatalog = NULL;

VOID open_catalog(VOID)
{	
    struct FC_String *fc;
	UWORD i;

	DEBUG(printf(__FUNC__ ": DockBotPrefs.catalog\n"));
	if( ! LocaleBase ) {
		DEBUG(printf(__FUNC__ ": Locale library not found.\n"));
		return;
	}

	if( !(DockBotPrefsCatalog = OpenCatalog(NULL, (STRPTR) "DockBotPrefs.catalog",
			 			 		     OC_BuiltInLanguage, "english",
								     OC_Version, 1,
								     TAG_DONE))) {
		DEBUG(printf(__FUNC__ ": Unable to open catalog.\n"));
		return;
	}

    for (i = 0, fc = DockBotPrefs_Strings;  i < 43;  i++, fc++) {
	
		fc->msg = GetCatalogStr(DockBotPrefsCatalog, fc->id, (STRPTR) fc->msg);
    }
}

VOID close_catalog(VOID) 
{
	DEBUG(printf(__FUNC__ ": DockBotPrefs.catalog\n"));
	if( ! DockBotPrefsCatalog ) {
		DEBUG(printf(__FUNC__ ": Catalog not open.\n"));
		return;
	}

	CloseCatalog(DockBotPrefsCatalog);
}

