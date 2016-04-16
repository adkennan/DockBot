/* Dock gadget functions*/
#pragma libcall DockBotBase GetDockGadgetBounds 1e 9802
#pragma libcall DockBotBase RequestDockQuit 24 801
#pragma libcall DockBotBase RequestDockGadgetDraw 2a 801
/* Drawing utilities*/
#pragma libcall DockBotBase DrawOutsetFrame 30 9802
#pragma libcall DockBotBase DrawInsetFrame 36 9802
/* Settings functions*/
#pragma libcall DockBotBase OpenSettingsRead 3c 801
#pragma libcall DockBotBase OpenSettingsWrite 42 801
#pragma libcall DockBotBase CloseSettings 48 801
#pragma libcall DockBotBase ReadBeginBlock 4e 801
#pragma libcall DockBotBase ReadEndBlock 54 801
#pragma libcall DockBotBase ReadSetting 5a 9802
#pragma libcall DockBotBase WriteBeginBlock 60 801
#pragma libcall DockBotBase WriteEndBlock 66 801
#pragma libcall DockBotBase WriteSetting 6c A9803
