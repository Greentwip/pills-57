# Ç±ÇÃÃß≤ŸÇÕçXêVÇ≥ÇÍÇ‹ÇπÇÒ
INC = $(ROOT)/usr/include
CC  = gcc
AS  = as

APP     = rom.out
TARGETS = rom.n64

include source.mak
OBJECTS = $(SOURCES:.c=.o)

GCCFLAG   = -c -I. -I$(INC)/PR -I$(INC) -D_MIPS_SZLONG=32 -D_MIPS_SZINT=32
OPTIMIZER = -G 0 -DF3DEX_GBI_2 -DNDEBUG -D_FINALROM
CFLAGS    = $(GCCFLAG) $(OPTIMIZER)

default: $(TARGETS)

.c.o:
	$(CC) $(CFLAGS) $<

$(TARGETS) $(APP): spec $(OBJECTS)
