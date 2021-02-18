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
#include "../../i10n/assets/zh/game/p2/alpha/top_gamelv_contents_alpha.txt"
#include "../../i10n/assets/zh/game/p2/alpha/top_speed_contents_alpha.txt"
#include "../../i10n/assets/zh/game/p2/alpha/bottom_stage_point_panel_alpha.txt"
#include "../../i10n/assets/zh/game/p2/alpha/bottom_virus_flash_panel_alpha.txt"
#include "../../i10n/assets/zh/game/p2/alpha/top_score_panel_1p_alpha.txt"
#include "../../i10n/assets/zh/game/p2/alpha/top_score_panel_2p_alpha.txt"
#include "../../i10n/assets/zh/game/p2/alpha/top_speed_panel_alpha.txt"
#else
#include "p2/alpha/top_gamelv_contents_alpha.txt"
#include "p2/alpha/top_speed_contents_alpha.txt"
#include "p2/alpha/bottom_stage_point_panel_alpha.txt"
#include "p2/alpha/bottom_virus_flash_panel_alpha.txt"
#include "p2/alpha/top_score_panel_1p_alpha.txt"
#include "p2/alpha/top_score_panel_2p_alpha.txt"
#include "p2/alpha/top_speed_panel_alpha.txt"
#endif

#if LOCAL==CHINA
#include "../../i10n/assets/zh/game/p2/color/bottom_flash_panel.txt"
#include "../../i10n/assets/zh/game/p2/color/bottom_stage_panel.txt"
#include "../../i10n/assets/zh/game/p2/color/top_gamelv_contents.txt"
#include "../../i10n/assets/zh/game/p2/color/top_score_panel_1p.txt"
#include "../../i10n/assets/zh/game/p2/color/top_score_panel_2p.txt"
#include "../../i10n/assets/zh/game/p2/color/top_speed_contents.txt"
#include "../../i10n/assets/zh/game/p2/color/top_speed_panel.txt"
#else
#include "p2/color/bottom_flash_panel.txt"
#include "p2/color/bottom_stage_panel.txt"
#include "p2/color/top_gamelv_contents.txt"
#include "p2/color/top_score_panel_1p.txt"
#include "p2/color/top_score_panel_2p.txt"
#include "p2/color/top_speed_contents.txt"
#include "p2/color/top_speed_panel.txt"
#endif

#if LOCAL==JAPAN
#include "p2/color/bottom_point_panel.txt"
#include "p2/color/bottom_virus_panel.txt"
#include "p2/color/top_gamelv_panel.txt"
#include "p2/alpha/top_gamelv_panel_alpha.txt"
#include "p2/color/top_ohanasi_panel.txt"
#include "p2/alpha/top_ohanasi_panel_alpha.txt"
#include "p2/color/top_viruslv_panel.txt"
#include "p2/alpha/top_viruslv_panel_alpha.txt"
#elif LOCAL==AMERICA
#include "p2/color/bottom_point_panel_am.txt"
#include "p2/color/bottom_virus_panel_am.txt"
#include "p2/color/top_gamelv_panel_am.txt"
#include "p2/alpha/top_gamelv_panel_alpha_am.txt"
#include "p2/color/top_ohanasi_panel_am.txt"
#include "p2/alpha/top_ohanasi_panel_alpha_am.txt"
#include "p2/color/top_viruslv_panel_am.txt"
#include "p2/alpha/top_viruslv_panel_alpha_am.txt"
#elif LOCAL==CHINA
#include "../../i10n/assets/zh/game/p2/color/bottom_point_panel_am.txt"
#include "../../i10n/assets/zh/game/p2/color/bottom_virus_panel_am.txt"
#include "../../i10n/assets/zh/game/p2/color/top_gamelv_panel_am.txt"
#include "../../i10n/assets/zh/game/p2/alpha/top_gamelv_panel_alpha_am.txt"
#include "../../i10n/assets/zh/game/p2/color/top_ohanasi_panel_am.txt"
#include "../../i10n/assets/zh/game/p2/alpha/top_ohanasi_panel_alpha_am.txt"
#include "../../i10n/assets/zh/game/p2/color/top_viruslv_panel_am.txt"
#include "../../i10n/assets/zh/game/p2/alpha/top_viruslv_panel_alpha_am.txt"
#endif
#undef NON_PALETTE

static STexInfo texTbl[] = {
	{ bottom_stage_point_panel_alpha_tex_addr, bottom_stage_point_panel_alpha_tex_size },
	{ bottom_virus_flash_panel_alpha_tex_addr, bottom_virus_flash_panel_alpha_tex_size },
	{ top_gamelv_contents_alpha_tex_addr, top_gamelv_contents_alpha_tex_size },
	{ top_score_panel_1p_alpha_tex_addr, top_score_panel_1p_alpha_tex_size },
	{ top_score_panel_2p_alpha_tex_addr, top_score_panel_2p_alpha_tex_size },
	{ top_speed_contents_alpha_tex_addr, top_speed_contents_alpha_tex_size },
	{ top_speed_panel_alpha_tex_addr, top_speed_panel_alpha_tex_size },

	{ bottom_flash_panel_tex_addr, bottom_flash_panel_tex_size },
	{ bottom_stage_panel_tex_addr, bottom_stage_panel_tex_size },
	{ top_gamelv_contents_tex_addr, top_gamelv_contents_tex_size },
	{ top_score_panel_1p_tex_addr, top_score_panel_1p_tex_size },
	{ top_score_panel_2p_tex_addr, top_score_panel_2p_tex_size },
	{ top_speed_contents_tex_addr, top_speed_contents_tex_size },
	{ top_speed_panel_tex_addr, top_speed_panel_tex_size },

#if LOCAL==JAPAN
	{ bottom_point_panel_tex_addr, bottom_point_panel_tex_size },
	{ bottom_virus_panel_tex_addr, bottom_virus_panel_tex_size },
	{ top_gamelv_panel_tex_addr, top_gamelv_panel_tex_size },
	{ top_gamelv_panel_alpha_tex_addr, top_gamelv_panel_alpha_tex_size },
	{ top_ohanasi_panel_tex_addr, top_ohanasi_panel_tex_size },
	{ top_ohanasi_panel_alpha_tex_addr, top_ohanasi_panel_alpha_tex_size },
	{ top_viruslv_panel_tex_addr, top_viruslv_panel_tex_size },
	{ top_viruslv_panel_alpha_tex_addr, top_viruslv_panel_alpha_tex_size },
#elif LOCAL==AMERICA
	{ bottom_point_panel_am_tex_addr, bottom_point_panel_am_tex_size },
	{ bottom_virus_panel_am_tex_addr, bottom_virus_panel_am_tex_size },
	{ top_gamelv_panel_am_tex_addr, top_gamelv_panel_am_tex_size },
	{ top_gamelv_panel_alpha_am_tex_addr, top_gamelv_panel_alpha_am_tex_size },
	{ top_ohanasi_panel_am_tex_addr, top_ohanasi_panel_am_tex_size },
	{ top_ohanasi_panel_alpha_am_tex_addr, top_ohanasi_panel_alpha_am_tex_size },
	{ top_viruslv_panel_am_tex_addr, top_viruslv_panel_am_tex_size },
	{ top_viruslv_panel_alpha_am_tex_addr, top_viruslv_panel_alpha_am_tex_size },
#elif LOCAL==CHINA
	{ bottom_point_panel_am_tex_addr, bottom_point_panel_am_tex_size },
	{ bottom_virus_panel_am_tex_addr, bottom_virus_panel_am_tex_size },
	{ top_gamelv_panel_am_tex_addr, top_gamelv_panel_am_tex_size },
	{ top_gamelv_panel_alpha_am_tex_addr, top_gamelv_panel_alpha_am_tex_size },
	{ top_ohanasi_panel_am_tex_addr, top_ohanasi_panel_am_tex_size },
	{ top_ohanasi_panel_alpha_am_tex_addr, top_ohanasi_panel_alpha_am_tex_size },
	{ top_viruslv_panel_am_tex_addr, top_viruslv_panel_am_tex_size },
	{ top_viruslv_panel_alpha_am_tex_addr, top_viruslv_panel_alpha_am_tex_size },
#endif
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };
