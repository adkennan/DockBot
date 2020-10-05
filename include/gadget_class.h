/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#ifndef __GADGET_CLASS_H__
#define __GADGET_CLASS_H__

#include <utility/tagitem.h>

VOID geta4(VOID);

// Class Information

//#define CLASS_NAME      
//#define CLASS_VERSION  
//#define CLASS_REVISION
//#define CLASS_VER_STR   
//#define CLASS_DESC		
//#define CLASS_COPYRIGHT 

#define PARENT_CLASS    DB_ROOT_CLASS

// Instance Data
//#define PRIVATE_DATA    

// Class Data
//#define GADGET_LIB_DATA     

// Class Set Up / Tear Down
//#define GADGET_LIB_INIT     
//#define GADGET_LIB_EXPUNGE  

// Instance Methods

//#define METHOD_NEW          
//#define METHOD_DISPOSE      

//#define METHOD_ADDED
//#define METHOD_REMOVED
//#define METHOD_TICK         
//#define METHOD_CLICK        
//#define METHOD_DROP     
//#define METHOD_HOTKEY		

//#define METHOD_GETSIZE      
//#define METHOD_DRAW         

//#define METHOD_READCONFIG   
//#define METHOD_WRITECONFIG
//#define METHOD_WRITECONFIG
//#define METHOD_INITEDIT
//#define METHOD_CANEDIT		
//#define METHOD_GETEDITOR
//#define METHOD_EDITOREVENT
//#define METHOD_EDITORUPDATE
  
//#define METHOD_GETHOTKEY
//#define METHOD_GETLABEL

//#define METHOD_LAUNCHED

//#define METHOD_MESSAGE

// Method with data and message.

#define DB_METHOD_DM(NAME,PARAM) ULONG __saveds METHOD_ ## NAME(Class *c, Object *o, Msg m) \
{ \
    struct PARAM *msg = (struct PARAM *)m; \
    struct PRIVATE_DATA *data = INST_DATA(c, o);

// Method with data and no message.

#define DB_METHOD_D(NAME) ULONG __saveds METHOD_ ## NAME(Class *c, Object *o, Msg m) \
{ \
    struct PRIVATE_DATA *data = INST_DATA(c, o);

// Method with message and no data.

#define DB_METHOD_M(NAME,PARAM) ULONG __saveds METHOD_ ## NAME(Class *c, Object *o, Msg m) \
{ \
    struct PARAM *msg = (struct PARAM *)m; 

// Method with no data or message.

#define DB_METHOD(NAME) ULONG __saveds METHOD_ ## NAME(Class *c, Object *o, Msg m) \
{ 

#ifdef DEBUG_BUILD

VOID DB_Printf(STRPTR fmt, ...);

#define __METHOD__ CLASS_NAME "::" __FUNC__ ": "

#endif

struct TagItem *DB_MakeTagList(ULONG data, ...);

APTR __saveds __asm GetLibData(VOID);

#endif

