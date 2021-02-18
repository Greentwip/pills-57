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

#include "e/ani/e01.txt"
#define NON_PALETTE
#include "e/ani/e02.txt"
#include "e/ani/e03.txt"
#include "e/ani/e04.txt"
#include "e/ani/e05.txt"
#include "e/ani/e06.txt"
#include "e/ani/e07.txt"
#include "e/ani/e08.txt"
#include "e/ani/e09.txt"
#include "e/ani/e10.txt"
#include "e/ani/e11.txt"
#include "e/ani/e12.txt"
#include "e/ani/e13.txt"
#include "e/ani/e14.txt"
#include "e/ani/e15.txt"
#include "e/ani/e16.txt"
#include "e/ani/e17.txt"
#include "e/ani/e18.txt"
#include "e/ani/e19.txt"
#include "e/ani/e20.txt"
#include "e/ani/e21.txt"
#include "e/ani/e22.txt"
#include "e/ani/e23.txt"
#include "e/ani/e24.txt"
#include "e/ani/e25.txt"
#include "e/ani/e26.txt"
#include "e/ani/e27.txt"
#include "e/ani/e28.txt"
#include "e/ani/e29.txt"
#include "e/ani/e30.txt"
#include "e/ani/e31.txt"
#include "e/ani/e32.txt"
#include "e/ani/e33.txt"
#include "e/ani/e34.txt"
#undef NON_PALETTE

static STexInfo texTbl[] = {
	{ e01_tex_addr, e01_tex_size },
	{ e02_tex_addr, e02_tex_size },
	{ e03_tex_addr, e03_tex_size },
	{ e04_tex_addr, e04_tex_size },
	{ e05_tex_addr, e05_tex_size },
	{ e06_tex_addr, e06_tex_size },
	{ e07_tex_addr, e07_tex_size },
	{ e08_tex_addr, e08_tex_size },
	{ e09_tex_addr, e09_tex_size },
	{ e10_tex_addr, e10_tex_size },
	{ e11_tex_addr, e11_tex_size },
	{ e12_tex_addr, e12_tex_size },
	{ e13_tex_addr, e13_tex_size },
	{ e14_tex_addr, e14_tex_size },
	{ e15_tex_addr, e15_tex_size },
	{ e16_tex_addr, e16_tex_size },
	{ e17_tex_addr, e17_tex_size },
	{ e18_tex_addr, e18_tex_size },
	{ e19_tex_addr, e19_tex_size },
	{ e20_tex_addr, e20_tex_size },
	{ e21_tex_addr, e21_tex_size },
	{ e22_tex_addr, e22_tex_size },
	{ e23_tex_addr, e23_tex_size },
	{ e24_tex_addr, e24_tex_size },
	{ e25_tex_addr, e25_tex_size },
	{ e26_tex_addr, e26_tex_size },
	{ e27_tex_addr, e27_tex_size },
	{ e28_tex_addr, e28_tex_size },
	{ e29_tex_addr, e29_tex_size },
	{ e30_tex_addr, e30_tex_size },
	{ e31_tex_addr, e31_tex_size },
	{ e32_tex_addr, e32_tex_size },
	{ e33_tex_addr, e33_tex_size },
	{ e34_tex_addr, e34_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };

//////////////////////////////////////////////////////////////////////////////
// タイムシートのテキスト化・つまりがえる

// 通常のアニメーション
static unsigned char anime0[] = {
	ASEQ_SET_LOOP(),
		1,1,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,
	ASEQ_EXE_LOOP(100),
};

// 攻撃のアニメーション
static unsigned char anime1[] = {
//	1,1,1,1,1,1,
	7,7,7,7,8,8,8,8,9,9,9,9,9,9,10,10,10,10,11,11,11,11,12,12,
	12,12,13,13,13,13,14,14,14,14,14,14,15,15,15,15,16,16,16,16,17,17,17,17,
	ASEQ_GOTO(ASEQ_NORMAL),
};

// ダメージのアニメーション
static unsigned char anime2[] = {
//	1,1,1,1,1,1,
	18,18,18,18,19,19,19,19,20,20,20,20,
	ASEQ_SET_REPEAT(3),
		21,21,21,21,21,21,22,22,22,22,23,23,23,23,24,24,24,24,24,25,25,25,25,25,25,25,25,
	ASEQ_EXE_REPEAT(),
	21,21,21,21,21,20,20,20,20,19,19,19,19,18,18,18,18,
	ASEQ_GOTO(ASEQ_NORMAL),
};

// 勝ちのアニメーション
static unsigned char anime3[] = {
	ASEQ_SET_LOOP(),
		26,26,26,26,26,26,27,27,27,27,28,28,28,
		28,29,29,29,29,29,30,30,30,30,30,30,30,
		ASEQ_SET_REPEAT(4),
			30,30,30,30,28,28,28,28,26,26,26,26,26,
		ASEQ_EXE_REPEAT(),
	ASEQ_EXE_LOOP(100),
};

// 負けのアニメーション
static unsigned char anime4[] = {
	31,31,31,31,32,32,32,32,32,32,32,32,33,33,33,34,34,34,34,
	ASEQ_SET_LOOP(),
		32,32,32,32,32,32,32,32,33,33,33,34,34,34,34,32,32,32,32,33,33,33,33,34,34,34,34,
	ASEQ_EXE_LOOP(100),
};

// アニメーションデータテーブル
static unsigned char *animeTbl[] = {
	anime0, anime1, anime2, anime3, anime4,
};
static int animeTblSize[] = { sizeof(animeTbl) / sizeof(*animeTbl) };
