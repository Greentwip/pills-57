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
#include "all/color/a_button.txt"
#include "all/color/b_button.txt"
#include "all/alpha/a_button_alpha.txt"
#include "all/alpha/b_button_alpha.txt"
#elif LOCAL==AMERICA
#include "all/color/a_button_am.txt"
#include "all/color/b_button_am.txt"
#include "all/alpha/a_button_alpha_am.txt"
#include "all/alpha/b_button_alpha_am.txt"
#elif LOCAL==CHINA
#include "../../i10n/assets/zh/tutorial/all/color/a_button_am.txt"
#include "../../i10n/assets/zh/tutorial/all/color/b_button_am.txt"
#include "../../i10n/assets/zh/tutorial/all/alpha/a_button_alpha_am.txt"
#include "../../i10n/assets/zh/tutorial/all/alpha/b_button_alpha_am.txt"
#endif
#include "all/color/account_finger.txt"
#include "all/alpha/account_finger_alpha.txt"
#undef NON_PALETTE

#if LOCAL==CHINA
#include "../../i10n/assets/zh/tutorial/all/normal2/account_controller.txt"
#else
#include "all/normal2/account_controller.txt"
#endif
#include "all/normal2/account_circle.txt"
#include "all/normal2/account_message.txt"

static STexInfo texTbl[] = {
#if LOCAL==JAPAN
	{ a_button_tex_addr, a_button_tex_size },
	{ b_button_tex_addr, b_button_tex_size },
	{ a_button_alpha_tex_addr, a_button_alpha_tex_size },
	{ b_button_alpha_tex_addr, b_button_alpha_tex_size },
#elif LOCAL==AMERICA
	{ a_button_am_tex_addr, a_button_am_tex_size },
	{ b_button_am_tex_addr, b_button_am_tex_size },
	{ a_button_alpha_am_tex_addr, a_button_alpha_am_tex_size },
	{ b_button_alpha_am_tex_addr, b_button_alpha_am_tex_size },
#elif LOCAL==CHINA
	{ a_button_am_tex_addr, a_button_am_tex_size },
	{ b_button_am_tex_addr, b_button_am_tex_size },
	{ a_button_alpha_am_tex_addr, a_button_alpha_am_tex_size },
	{ b_button_alpha_am_tex_addr, b_button_alpha_am_tex_size },
#endif
	{ account_finger_tex_addr, account_finger_tex_size },
	{ account_finger_alpha_tex_addr, account_finger_alpha_tex_size },

	{ account_circle_tex_addr, account_circle_tex_size },
	{ account_controller_tex_addr, account_controller_tex_size },
	{ account_message_tex_addr, account_message_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };
