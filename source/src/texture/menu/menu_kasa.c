
#include <ultra64.h>
#include "tex_func.h"

static STexInfo  texTbl[];
static STexInfo *texTblPtr = texTbl;
static int       texTblSize[];
static int      *texTblSizePtr = texTblSize;

#define USE_TEX_SIZE
#define USE_TEX_ADDR

#define NON_PALETTE
#include "kasa/alpha/kasamaru_alpha.txt"

#include "kasa/color/kasamaru01.txt"
#include "kasa/color/kasamaru02.txt"
#include "kasa/color/kasamaru03.txt"
#include "kasa/color/kasamaru04.txt"
#undef NON_PALETTE

//#include "kasa/normal/kasamaru_mouth.txt"

static STexInfo texTbl[] = {
	{ kasamaru_alpha_tex_addr, kasamaru_alpha_tex_size },

	{ kasamaru01_tex_addr, kasamaru01_tex_size },
	{ kasamaru02_tex_addr, kasamaru02_tex_size },
	{ kasamaru03_tex_addr, kasamaru03_tex_size },
	{ kasamaru04_tex_addr, kasamaru04_tex_size },

//	{ kasamaru_mouth_tex_addr, kasamaru_mouth_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };
