#include <ultra64.h>
#include "tex_func.h"
#include "local.h"

static STexInfo  texTbl[];
static STexInfo *texTblPtr = texTbl;
static int       texTblSize[];
static int      *texTblSizePtr = texTblSize;

#define USE_TEX_SIZE
#define USE_TEX_ADDR

#include "p1/normal/p1_bottle_01.txt"
#include "p1/normal2/p1_bottle_shadow_00.txt"

#define NON_PALETTE
#include "p1/color/p1_vs_time_number.txt"
#include "p1/color/count_num.txt"
#include "p1/color/count_num_1p.txt"
#include "p1/color/count_num_2p.txt"

#include "p1/alpha/p1_vs_time_number_alpha.txt"
#include "p1/alpha/count_num_alpha.txt"

#if LOCAL==JAPAN
#include "p1/color/totalscore_panel.txt"
#include "p1/alpha/totalscore_panel_alpha.txt"
#elif LOCAL==AMERICA
#include "p1/color/totalscore_panel_am.txt"
#include "p1/alpha/totalscore_panel_alpha.txt"
#elif LOCAL==CHINA
#include "../../i10n/assets/zh/game/p1/color/totalscore_panel_am.txt"
#include "../../i10n/assets/zh/game/p1/alpha/totalscore_panel_alpha.txt"
#endif

#undef NON_PALETTE

static STexInfo texTbl[] = {
	{ p1_bottle_01_tex_addr, p1_bottle_01_tex_size },
	{ p1_bottle_shadow_00_tex_addr, p1_bottle_shadow_00_tex_size },

	{ p1_vs_time_number_tex_addr, p1_vs_time_number_tex_size },
	{ count_num_tex_addr, count_num_tex_size },
	{ count_num_1p_tex_addr, count_num_1p_tex_size },
	{ count_num_2p_tex_addr, count_num_2p_tex_size },

	{ p1_vs_time_number_alpha_tex_addr, p1_vs_time_number_alpha_tex_size },
	{ count_num_alpha_tex_addr, count_num_alpha_tex_size },
#if LOCAL==JAPAN
	{ totalscore_panel_tex_addr, totalscore_panel_tex_size },
	{ totalscore_panel_alpha_tex_addr, totalscore_panel_alpha_tex_size },
#elif LOCAL==AMERICA
	{ totalscore_panel_am_tex_addr, totalscore_panel_am_tex_size },
	{ totalscore_panel_alpha_tex_addr, totalscore_panel_alpha_tex_size },
#elif LOCAL==CHINA
	{ totalscore_panel_am_tex_addr, totalscore_panel_am_tex_size },
	{ totalscore_panel_alpha_tex_addr, totalscore_panel_alpha_tex_size },
#endif
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };
