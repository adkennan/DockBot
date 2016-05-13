
LIBNAME=	dockbot
VERSION=	1
REVISION=	0

INC=		//include

LIBFILE=	$(LIBNAME).library
LIBDIR=		//bin/

FD_CONV=	SC:c/fd2pragma
FD_FILE=	$(LIBNAME).fd
PRAGMA_FILE=	$(INC)/$(LIBNAME)_pragmas.h

C_COMPILER=	SC:c/sc

CPU=		68030
C_OPTS=		CPU=$(CPU) STREQ STRMER NOSTKCHK NOSTANDARDIO LIBCODE INCLUDEDIRECTORY=$(INC)

LINKER=		SC:c/slink

C_SOURCES=	lib.c gfx.c gadget.c settings.c memory.c

OBJECTS=	lib.o gfx.o gadget.o settings.o memory.o

LIBS=		LIB:sc.lib LIB:amiga.lib

LIBENT=		LIB:libent.o
LIBINIT=	LIB:libinit.o

################
#
# Build Library

$(LIBFILE): $(OBJECTS) $(LIBS) $(PRAGMA_FILE)
	$(LINKER) WITH << 
TO $(LIBDIR)$(LIBFILE)
FROM $(LIBENT) $(LIBINIT) $(OBJECTS)
LIBFD $(FD_FILE)
LIBVERSION $(VERSION)
LIBREVISION $(REVISION)
LIB $(LIBS)
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