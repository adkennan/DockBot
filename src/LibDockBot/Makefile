
LIBNAME=	dockbot
VERSION=	1
REVISION=	0

INC=		//include
DIR=		/bin


LIBFILE=	$(LIBNAME).library
LIBDIR=		/$(DIR)/

FD_CONV=	SC:c/fd2pragma
FD_FILE=	$(LIBNAME).fd
PRAGMA_FILE=	$(INC)/$(LIBNAME)_pragmas.h

DEF_DEBUG=
DEF_STRIP=
DEF_OPT

C_COMPILER=	SC:c/sc

CPU=		68030
C_OPTS=		CPU=$(CPU) NOICONS STREQ $(DEF_OPT) STRINGMERGE NOSTKCHK NOSTANDARDIO INCLUDEDIRECTORY=$(INC) $(DEF_DEBUG)

LINKER=		SC:c/slink

C_SOURCES=	lib.c gfx.c gadget.c settings.c memory.c dock_gadget.c classes.c req.c

OBJECTS=	lib.o gfx.o gadget.o settings.o memory.o dock_gadget.o classes.o req.o

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
LIB $(LIBS) NOICONS $(DEF_STRIP) SMALLDATA
<

$(PRAGMA_FILE): $(FD_FILE).h

.c.o:
	$(C_COMPILER) $(C_OPTS) $*.c

$(FD_FILE).h:
	$(FD_CONV) $(FD_FILE) $(PRAGMA_FILE)

cleanobj:
	@Delete $(OBJECTS)

clean:
	@Delete $(OBJECTS)
	@Delete $(LIBDIR)$(LIBFILE)(|.info)
	@Delete $(PRAGMA_FILE)

reload:
	@version $(LIBFILE)
	@flushlibs