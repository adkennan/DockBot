
EXENAME=	DockBotPrefs
EXEPATH=	//bin/

INC=		//include

COMPILER=	SC:c/sc
LINKER=		SC:c/slink

LIBS=		LIB:sc.lib LIB:amiga.lib LIB:triton.lib

CPU=		68030
DEF_DEBUG_MEM=

C_OPTS=		opt noicons cpu=$(CPU) INCLUDEDIRECTORY=$(INC) $(DEF_DEBUG_MEM)

C_SOURCES=	main.c ui.c dock_gadget.c prefs_base.c settings.c dock_button.c

OBJECTS=	main.o ui.o dock_gadget.o prefs_base.o settings.o dock_button.o

$(EXENAME): $(OBJECTS)
   $(LINKER) WITH <<
 TO $(EXEPATH)$(EXENAME)
 FROM LIB:c.o $(OBJECTS)
 LIBRARY $(LIBS) NOICONS
<
.c.o:
	$(COMPILER) $(C_OPTS) $*.c

clean:
	@Delete $(OBJECTS)
	@Delete $(EXEPATH)$(EXENAME) 
