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
#include "cont/normal2/account_controller_line_b.txt"
#elif LOCAL==AMERICA
#include "cont/normal2/account_controller_line_b_am.txt"
#elif LOCAL==CHINA
#include "../../i10n/assets/zh/menu/cont/normal2/account_controller_line_b_am.txt"
#endif
#undef NON_PALETTE

#if LOCAL==JAPAN
#include "cont/normal2/account_operation.txt"
#include "cont/normal2/r_triger.txt"
#include "cont/normal2/l_triger.txt"
#elif LOCAL==AMERICA
#include "cont/normal2/control_pad_a.txt"
#include "cont/normal2/control_pad_b.txt"
#elif LOCAL==CHINA
#include "../../i10n/assets/zh/menu/cont/normal2/control_pad_a.txt"
#include "../../i10n/assets/zh/menu/cont/normal2/control_pad_b.txt"
#endif

static STexInfo texTbl[] = {
#if LOCAL==JAPAN
	{ account_controller_line_b_tex_addr, account_controller_line_b_tex_size },
#elif LOCAL==AMERICA
	{ account_controller_line_b_am_tex_addr, account_controller_line_b_am_tex_size },
#elif LOCAL==CHINA
	{ account_controller_line_b_am_tex_addr, account_controller_line_b_am_tex_size },
#endif

#if LOCAL==JAPAN
	{ account_operation_tex_addr, account_operation_tex_size },
	{ r_triger_tex_addr, r_triger_tex_size },
	{ l_triger_tex_addr, l_triger_tex_size },
#elif LOCAL==AMERICA
	{ control_pad_a_tex_addr, control_pad_a_tex_size },
	{ control_pad_b_tex_addr, control_pad_b_tex_size },
#elif LOCAL==CHINA
	{ control_pad_a_tex_addr, control_pad_a_tex_size },
	{ control_pad_b_tex_addr, control_pad_b_tex_size },
#endif
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };
