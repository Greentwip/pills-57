
#include <ultra64.h>
#include "local.h"
#include "tex_func.h"

static STexInfo  texTbl[];
static STexInfo *texTblPtr = texTbl;
static int       texTblSize[];
static int      *texTblSizePtr = texTblSize;

#define USE_TEX_SIZE
#define USE_TEX_ADDR

#define NON_PALETTE
#if LOCAL==CHINA
#include "../../i10n/assets/zh/menu/story/alpha/character_answer_alpha.txt"
#include "../../i10n/assets/zh/menu/story/color/character_answer.txt"
#else
#include "story/alpha/character_answer_alpha.txt"
#include "story/color/character_answer.txt"
#endif
#undef NON_PALETTE

#include "story/normal/map_mario_a.txt"
#include "story/normal/map_mario_b.txt"
#include "story/normal/map_wario_b.txt"

#include "story/normal2/map_cursor.txt"
#if LOCAL==CHINA
#include "../../i10n/assets/zh/menu/story/normal2/ohanashi_panel.txt"
#else
#include "story/normal2/ohanashi_panel.txt"
#endif

#include "story/normal2/short_mario.txt"
#include "story/normal2/short_wario.txt"

#define NON_PALETTE
#include "story/normal2/map_mario_a_cover.txt"
#include "story/normal2/map_mario_b_cover.txt"
#include "story/normal2/map_wario_b_cover.txt"
#include "story/normal2/spot_chr.txt"
#undef NON_PALETTE

static STexInfo texTbl[] = {
	{ character_answer_alpha_tex_addr, character_answer_alpha_tex_size },

	{ character_answer_tex_addr, character_answer_tex_size },

	{ map_mario_a_tex_addr, map_mario_a_tex_size },
	{ map_mario_b_tex_addr, map_mario_b_tex_size },
	{ map_wario_b_tex_addr, map_wario_b_tex_size },

	{ map_cursor_tex_addr, map_cursor_tex_size },
	{ ohanashi_panel_tex_addr, ohanashi_panel_tex_size },

	{ short_mario_tex_addr, short_mario_tex_size },
	{ short_wario_tex_addr, short_wario_tex_size },

	{ map_mario_a_cover_tex_addr, map_mario_a_cover_tex_size },
	{ map_mario_b_cover_tex_addr, map_mario_b_cover_tex_size },
	{ map_wario_b_cover_tex_addr, map_wario_b_cover_tex_size },
	{ spot_chr_tex_addr, spot_chr_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };
