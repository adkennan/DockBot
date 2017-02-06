
EXENAME=	DockBotPrefs
EXEPATH=	//bin/

INC=		INCLUDEDIRECTORY=//include INCLUDEDIRECTORY=/shared

COMPILER=	SC:c/sc
LINKER=		SC:c/slink

LIBS=		LIB:sc.lib LIB:amiga.lib LIB:triton.lib

CPU=		68030
DEF_DEBUG_MEM=
DEF_DEBUG=

C_OPTS=		opt noicons cpu=$(CPU) $(INC) $(DEF_DEBUG_MEM) $(DEF_DEBUG)

C_SOURCES=	main.c /shared/dock_gadget.c settings.c

OBJECTS=	main.o /shared/dock_gadget.o settings.o

$(EXENAME): $(OBJECTS)
   $(LINKER) WITH <<
 TO $(EXEPATH)$(EXENAME)
 FROM LIB:c.o $(OBJECTS)
 LIBRARY $(LIBS) NOICONS ADDSYM
<
.c.o:
	$(COMPILER) $(C_OPTS) $*.c

clean:
	@Delete $(OBJECTS)
	@Delete $(EXEPATH)$(EXENAME) 
