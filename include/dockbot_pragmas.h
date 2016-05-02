/************************************/
/*                                  */
/*  DockBot - A Dock For AmigaOS 3  */
/*                                  */
/*  � 2016 Andrew Kennan            */
/*                                  */
/************************************/
/* Dock gadget functions*/
#pragma libcall DockBotBase DB_GetDockGadgetBounds 1e 9802
#pragma libcall DockBotBase DB_RequestDockQuit 24 801
#pragma libcall DockBotBase DB_RequestDockGadgetDraw 2a 801
/* Drawing utilities*/
#pragma libcall DockBotBase DB_DrawOutsetFrame 30 9802
#pragma libcall DockBotBase DB_DrawInsetFrame 36 9802
/* Settings functions*/
#pragma libcall DockBotBase DB_OpenSettingsRead 3c 801
#pragma libcall DockBotBase DB_OpenSettingsWrite 42 801
#pragma libcall DockBotBase DB_CloseSettings 48 801
#pragma libcall DockBotBase DB_ReadBeginBlock 4e 801
#pragma libcall DockBotBase DB_ReadEndBlock 54 801
#pragma libcall DockBotBase DB_ReadSetting 5a 9802
#pragma libcall DockBotBase DB_WriteBeginBlock 60 801
#pragma libcall DockBotBase DB_WriteEndBlock 66 801
#pragma libcall DockBotBase DB_WriteSetting 6c A9803
/* Memory Management*/
#pragma libcall DockBotBase DB_AllocMem 72 1002
#pragma libcall DockBotBase DB_FreeMem 78 0802
#pragma libcall DockBotBase DB_GetMemInfo 7e 0
