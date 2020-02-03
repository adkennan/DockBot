
/****************************************************************

   This file was created automatically by `FlexCat 2.18'
   from "/Locale/Catalogs/DockBotPrefs.cd".

   Do NOT edit by hand!

****************************************************************/

/****************************************************************
    This file uses the auto initialization features of
    Dice, gcc and SAS/C, respectively.

    Dice does this by using the __autoinit and __autoexit
    keywords, whereas SAS/C uses names beginning with _STI
    or _STD, respectively. gcc uses the asm() instruction
    to emulate C++ constructors and destructors.

    Using this file you don't have *all* the benefits of
    locale.library (no Locale or Language arguments are
    supported when opening the catalog). However, these are
    *very* rarely used, so this should be sufficient for most
    applications.
****************************************************************/

/*
    Include files and compiler specific stuff
*/

#include <exec/memory.h>
#include <libraries/locale.h>
#include <libraries/iffparse.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/locale.h>
#include <proto/utility.h>
#include <proto/iffparse.h>

#include <stdlib.h>
#include <string.h>



#include "DockBotPrefs_cat.h"


/*
    Variables
*/

struct FC_String DockBotPrefs_Strings[41] = {
    { (STRPTR) "DockBot Preferences", 0 },
    { (STRPTR) "Preferences editor for DockBot", 1 },
    { (STRPTR) "_OK", 2 },
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
    { (STRPTR) "Gadgets", 29 },
    { (STRPTR) "_New...", 30 },
    { (STRPTR) "_Edit...", 31 },
    { (STRPTR) "Delete", 32 },
    { (STRPTR) "_Up", 33 },
    { (STRPTR) "_Down", 34 },
    { (STRPTR) "Save", 35 },
    { (STRPTR) "Test", 36 },
    { (STRPTR) "Use", 37 },
    { (STRPTR) "Delete Gadget", 38 },
    { (STRPTR) "Are you sure you want to delete this gadget?", 39 },
    { (STRPTR) "_OK|Cancel", 40 }
};

STATIC struct Catalog *DockBotPrefsCatalog = NULL;
#ifdef LOCALIZE_V20
STATIC STRPTR DockBotPrefsStrings = NULL;
STATIC ULONG DockBotPrefsStringsSize;
#endif


#if defined(_DCC)
STATIC __autoexit VOID _STDCloseDockBotPrefsCatalog(VOID)
#elif defined(__SASC)
VOID _STDCloseDockBotPrefsCatalog(VOID)
#elif defined(__GNUC__)
STATIC VOID __attribute__ ((destructor)) _STDCloseDockBotPrefsCatalog(VOID)
#else
VOID CloseDockBotPrefsCatalog(VOID)
#endif

{
    if (DockBotPrefsCatalog) {
	CloseCatalog(DockBotPrefsCatalog);
    }
#ifdef LOCALIZE_V20
    if (DockBotPrefsStrings) {
	FreeMem(DockBotPrefsStrings, DockBotPrefsStringsSize);
    }
#endif
}


#if defined(_DCC)
STATIC __autoinit VOID _STIOpenDockBotPrefsCatalog(VOID)
#elif defined(__SASC)
VOID _STIOpenDockBotPrefsCatalog(VOID)
#elif defined(__GNUC__)
VOID __attribute__ ((constructor)) _STIOpenDockBotPrefsCatalog(VOID)
#else
VOID OpenDockBotPrefsCatalog(VOID)
#endif

{
    if (LocaleBase) {
	if ((DockBotPrefsCatalog = OpenCatalog(NULL, (STRPTR) "DockBotPrefs.catalog",
				     OC_BuiltInLanguage, "english",
				     OC_Version, 1,
				     TAG_DONE))) {
	    struct FC_String *fc;
	    int i;

	    for (i = 0, fc = DockBotPrefs_Strings;  i < 41;  i++, fc++) {
		 fc->msg = GetCatalogStr(DockBotPrefsCatalog, fc->id, (STRPTR) fc->msg);
	    }
	}
    }
}




#ifdef LOCALIZE_V20
VOID InitDockBotPrefsCatalog(STRPTR language)

{
    struct IFFHandle *iffHandle;

    /*
    **  Use iffparse.library only, if we need to.
    */
    if (LocaleBase  ||  !IFFParseBase  ||  !language  ||
	Stricmp(language, "english") == 0) {
	return;
    }

    if ((iffHandle = AllocIFF())) {
	char path[128];  /* Enough to hold 4 path items (dos.library V40) */
	strcpy(path, "PROGDIR:Catalogs");
	AddPart((STRPTR) path, language, sizeof(path));
	AddPart((STRPTR) path, "DockBotPrefs.catalog", sizeof(path));
	if (!(iffHandle->iff_Stream = Open((STRPTR) path, MODE_OLDFILE))) {
	    strcpy(path, "LOCALE:Catalogs");
	    AddPart((STRPTR) path, language, sizeof(path));
	    AddPart((STRPTR) path, language, sizeof(path));
	    iffHandle->iff_Stream = Open((STRPTR) path, MODE_OLDFILE);
	}

	if (iffHandle->iff_Stream) {
	    InitIFFasDOS(iffHandle);
	    if (!OpenIFF(iffHandle, IFFF_READ)) {
		if (!PropChunk(iffHandle, MAKE_ID('C','T','L','G'),
			       MAKE_ID('S','T','R','S'))) {
		    struct StoredProperty *sp;
		    int error;

		    for (;;) {
			if ((error = ParseIFF(iffHandle, IFFPARSE_STEP))
				   ==  IFFERR_EOC) {
			    continue;
			}
			if (error) {
			    break;
			}

			if ((sp = FindProp(iffHandle, MAKE_ID('C','T','L','G'),
					   MAKE_ID('S','T','R','S')))) {
			    /*
			    **  Check catalog and calculate the needed
			    **  number of bytes.
			    **  A catalog string consists of
			    **      ID (LONG)
			    **      Size (LONG)
			    **      Bytes (long word padded)
			    */
			    LONG bytesRemaining;
			    LONG *ptr;

			    DockBotPrefsStringsSize = 0;
			    bytesRemaining = sp->sp_Size;
			    ptr = (LONG *) sp->sp_Data;

			    while (bytesRemaining > 0) {
				LONG skipSize, stringSize;

				ptr++;                  /* Skip ID */
				stringSize = *ptr++;
				skipSize = ((stringSize+3) >> 2);

				DockBotPrefsStringsSize += stringSize+1;  /* NUL */
				bytesRemaining -= 8 + (skipSize << 2);
				ptr += skipSize;
			    }

			    if (!bytesRemaining  &&
				(DockBotPrefsStrings = AllocMem(DockBotPrefsStringsSize, MEMF_ANY))) {
				STRPTR sptr;

				bytesRemaining = sp->sp_Size;
				ptr = (LONG *) sp->sp_Data;
				sptr = DockBotPrefsStrings;

				while (bytesRemaining) {
				    LONG skipSize, stringSize, id;
				    struct FC_String *fc;
				    int i;

				    id = *ptr++;
				    stringSize = *ptr++;
				    skipSize = ((stringSize+3) >> 2);

				    CopyMem(ptr, sptr, stringSize);
				    bytesRemaining -= 8 + (skipSize << 2);
				    ptr += skipSize;

				    for (i = 0, fc = DockBotPrefs_Strings;  i < 41;  i++, fc++) {
					if (fc->id == id) {
					    fc->msg = sptr;
					}
				    }

				    sptr += stringSize;
				    *sptr++ = '\0';
				}
			    }
			    break;
			}
		    }
		}
		CloseIFF(iffHandle);
	    }
	    Close(iffHandle->iff_Stream);
	}
	FreeIFF(iffHandle);
    }
}
#endif
