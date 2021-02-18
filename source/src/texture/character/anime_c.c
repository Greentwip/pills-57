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

#include "c/ani/c01.txt"
#define NON_PALETTE
#include "c/ani/c02.txt"
#include "c/ani/c03.txt"
#include "c/ani/c04.txt"
#include "c/ani/c05.txt"
#include "c/ani/c06.txt"
#include "c/ani/c07.txt"
#include "c/ani/c08.txt"
#include "c/ani/c09.txt"
#include "c/ani/c10.txt"
#include "c/ani/c11.txt"
#include "c/ani/c12.txt"
#include "c/ani/c13.txt"
#include "c/ani/c14.txt"
#include "c/ani/c15.txt"
#include "c/ani/c16.txt"
#include "c/ani/c17.txt"
#include "c/ani/c18.txt"
#include "c/ani/c19.txt"
#include "c/ani/c20.txt"
#include "c/ani/c21.txt"
#include "c/ani/c22.txt"
#include "c/ani/c23.txt"
#include "c/ani/c24.txt"
#include "c/ani/c25.txt"
#include "c/ani/c26.txt"
#include "c/ani/c27.txt"
#include "c/ani/c28.txt"
#include "c/ani/c29.txt"
#include "c/ani/c30.txt"
#include "c/ani/c31.txt"
#include "c/ani/c32.txt"
#include "c/ani/c33.txt"
#include "c/ani/c34.txt"
#undef NON_PALETTE

static STexInfo texTbl[] = {
	{ c01_tex_addr, c01_tex_size },
	{ c02_tex_addr, c02_tex_size },
	{ c03_tex_addr, c03_tex_size },
	{ c04_tex_addr, c04_tex_size },
	{ c05_tex_addr, c05_tex_size },
	{ c06_tex_addr, c06_tex_size },
	{ c07_tex_addr, c07_tex_size },
	{ c08_tex_addr, c08_tex_size },
	{ c09_tex_addr, c09_tex_size },
	{ c10_tex_addr, c10_tex_size },
	{ c11_tex_addr, c11_tex_size },
	{ c12_tex_addr, c12_tex_size },
	{ c13_tex_addr, c13_tex_size },
	{ c14_tex_addr, c14_tex_size },
	{ c15_tex_addr, c15_tex_size },
	{ c16_tex_addr, c16_tex_size },
	{ c17_tex_addr, c17_tex_size },
	{ c18_tex_addr, c18_tex_size },
	{ c19_tex_addr, c19_tex_size },
	{ c20_tex_addr, c20_tex_size },
	{ c21_tex_addr, c21_tex_size },
	{ c22_tex_addr, c22_tex_size },
	{ c23_tex_addr, c23_tex_size },
	{ c24_tex_addr, c24_tex_size },
	{ c25_tex_addr, c25_tex_size },
	{ c26_tex_addr, c26_tex_size },
	{ c27_tex_addr, c27_tex_size },
	{ c28_tex_addr, c28_tex_size },
	{ c29_tex_addr, c29_tex_size },
	{ c30_tex_addr, c30_tex_size },
	{ c31_tex_addr, c31_tex_size },
	{ c32_tex_addr, c32_tex_size },
	{ c33_tex_addr, c33_tex_size },
	{ c34_tex_addr, c34_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };

//////////////////////////////////////////////////////////////////////////////
// タイムシートのテキスト化・ハンマーロボ

// 通常のアニメーション
static unsigned char anime0[] = {
	ASEQ_SET_LOOP(),
		1,1,1,2,2,2,3,3,3,4,4,4,2,2,2,5,5,5,
	ASEQ_EXE_LOOP(100),
};

// 攻撃のアニメーション
static unsigned char anime1[] = {
//	1,1,1,
	6,6,6,7,7,7,8,8,8,9,9,9,
	10,10,10,11,11,11,11,11,11,12,12,13,13,14,14,
	15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,16,16,16,
	ASEQ_GOTO(ASEQ_NORMAL),
};

// ダメージのアニメーション
static unsigned char anime2[] = {
//	1,1,1,
	17,17,17,17,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,
	ASEQ_GOTO(ASEQ_NORMAL),
};

// 勝ちのアニメーション
static unsigned char anime3[] = {
//	1,1,1,
	19,19,19,
	ASEQ_SET_LOOP(),
		20,20,20,21,21,21,22,22,22,22,22,22,21,21,21,20,20,20,23,23,23,
		24,24,24,25,25,25,26,26,26,26,26,26,25,25,25,24,24,24,23,23,23,
	ASEQ_EXE_LOOP(100),
};

// 負けのアニメーション
static unsigned char anime4[] = {
//	1,1,1,
	27,27,27,27,27,27,27,27,27,
	ASEQ_SET_LOOP(),
		28,29,30,31,30,29,28,32,33,34,33,32,
	ASEQ_EXE_LOOP(100),
};

// アニメーションデータテーブル
static unsigned char *animeTbl[] = {
	anime0, anime1, anime2, anime3, anime4,
};
static int animeTblSize[] = { sizeof(animeTbl) / sizeof(*animeTbl) };
