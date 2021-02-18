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

#include "main/normal2/game_screen.txt"
#include "main/normal2/menu_panel_2p.txt"
#include "main/normal2/menu_panel_3p.txt"
#include "main/normal2/menu_panel_4p.txt"
#include "main/normal2/menu_panel_5p.txt"
#include "main/normal2/menu_panel_6p.txt"

#include "main/normal2/message_window.txt"
#include "main/normal2/name_panel.txt"
#include "main/normal2/number_screen.txt"

#define NON_PALETTE
#include "main/normal2/game_window.txt"
#include "main/normal2/smoke.txt"
#undef NON_PALETTE

#if LOCAL==JAPAN
#include "main/normal2/menu_panel_s.txt"
#include "main/normal/menu_panel_s_2.txt"
#include "main/normal2/end_panel.txt"
#include "main/normal2/gest_panel.txt"
#elif LOCAL==AMERICA
#include "main/normal2/menu_panel_s_am.txt"
#include "main/normal/menu_panel_s_2_am.txt"
#include "main/normal2/end_panel_am.txt"
#include "main/normal2/guest_panel_am.txt"
#elif LOCAL==CHINA
#include "../../i10n/assets/zh/menu/main/normal2/menu_panel_s_am.txt"
#include "../../i10n/assets/zh/menu/main/normal/menu_panel_s_2_am.txt"
#include "../../i10n/assets/zh/menu/main/normal2/end_panel_am.txt"
#include "../../i10n/assets/zh/menu/main/normal2/guest_panel_am.txt"
#endif

static STexInfo texTbl[] = {
	{ game_screen_tex_addr, game_screen_tex_size },
	{ menu_panel_2p_tex_addr, menu_panel_2p_tex_size },
	{ menu_panel_3p_tex_addr, menu_panel_3p_tex_size },
	{ menu_panel_4p_tex_addr, menu_panel_4p_tex_size },
	{ menu_panel_5p_tex_addr, menu_panel_5p_tex_size },
	{ menu_panel_6p_tex_addr, menu_panel_6p_tex_size },

	{ message_window_tex_addr, message_window_tex_size },
	{ name_panel_tex_addr, name_panel_tex_size },
	{ number_screen_tex_addr, number_screen_tex_size },

	{ game_window_tex_addr, game_window_tex_size },
	{ smoke_tex_addr, smoke_tex_size },

#if LOCAL==JAPAN
	{ menu_panel_s_tex_addr, menu_panel_s_tex_size },
	{ menu_panel_s_2_tex_addr, menu_panel_s_2_tex_size },
	{ end_panel_tex_addr, end_panel_tex_size },
	{ gest_panel_tex_addr, gest_panel_tex_size },
#elif LOCAL==AMERICA
	{ menu_panel_s_am_tex_addr, menu_panel_s_am_tex_size },
	{ menu_panel_s_2_am_tex_addr, menu_panel_s_2_am_tex_size },
	{ end_panel_am_tex_addr, end_panel_am_tex_size },
	{ guest_panel_am_tex_addr, guest_panel_am_tex_size },
#elif LOCAL==CHINA
	{ menu_panel_s_am_tex_addr, menu_panel_s_am_tex_size },
	{ menu_panel_s_2_am_tex_addr, menu_panel_s_2_am_tex_size },
	{ end_panel_am_tex_addr, end_panel_am_tex_size },
	{ guest_panel_am_tex_addr, guest_panel_am_tex_size },
#endif
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };
