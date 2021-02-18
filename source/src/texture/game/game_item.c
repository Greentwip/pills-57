
#include <ultra64.h>
#include "tex_func.h"

static STexInfo  texTbl[];
static STexInfo *texTblPtr = texTbl;
static int       texTblSize[];
static int      *texTblSizePtr = texTblSize;

#define USE_TEX_SIZE
#define USE_TEX_ADDR

#include "item/mitem08r.txt"
#include "item/mitem08rx.txt"
#include "item/mitem10r.txt"
#include "item/mitem10rx.txt"

#define NON_TEXTURE
#include "item/mitem08b.txt"
#include "item/mitem08bx.txt"
#include "item/mitem08y.txt"
#include "item/mitem08yx.txt"
#include "item/mitem10b.txt"
#include "item/mitem10bx.txt"
#include "item/mitem10y.txt"
#include "item/mitem10yx.txt"
#undef NON_TEXTURE

#define NON_PALETTE
#include "item/f1.txt"
#include "item/f2.txt"
#include "item/f3.txt"
#undef NON_PALETTE

static STexInfo texTbl[] = {
	{ mitem08b_tex_addr, mitem08b_tex_size },
	{ mitem08bx_tex_addr, mitem08bx_tex_size },
	{ mitem08r_tex_addr, mitem08r_tex_size },
	{ mitem08rx_tex_addr, mitem08rx_tex_size },
	{ mitem08y_tex_addr, mitem08y_tex_size },
	{ mitem08yx_tex_addr, mitem08yx_tex_size },
	{ mitem10b_tex_addr, mitem10b_tex_size },
	{ mitem10bx_tex_addr, mitem10bx_tex_size },
	{ mitem10r_tex_addr, mitem10r_tex_size },
	{ mitem10rx_tex_addr, mitem10rx_tex_size },
	{ mitem10y_tex_addr, mitem10y_tex_size },
	{ mitem10yx_tex_addr, mitem10yx_tex_size },

	{ f1_tex_addr, f1_tex_size },
	{ f2_tex_addr, f2_tex_size },
	{ f3_tex_addr, f3_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };
