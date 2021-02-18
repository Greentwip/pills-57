
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
#include "common/alpha/finger_alpha.txt"

#include "common/color/finger.txt"
#undef NON_PALETTE

#include "common/normal/obi.txt"

#define NON_PALETTE
#include "common/normal2/cursor_low.txt"
#include "common/normal2/cursor_med.txt"
#include "common/normal2/cursor_hi.txt"
#include "common/normal2/constituent_cursor.txt"
#include "common/normal2/constituent_cursor2.txt"
#include "common/normal2/item_cursor.txt"
#include "common/normal2/new_cursor_4p.txt"
#include "common/normal2/new_cursor_music.txt"
#include "common/normal2/new_cursor_speed.txt"
#include "common/normal2/new_cursor_viruslv_l.txt"
#include "common/normal2/new_cursor_viruslv_s.txt"
#undef NON_PALETTE

#if LOCAL==JAPAN
#include "common/normal2/choice_a.txt"
#include "common/normal2/menu_title.txt"
#elif LOCAL==AMERICA
#include "common/normal2/choice_a_am.txt"
#include "common/normal2/menu_title_am.txt"
#elif LOCAL==CHINA
#include "../../i10n/assets/zh/menu/common/normal2/choice_a_am.txt"
#include "../../i10n/assets/zh/menu/common/normal2/menu_title_am.txt"
#endif

static STexInfo texTbl[] = {
	{ finger_alpha_tex_addr, finger_alpha_tex_size },

	{ finger_tex_addr, finger_tex_size },

	{ obi_tex_addr, obi_tex_size },

	{ cursor_low_tex_addr, cursor_low_tex_size },
	{ cursor_med_tex_addr, cursor_med_tex_size },
	{ cursor_hi_tex_addr, cursor_hi_tex_size },
	{ constituent_cursor_tex_addr, constituent_cursor_tex_size },
	{ constituent_cursor2_tex_addr, constituent_cursor2_tex_size },
	{ item_cursor_tex_addr, item_cursor_tex_size },
	{ new_cursor_4p_tex_addr, new_cursor_4p_tex_size },
	{ new_cursor_music_tex_addr, new_cursor_music_tex_size },
	{ new_cursor_speed_tex_addr, new_cursor_speed_tex_size },
	{ new_cursor_viruslv_l_tex_addr, new_cursor_viruslv_l_tex_size },
	{ new_cursor_viruslv_s_tex_addr, new_cursor_viruslv_s_tex_size },

#if LOCAL==JAPAN
	{ choice_a_tex_addr, choice_a_tex_size },
	{ menu_title_tex_addr, menu_title_tex_size },
#elif LOCAL==AMERICA
	{ choice_a_am_tex_addr, choice_a_am_tex_size },
	{ menu_title_am_tex_addr, menu_title_am_tex_size },
#elif LOCAL==CHINA
	{ choice_a_am_tex_addr, choice_a_am_tex_size },
	{ menu_title_am_tex_addr, menu_title_am_tex_size },
#endif
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };
