#include <ultra64.h>
#include "tex_func.h"
#include "local.h"

static STexInfo  texTbl[];
static STexInfo *texTblPtr = texTbl;
static int       texTblSize[];
static int      *texTblSizePtr = texTblSize;

#define USE_TEX_SIZE
#define USE_TEX_ADDR

#include "p4/normal/p4_bottle.txt"
#include "p4/normal/p4_bottle_shadow.txt"

#if LOCAL==CHINA
#include "../../i10n/assets/zh/game/p4/normal2/p4_toppanel_team_1.txt"
#include "../../i10n/assets/zh/game/p4/normal2/p4_toppanel_team_2.txt"
#include "../../i10n/assets/zh/game/p4/normal2/p4_toppanel_team_3.txt"
#else
#include "p4/normal2/p4_toppanel_team_1.txt"
#include "p4/normal2/p4_toppanel_team_2.txt"
#include "p4/normal2/p4_toppanel_team_3.txt"
#endif
#include "p4/normal2/p4_toppanel_vs_1.txt"
#include "p4/normal2/p4_toppanel_vs_2.txt"
#include "p4/normal/p4_toppanel_vs_3.txt"

#include "p4/normal2/p4_bottompanel.txt"
#include "p4/normal2/p4_cp_panel.txt"

#define NON_PALETTE
#include "p4/color/p4_toppanel_stocklamp.txt"
#include "p4/alpha/p4_toppanel_stocklamp_alpha.txt"
#if LOCAL==CHINA
#include "../../i10n/assets/zh/game/p4/color/attack_p.txt"
#include "../../i10n/assets/zh/game/p4/color/p4_panel_flash.txt"
#include "../../i10n/assets/zh/game/p4/color/p4_team_a.txt"
#include "../../i10n/assets/zh/game/p4/color/p4_team_b.txt"
#include "../../i10n/assets/zh/game/p4/color/p4_toppanel_story.txt"
#include "../../i10n/assets/zh/game/p4/color/stock_a.txt"
#include "../../i10n/assets/zh/game/p4/color/stock_b.txt"

#include "../../i10n/assets/zh/game/p4/alpha/attack_p_alpha.txt"
#include "../../i10n/assets/zh/game/p4/alpha/p4_panel_flash_alpha.txt"
#include "../../i10n/assets/zh/game/p4/alpha/p4_team_a_alpha.txt"
#include "../../i10n/assets/zh/game/p4/alpha/p4_team_b_alpha.txt"
#include "../../i10n/assets/zh/game/p4/alpha/p4_toppanel_story_alpha.txt"
#include "../../i10n/assets/zh/game/p4/alpha/stock_alpha.txt"

#else

#include "p4/color/attack_p.txt"
#include "p4/color/p4_panel_flash.txt"
#include "p4/color/p4_team_a.txt"
#include "p4/color/p4_team_b.txt"
#include "p4/color/p4_toppanel_story.txt"
#include "p4/color/stock_a.txt"
#include "p4/color/stock_b.txt"

#include "p4/alpha/attack_p_alpha.txt"
#include "p4/alpha/p4_panel_flash_alpha.txt"
#include "p4/alpha/p4_team_a_alpha.txt"
#include "p4/alpha/p4_team_b_alpha.txt"
#include "p4/alpha/p4_toppanel_story_alpha.txt"
#include "p4/alpha/stock_alpha.txt"
#endif

#if LOCAL==JAPAN
#include "p4/color/p4_panel_virus.txt"
#include "p4/alpha/p4_panel_virus_alpha.txt"
#include "p4/color/practice.txt"
#include "p4/alpha/practice_alpha.txt"
#include "p4/color/practices.txt"
#include "p4/alpha/practices_alpha.txt"
#elif LOCAL==AMERICA
#include "p4/color/p4_panel_virus_am.txt"
#include "p4/alpha/p4_panel_virus_alpha_am.txt"
#include "p4/color/practice_am.txt"
#include "p4/alpha/practice_alpha_am.txt"
#include "p4/color/practices_am.txt"
#include "p4/alpha/practices_alpha_am.txt"
#elif LOCAL==CHINA
#include "../../i10n/assets/zh/game/p4/color/p4_panel_virus_am.txt"
#include "../../i10n/assets/zh/game/p4/alpha/p4_panel_virus_alpha_am.txt"
#include "../../i10n/assets/zh/game/p4/color/practice_am.txt"
#include "../../i10n/assets/zh/game/p4/alpha/practice_alpha_am.txt"
#include "../../i10n/assets/zh/game/p4/color/practices_am.txt"
#include "../../i10n/assets/zh/game/p4/alpha/practices_alpha_am.txt"
#endif
#undef NON_PALETTE

static STexInfo texTbl[] = {
	{ p4_bottle_tex_addr, p4_bottle_tex_size },
	{ p4_bottle_shadow_tex_addr, p4_bottle_shadow_tex_size },
	{ p4_toppanel_team_1_tex_addr, p4_toppanel_team_1_tex_size },
	{ p4_toppanel_team_2_tex_addr, p4_toppanel_team_2_tex_size },
	{ p4_toppanel_team_3_tex_addr, p4_toppanel_team_3_tex_size },
	{ p4_toppanel_vs_1_tex_addr, p4_toppanel_vs_1_tex_size },
	{ p4_toppanel_vs_2_tex_addr, p4_toppanel_vs_2_tex_size },
	{ p4_toppanel_vs_3_tex_addr, p4_toppanel_vs_3_tex_size },

	{ p4_bottompanel_tex_addr, p4_bottompanel_tex_size },
	{ p4_cp_panel_tex_addr, p4_cp_panel_tex_size },

	{ attack_p_tex_addr, attack_p_tex_size },
	{ p4_panel_flash_tex_addr, p4_panel_flash_tex_size },
	{ p4_team_a_tex_addr, p4_team_a_tex_size },
	{ p4_team_b_tex_addr, p4_team_b_tex_size },
	{ p4_toppanel_stocklamp_tex_addr, p4_toppanel_stocklamp_tex_size },
	{ p4_toppanel_story_tex_addr, p4_toppanel_story_tex_size },
	{ stock_a_tex_addr, stock_a_tex_size },
	{ stock_b_tex_addr, stock_b_tex_size },

	{ attack_p_alpha_tex_addr, attack_p_alpha_tex_size },
	{ p4_panel_flash_alpha_tex_addr, p4_panel_flash_alpha_tex_size },
	{ p4_team_a_alpha_tex_addr, p4_team_a_alpha_tex_size },
	{ p4_team_b_alpha_tex_addr, p4_team_b_alpha_tex_size },
	{ p4_toppanel_stocklamp_alpha_tex_addr, p4_toppanel_stocklamp_alpha_tex_size },
	{ p4_toppanel_story_alpha_tex_addr, p4_toppanel_story_alpha_tex_size },
	{ stock_alpha_tex_addr, stock_alpha_tex_size },

#if LOCAL==JAPAN
	{ p4_panel_virus_tex_addr, p4_panel_virus_tex_size },
	{ p4_panel_virus_alpha_tex_addr, p4_panel_virus_alpha_tex_size },
	{ practice_tex_addr, practice_tex_size },
	{ practice_alpha_tex_addr, practice_alpha_tex_size },
	{ practices_tex_addr, practices_tex_size },
	{ practices_alpha_tex_addr, practices_alpha_tex_size },
#elif LOCAL==AMERICA
	{ p4_panel_virus_am_tex_addr, p4_panel_virus_am_tex_size },
	{ p4_panel_virus_alpha_am_tex_addr, p4_panel_virus_alpha_am_tex_size },
	{ practice_am_tex_addr, practice_am_tex_size },
	{ practice_alpha_am_tex_addr, practice_alpha_am_tex_size },
	{ practices_am_tex_addr, practices_am_tex_size },
	{ practices_alpha_am_tex_addr, practices_alpha_am_tex_size },
#elif LOCAL==CHINA
	{ p4_panel_virus_am_tex_addr, p4_panel_virus_am_tex_size },
	{ p4_panel_virus_alpha_am_tex_addr, p4_panel_virus_alpha_am_tex_size },
	{ practice_am_tex_addr, practice_am_tex_size },
	{ practice_alpha_am_tex_addr, practice_alpha_am_tex_size },
	{ practices_am_tex_addr, practices_am_tex_size },
	{ practices_alpha_am_tex_addr, practices_alpha_am_tex_size },
#endif
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };
