
LIBNAME=	dockbot
VERSION=	1
REVISION=	0

INC=		//include

LIBFILE=	$(LIBNAME).library
LIBDIR=		//bin/

FD_CONV=	SC:c/fd2pragma
FD_FILE=	$(LIBNAME).fd
PRAGMA_FILE=	$(INC)/$(LIBNAME)_pragmas.h

DEF_DEBUG=

C_COMPILER=	SC:c/sc

CPU=		68030
C_OPTS=		CPU=$(CPU) NOICONS STREQ STRMER NOSTKCHK NOSTANDARDIO INCLUDEDIRECTORY=$(INC) $(DEF_DEBUG)

LINKER=		SC:c/slink

C_SOURCES=	lib.c gfx.c gadget.c settings.c memory.c dock_gadget.c classes.c

OBJECTS=	lib.o gfx.o gadget.o settings.o memory.o dock_gadget.o classes.o

LIBS=		LIB:sc.lib LIB:amiga.lib

################
#
# Build Library

$(LIBFILE): $(OBJECTS) $(LIBS) $(PRAGMA_FILE)
	$(LINKER) WITH << 
TO $(LIBDIR)$(LIBFILE)
FROM $(OBJECTS)
LIBFD $(FD_FILE)
LIBVERSION $(VERSION)
LIBREVISION $(REVISION)
LIB $(LIBS) NOICONS ADDSYM
<

$(PRAGMA_FILE): $(FD_FILE).h

.c.o:
	$(C_COMPILER) $(C_OPTS) $*.c

$(FD_FILE).h:
	$(FD_CONV) $(FD_FILE) $(PRAGMA_FILE)

clean:
	@Delete $(OBJECTS)
	@Delete $(LIBDIR)$(LIBFILE)(|.info)
	@Delete $(PRAGMA_FILE)

reload:
	@version $(LIBFILE)
	@flushlibs