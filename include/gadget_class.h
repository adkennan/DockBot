/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2016 Andrew Kennan
**
************************************/

#ifndef __GADGET_CLASS_H__
#define __GADGET_CLASS_H__

//#define CLASS_NAME      
//#define CLASS_VERSION  
//#define CLASS_REVISION
//#define CLASS_VER_STR   
//#define CLASS_DESC		
//#define CLASS_COPYRIGHT 
//#define PRIVATE_DATA    
#define PARENT_CLASS    DB_ROOT_CLASS

//#define GADGET_LIB_DATA     

//#define GADGET_LIB_INIT     
//#define GADGET_LIB_EXPUNGE  

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
//#define METHOD_GETSETTINGS
  
//#define METHOD_GETHOTKEY
//#define METHOD_GETLABEL

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


#endif 
