
#include <ultra64.h>
#include "tex_func.h"
#include "local.h"

static STexInfo  texTbl[];
static STexInfo *texTblPtr = texTbl;
static int       texTblSize[];
static int      *texTblSizePtr = texTblSize;

#define USE_TEX_SIZE
#define USE_TEX_ADDR

#if LOCAL==JAPAN
#include "p4/normal2/character_face_b.txt"
#elif LOCAL==AMERICA
#include "p4/normal2/character_face_b_am.txt"
#elif LOCAL==CHINA
#include "p4/normal2/character_face_b_am.txt"
#endif

#if LOCAL==CHINA
#include "../../i10n/assets/zh/menu/p4/normal2/p4_battle_panel.txt"
#include "../../i10n/assets/zh/menu/p4/normal/p4_newmode_panel.txt"
#else
#include "p4/normal2/p4_battle_panel.txt"
#include "p4/normal/p4_newmode_panel.txt"
#endif

#define NON_PALETTE
#if LOCAL==CHINA
#include "../../i10n/assets/zh/menu/p4/normal2/new_cursor_gamelv_4p_easy.txt"
#include "../../i10n/assets/zh/menu/p4/normal2/new_cursor_gamelv_4p_normal.txt"
#include "../../i10n/assets/zh/menu/p4/normal2/new_cursor_gamelv_4p_hard.txt"

#include "../../i10n/assets/zh/menu/p4/normal2/new_cursor_speed_4p_low.txt"
#include "../../i10n/assets/zh/menu/p4/normal2/new_cursor_speed_4p_med.txt"
#include "../../i10n/assets/zh/menu/p4/normal2/new_cursor_speed_4p_hi.txt"
#else
#include "p4/normal2/new_cursor_gamelv_4p_easy.txt"
#include "p4/normal2/new_cursor_gamelv_4p_normal.txt"
#include "p4/normal2/new_cursor_gamelv_4p_hard.txt"

#include "p4/normal2/new_cursor_speed_4p_low.txt"
#include "p4/normal2/new_cursor_speed_4p_med.txt"
#include "p4/normal2/new_cursor_speed_4p_hi.txt"
#endif
#undef NON_PALETTE

static STexInfo texTbl[] = {
#if LOCAL==JAPAN
	{ character_face_b_tex_addr, character_face_b_tex_size },
#elif LOCAL==AMERICA
	{ character_face_b_am_tex_addr, character_face_b_am_tex_size },
#elif LOCAL==CHINA
	{ character_face_b_am_tex_addr, character_face_b_am_tex_size },
#endif

	{ p4_battle_panel_tex_addr, p4_battle_panel_tex_size },
	{ p4_newmode_panel_tex_addr, p4_newmode_panel_tex_size },

	{ new_cursor_gamelv_4p_easy_tex_addr, new_cursor_gamelv_4p_easy_tex_size },
	{ new_cursor_gamelv_4p_normal_tex_addr, new_cursor_gamelv_4p_normal_tex_size },
	{ new_cursor_gamelv_4p_hard_tex_addr, new_cursor_gamelv_4p_hard_tex_size },

	{ new_cursor_speed_4p_low_tex_addr, new_cursor_speed_4p_low_tex_size },
	{ new_cursor_speed_4p_med_tex_addr, new_cursor_speed_4p_med_tex_size },
	{ new_cursor_speed_4p_hi_tex_addr, new_cursor_speed_4p_hi_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };
