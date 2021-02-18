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
#include "../../i10n/assets/zh/menu/char/color/comlv.txt"
#include "../../i10n/assets/zh/menu/char/alpha/comlv_alpha.txt"
#else
#include "char/color/comlv.txt"
#include "char/alpha/comlv_alpha.txt"
#endif
#undef NON_PALETTE

#if LOCAL==CHINA
#include "../../i10n/assets/zh/menu/char/normal2/hard_comlv.txt"
#else
#include "char/normal2/hard_comlv.txt"
#endif

#if LOCAL==JAPAN
#include "char/normal/m_mario_face.txt"
#include "char/normal/v_wario_face.txt"
#include "char/normal2/character_panel.txt"
#include "char/normal2/character_lv_panel.txt"
#elif LOCAL==AMERICA
#include "char/normal/m_mario_face_am.txt"
#include "char/normal/v_wario_face_am.txt"
#include "char/normal2/character_panel_am.txt"
#include "char/normal2/character_lv_panel_am.txt"
#elif LOCAL==CHINA
#include "../../i10n/assets/zh/menu/char/normal/m_mario_face_am.txt"
#include "../../i10n/assets/zh/menu/char/normal/v_wario_face_am.txt"
#include "../../i10n/assets/zh/menu/char/normal2/character_panel_am.txt"
#include "../../i10n/assets/zh/menu/char/normal2/character_lv_panel_am.txt"
#endif

static STexInfo texTbl[] = {
	{ comlv_tex_addr, comlv_tex_size },
	{ comlv_alpha_tex_addr, comlv_alpha_tex_size },

	{ hard_comlv_tex_addr, hard_comlv_tex_size },

#if LOCAL==JAPAN
	{ m_mario_face_tex_addr, m_mario_face_tex_size },
	{ v_wario_face_tex_addr, v_wario_face_tex_size },
	{ character_panel_tex_addr, character_panel_tex_size },
	{ character_lv_panel_tex_addr, character_lv_panel_tex_size },
#elif LOCAL==AMERICA
	{ m_mario_face_am_tex_addr, m_mario_face_am_tex_size },
	{ v_wario_face_am_tex_addr, v_wario_face_am_tex_size },
	{ character_panel_am_tex_addr, character_panel_am_tex_size },
	{ character_lv_panel_am_tex_addr, character_lv_panel_am_tex_size },
#elif LOCAL==CHINA
	{ m_mario_face_am_tex_addr, m_mario_face_am_tex_size },
	{ v_wario_face_am_tex_addr, v_wario_face_am_tex_size },
	{ character_panel_am_tex_addr, character_panel_am_tex_size },
	{ character_lv_panel_am_tex_addr, character_lv_panel_am_tex_size },
#endif
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };
