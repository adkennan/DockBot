
/****************************************************************

   This file was created automatically by `FlexCat 2.18'
   from "/Locale/Catalogs/DockBotPrefs.cd".

   Do NOT edit by hand!

****************************************************************/

#ifndef DockBotPrefs_CAT_H
#define DockBotPrefs_CAT_H


#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif


/*
**  Prototypes
*/
#if !defined(__GNUC__)  && !defined(__SASC)  &&  !defined(_DCC)
extern VOID OpenDockBotPrefsCatalog(VOID);
extern VOID CloseDockBotPrefsCatalog(VOID);
#endif
#ifdef LOCALIZE_V20
extern void InitDockBotPrefsCatalog(STRPTR);
#endif



struct FC_String {
    const UBYTE *msg;
    LONG id;
};

extern struct FC_String DockBotPrefs_Strings[40];

#define MSG_APP_Title (DockBotPrefs_Strings[0].msg)
#define _MSG_APP_Title (DockBotPrefs_Strings+0)
#define MSG_APP_Info (DockBotPrefs_Strings[1].msg)
#define _MSG_APP_Info (DockBotPrefs_Strings+1)
#define MSG_OK (DockBotPrefs_Strings[2].msg)
#define _MSG_OK (DockBotPrefs_Strings+2)
#define MSG_Cancel (DockBotPrefs_Strings[3].msg)
#define _MSG_Cancel (DockBotPrefs_Strings+3)
#define MSG_ERR_ClassList (DockBotPrefs_Strings[4].msg)
#define _MSG_ERR_ClassList (DockBotPrefs_Strings+4)
#define MSG_ERR_LoadConfig (DockBotPrefs_Strings[5].msg)
#define _MSG_ERR_LoadConfig (DockBotPrefs_Strings+5)
#define MSG_ERR_OpenWindow (DockBotPrefs_Strings[6].msg)
#define _MSG_ERR_OpenWindow (DockBotPrefs_Strings+6)
#define MSG_ERR_OpenLibrary (DockBotPrefs_Strings[7].msg)
#define _MSG_ERR_OpenLibrary (DockBotPrefs_Strings+7)
#define MSG_ERR_EditorFailure (DockBotPrefs_Strings[8].msg)
#define _MSG_ERR_EditorFailure (DockBotPrefs_Strings+8)
#define MSG_ERR_CantAddDirectory (DockBotPrefs_Strings[9].msg)
#define _MSG_ERR_CantAddDirectory (DockBotPrefs_Strings+9)
#define MSG_POS_Left (DockBotPrefs_Strings[10].msg)
#define _MSG_POS_Left (DockBotPrefs_Strings+10)
#define MSG_POS_Right (DockBotPrefs_Strings[11].msg)
#define _MSG_POS_Right (DockBotPrefs_Strings+11)
#define MSG_POS_Top (DockBotPrefs_Strings[12].msg)
#define _MSG_POS_Top (DockBotPrefs_Strings+12)
#define MSG_POS_Bottom (DockBotPrefs_Strings[13].msg)
#define _MSG_POS_Bottom (DockBotPrefs_Strings+13)
#define MSG_ALIGN_TopLeft (DockBotPrefs_Strings[14].msg)
#define _MSG_ALIGN_TopLeft (DockBotPrefs_Strings+14)
#define MSG_ALIGN_Center (DockBotPrefs_Strings[15].msg)
#define _MSG_ALIGN_Center (DockBotPrefs_Strings+15)
#define MSG_ALIGN_BottomRight (DockBotPrefs_Strings[16].msg)
#define _MSG_ALIGN_BottomRight (DockBotPrefs_Strings+16)
#define MSG_NGD_Title (DockBotPrefs_Strings[17].msg)
#define _MSG_NGD_Title (DockBotPrefs_Strings+17)
#define MSG_NGD_ChooseGadget (DockBotPrefs_Strings[18].msg)
#define _MSG_NGD_ChooseGadget (DockBotPrefs_Strings+18)
#define MSG_NGD_About (DockBotPrefs_Strings[19].msg)
#define _MSG_NGD_About (DockBotPrefs_Strings+19)
#define MSG_MENU_Project (DockBotPrefs_Strings[20].msg)
#define _MSG_MENU_Project (DockBotPrefs_Strings+20)
#define MSG_MENU_About (DockBotPrefs_Strings[21].msg)
#define _MSG_MENU_About (DockBotPrefs_Strings+21)
#define MSG_MENU_Quit (DockBotPrefs_Strings[22].msg)
#define _MSG_MENU_Quit (DockBotPrefs_Strings+22)
#define MSG_MW_Title (DockBotPrefs_Strings[23].msg)
#define _MSG_MW_Title (DockBotPrefs_Strings+23)
#define MSG_MW_DockSettings (DockBotPrefs_Strings[24].msg)
#define _MSG_MW_DockSettings (DockBotPrefs_Strings+24)
#define MSG_MW_Position (DockBotPrefs_Strings[25].msg)
#define _MSG_MW_Position (DockBotPrefs_Strings+25)
#define MSG_MW_Alignment (DockBotPrefs_Strings[26].msg)
#define _MSG_MW_Alignment (DockBotPrefs_Strings+26)
#define MSG_MW_ShowLabels (DockBotPrefs_Strings[27].msg)
#define _MSG_MW_ShowLabels (DockBotPrefs_Strings+27)
#define MSG_MW_Gadgets (DockBotPrefs_Strings[28].msg)
#define _MSG_MW_Gadgets (DockBotPrefs_Strings+28)
#define MSG_MW_NewGadget (DockBotPrefs_Strings[29].msg)
#define _MSG_MW_NewGadget (DockBotPrefs_Strings+29)
#define MSG_MW_EditGadget (DockBotPrefs_Strings[30].msg)
#define _MSG_MW_EditGadget (DockBotPrefs_Strings+30)
#define MSG_MW_DeleteGadget (DockBotPrefs_Strings[31].msg)
#define _MSG_MW_DeleteGadget (DockBotPrefs_Strings+31)
#define MSG_MW_GadgetUp (DockBotPrefs_Strings[32].msg)
#define _MSG_MW_GadgetUp (DockBotPrefs_Strings+32)
#define MSG_MW_GadgetDown (DockBotPrefs_Strings[33].msg)
#define _MSG_MW_GadgetDown (DockBotPrefs_Strings+33)
#define MSG_MW_ConfigSave (DockBotPrefs_Strings[34].msg)
#define _MSG_MW_ConfigSave (DockBotPrefs_Strings+34)
#define MSG_MW_ConfigTest (DockBotPrefs_Strings[35].msg)
#define _MSG_MW_ConfigTest (DockBotPrefs_Strings+35)
#define MSG_MW_ConfigUse (DockBotPrefs_Strings[36].msg)
#define _MSG_MW_ConfigUse (DockBotPrefs_Strings+36)
#define MSG_DEL_Title (DockBotPrefs_Strings[37].msg)
#define _MSG_DEL_Title (DockBotPrefs_Strings+37)
#define MSG_DEL_Text (DockBotPrefs_Strings[38].msg)
#define _MSG_DEL_Text (DockBotPrefs_Strings+38)
#define MSG_DEL_Buttons (DockBotPrefs_Strings[39].msg)
#define _MSG_DEL_Buttons (DockBotPrefs_Strings+39)

#endif
