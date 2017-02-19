
EXENAME=	DockBot
EXEPATH=	//bin/

INC=		INCLUDEDIRECTORY=//include INCLUDEDIRECTORY=/shared

COMPILER=	SC:c/sc
LINKER=		SC:c/slink

LIBS=		LIB:sc.lib LIB:amiga.lib 

CPU=		68030

DEF_DEBUG_MEM=
DEF_DEBUG=opt

C_OPTS=		noicons cpu=$(CPU) $(INC) $(DEF_DEBUG_MEM) $(DEF_DEBUG)

C_SOURCES=	main.c dock.c /shared/dock_gadget.c dock_handle.c events.c gadgets.c layout.c settings.c debug.c

OBJECTS=	main.o dock.o /shared/dock_gadget.o dock_handle.o events.o gadgets.o layout.o settings.o debug.o

$(EXENAME): $(OBJECTS)
   $(LINKER) WITH <<
 TO $(EXEPATH)$(EXENAME)
 FROM LIB:c.o $(OBJECTS)
 LIBRARY $(LIBS) NOICONS ADDSYM
<
	@Copy /Images/$(EXENAME).info $(EXEPATH)
	@Copy /Images/$(EXENAME)Min.info $(EXEPATH)

.c.o:
	$(COMPILER) $(C_OPTS) $*.c

clean:
	@Delete $(OBJECTS)
	@Delete $(EXEPATH)$(EXENAME) $(EXEPATH)$(EXENAME).info $(EXEPATH)$(EXENAME)Min.info
