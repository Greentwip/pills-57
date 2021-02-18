#include <ultra64.h>
#include "tex_func.h"
#include "local.h"

static STexInfo  texTbl[];
static STexInfo *texTblPtr = texTbl;
static int       texTblSize[];
static int      *texTblSizePtr = texTblSize;

#define USE_TEX_SIZE
#define USE_TEX_ADDR

#define NON_PALETTE
#undef NON_PALETTE

#define NON_PALETTE
#undef NON_PALETTE

#if LOCAL==CHINA
#include "../../i10n/assets/zh/menu/rank/normal/rank_com_name.txt"
#include "../../i10n/assets/zh/menu/rank/normal/rank_lv.txt"
#include "../../i10n/assets/zh/menu/rank/normal/rank_operation_b.txt"
#include "../../i10n/assets/zh/menu/rank/normal2/rank_operation_a.txt"
#else
#include "rank/normal/rank_com_name.txt"
#include "rank/normal/rank_lv.txt"
#include "rank/normal/rank_operation_b.txt"
#include "rank/normal2/rank_operation_a.txt"
#endif
#include "rank/normal2/mode_panel_sub.txt"
#include "rank/normal2/rank_panel.txt"
#include "rank/normal2/vs.txt"

#if LOCAL==JAPAN
#include "rank/normal2/rank_head_a.txt"
#include "rank/normal/rank_head_b.txt"
#elif LOCAL==AMERICA
#include "rank/normal2/rank_head_a_am.txt"
#include "rank/normal/rank_head_b_am.txt"
#elif LOCAL==CHINA
#include "../../i10n/assets/zh/menu/rank/normal2/rank_head_a_am.txt"
#include "../../i10n/assets/zh/menu/rank/normal/rank_head_b_am.txt"
#endif

static STexInfo texTbl[] = {
	{ rank_com_name_tex_addr, rank_com_name_tex_size },
	{ rank_lv_tex_addr, rank_lv_tex_size },
	{ rank_operation_b_tex_addr, rank_operation_b_tex_size },

	{ mode_panel_sub_tex_addr, mode_panel_sub_tex_size },
	{ rank_operation_a_tex_addr, rank_operation_a_tex_size },
	{ rank_panel_tex_addr, rank_panel_tex_size },
	{ vs_tex_addr, vs_tex_size },

#if LOCAL==JAPAN
	{ rank_head_a_tex_addr, rank_head_a_tex_size },
	{ rank_head_b_tex_addr, rank_head_b_tex_size },
#elif LOCAL==AMERICA
	{ rank_head_a_am_tex_addr, rank_head_a_am_tex_size },
	{ rank_head_b_am_tex_addr, rank_head_b_am_tex_size },
#elif LOCAL==CHINA
	{ rank_head_a_am_tex_addr, rank_head_a_am_tex_size },
	{ rank_head_b_am_tex_addr, rank_head_b_am_tex_size },
#endif
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };
