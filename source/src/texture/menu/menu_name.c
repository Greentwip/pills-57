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
#if LOCAL==JAPAN
#include "name/color/name_frame_no.txt"
#include "name/alpha/name_frame_no_alpha.txt"
#endif
#include "name/color/position_cursor.txt"
#include "name/alpha/position_cursor_alpha.txt"
#undef NON_PALETTE

#if LOCAL==JAPAN
#include "name/normal2/letter_panel.txt"
#include "name/normal2/name_frame.txt"
#elif LOCAL==AMERICA
#include "name/normal2/letter_panel_am.txt"
#include "name/normal/name_frame_2_am.txt"
#include "name/normal2/name_frame_am.txt"
#elif LOCAL==CHINA
#include "../../i10n/assets/zh/menu/name/normal2/letter_panel_am.txt"
#include "../../i10n/assets/zh/menu/name/normal/name_frame_2_am.txt"
#include "../../i10n/assets/zh/menu/name/normal2/name_frame_am.txt"
#endif

static STexInfo texTbl[] = {
#if LOCAL==JAPAN
	{ name_frame_no_tex_addr, name_frame_no_tex_size },
	{ name_frame_no_alpha_tex_addr, name_frame_no_alpha_tex_size },
#endif
	{ position_cursor_tex_addr, position_cursor_tex_size },
	{ position_cursor_alpha_tex_addr, position_cursor_alpha_tex_size },

#if LOCAL==JAPAN
	{ letter_panel_tex_addr, letter_panel_tex_size },
	{ name_frame_tex_addr, name_frame_tex_size },
#elif LOCAL==AMERICA
	{ letter_panel_am_tex_addr, letter_panel_am_tex_size },
	{ name_frame_am_tex_addr, name_frame_am_tex_size },
	{ name_frame_2_am_tex_addr, name_frame_2_am_tex_size },
#elif LOCAL==CHINA
	{ letter_panel_am_tex_addr, letter_panel_am_tex_size },
	{ name_frame_am_tex_addr, name_frame_am_tex_size },
	{ name_frame_2_am_tex_addr, name_frame_2_am_tex_size },
#endif
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };
