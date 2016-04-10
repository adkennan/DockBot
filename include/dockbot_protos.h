
#ifndef __DOCKBOT_PROTOS_H__
#define __DOCKBOT_PROTOS_H__

#include <exec/types.h>
#include <intuition/intuition.h>

#include "dockbot.h"

VOID __asm __saveds GetDockGadgetBounds(
	register __a0 Object *obj, 
	register __a1 struct Rect *bounds);

VOID __asm __saveds DrawOutsetFrame(
	register __a0 struct RastPort *rp, 
	register __a1 struct Rect *bounds);

VOID __asm __saveds DrawInsetFrame(
	register __a0 struct RastPort *rp, 
	register __a1 struct Rect *bounds);


struct DockSettings * __asm __saveds OpenSettingsRead(
    register __a0 STRPTR filename);

struct DockSettings * __asm __saveds OpenSettingsWrite(
    register __a0 STRPTR filename);

VOID __asm __saveds CloseSettings(
    register __a0 struct DockSettings *settings);

BOOL __asm __saveds ReadBeginBlock(
    register __a0 struct DockSettings *settings);

BOOL __asm __saveds ReadEndBlock(
    register __a0 struct DockSettings *settings);

BOOL __asm __saveds ReadSetting(
    register __a0 struct DockSettings *settings, 
    register __a1 struct DockSettingValue *value);

BOOL __asm __saveds WriteBeginBlock(
    register __a0 struct DockSettings *settings);

BOOL __asm __saveds WriteEndBlock(
    register __a0 struct DockSettings *settings);

BOOL __asm __saveds WriteSetting(
    register __a0 struct DockSettings *settings, 
    register __a1 STRPTR key, 
    register __a2 STRPTR value);

#endif