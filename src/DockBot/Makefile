
DIR=		/bin

EXENAME=	DockBot
EXEPATH=	/$(DIR)/

INC=		INCLUDEDIRECTORY=//include INCLUDEDIRECTORY=/shared

COMPILER=	SC:c/sc
LINKER=		SC:c/slink

LIBS=		LIB:sc.lib LIB:amiga.lib 

CPU=		68030

DEF_DEBUG_MEM=
DEF_DEBUG=
DEF_STRIP=
DEF_OPT=

C_OPTS=		noicons cpu=$(CPU) $(DEF_OPT) STRINGMERGE $(INC) $(DEF_DEBUG_MEM) $(DEF_DEBUG)

C_SOURCES=	main.c dock.c /shared/dock_gadget.c dock_handle.c events.c gadgets.c layout.c settings.c debug.c

OBJECTS=	main.o dock.o /shared/dock_gadget.o dock_handle.o events.o gadgets.o layout.o settings.o debug.o

$(EXENAME): $(OBJECTS)
   $(LINKER) WITH <<
 TO $(EXEPATH)$(EXENAME)
 FROM LIB:c.o $(OBJECTS)
 LIBRARY $(LIBS) NOICONS $(DEF_STRIP) SMALLDATA
<

.c.o:
	$(COMPILER) $(C_OPTS) $*.c

cleanobj:
	@Delete $(OBJECTS)

clean:
	@Delete $(OBJECTS)
	@Delete $(EXEPATH)$(EXENAME) 
