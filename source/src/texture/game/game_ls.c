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
#if LOCAL==CHINA
#include "../../i10n/assets/zh/game/ls/alpha/endless_speed_alpha.txt"
#include "../../i10n/assets/zh/game/ls/alpha/newmode_level_alpha.txt"
#else
#include "ls/alpha/endless_speed_alpha.txt"
#include "ls/alpha/newmode_level_alpha.txt"
#endif
#include "ls/alpha/l_panel_alpha.txt"
#include "ls/alpha/mario_platform_alpha.txt"
#include "ls/alpha/r_panel_alpha.txt"

#if LOCAL==CHINA
#include "../../i10n/assets/zh/game/ls/color/endless_speed.txt"
#include "../../i10n/assets/zh/game/ls/color/l_panel_endless.txt"
#include "../../i10n/assets/zh/game/ls/color/l_panel_taikyu.txt"
#include "../../i10n/assets/zh/game/ls/color/l_panel_timeattack.txt"
#include "../../i10n/assets/zh/game/ls/color/newmode_level.txt"
#include "../../i10n/assets/zh/game/ls/color/r_panel_level.txt"
#include "../../i10n/assets/zh/game/ls/color/r_panel_speed.txt"
#else
#include "ls/color/endless_speed.txt"
#include "ls/color/l_panel_endless.txt"
#include "ls/color/l_panel_taikyu.txt"
#include "ls/color/l_panel_timeattack.txt"
#include "ls/color/newmode_level.txt"
#include "ls/color/r_panel_level.txt"
#include "ls/color/r_panel_speed.txt"
#endif
#include "ls/color/mario_platform.txt"

#if LOCAL==JAPAN
#include "ls/color/r_panel_virus.txt"
#include "ls/color/r_panel_viruslv.txt"
#elif LOCAL==AMERICA
#include "ls/color/r_panel_virus_am.txt"
#include "ls/color/r_panel_viruslv_am.txt"
#elif LOCAL==CHINA
#include "../../i10n/assets/zh/game/ls/color/r_panel_virus_am.txt"
#include "../../i10n/assets/zh/game/ls/color/r_panel_viruslv_am.txt"
#endif
#undef NON_PALETTE

#include "ls/normal/bg_endless.txt"
#include "ls/normal/bg_taikyu.txt"
#include "ls/normal/bg_timeattack.txt"
#include "ls/normal/magnifier_endless.txt"
#include "ls/normal/magnifier_taikyu.txt"
#include "ls/normal/magnifier_timeattack.txt"

#include "ls/normal2/coin_00.txt"

static STexInfo texTbl[] = {
	{ endless_speed_alpha_tex_addr, endless_speed_alpha_tex_size },
	{ l_panel_alpha_tex_addr, l_panel_alpha_tex_size },
	{ mario_platform_alpha_tex_addr, mario_platform_alpha_tex_size },
	{ newmode_level_alpha_tex_addr, newmode_level_alpha_tex_size },
	{ r_panel_alpha_tex_addr, r_panel_alpha_tex_size },

	{ endless_speed_tex_addr, endless_speed_tex_size },
	{ l_panel_endless_tex_addr, l_panel_endless_tex_size },
	{ l_panel_taikyu_tex_addr, l_panel_taikyu_tex_size },
	{ l_panel_timeattack_tex_addr, l_panel_timeattack_tex_size },
	{ mario_platform_tex_addr, mario_platform_tex_size },
	{ newmode_level_tex_addr, newmode_level_tex_size },
	{ r_panel_level_tex_addr, r_panel_level_tex_size },
	{ r_panel_speed_tex_addr, r_panel_speed_tex_size },

#if LOCAL==JAPAN
	{ r_panel_virus_tex_addr, r_panel_virus_tex_size },
	{ r_panel_viruslv_tex_addr, r_panel_viruslv_tex_size },
#elif LOCAL==AMERICA
	{ r_panel_virus_am_tex_addr, r_panel_virus_am_tex_size },
	{ r_panel_viruslv_am_tex_addr, r_panel_viruslv_am_tex_size },
#elif LOCAL==CHINA
	{ r_panel_virus_am_tex_addr, r_panel_virus_am_tex_size },
	{ r_panel_viruslv_am_tex_addr, r_panel_viruslv_am_tex_size },
#endif

	{ bg_endless_tex_addr, bg_endless_tex_size },
	{ bg_taikyu_tex_addr, bg_taikyu_tex_size },
	{ bg_timeattack_tex_addr, bg_timeattack_tex_size },
	{ magnifier_endless_tex_addr, magnifier_endless_tex_size },
	{ magnifier_taikyu_tex_addr, magnifier_taikyu_tex_size },
	{ magnifier_timeattack_tex_addr, magnifier_timeattack_tex_size },

	{ coin_00_tex_addr, coin_00_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };
