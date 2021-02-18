
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

#if LOCAL==CHINA
#include "../../i10n/assets/zh/menu/p2/normal2/p2_battle_panel.txt"
#include "../../i10n/assets/zh/menu/p2/normal/p2_newmode_panel.txt"
#else
#include "p2/normal2/p2_battle_panel.txt"
#include "p2/normal/p2_newmode_panel.txt"
#endif

static STexInfo texTbl[] = {
	{ p2_battle_panel_tex_addr, p2_battle_panel_tex_size },
	{ p2_newmode_panel_tex_addr, p2_newmode_panel_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };
