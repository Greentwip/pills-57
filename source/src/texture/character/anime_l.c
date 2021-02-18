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

#include "l/ani/l01.txt"
#define NON_PALETTE
#include "l/ani/l02.txt"
#include "l/ani/l03.txt"
#include "l/ani/l04.txt"
#include "l/ani/l05.txt"
#include "l/ani/l06.txt"
#include "l/ani/l07.txt"
#include "l/ani/l08.txt"
#include "l/ani/l09.txt"
#include "l/ani/l10.txt"
#include "l/ani/l11.txt"
#include "l/ani/l12.txt"
#include "l/ani/l13.txt"
#include "l/ani/l14.txt"
#include "l/ani/l15.txt"
#include "l/ani/l16.txt"
#include "l/ani/l17.txt"
#include "l/ani/l18.txt"
#include "l/ani/l19.txt"
#include "l/ani/l20.txt"
#include "l/ani/l21.txt"
#include "l/ani/l22.txt"
#include "l/ani/l23.txt"
//#include "l/ani/l24.txt"
#include "l/ani/l25.txt"
#include "l/ani/l26.txt"
#include "l/ani/l27.txt"
#include "l/ani/l28.txt"
#include "l/ani/l29.txt"
#include "l/ani/l30.txt"
#include "l/ani/l31.txt"
#undef NON_PALETTE

static STexInfo texTbl[] = {
	{ l01_tex_addr, l01_tex_size },
	{ l02_tex_addr, l02_tex_size },
	{ l03_tex_addr, l03_tex_size },
	{ l04_tex_addr, l04_tex_size },
	{ l05_tex_addr, l05_tex_size },
	{ l06_tex_addr, l06_tex_size },
	{ l07_tex_addr, l07_tex_size },
	{ l08_tex_addr, l08_tex_size },
	{ l09_tex_addr, l09_tex_size },
	{ l10_tex_addr, l10_tex_size },
	{ l11_tex_addr, l11_tex_size },
	{ l12_tex_addr, l12_tex_size },
	{ l13_tex_addr, l13_tex_size },
	{ l14_tex_addr, l14_tex_size },
	{ l15_tex_addr, l15_tex_size },
	{ l16_tex_addr, l16_tex_size },
	{ l17_tex_addr, l17_tex_size },
	{ l18_tex_addr, l18_tex_size },
	{ l19_tex_addr, l19_tex_size },
	{ l20_tex_addr, l20_tex_size },
	{ l21_tex_addr, l21_tex_size },
	{ l22_tex_addr, l22_tex_size },
	{ l23_tex_addr, l23_tex_size },
	{ NULL, NULL }, //{ l24_tex_addr, l24_tex_size },
	{ l25_tex_addr, l25_tex_size },
	{ l26_tex_addr, l26_tex_size },
	{ l27_tex_addr, l27_tex_size },
	{ l28_tex_addr, l28_tex_size },
	{ l29_tex_addr, l29_tex_size },
	{ l30_tex_addr, l30_tex_size },
	{ l31_tex_addr, l31_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };

//////////////////////////////////////////////////////////////////////////////
// タイムシートのテキスト化・バンパイアワリオ

// 通常のアニメーション
static unsigned char anime0[] = {
	ASEQ_SET_LOOP(),
		ASEQ_SET_LOOP(),
			1,1,2,2,3,3,4,4,1,1,2,2,3,3,4,4,
		ASEQ_EXE_LOOP(75),
		1,1,5,5,5,6,6,6,7,7,7,6,6,6,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	ASEQ_EXE_LOOP(100),
};

// 攻撃のアニメーション
static unsigned char anime1[] = {
//	3,3,4,4,1,1,2,2,3,3,4,4,1,1,
	8,8,9,9,10,10,11,11,12,12,13,13,13,13,13,13,13,13,13,
	13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
	ASEQ_GOTO(ASEQ_NORMAL),
};

// ダメージのアニメーション
static unsigned char anime2[] = {
//	1,1,2,2,3,3,4,4,1,1,2,2,3,3,
	14,14,14,15,15,15,16,16,16,16,16,16,16,16,16,16,16,16,16,
	16,16,16,17,17,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,
	ASEQ_GOTO(ASEQ_NORMAL),
};

// 勝ちのアニメーション
static unsigned char anime3[] = {
//	1,1,2,2,3,3,4,4,1,1,5,5,5,6,6,6,7,7,7,7,
	ASEQ_SET_REPEAT(4),
		25,25,25,26,26,26,
	ASEQ_EXE_REPEAT(),
	25,25,3,3,4,4,1,1,2,2,3,3,4,4,1,1,5,5,5,6,6,6,7,7,7,6,6,6,5,5,5,
	ASEQ_SET_LOOP(),
		1,1,2,2,3,3,4,4,
	ASEQ_EXE_LOOP(100),
};

// 負けのアニメーション
static unsigned char anime4[] = {
	14,14,15,15,15,16,16,16,16,16,16,
	16,16,16,16,19,19,20,20,21,21,21,22,22,22,23,23,23,29,29,29,
	ASEQ_SET_LOOP(),
		27,27,27,30,30,30,28,28,28,31,31,31,28,28,28,30,30,30,
	ASEQ_EXE_LOOP(100),
};

// アニメーションデータテーブル
static unsigned char *animeTbl[] = {
	anime0, anime1, anime2, anime3, anime4,
};
static int animeTblSize[] = { sizeof(animeTbl) / sizeof(*animeTbl) };
