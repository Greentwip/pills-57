#include <ultra64.h>
#include "tex_func.h"
#include "char_anime.h"

static STexInfo  texTbl[];
static STexInfo *texTblPtr = texTbl;
static int       texTblSize[];
static int      *texTblSizePtr = texTblSize;

static unsigned char  *animeTbl[];
static unsigned char **animeTblPtr = animeTbl;
static int             animeTblSize[];
static int            *animeTblSizePtr = animeTblSize;

//////////////////////////////////////////////////////////////////////////////

#define USE_TEX_SIZE
#define USE_TEX_ADDR

#include "d/ani/d01.txt"
#define NON_PALETTE
#include "d/ani/d02.txt"
#include "d/ani/d03.txt"
#include "d/ani/d04.txt"
#include "d/ani/d05.txt"
#include "d/ani/d06.txt"
#include "d/ani/d07.txt"
#include "d/ani/d08.txt"
#include "d/ani/d09.txt"
#include "d/ani/d10.txt"
#include "d/ani/d11.txt"
#include "d/ani/d12.txt"
#include "d/ani/d13.txt"
#include "d/ani/d14.txt"
#include "d/ani/d15.txt"
#include "d/ani/d16.txt"
#include "d/ani/d17.txt"
#include "d/ani/d18.txt"
#include "d/ani/d19.txt"
#include "d/ani/d20.txt"
#include "d/ani/d21.txt"
#include "d/ani/d22.txt"
#include "d/ani/d23.txt"
#include "d/ani/d24.txt"
#include "d/ani/d25.txt"
#include "d/ani/d26.txt"
#include "d/ani/d27.txt"
#include "d/ani/d28.txt"
#include "d/ani/d29.txt"
#include "d/ani/d30.txt"
#include "d/ani/d31.txt"
#include "d/ani/d32.txt"
#include "d/ani/d33.txt"
#include "d/ani/d34.txt"
#undef NON_PALETTE

static STexInfo texTbl[] = {
	{ d01_tex_addr, d01_tex_size },
	{ d02_tex_addr, d02_tex_size },
	{ d03_tex_addr, d03_tex_size },
	{ d04_tex_addr, d04_tex_size },
	{ d05_tex_addr, d05_tex_size },
	{ d06_tex_addr, d06_tex_size },
	{ d07_tex_addr, d07_tex_size },
	{ d08_tex_addr, d08_tex_size },
	{ d09_tex_addr, d09_tex_size },
	{ d10_tex_addr, d10_tex_size },
	{ d11_tex_addr, d11_tex_size },
	{ d12_tex_addr, d12_tex_size },
	{ d13_tex_addr, d13_tex_size },
	{ d14_tex_addr, d14_tex_size },
	{ d15_tex_addr, d15_tex_size },
	{ d16_tex_addr, d16_tex_size },
	{ d17_tex_addr, d17_tex_size },
	{ d18_tex_addr, d18_tex_size },
	{ d19_tex_addr, d19_tex_size },
	{ d20_tex_addr, d20_tex_size },
	{ d21_tex_addr, d21_tex_size },
	{ d22_tex_addr, d22_tex_size },
	{ d23_tex_addr, d23_tex_size },
	{ d24_tex_addr, d24_tex_size },
	{ d25_tex_addr, d25_tex_size },
	{ d26_tex_addr, d26_tex_size },
	{ d27_tex_addr, d27_tex_size },
	{ d28_tex_addr, d28_tex_size },
	{ d29_tex_addr, d29_tex_size },
	{ d30_tex_addr, d30_tex_size },
	{ d31_tex_addr, d31_tex_size },
	{ d32_tex_addr, d32_tex_size },
	{ d33_tex_addr, d33_tex_size },
	{ d34_tex_addr, d34_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };

//////////////////////////////////////////////////////////////////////////////
// タイムシートのテキスト化・ふーせんまじん

// 通常のアニメーション
static unsigned char anime0[] = {
	ASEQ_SET_LOOP(),
		1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,
		7,7,7,7,6,6,6,6,5,5,5,5,4,4,4,4,3,3,3,3,2,2,2,2,
	ASEQ_EXE_LOOP(100),
};

// 攻撃のアニメーション
static unsigned char anime1[] = {
	8,8,8,8,9,9,9,10,10,10,11,11,11,12,12,12,13,13,
	13,10,10,10,11,11,11,12,12,12,13,13,13,10,10,10,
	ASEQ_GOTO(ASEQ_NORMAL),
};

// ダメージのアニメーション
static unsigned char anime2[] = {
	14,14,14,
	ASEQ_SET_REPEAT(2),
		15,15,15,16,16,16,17,17,17,18,18,18,17,17,17,16,16,16,
	ASEQ_EXE_REPEAT(),
	15,15,15,16,16,16,17,17,17,
	ASEQ_GOTO(ASEQ_NORMAL),
};

// 勝ちのアニメーション
static unsigned char anime3[] = {
	19,19,19,19,20,20,20,21,21,21,
	ASEQ_SET_LOOP(),
		19,19,19,19,20,20,20,21,21,21,
	ASEQ_EXE_LOOP(100),
};

// 負けのアニメーション
static unsigned char anime4[] = {
	23,23,23,23,24,24,24,24,25,25,25,25,26,26,26,26,27,27,
	27,27,28,28,28,28,29,29,29,29,30,30,30,30,31,31,31,31,
	ASEQ_SET_LOOP(),
		32,32,32,32,33,33,33,33,34,34,34,34,
	ASEQ_EXE_LOOP(100),
};

// アニメーションデータテーブル
static unsigned char *animeTbl[] = {
	anime0, anime1, anime2, anime3, anime4,
};
static int animeTblSize[] = { sizeof(animeTbl) / sizeof(*animeTbl) };
