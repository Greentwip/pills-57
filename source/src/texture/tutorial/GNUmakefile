
_FINAL = 1

ifndef LOCAL
LOCAL=JAPAN
endif

##############################################################################

LIB = $(ROOT)/usr/lib
LPR = $(LIB)/PR
INC = $(ROOT)/usr/include

#CC = gcc
#LD = ld
CC=mips-linux-gcc -fno-pic -I/usr/lib/gcc-lib/mips-linux/egcs-2.91.66/include/ -mcpu=4300 -g
LD=/usr/mips-linux/bin/ld

#MAKEROM  = mild
#MAKEMASK = makemask
MAKEROM  = $(ROOT)/usr/sbin/makerom
MAKEMASK = $(ROOT)/usr/sbin/makemask

#COMP  = $(DM64ROOT)\tool\comp
#COMP2 = $(DM64ROOT)\tool\compress -c9
#NMSYM = $(DM64ROOT)\tool\nmsym
#MKDEP = $(DM64ROOT)\tool\mkdep
#SEGSIZE = $(DM64ROOT)\tool\segsize

COMP2 = ../../tool/minigzip -9
NMSYM = ../../tool/extractseg.pl
SEGSIZE = ../../tool/segsize/segsize

APP     = data
TARGETS = data.n64

##############################################################################

CFLAGS  = $(LCDEFS) $(LCINCS) $(LCOPTS) $(GCCFLAG) $(OPTIMIZER)
AFLAGS  = -mcpu=vr4300 $(CFLAGS)
GCCFLAG = -c -D_MIPS_SZLONG=32 -D_MIPS_SZINT=32 -mno-mips-tfile

#HW_FLAGS = -DPTN64

ifdef _FINAL
LCDEFS = $(HW_FLAGS) -DLOCAL=$(LOCAL) -DF3DEX_GBI_2 -DNDEBUG -D_FINALROM
LCOPTS = -G 0
else
LCDEFS = $(HW_FLAGS) -DLOCAL=$(LOCAL) -DF3DEX_GBI_2 -DDEBUG
LCOPTS = -G 0 -g
endif

LCINCS = -I. -I$(INC)/PR -I$(INC) -I$(GCCDIR)/mipse/include -I../../

OPTIMIZER = -O2

##############################################################################

LDFLAGS = $(MKDEPOPT) -L$(LIB) $(N64LIB) -L$(GCCDIR)/mipse/lib -lkmc

ifdef _FINAL
N64LIB = -lgultra_rom
else
N64LIB = -lgultra_d
endif

##############################################################################

ifdef _FINAL
MILDFLAGS   = -I../.. -DLOCAL=$(LOCAL) #-s 9
DO_MAKEMASK = $(MAKEMASK) $(TARGETS)
else
MILDFLAGS   = -I../.. -DLOCAL=$(LOCAL) -m
DO_MAKEMASK = rem $(MAKEMASK) $(TARGETS)
endif

##############################################################################

.SUFFIXES: .seg .cmp .zzz

.c.o:
	$(CC) $(CFLAGS) $<

.s.o:
	$(CC) $(AFLAGS) $<

#.seg.cmp:
#	$(COMP) $<>>map.txt

.seg.zzz:
#	$(COMP2) $<>>map.txt
	gzip -cn9 $< > $<.gz && dd if=$<.gz bs=10 skip=1 of=$@
	cp $@ $(LOCAL)

##############################################################################

ASMFILES = 

ASMOBJECTS = $(ASMFILES:.s=.o)

##############################################################################

CODEFILES = 

CODEOBJECTS = $(CODEFILES:.c=.o)

##############################################################################

DATAFILES = tutorial_data.c

DATAOBJECTS = $(DATAFILES:.c=.o)

##############################################################################

CODESEGMENT = ../../i10n/tools/codesegment.o
OBJECTS     = $(CODEOBJECTS) $(DATAOBJECTS)
SEGFILES    = $(OBJECTS:.o=.seg)
CMPFILES    = $(OBJECTS:.o=.cmp)
ZZZFILES    = $(OBJECTS:.o=.zzz)
SEGMENTSIZE = $(LOCAL)/segsize.h

##############################################################################

default: $(ZZZFILES)

#$(CODESEGMENT): $(CODEOBJECTS) $(ASMOBJECTS)
#	$(LD) -o $(CODESEGMENT) -r $(CODEOBJECTS) $(ASMOBJECTS) $(LDFLAGS)

$(TARGETS) $(APP): spec $(CODESEGMENT) $(OBJECTS)
	$(MAKEROM) spec -r $(TARGETS) $(MILDFLAGS) -m >map.txt
#	$(DO_MAKEMASK)

$(SEGFILES): $(TARGETS) $(APP)
	nm $(APP)|$(NMSYM) $(TARGETS) tutorial_data

$(SEGMENTSIZE): $(SEGFILES)
#	dir /b *.seg|sort>segment.txt
	ls *.seg > segment.txt
	$(SEGSIZE)<segment.txt>$(SEGMENTSIZE)
	rm segment.txt

$(CMPFILES): $(SEGFILES) $(SEGMENTSIZE)

$(ZZZFILES): $(SEGFILES) $(SEGMENTSIZE)

##############################################################################

rom:
	del codesegment.o
	$(MAKE) _FINAL=1

dep:
	$(MKDEP) $(LCINCS) $(CODEFILES) $(DATAFILES)>con

clean:
	rm -f *.o *.seg *.zzz *.gz $(APP)$(TARGETS) $(LOCAL)/*.zzz $(LOCAL)/segsize.h map.txt

##############################################################################

