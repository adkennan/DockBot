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

#include "dockbot.h"

VOID __asm __saveds DB_GetDockGadgetBounds(
	register __a0 Object *obj, 
	register __a1 struct Rect *bounds);

VOID __asm __saveds DB_DrawOutsetFrame(
	register __a0 struct RastPort *rp, 
	register __a1 struct Rect *bounds);

VOID __asm __saveds DB_DrawInsetFrame(
	register __a0 struct RastPort *rp, 
	register __a1 struct Rect *bounds);


struct DockSettings * __asm __saveds DB_OpenSettingsRead(
    register __a0 STRPTR filename);

struct DockSettings * __asm __saveds DB_OpenSettingsWrite(
    register __a0 STRPTR filename);

VOID __asm __saveds DB_CloseSettings(
    register __a0 struct DockSettings *settings);

BOOL __asm __saveds DB_ReadBeginBlock(
    register __a0 struct DockSettings *settings);

BOOL __asm __saveds DB_ReadEndBlock(
    register __a0 struct DockSettings *settings);

BOOL __asm __saveds DB_ReadSetting(
    register __a0 struct DockSettings *settings, 
    register __a1 struct DockSettingValue *value);

BOOL __asm __saveds DB_WriteBeginBlock(
    register __a0 struct DockSettings *settings);

BOOL __asm __saveds DB_WriteEndBlock(
    register __a0 struct DockSettings *settings);

BOOL __asm __saveds DB_WriteSetting(
    register __a0 struct DockSettings *settings, 
    register __a1 STRPTR key, 
    register __a2 STRPTR value);


VOID __asm __saveds DB_RequestDockQuit(
	register __a0 Object *obj);


VOID __asm __saveds DB_RequestDockGadgetDraw(
	register __a0 Object *obj);


VOID* __asm __saveds DB_AllocMem(
    register __d0 ULONG byteSize,
    register __d1 ULONG attributes);

VOID __asm __saveds DB_FreeMem(
    register __a0 VOID *memoryBlock,
    register __d0 ULONG byteSize);

VOID* __asm __saveds DB_GetMemInfo(VOID);

#endif