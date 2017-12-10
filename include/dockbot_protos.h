/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#ifndef __DOCKBOT_PROTOS_H__
#define __DOCKBOT_PROTOS_H__

#include <exec/types.h>
#include <intuition/intuition.h>
#include <intuition/classes.h>

#include "dockbot.h"

/****** dockbot.library/DB_GetDockGadgetBounds *****************************
*
*   NAME
*	DB_GetDockGadgetBounds -- Get the bounding box of a dock gadget.
*
*   SYNOPSIS
*	DB_GetDockGadgetBounds(obj, bounds)
*	                       A0     A1
*	VOID DB_GetDockGadgetBounds(Object *, struct Rect *);
*
*   FUNCTION
*	Queries a dock gadget for it's position and size.
*
*   INPUTS
*	obj    - The dock gadget to query.
*	bounds - A pointer to a Rect to be filled in by the gadget.
*
*   RESULT
*	The parameter bounds will be filled contain the bounds of the gadget.
*
*   EXAMPLE
*	struct Rect bounds;
*	DB_GetDockGadgetBounds(obj, &bounds);
*
***************************************************************************/
VOID __asm __saveds DB_GetDockGadgetBounds(
	register __a0 Object *obj, 
	register __a1 struct Rect *bounds);


/****** dockbot.library/DB_DrawOutsetFrame *********************************
*
*   NAME
*	DB_DrawOutsetFrame -- Draws a raised 3d rectangle.
*
*   SYNOPSIS
*	DB_DrawOutsetFrame(rp, bounds)
*	                   A0    A1
*	VOID DB_DrawOutsetFrame(struct RastPort *, struct Rect *);
*
*   FUNCTION
*	Draws a raised 3d frame with the supplied RastPort.
*
*   INPUTS
*	rp     - The RastPort to draw the frame with.
*	bounds - A pointer to a Rect that defines the frame to draw.
*
*   RESULT
*	A 3d frame will be drawn to the window.
*
*   EXAMPLE
*	struct Rect bounds;
*	DB_GetDockGadgetBounds(obj, &bounds);
*	DB_DrawOutsetFrame(rp, &bounds);

***************************************************************************/
VOID __asm __saveds DB_DrawOutsetFrame(
	register __a0 struct RastPort *rp, 
	register __a1 struct Rect *bounds);


/****** dockbot.library/DB_DrawInsetFrame **********************************
*
*   NAME
*	DB_DrawOutsetFrame -- Draws a lowered 3d rectangle.
*
*   SYNOPSIS
*	DB_DrawInsetFrame(rp, bounds)
*	                  A0    A1
*	VOID DB_DrawInsetFrame(struct RastPort *, struct Rect *);
*
*   FUNCTION
*	Draws a lowered 3d frame with the supplied RastPort.
*
*   INPUTS
*	rp     - The RastPort to draw the frame with.
*	bounds - A pointer to a Rect that defines the frame to draw.
*
*   RESULT
*	A 3d frame will be drawn to the window.
*
*   EXAMPLE
*	struct Rect bounds;
*	DB_GetDockGadgetBounds(obj, &bounds);
*	DB_DrawInsetFrame(rp, &bounds);
*
***************************************************************************/
VOID __asm __saveds DB_DrawInsetFrame(
	register __a0 struct RastPort *rp, 
	register __a1 struct Rect *bounds);



/****** dockbot.library/DB_OpenSettingsRead ********************************

   NAME
	DB_OpenSettingsRead -- Open the settings file for reading.

   SYNOPSIS
	Settings = DB_OpenSettingsRead(filename)
	  D0                              A0
	struct DockSettings *DB_OpenSettingsRead(STRPTR);

   FUNCTION
	Opens the settings file for reading.

   INPUTS
	filename - The full path to the settings file to open.

   RESULT
	Returns a pointer to a DockSettings structure set up for reading.
	Returns NULL if there was an error opening or reading the file.

   EXAMPLE
	struct DockSettings *s;
	if( s = DB_OpenSettingsRead("ENV:DockBot.prefs") ) {
	  ...
	  DB_CloseSettings(s);
	}

   SEE ALSO
	DB_OpenSettingsWrite(),DB_CloseSettings()

***************************************************************************/
struct DockSettings * __asm __saveds DB_OpenSettingsRead(
    register __a0 STRPTR filename);



/****** dockbot.library/DB_OpenSettingsWrite *****************************

   NAME
	DB_OpenSettingsWrite -- Open the settings file for writing.

   SYNOPSIS
	Settings = DB_OpenSettingsWrite(filename)
	  D0                              A0
	struct DockSettings *DB_OpenSettingsWrite(STRPTR);

   FUNCTION
	Opens the settings file for writing. If the file doesn't exist it will
	be created.

   INPUTS
	filename - The full path to the settings file to open.

   RESULT
	Returns a pointer to a DockSettings structure set up for writing.
	Returns NULL if there was an error opening the file.

   EXAMPLE
	struct DockSettings *s;
	if( s = DB_OpenSettingsWrite("ENV:DockBot.prefs") ) {
	  ...
	  DB_CloseSettings(s);
	}

   SEE ALSO
	DB_OpenSettingsRead(),DB_CloseSettings()

***************************************************************************/
struct DockSettings * __asm __saveds DB_OpenSettingsWrite(
    register __a0 STRPTR filename);



/****** dockbot.library/DB_CloseSettings ***********************************

   NAME
	DB_CloseSettings -- Closes a previously opened settings file.

   SYNOPSIS
	DB_CloseSettings(settings)
	                   A0
	VOID DB_OpenSettingsWrite(struct DockSettings *);

   FUNCTION
	Closes and deallocates a DockSettings structure previously allocated by
	DB_OpenSettingsRead or DB_OpenSettingsWrite. 
	Do not use the settings object after calling this function.

   INPUTS
	settings - The DockSettings structure previously allocated.

   RESULT
	The DockSettings structure will be deallocated.

   EXAMPLE
	struct DockSettings *s;
	if( s = DB_OpenSettingsRead("ENV:DockBot.prefs") ) {
	  ...
	  DB_CloseSettings(s);
	}

   SEE ALSO
	DB_OpenSettingsRead(),DB_OpenSettingsWrite()

***************************************************************************/
VOID __asm __saveds DB_CloseSettings(
    register __a0 struct DockSettings *settings);


/****** dockbot.library/DB_ReadBeginBlock *********************************

   NAME
	DB_ReadBeginBlock -- Attempts to read a block start from the setings.

   SYNOPSIS
	success = DB_ReadBeginBlock(settings)
	  D0                          A0
	BOOL DB_ReadBeginBlock(struct DockSettings *);

   FUNCTION
	Looks for the word "begin" on the current line of the settings file.
	If found the DockSettings struct records the beginning of an indented 
	block.

   INPUTS
	settings - The DockSettings structure allocated by DB_OpenSettingsRead.

   RESULT
	TRUE if a block start was found, FALSE if it was not.

   EXAMPLE
	struct DockSettings *s;
	if( s = DB_OpenSettingsRead("ENV:DockBot.prefs") ) {
	  if( DB_ReadBeginBlock(s) ) {
	    while(!DB_ReadEndBlock(s)) {
	      ...
	    }
	  }
	  DB_CloseSettings(s);
	}

   SEE ALSO
	DB_OpenSettingsRead(),DB_ReadEndBlock()

***************************************************************************/
BOOL __asm __saveds DB_ReadBeginBlock(
    register __a0 struct DockSettings *settings);


/****** dockbot.library/DB_ReadEndBlock ************************************

   NAME
	DB_ReadEndBlock -- Attempts to read a block end from the setings.

   SYNOPSIS
	success = DB_ReadEndBlock(settings)
	  D0                        A0
	BOOL DB_ReadEndBlock(struct DockSettings *);

   FUNCTION
	Looks for the word "end" on the current line of the settings file.
	If found the DockSettings struct records the end of an indented block.

   INPUTS
	settings - The DockSettings structure allocated by DB_OpenSettingsRead.

   RESULT
	TRUE if a block end was found, FALSE if it was not.

   EXAMPLE
	struct DockSettings *s;
	if( s = DB_OpenSettingsRead("ENV:DockBot.prefs") ) {
	  if( DB_ReadBeginBlock(s) ) {
	    while(!DB_ReadEndBlock(s)) {
	      ...
	    }
	  }
	  DB_CloseSettings(s);
	}

   SEE ALSO
	DB_OpenSettingsRead(),DB_ReadBeginBlock()

***************************************************************************/
BOOL __asm __saveds DB_ReadEndBlock(
    register __a0 struct DockSettings *settings);


/****** dockbot.library/DB_ReadSetting *************************************

   NAME
	DB_ReadSetting -- Attempts to read a value from the settings file.

   SYNOPSIS
	success = DB_ReadSetting(settings, value)
	  D0                       A0        A1
	BOOL DB_ReadSetting(struct DockSettings *, struct DockSettingValue *);

   FUNCTION
	Looks for the key and value separated by an equals sign on the current 
	line of the settings file.
	If found the value struct will be filled in with key and value.

   INPUTS
	settings - The DockSettings structure allocated by DB_OpenSettingsRead.

   RESULT
	TRUE if a block end was found, FALSE if it was not.

   EXAMPLE
	struct DockSettings *s;
	struct DockSettingValue val;
	if( s = DB_OpenSettingsRead("ENV:DockBot.prefs") ) {
	  if( DB_ReadBeginBlock(s) ) {
	    while(!DB_ReadEndBlock(s)) {
	      if( DB_ReadSetting(s, &val) ) {
	        ...
	      }
	    }
	  }
	  DB_CloseSettings(s);
	}

   SEE ALSO
	DB_OpenSettingsRead()

***************************************************************************/
BOOL __asm __saveds DB_ReadSetting(
    register __a0 struct DockSettings *settings, 
    register __a1 struct DockSettingValue *value);


/****** dockbot.library/DB_WriteBeginBlock *********************************
*
*   NAME
*	DB_WriteBeginBlock -- Writes the start of a block to the settings file.
*
*   SYNOPSIS
*	success = DB_WriteBeginBlock(settings)
*	  D0                            A0       
*	BOOL DB_WriteBeginBlock(struct DockSettings *);
*
*   FUNCTION
*	Writes the word "begin" to the settings file previously opened with 
*	DB_OpenSettingsWrite. Subsequent writes to the file will be indented.
*
*   INPUTS
*	settings - The DockSettings structure allocated by DB_OpenSettingsWrite.
*
*   RESULT
*	TRUE if a block begin was written or FALSE if it was not.
*
*   EXAMPLE
*	struct DockSettings *s;
*	if( s = DB_OpenSettingsWrite("ENV:DockBot.prefs") ) {
*	  if( DB_WriteBeginBlock(s) ) {
*	    ...
*	  }
*	  DB_CloseSettings(s);
*	}
*
*   SEE ALSO
*	DB_OpenSettingsWrite(), DB_WriteEndBlock()
*
***************************************************************************/
BOOL __asm __saveds DB_WriteBeginBlock(
    register __a0 struct DockSettings *settings);


/****** dockbot.library/DB_WriteEndBlock ***********************************
*
*   NAME
*	DB_WriteEndBlock -- Writes the end of a block to the settings file.
*
*   SYNOPSIS
*	success = DB_WriteBeginEnd(settings)
*	  D0                         A0       
*	BOOL DB_WriteBeginEnd(struct DockSettings *);
*
*   FUNCTION
*	Writes the word "end" to the settings file previously opened with 
*	DB_OpenSettingsWrite.
*
*   INPUTS
*	settings - The DockSettings structure allocated by DB_OpenSettingsWrite.
*
*   RESULT
*	TRUE if a block end was written or FALSE if it was not.
*
*   EXAMPLE
*	struct DockSettings *s;
*	if( s = DB_OpenSettingsWrite("ENV:DockBot.prefs") ) {
*	  if( DB_WriteBeginBlock(s) ) {
*	    ...
*	    if( DB_WriteEndBlock(s) ) {
*	      ...
*	    }
*	  }
*	  DB_CloseSettings(s);
*	}
*
*   SEE ALSO
*	DB_OpenSettingsWrite(), DB_WriteBeginBlock()
*
***************************************************************************/
BOOL __asm __saveds DB_WriteEndBlock(
    register __a0 struct DockSettings *settings);


/****** dockbot.library/DB_WriteSetting ************************************
*
*   NAME
*	DB_WriteSetting -- Writes a key and value pair to the settings file.
*
*   SYNOPSIS
*	success = DB_WriteSetting(settings,key,value)
*	  D0                        A0     A1  A2  
*	BOOL DB_WriteBeginEnd(struct DockSettings *,STRPTR,STRPTR);
*
*   FUNCTION
*	Writes a key and value to a settings file previously opened with
*	DB_OpenSettingsWrite. An equals sign, =, will separate the two.
*
*   INPUTS
*	settings - The DockSettings structure allocated by DB_OpenSettingsWrite.
*	key      - A pointer to a NULL terminated string containing the key.
*	value    - A pointer to a NULL terminated string containing the value.
*
*   RESULT
*	TRUE if the setting was written or FALSE if it was not.
*
*   EXAMPLE
*	struct DockSettings *s;
*	if( s = DB_OpenSettingsWrite("ENV:DockBot.prefs") ) {
*	  if( DB_WriteBeginBlock(s) ) {
*	    if( DB_WriteSetting(s, "key", "value") ) {
*	      ...
*	    }
*	  }
*	  DB_CloseSettings(s);
*	}
*
*   SEE ALSO
*	DB_OpenSettingsWrite(), DB_WriteBeginBlock()
*
***************************************************************************/
BOOL __asm __saveds DB_WriteSetting(
    register __a0 struct DockSettings *settings, 
    register __a1 STRPTR key, 
    register __a2 STRPTR value);


BOOL __asm __saveds DB_ReadConfig(
    register __a0 struct DockConfig *cfg,
    register __a1 struct DockSettings *settings);


BOOL __asm __saveds DB_WriteConfig(
    register __a0 struct DockConfig *cfg,
    register __a1 struct DockSettings *settings);


/****** dockbot.library/DB_RequestDockQuit *********************************
*
*   NAME
*	DB_RequestDockQuit -- Asks the DockBot application to quit.
*
*   SYNOPSIS
*	DB_RequestDockQuit(obj)
*	                   A0  
*	DB_RequestDockQuit(Object *);
*
*   FUNCTION
*	Sends an asynchronous message from a gadget to the DockBot application 
*	requesting that it quit.
*
*   INPUTS
*	obj - The dock gadget requesting the quit.
*
*   EXAMPLE
*	void request_quit(Object *obj) {
*	  DB_RequestDockQuit(obj);
*	}
*
*   SEE ALSO
*	dockbot.h
*
***************************************************************************/
VOID __asm __saveds DB_RequestDockQuit(
	register __a0 Object *obj);


/****** dockbot.library/DB_RequestDockGadgetDraw ****************************
*
*   NAME
*	DB_RequestDockGadgetDraw -- Requests the redraw of a dock gadget.
*
*   SYNOPSIS
*	DB_RequestDockGadgetDraw(obj)
*	                         A0  
*	DB_RequestDockGadgetDraw(Object *);
*
*   FUNCTION
*	Sends an asynchronous message from a gadget to the DockBot application 
*	requesting that it be redrawn.
*
*   INPUTS
*	obj - The dock gadget requesting to be redrawn.
*
*   EXAMPLE
*	void request_draw(Object *obj) {
*	  DB_RequestDockGadgetDraw(obj);
*	}
*
*   SEE ALSO
*	dockbot.h
*
***************************************************************************/
VOID __asm __saveds DB_RequestDockGadgetDraw(
	register __a0 Object *obj);


/****** dockbot.library/DB_AllocMem ****************************************
*
*   NAME
*	DB_AllocMem -- Allocate memory from the pools maintained by the library.
*
*   SYNOPSIS
*	BLOCK = DB_AllocMem(byteSize, attributes)
*	  D0                  D0         D1
*	VOID* DB_AllocMem(ULONG, ULONG);
*
*   FUNCTION
*	Allocates a block of memory from the pools maintained internally by the
*	DockBot library.
*
*   INPUTS
*	byteSize   - The size, in bytes, to allocate.
*	attributes - The requirements and options for the memory. This is a 
*	             subset of those accepted by the AllocMem function.
*
*	             requirements:
*
*	               MEMF_CHIP  - Allocate chip memory.
*	               
*	               MEMF_FAST  - Attempt to allocate fast memory.
*
*	             options:
*
*	               MEMF_CLEAR - The memory will be initialized to all zeros.
*
*   RESULT
*	The newly allocated block of memory will be returned. If the request
*	could not be satisfied 0 will be returned.
*
*   EXAMPLE
*	DB_AllocMem(64,0) - Allocate the best memory available.
*	DB_AllocMem(64, MEMF_CLEAR) - Allocate the best memory available and
*	                              clear it.
*	DB_AllocMem(64, MEMF_CHIP|MEMF_CLEAR) - Allocate cleared chip memory.
*
*   SEE ALSO
*	DB_FreeMem(), AllocMem()
*
***************************************************************************/
VOID* __asm __saveds DB_AllocMem(
    register __d0 ULONG byteSize,
    register __d1 ULONG attributes);


/****** dockbot.library/DB_FreeMem *****************************************
*
*   NAME
*	DB_FreeMem - Free a block of memory allocated by DB_AllocMem.
*
*   SYNOPSIS
*	DB_FreeMem(memoryBlock,byteSize)
*	              A0         D0
*	DB_FreeMem(VOID *,ULONG);
*
*   FUNCTION
*	Returns a block of memory to the pools managed by the DockBot library.
*	Only use this function with memory blocks previously allocated by the
*	DB_AllocMem function.
*
*   INPUTS
*	memoryBlock - The block of memory allocated by DB_AllocMem.
*	byteSize    - The size of the block.
*
*   SEE ALSO
*	DB_AllocMem(), FreeMem()
*
***************************************************************************/
VOID __asm __saveds DB_FreeMem(
    register __a0 VOID *memoryBlock,
    register __d0 ULONG byteSize);


VOID* __asm __saveds DB_GetMemInfo(VOID);

Object * __asm __saveds DB_CreateDockGadget(
    register __a0 STRPTR name);

BOOL __asm __saveds DB_ListClasses(
    register __a0 struct List *list);


/****** dockbot.library/DB_ShowError ***************************************
*
*   NAME
*	DB_ShowError - Display an error message to the user.
*
*   SYNOPSIS
*	DB_FreeMem(message)
*	              A0
*	DB_FreeMem(STRPTR);
*
*   FUNCTION
*   Displays the message to the user in a requester.
*
*   INPUTS
*   message - The error message to display to the user.
*
***************************************************************************/
VOID __asm __saveds DB_ShowError(
    register __a0 STRPTR message);

#endif