
EXENAME=	DockBotPrefs
EXEPATH=	//bin/

INC=		INCLUDEDIRECTORY=//include INCLUDEDIRECTORY=MUI:Developer/C/Include

COMPILER=	SC:c/sc
LINKER=		SC:c/slink

LIBS=		LIB:sc.lib LIB:amiga.lib 

C_OPTS=		$(INC)

C_SOURCES=	main.c 

OBJECTS=	main.o 

$(EXENAME): $(OBJECTS)
   $(LINKER) WITH <<
 TO $(EXEPATH)$(EXENAME)
 FROM LIB:c.o $(OBJECTS)
 LIBRARY $(LIBS)
<

.c.o:
	$(COMPILER) $(C_OPTS) $*.c

clean:
	@Delete $(OBJECTS)
	@Delete $(EXEPATH)$(EXENAME)
