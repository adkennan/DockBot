@DATABASE "gadget_class.h"
@MASTER   "Stuff:Andrew/DockBot/include/gadget_class.h"
@REMARK   This file was created by ADtoHT 2.1 on 21-Jun-19 17:12:28
@REMARK   Do not edit
@REMARK   ADtoHT is © 1993-1995 Christian Stieber

@NODE MAIN "gadget_class.h"

@{"gadget_class.h" LINK File}


@{b}#defines@{ub}

@{"CLASS_COPYRIGHT" LINK "gadget_class.h/File" 21}   @{"CLASS_DESC" LINK "gadget_class.h/File" 20}          @{"CLASS_NAME" LINK "gadget_class.h/File" 16}
@{"CLASS_REVISION" LINK "gadget_class.h/File" 18}    @{"CLASS_VER_STR" LINK "gadget_class.h/File" 19}       @{"CLASS_VERSION" LINK "gadget_class.h/File" 17}
@{"DB_METHOD()" LINK "gadget_class.h/File" 85}       @{"DB_METHOD_D()" LINK "gadget_class.h/File" 73}       @{"DB_METHOD_DM()" LINK "gadget_class.h/File" 66}
@{"DB_METHOD_M()" LINK "gadget_class.h/File" 79}     @{"GADGET_LIB_DATA" LINK "gadget_class.h/File" 29}     @{"GADGET_LIB_EXPUNGE" LINK "gadget_class.h/File" 33}
@{"GADGET_LIB_INIT" LINK "gadget_class.h/File" 32}   @{"METHOD_ADDED" LINK "gadget_class.h/File" 40}        @{"METHOD_CANEDIT" LINK "gadget_class.h/File" 54}
@{"METHOD_CLICK" LINK "gadget_class.h/File" 43}      @{"METHOD_DISPOSE" LINK "gadget_class.h/File" 38}      @{"METHOD_DRAW" LINK "gadget_class.h/File" 48}
@{"METHOD_DROP" LINK "gadget_class.h/File" 44}       @{"METHOD_EDITOREVENT" LINK "gadget_class.h/File" 56}  @{"METHOD_EDITORUPDATE" LINK "gadget_class.h/File" 57}
@{"METHOD_GETEDITOR" LINK "gadget_class.h/File" 55}  @{"METHOD_GETHOTKEY" LINK "gadget_class.h/File" 59}    @{"METHOD_GETLABEL" LINK "gadget_class.h/File" 60}
@{"METHOD_GETSIZE" LINK "gadget_class.h/File" 47}    @{"METHOD_HOTKEY" LINK "gadget_class.h/File" 45}       @{"METHOD_INITEDIT" LINK "gadget_class.h/File" 53}
@{"METHOD_LAUNCHED" LINK "gadget_class.h/File" 62}   @{"METHOD_NEW" LINK "gadget_class.h/File" 37}          @{"METHOD_READCONFIG" LINK "gadget_class.h/File" 50}
@{"METHOD_REMOVED" LINK "gadget_class.h/File" 41}    @{"METHOD_TICK" LINK "gadget_class.h/File" 42}         @{"METHOD_WRITECONFIG" LINK "gadget_class.h/File" 51}
@{"PARENT_CLASS" LINK "gadget_class.h/File" 23}      @{"PRIVATE_DATA" LINK "gadget_class.h/File" 26}        

@ENDNODE
@NODE File "gadget_class.h"
/************************************
**
**  DockBot - A Dock For AmigaOS 3
**
**  © 2019 Andrew Kennan
**
************************************/

#ifndef __GADGET_CLASS_H__
#define __GADGET_CLASS_H__

VOID geta4(VOID);

// Class Information

//#define CLASS_NAME
//#define CLASS_VERSION
//#define CLASS_REVISION
//#define CLASS_VER_STR
//#define CLASS_DESC
//#define CLASS_COPYRIGHT

#define PARENT_CLASS    @{"DB_ROOT_CLASS" LINK "dockbot.h/File" 68}

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
//#define @{"METHOD_WRITECONFIG" LINK "File" 52}
//#define @{"METHOD_WRITECONFIG" LINK "File" 51}
//#define METHOD_INITEDIT
//#define METHOD_CANEDIT
//#define METHOD_GETEDITOR
//#define METHOD_EDITOREVENT
//#define METHOD_EDITORUPDATE

//#define METHOD_GETHOTKEY
//#define METHOD_GETLABEL

//#define METHOD_LAUNCHED

// Method with data and message.

#define DB_METHOD_DM(NAME,PARAM) ULONG __saveds METHOD_ ## NAME(Class *c, Object *o, Msg m) \\
{ \\
    struct PARAM *msg = (struct PARAM *)m; \\
    struct PRIVATE_DATA *data = INST_DATA(c, o);

// Method with data and no message.

#define DB_METHOD_D(NAME) ULONG __saveds METHOD_ ## NAME(Class *c, Object *o, Msg m) \\
{ \\
    struct PRIVATE_DATA *data = INST_DATA(c, o);

// Method with message and no data.

#define DB_METHOD_M(NAME,PARAM) ULONG __saveds METHOD_ ## NAME(Class *c, Object *o, Msg m) \\
{ \\
    struct PARAM *msg = (struct PARAM *)m;

// Method with no data or message.

#define DB_METHOD(NAME) ULONG __saveds METHOD_ ## NAME(Class *c, Object *o, Msg m) \\
{

#endif
@ENDNODE
