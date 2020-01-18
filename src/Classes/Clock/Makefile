
LIBNAME=	DockClock
VERSION=	1
REVISION=	0

INC=		INCLUDEDIRECTORY=///include INCLUDEDIRECTORY=/
DIR=		/bin

LIBFILE=	$(LIBNAME).class
LIBDIR=		//$(DIR)/Gadgets/

FD_FILE=	/template.fd
PRAGMA_FILE=	pragmas.h

C_OPTS=		CPU=$(CPU) $(SC_OPTS) NOSTANDARDIO $(INC) $(DEF_DS) $(DEF_BV)

C_SOURCES=	lib.c clock.c dockclock_cat.c

OBJECTS=	lib.o clock.o dockclock_cat.o

LIBS=		LIB:sc.lib LIB:amiga.lib LIB:triton.lib

################
#
# Build Library

$(LIBFILE): catalog $(OBJECTS) $(LIBS) $(PRAGMA_FILE)
	$(LINKER) WITH << 
TO $(LIBDIR)$(LIBFILE)
FROM $(OBJECTS)
LIB $(LIBS) $(LD_OPTS)
<

$(PRAGMA_FILE): $(FD_FILE).h

.c.o:
	$(COMPILER) $(C_OPTS) $*.c

lib.c: /template.c
	$(COMPILER) $(C_OPTS) $*.c

$(FD_FILE).h:
	$(FD_CONV) $(FD_FILE) $(PRAGMA_FILE)

cleanobj:
	@Delete $(OBJECTS)

clean:
	@Delete $(OBJECTS)
	@Delete $(LIBDIR)$(LIBFILE)(|.info)
	@Delete $(PRAGMA_FILE)

reload:
	@copy $(LIBDIR)$(LIBFILE) LIBS:
	@version $(LIBFILE)
	@flushlibs

catalog: //Locale/Catalogs/DockClock.cd
	@Execute << 
		flexcat //Locale/Catalogs/DockClock.cd dockclock_cat.c=//Locale/Class_c.sd NOBEEP
		flexcat //Locale/Catalogs/DockClock.cd dockclock_cat.h=//Locale/Class_h.sd NOBEEP
		echo "Catalog Updated"
	<
