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
#include "al/normal2/stardust01.txt"
#include "al/normal2/stardust02.txt"
#include "al/normal2/stardust03.txt"
#include "al/normal2/stardust04.txt"
#include "al/normal2/stardust05.txt"
#include "al/normal2/stardust06.txt"
#include "al/normal2/stardust07.txt"
#include "al/normal2/stardust08.txt"
#include "al/normal2/stardust09.txt"
#include "al/normal2/stardust10.txt"
#include "al/normal2/stardust11.txt"
#include "al/normal2/stardust12.txt"
#include "../menu/common/normal2/constituent_cursor2.txt"
#undef NON_PALETTE

#define NON_PALETTE
#include "al/color/al_virus_number_01.txt"
#if LOCAL==CHINA
#include "../../i10n/assets/zh/game/al/color/dm_title_01.txt"
#include "../../i10n/assets/zh/game/al/color/replay.txt"
#else
#include "al/color/dm_title_01.txt"
#include "al/color/replay.txt"
#endif
#include "al/color/new_star.txt"
#include "al/color/rank_figure.txt"

#include "al/alpha/al_virus_number_alpha.txt"
#if LOCAL==CHINA
#include "../../i10n/assets/zh/game/al/alpha/dm_title_01_alpha.txt"
#include "../../i10n/assets/zh/game/al/alpha/replay_alpha.txt"
#else
#include "al/alpha/dm_title_01_alpha.txt"
#include "al/alpha/replay_alpha.txt"
#endif
#include "al/alpha/new_star_alpha.txt"
#include "al/alpha/rank_figure_alpha.txt"

#if LOCAL==JAPAN
#include "al/color/dm_info_01.txt"
#include "al/alpha/dm_info_01_alpha.txt"
#include "al/color/dm_logo_02.txt"
#include "al/alpha/dm_logo_02_alpha.txt"
#include "al/color/dm_title_02.txt"
#include "al/alpha/dm_title_02_alpha.txt"
#elif LOCAL==AMERICA
#include "al/color/dm_info_01_am.txt"
#include "al/alpha/dm_info_01_alpha_am.txt"
#include "al/color/dm_logo_02_am.txt"
#include "al/alpha/dm_logo_02_alpha_am.txt"
#include "al/color/dm_title_02_am.txt"
#include "al/alpha/dm_title_02_alpha_am.txt"
#elif LOCAL==CHINA
#include "../../i10n/assets/zh/game/al/color/dm_info_01_am.txt"
#include "../../i10n/assets/zh/game/al/alpha/dm_info_01_alpha_am.txt" 
#include "../../i10n/assets/zh/game/al/color/dm_logo_02_am.txt"
#include "../../i10n/assets/zh/game/al/alpha/dm_logo_02_alpha_am.txt"
#include "../../i10n/assets/zh/game/al/color/dm_title_02_am.txt"
#include "../../i10n/assets/zh/game/al/alpha/dm_title_02_alpha_am.txt"
#endif
#undef NON_PALETTE

static STexInfo texTbl[] = {
	{ stardust01_tex_addr, stardust01_tex_size },
	{ stardust02_tex_addr, stardust02_tex_size },
	{ stardust03_tex_addr, stardust03_tex_size },
	{ stardust04_tex_addr, stardust04_tex_size },
	{ stardust05_tex_addr, stardust05_tex_size },
	{ stardust06_tex_addr, stardust06_tex_size },
	{ stardust07_tex_addr, stardust07_tex_size },
	{ stardust08_tex_addr, stardust08_tex_size },
	{ stardust09_tex_addr, stardust09_tex_size },
	{ stardust10_tex_addr, stardust10_tex_size },
	{ stardust11_tex_addr, stardust11_tex_size },
	{ stardust12_tex_addr, stardust12_tex_size },
	{ constituent_cursor2_tex_addr, constituent_cursor2_tex_size },

	{ al_virus_number_01_tex_addr, al_virus_number_01_tex_size },
	{ dm_title_01_tex_addr, dm_title_01_tex_size },
	{ replay_tex_addr, replay_tex_size },
	{ new_star_tex_addr, new_star_tex_size },
	{ rank_figure_tex_addr, rank_figure_tex_size },

	{ al_virus_number_alpha_tex_addr, al_virus_number_alpha_tex_size },
	{ dm_title_01_alpha_tex_addr, dm_title_01_alpha_tex_size },
	{ replay_alpha_tex_addr, replay_alpha_tex_size },
	{ new_star_alpha_tex_addr, new_star_alpha_tex_size },
	{ rank_figure_alpha_tex_addr, rank_figure_alpha_tex_size },

#if LOCAL==JAPAN
	{ dm_info_01_tex_addr, dm_info_01_tex_size },
	{ dm_info_01_alpha_tex_addr, dm_info_01_alpha_tex_size },
	{ dm_logo_02_tex_addr, dm_logo_02_tex_size },
	{ dm_logo_02_alpha_tex_addr, dm_logo_02_alpha_tex_size },
	{ dm_title_02_tex_addr, dm_title_02_tex_size },
	{ dm_title_02_alpha_tex_addr, dm_title_02_alpha_tex_size },
#elif LOCAL==AMERICA
	{ dm_info_01_am_tex_addr, dm_info_01_am_tex_size },
	{ dm_info_01_alpha_am_tex_addr, dm_info_01_alpha_am_tex_size },
	{ dm_logo_02_am_tex_addr, dm_logo_02_am_tex_size },
	{ dm_logo_02_alpha_am_tex_addr, dm_logo_02_alpha_am_tex_size },
	{ dm_title_02_am_tex_addr, dm_title_02_am_tex_size },
	{ dm_title_02_alpha_am_tex_addr, dm_title_02_alpha_am_tex_size },
#elif LOCAL==CHINA
        { dm_info_01_am_tex_addr, dm_info_01_am_tex_size },
        { dm_info_01_alpha_am_tex_addr, dm_info_01_alpha_am_tex_size },
        { dm_logo_02_am_tex_addr, dm_logo_02_am_tex_size },
        { dm_logo_02_alpha_am_tex_addr, dm_logo_02_alpha_am_tex_size },
        { dm_title_02_am_tex_addr, dm_title_02_am_tex_size },
        { dm_title_02_alpha_am_tex_addr, dm_title_02_alpha_am_tex_size },
#endif
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };
