
#include <ultra64.h>
#include "local.h"
#include "tex_func.h"

static STexInfo  texTbl[];
static STexInfo *texTblPtr = texTbl;
static int       texTblSize[];
static int      *texTblSizePtr = texTblSize;

#define USE_TEX_SIZE
#define USE_TEX_ADDR

#define NON_PALETTE
#undef NON_PALETTE

#include "level/normal/bottle_virus.txt"

#if LOCAL==CHINA
#include "../../i10n/assets/zh/menu/level/normal2/endless_illust.txt"
#include "../../i10n/assets/zh/menu/level/normal2/endless_panel.txt"
#include "../../i10n/assets/zh/menu/level/normal2/newmode_panel.txt"
#include "../../i10n/assets/zh/menu/level/normal2/virus_gauge_lv21.txt"
#include "../../i10n/assets/zh/menu/level/normal2/endless_illust_music.txt"
#else
#include "level/normal2/endless_illust.txt"
#include "level/normal2/endless_panel.txt"
#include "level/normal2/newmode_panel.txt"
#include "level/normal2/virus_gauge_lv21.txt"
#include "level/normal2/endless_illust_music.txt"
#endif
#include "level/normal2/endless_illust_speed.txt"

static STexInfo texTbl[] = {
	{ bottle_virus_tex_addr, bottle_virus_tex_size },

	{ endless_illust_tex_addr, endless_illust_tex_size },
	{ endless_illust_music_tex_addr, endless_illust_music_tex_size },
	{ endless_illust_speed_tex_addr, endless_illust_speed_tex_size },
	{ endless_panel_tex_addr, endless_panel_tex_size },
	{ newmode_panel_tex_addr, newmode_panel_tex_size },
	{ virus_gauge_lv21_tex_addr, virus_gauge_lv21_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };
