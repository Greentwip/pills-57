
ifndef OUTDIR
OUTDIR = RELEASE
endif

ifndef LOCAL
LOCAL = CHINA
endif

##############################################################################

LIB = $(ROOT)/usr/lib
LPR = $(LIB)/PR
INC = $(ROOT)/usr/include

CC = gcc
LD = ld

#MAKEROM  = mild
MAKEROM  = $(ROOT)/usr/sbin/makerom
MAKEMASK = $(ROOT)/usr/sbin/makemask

#APP     = game.out
APP     = game
TARGETS = game.n64

ifeq ($(LOCAL), JAPAN)
MASTER = UNN6J00.SRB
endif

ifeq ($(LOCAL), AMERICA)
#MASTER = UNN6E00.SRB
MASTER = UNN6G00.SRB
endif

ifeq ($(LOCAL), CHINA)
#MASTER = UNN6E00.SRB
MASTER = UNN6G00.SRB
CC += -Ii10n/assets/zh/mes
endif
##############################################################################

AFLAGS  = -mcpu=vr4300 $(CFLAGS)
CFLAGS  = $(LCDEFS) $(LCINCS) $(LCOPTS) $(GCCFLAG) $(OPTIMIZER)
GCCFLAG = -c 

#HW_FLAGS = -DPTN64

ifeq ($(OUTDIR), RELEASE)
LCDEFS = $(HW_FLAGS) -DLOCAL=$(LOCAL) -DF3DEX_GBI_2 -DNDEBUG -D_FINALROM
LCOPTS = -G 0
else
LCDEFS = $(HW_FLAGS) -DLOCAL=$(LOCAL) -DF3DEX_GBI_2 -DDEBUG
LCOPTS = -G 0 -g
endif
LCDEFS += -DLINUX_BUILD

LCINCS = -I. -I$(INC)/PR -I$(INC) -I$(INC)/nustd

OPTIMIZER = -O2

##############################################################################

LDFLAGS = $(MKDEPOPT) -L$(LIB) $(N64LIB) ${-lkmc} $(LDFORCE)

ifeq ($(OUTDIR), RELEASE)
N64LIB = -lultra_rom
else
N64LIB = -lultra_d
endif

ifeq ($(OUTDIR), RELEASE)
MUSLIB = -lmus
else
MUSLIB = -lmus_d
endif

LDFORCE = \
	-u alSynAddPlayer \
	-u alSynAllocVoice \
	-u alSynStopVoice \
	-u alSynSetVol \
	-u alSynSetFXMix \
	-u alSynSetVol \
	-u alSynStopVoice \
	-u alSynStopVoice \
	-u alSynStartVoice \
	-u alSynSetVol \
	-u alSynSetPan \
	-u alSynSetPitch \
	-u osEPiStartDma \
	-u osScAddClient \
	-u osScGetCmdQ \
	-u osAiSetFrequency \
	-u osAiGetStatus \
	-u osAiGetLength \
	-u osAiSetNextBuffer \
	-u alHeapInit \
	-u _ctype \
	-u alEnvmixerPull \
	-u bcmp \
	-u bcopy \
	-u cosf \
	-u guMtxCatF \
	-u guMtxF2L \
	-u guMtxIdent \
	-u guMtxL2F \
	-u guOrtho \
	-u guPerspective \
	-u guPerspectiveF \
	-u guRotateRPY \
	-u guRotateRPYF \
	-u guScale \
	-u guScaleF \
	-u guTranslate \
	-u guTranslateF \
	-u memmove \
	-u memset \
	-u modf \
	-u osCartRomInit \
	-u osContGetQuery \
	-u osContGetReadData \
	-u osContInit \
	-u osContSetCh \
	-u osContStartQuery \
	-u osContStartReadData \
	-u osCreateViManager \
	-u osEepromLongRead \
	-u osEepromLongWrite \
	-u osEepromProbe \
	-u osSetTime \
	-u osSetTimer \
	-u osSpTaskLoad \
	-u osSpTaskStartGo \
	-u osSpTaskYield \
	-u osSpTaskYielded \
	-u osViBlack \
	-u osViGetCurrentFramebuffer \
	-u osViGetNextFramebuffer \
	-u osViModeTable \
	-u osViSetEvent \
	-u osViSetMode \
	-u osViSetSpecialFeatures \
	-u osViSetYScale \
	-u osViSwapBuffer \
	-u osWritebackDCacheAll \
	-u rand \
	-u sinf \
	-u sins \
	-u sqrtf \
	-u strcpy \
	-u strlen \
	-u __MojiStat \
	-u __udivdi3

##############################################################################

ifeq ($(OUTDIR), RELEASE)
MILDFLAGS   = -s 32
MILDFLAGS   =
DO_MAKEMASK = $(MAKEMASK) $(TARGETS)
else
MILDFLAGS   = 
#DO_MAKEMASK = rem
DO_MAKEMASK = true
endif

OBJDIR = obj

##############################################################################

.c.o:
	$(CC) $(CFLAGS) $< 

.s.o:
	$(CC) $(AFLAGS) $< 

##############################################################################

ASMFILES = calcsub.s hcopy.s

ASMOBJECTS = $(ASMFILES:.s=.o)

##############################################################################

BOOTFILES = \
	boot.c message.c dmacopy.c inflate.c unzip.c util.c boot_data.c osHalt.c

BOOTOBJECTS = $(BOOTFILES:.c=.o)

##############################################################################

CODEFILES = \
	rdp_output.c dram_stack.c main.c nnsched.c joy.c \
	sound.c graphic.c gfxyield.c static.c musdrv.c \
	evsworks.c main1x.c \
	vr_init.c aiset.c aidata.c \
	record.c hardcopy.c \
	game_etc.c \
	io_file.c ${io_vprintf.c} dbg_font.c aidebug.c \
	cpu_task.c tex_func.c main_menu.c font.c msgwnd.c char_anime.c \
	dm_virus_init.c \
	dm_game_main.c \
	dm_manual_main.c \
	dm_title_main.c \
	main_story.c lws.c calc.c matrix.c vector.c \
	version.c replay.c passwd.c

CODEOBJECTS = $(CODEFILES:.c=$(OBJDIR)/.o)

##############################################################################

ENTRYFILES = code_entry.c

ENTRYOBJECTS = $(ENTRYFILES:.c=$(OBJDIR)/.o)

##############################################################################

DATAFILES = \
	fbuffer.c \
	gfxfree.c \

DATAOBJECTS = $(DATAFILES:.c=$(OBJDIR)/.o)

##############################################################################

BOOTSEGMENT = bootsegment.o
CODESEGMENT = codesegment.o
OBJECTS     = $(BOOTSEGMENT) $(CODESEGMENT) $(ENTRYOBJECTS) $(DATAOBJECTS)

GAME_ZZZ = texture/game/$(LOCAL)/*.zzz \
	texture/game/$(LOCAL)/segsize.h

TUTORIAL_ZZZ = texture/tutorial/$(LOCAL)/tutorial_data.zzz \
	texture/tutorial/$(LOCAL)/segsize.h

MENU_ZZZ = texture/menu/$(LOCAL)/*.zzz \
	texture/menu/$(LOCAL)/segsize.h

##############################################################################

ifeq ($(OUTDIR), RELEASE)
default: $(MASTER)
else
default: $(TARGETS)
endif

$(BOOTSEGMENT): $(BOOTOBJECTS)
	$(LD) -o $(BOOTSEGMENT) -r $(BOOTOBJECTS) $(LDFLAGS) -lgcc -lnustd

$(CODESEGMENT): $(CODEOBJECTS) $(ASMOBJECTS) 
	$(LD) -o $(CODESEGMENT) -r  $(CODEOBJECTS) $(ASMOBJECTS) -L$(LIB) $(MUSLIB)

$(TARGETS) $(APP): spec $(OBJECTS) tool/minigzip tool/segsize/segsize $(GAME_ZZZ) $(TUTORIAL_ZZZ) $(MENU_ZZZ)
	cpp -P -DLOCAL=$(LOCAL) spec>spec.tmp
	cpp -P -DLOCAL=$(LOCAL) spec | sed -e "s/\$$(LOCAL)/$(LOCAL)/">spec.tmp
#	$(MAKEROM) spec.tmp -r $(TARGETS) -e $(APP) $(MILDFLAGS)>map.txt
	$(MAKEROM) -m -r $(TARGETS) $(MILDFLAGS) spec.tmp>map.txt
	rm spec.tmp
#	nm $(APP)|tool\nmsym $(TARGETS) code code_entry
	nm $(APP)|tool/extractseg.pl $(TARGETS) code code_entry
#	tool\compress -c9 code.seg
#	gzip -c9 code.seg > code.zzz
	tool/minigzip -9 code.seg && dd if=code.seg.gz bs=10 skip=1 of=code.zzz
	mv $(APP) $(APP).bak
#	cpp -P -DLOCAL=$(LOCAL) -DCOMPRESSED spec>spec.tmp
	cpp -P -DLOCAL=$(LOCAL) -DCOMPRESSED spec| sed -e "s/\$$(LOCAL)/$(LOCAL)/">spec.tmp
	$(MAKEROM) spec.tmp -m -r $(TARGETS) $(MILDFLAGS) >> map.txt
ifeq ($(LOCAL), CHINA)
	$(ROOT)/usr/sbin/pubrom -d -p dm.prop -i metadata/thumb.rgba -T metadata/title.inta -n metadata/title_z.txt game.n64 dm
else
	$(ROOT)/usr/sbin/pubrom -d -p dm.prop -i metadata/thumb.rgba -T metadata/title.inta -n metadata/title_e.txt game.n64 dm_e
endif

	mv $(APP) $(APP).cpr
#	rm spec.tmp $(APP) code.zzz code.seg code_entry.seg
	mv $(APP).bak $(APP)
	$(DO_MAKEMASK)

$(MASTER): $(TARGETS)
	cp $(TARGETS) $(MASTER)
#	tool\nrdc -p -d -c -t"DR.MARIO 64" $(MASTER)>$(MASTER).txt
#	cp $(MASTER).txt con

version.c: version.txt
	cpp -P $< -o $@

tool/minigzip : tool/minigzip.c
	gcc -o $@ $< -lz

tool/segsize/segsize : tool/segsize/segsize.c
	gcc -o $@ $< -lz

$(GAME_ZZZ): texture/game/*.c 
	(cd texture/game; make LOCAL=$(LOCAL) clean; make LOCAL=$(LOCAL); cd ../..)

$(TUTORIAL_ZZZ): texture/tutorial/tutorial_data.c
	(cd texture/tutorial; make LOCAL=$(LOCAL) clean; make LOCAL=$(LOCAL); cd ../..)

$(MENU_ZZZ): texture/menu/*.c
	(cd texture/menu; make LOCAL=$(LOCAL) clean; make LOCAL=$(LOCAL); cd ../..)

game_etc.o: etc.tbl

etc.tbl:
	(cd texture/story_bmp; make LOCAL=$(LOCAL))

clobber:
	rm -rf *.o core make.out map.txt spec.tmp version.c \
	code.seg.gz code.zzz code_entry.seg game game.cpr game.n64 tool/minigzip tool/segsize/segsize \
	texture/game/*.o texture/menu/*.o texture/tutorial/*.o \
	texture/game/*.zzz texture/menu/*.zzz texture/tutorial/*.zzz \
	$(GAME_ZZZ) $(TUTORIAL_ZZZ) $(MENU_ZZZ)
	(cd texture/story_bmp; make LOCAL=$(LOCAL) clean)

##############################################################################

dep:
	tool\mkdep $(BOOTFILES) $(CODEFILES) $(DATAFILES)>con

##############################################################################

# DO NOT DELETE THIS LINE.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
boot.o: \
	boot.c \
		def.h \
		message.h \
		main.h \
		util.h \
		dmacopy.h \
		code_entry.h
message.o: \
	message.c \
		message.h
dmacopy.o: \
	dmacopy.c
inflate.o: \
	inflate.c \
		gzip.h
unzip.o: \
	unzip.c \
		gzip.h \
		dmacopy.h
util.o: \
	util.c \
		def.h \
		nnsched.h \
		graphic.h \
		audio.h \
			sound.h \
				sound/se.h \
			dmacopy.h \
		util.h \
		debug.h
boot_data.o: \
	boot_data.c \
		boot_data.h \
		util.h \
		sound/sound_data.h \
		texture/character/char_data.h
osHalt.o: \
	osHalt.c \
		osHalt.h
rdp_output.o: \
	rdp_output.c \
		message.h \
		def.h
dram_stack.o: \
	dram_stack.c
main.o: \
	main.c \
		def.h \
		segment.h \
		message.h \
		nnsched.h \
		main.h \
		joy.h \
		audio.h \
			sound.h \
				sound/se.h \
			dmacopy.h \
		graphic.h \
		evsworks.h \
		aiset.h \
			dm_game_def.h \
				aidef.h \
				char_anime.h \
					tex_func.h \
		record.h \
			msgwnd.h \
		main_menu.h \
		main_story.h \
		local.h \
		dm_game_main.h \
		dm_manual_main.h \
		dm_title_main.h
nnsched.o: \
	nnsched.c \
		nnsched.h \
		joy.h
joy.o: \
	joy.c \
		nnsched.h \
		def.h \
		joy.h \
		message.h \
		osHalt.h \
		debug.h
sound.o: \
	sound.c \
		def.h \
		nnsched.h \
		sound.h \
			sound/se.h \
		musdrv.h \
		util.h \
		evsworks.h \
		boot_data.h \
		debug.h \
		sound/tune.h
graphic.o: \
	graphic.c \
		def.h \
		segment.h \
		message.h \
		nnsched.h \
		graphic.h \
		static.h \
		main.h \
		cpu_task.h \
		dm_game_main.h \
			dm_game_def.h \
				aidef.h \
				char_anime.h \
					tex_func.h \
		dm_manual_main.h \
		dm_title_main.h \
		main_menu.h
gfxyield.o: \
	gfxyield.c
static.o: \
	static.c \
		def.h \
		segment.h \
		message.h \
		nnsched.h \
		graphic.h
musdrv.o: \
	musdrv.c \
		nnsched.h \
		dmacopy.h \
		debug.h
evsworks.o: \
	evsworks.c \
		aiset.h \
			dm_game_def.h \
				aidef.h \
				char_anime.h \
					tex_func.h \
		evsworks.h \
		segment.h \
		record.h \
			msgwnd.h
main1x.o: \
	main1x.c \
		def.h \
		nnsched.h \
		main.h \
		audio.h \
			sound.h \
				sound/se.h \
			dmacopy.h \
		joy.h \
		graphic.h \
		evsworks.h \
		vr_init.h \
		aiset.h \
			dm_game_def.h \
				aidef.h \
				char_anime.h \
					tex_func.h \
		aidata.h \
		font.h \
		record.h \
			msgwnd.h \
		dm_game_main.h \
		main_story.h \
		debug.h \
		util.h
vr_init.o: \
	vr_init.c \
		vr_init.h
aiset.o: \
	aiset.c \
		evsworks.h \
		aiset.h \
			dm_game_def.h \
				aidef.h \
				char_anime.h \
					tex_func.h \
		aidata.h \
		nnsched.h \
		vr_init.h \
		dm_game_main.h \
		joy.h
aidata.o: \
	aidata.c \
		aiset.h \
			dm_game_def.h \
				aidef.h \
				char_anime.h \
					tex_func.h \
		aidata.h
record.o: \
	record.c \
		evsworks.h \
		nnsched.h \
		record.h \
			msgwnd.h \
		def.h \
		util.h \
		tex_func.h \
		debug.h \
		local.h \
		texture/font/mess_panel.txt
hardcopy.o: \
	hardcopy.c \
		def.h \
		nnsched.h \
		graphic.h \
		hardcopy.h \
		tex_func.h \
		util.h \
		debug.h
game_etc.o: \
	game_etc.c \
		def.h \
		segment.h \
		message.h \
		nnsched.h \
		main.h \
		audio.h \
			sound.h \
				sound/se.h \
			dmacopy.h \
		graphic.h \
		joy.h \
		static.h \
		evsworks.h \
		font.h \
		msgwnd.h \
		comp.h \
		main_story.h \
		game_etc.h \
		tex_func.h \
		calc.h \
			vector.h \
			matrix.h \
		lws.h \
		dm_game_main.h \
			dm_game_def.h \
				aidef.h \
				char_anime.h
io_file.o: \
	io_file.c \
		io_file.h
io_vprintf.o: \
	io_vprintf.c \
		io_file.h
dbg_font.o: \
	dbg_font.c \
		io_file.h \
		dbg_font.h \
		texture/debug/dbg_font.bft
aidebug.o: \
	aidebug.c \
		dbg_font.h \
		nnsched.h \
		joy.h \
		evsworks.h \
		aiset.h \
			dm_game_def.h \
				aidef.h \
				char_anime.h \
					tex_func.h \
		aidata.h \
		aidebug.h \
		util.h
cpu_task.o: \
	cpu_task.c \
		def.h \
		cpu_task.h
tex_func.o: \
	tex_func.c \
		def.h \
		nnsched.h \
		graphic.h \
		tex_func.h \
		util.h \
		debug.h
main_menu.o: \
	main_menu.c \
		debug.h \
		def.h \
		nnsched.h \
		util.h \
		tex_func.h \
		char_anime.h \
		msgwnd.h \
		font.h \
		cpu_task.h \
		main_story.h \
		aidebug.h \
		local.h \
		texture/menu/menu_char.h \
		texture/menu/menu_common.h \
		texture/menu/menu_level.h \
		texture/menu/menu_main.h \
		texture/menu/menu_name.h \
		texture/menu/menu_p2.h \
		texture/menu/menu_p4.h \
		texture/menu/menu_rank.h \
		texture/menu/menu_setup.h \
		texture/menu/menu_story.h \
		texture/game/game_al.h \
		texture/game/game_p1.h \
		texture/menu/menu_cont.h \
		texture/tutorial/tutorial_data.h \
		texture/menu/menu_kasa.h \
		sound.h \
			sound/se.h \
		evsworks.h \
		record.h \
		dm_game_main.h \
			dm_game_def.h \
				aidef.h \
		main_menu.mes \
		main_menu_am.mes \
		record.mes \
		cap_pos.dat \
		main.h \
		dm_virus_init.h \
		graphic.h \
		message.h \
		segment.h \
		joy.h \
		boot_data.h \
		hardcopy.h
font.o: gbfont12\
	font.c \
		tex_func.h \
		font.h \
		texture/font/font_a.txt \
		texture/font/font_2.txt \
		texture/font/font_e.txt \
		texture/font/font_e2.txt \
		texture/font/ascii_code.tbl \
		texture/font/ascii_code2.tbl \
		texture/font/char_code.tbl \
		texture/story_bmp/Fonts/kanji_font.DMP

gbfont12:
	cd i10n/tools; make

msgwnd.o: \
	msgwnd.c \
		def.h \
		nnsched.h \
		util.h \
		joy.h \
		font.h \
		msgwnd.h \
		debug.h
char_anime.o: \
	char_anime.c \
		def.h \
		debug.h \
		util.h \
		tex_func.h \
		char_anime.h \
		dmacopy.h \
		vr_init.h \
		local.h \
		boot_data.h \
		texture/character/AMERICA/segsize.h \
		texture/character/JAPAN/segsize.h
dm_virus_init.o: \
	dm_virus_init.c \
		nnsched.h \
		debug.h \
		util.h \
		calc.h \
			vector.h \
			matrix.h \
		evsworks.h \
		dm_game_def.h \
			aidef.h \
			char_anime.h \
				tex_func.h \
		dm_virus_init.h
dm_game_main.o: \
	dm_game_main.c \
		calc.h \
			vector.h \
			matrix.h \
		def.h \
		segment.h \
		message.h \
		nnsched.h \
		main.h \
		audio.h \
			sound.h \
				sound/se.h \
			dmacopy.h \
		graphic.h \
		joy.h \
		static.h \
		musdrv.h \
		aiset.h \
			dm_game_def.h \
				aidef.h \
				char_anime.h \
					tex_func.h \
		aidata.h \
		evsworks.h \
		dm_game_main.h \
		record.h \
			msgwnd.h \
		cpu_task.h \
		vr_init.h \
		dm_virus_init.h \
		util.h \
		game_etc.h \
		main_story.h \
		aidebug.h \
		debug.h \
		replay.h \
		passwd.h \
		local.h \
		tech.mes \
		tech_am.mes \
		record.mes \
		texture/game/game_al.h \
		texture/game/game_p1.h \
		texture/game/game_p2.h \
		texture/game/game_p4.h \
		texture/game/game_ls.h \
		texture/game/game_item.h \
		texture/menu/menu_kasa.h \
		boot_data.h
dm_manual_main.o: \
	dm_manual_main.c \
		def.h \
		segment.h \
		message.h \
		nnsched.h \
		main.h \
		graphic.h \
		audio.h \
			sound.h \
				sound/se.h \
			dmacopy.h \
		joy.h \
		static.h \
		util.h \
		msgwnd.h \
		tex_func.h \
		evsworks.h \
		main_story.h \
		dm_game_main.h \
			dm_game_def.h \
				aidef.h \
				char_anime.h \
		dm_virus_init.h \
		dm_manual_main.h \
		debug.h \
		local.h \
		texture/tutorial/tutorial_data.h \
		texture/menu/menu_kasa.h \
		dm_manual_data.h \
		dm_manual.mes \
		dm_manual_am.mes \
		boot_data.h
dm_title_main.o: \
	dm_title_main.c \
		def.h \
		segment.h \
		message.h \
		nnsched.h \
		main.h \
		graphic.h \
		audio.h \
			sound.h \
				sound/se.h \
			dmacopy.h \
		joy.h \
		static.h \
		tex_func.h \
		evsworks.h \
		record.h \
			msgwnd.h \
		dm_title_main.h \
		main_story.h \
		util.h \
		debug.h \
		dbg_font.h \
		local.h \
		dm_title.mes \
		dm_title_am.mes
main_story.o: \
	main_story.c \
		def.h \
		segment.h \
		message.h \
		nnsched.h \
		main.h \
		audio.h \
			sound.h \
				sound/se.h \
			dmacopy.h \
		graphic.h \
		joy.h \
		static.h \
		evsworks.h \
		util.h \
		font.h \
		msgwnd.h \
		comp.h \
		main_story.h \
		tex_func.h \
		game_etc.h \
		calc.h \
			vector.h \
			matrix.h \
		lws.h \
		local.h \
		texture/story_bmp/curtain/curtain_alpha_00.txt \
		texture/story_bmp/curtain/curtain_00.txt \
		texture/story_bmp/title_bmp/changestar.txt \
		main_story_j.mes \
		main_story.mes \
		story_mario.h \
		story_wario.h
lws.o: \
	lws.c \
		def.h \
		calc.h \
			vector.h \
			matrix.h \
		lws.h
calc.o: \
	calc.c \
		def.h \
		calc.h \
			vector.h \
			matrix.h
matrix.o: \
	matrix.c \
		def.h \
		matrix.h \
			vector.h
vector.o: \
	vector.c \
		vector.h
version.o:
replay.o: \
	replay.c \
		replay.h
passwd.o: \
	passwd.c \
		passwd.h
fbuffer.o: \
	fbuffer.c \
		def.h
gfxfree.o: \
	gfxfree.c \
		def.h
# IF YOU PUT ANYTHING HERE IT WILL GO AWAY.
