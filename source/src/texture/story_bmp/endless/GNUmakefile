ifndef LOCAL
LOCAL=JAPAN
endif

INC = $(ROOT)/usr/include
CC  = mips-linux-gcc
AS  = mips-linux-as

include source.mak
OBJECTS = $(SOURCES:.c=.o)

GCCFLAG   = -c -I. -I$(INC)/PR -I$(INC) -I../../.. -DLOCAL=$(LOCAL) -D_MIPS_SZLONG=32 -D_MIPS_SZINT=32
OPTIMIZER = -G 0 -DF3DEX_GBI_2 -DNDEBUG -D_FINALROM
CFLAGS    = $(GCCFLAG) $(OPTIMIZER)


.SUFFIXES: .seg .zzz

.c.o:
	$(CC) $(CFLAGS) $<

.seg.zzz:
	gzip -n -9 -c $< > $<.gz && dd if=$<.gz bs=10 skip=1 of=$@
	rm -f $<.gz

default: coffee01.zzz

coffee01.seg: $(OBJECTS)
	-mips-linux-ld -Tdata 0x05000000 $(OBJECTS)
	mips-linux-objcopy -j .data -O binary a.out coffee01.seg
	rm -f a.out

coffee01.zzz: coffee01.seg

clean:
	rm -f *.o *.seg *.zzz *.gz 
