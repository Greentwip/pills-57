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

#include "g/ani/g01.txt"
#define NON_PALETTE
#include "g/ani/g02.txt"
#include "g/ani/g03.txt"
#include "g/ani/g04.txt"
#include "g/ani/g05.txt"
#include "g/ani/g06.txt"
#include "g/ani/g07.txt"
#include "g/ani/g08.txt"
#include "g/ani/g09.txt"
#include "g/ani/g10.txt"
#include "g/ani/g11.txt"
#include "g/ani/g12.txt"
#include "g/ani/g13.txt"
//#include "g/ani/g14.txt"
//#include "g/ani/g15.txt"
#include "g/ani/g16.txt"
#include "g/ani/g17.txt"
#include "g/ani/g18.txt"
#include "g/ani/g19.txt"
#include "g/ani/g20.txt"
#include "g/ani/g21.txt"
#include "g/ani/g22.txt"
#include "g/ani/g23.txt"
#include "g/ani/g24.txt"
#include "g/ani/g25.txt"
#include "g/ani/g26.txt"
#include "g/ani/g27.txt"
#include "g/ani/g28.txt"
#undef NON_PALETTE

static STexInfo texTbl[] = {
	{ g01_tex_addr, g01_tex_size },
	{ g02_tex_addr, g02_tex_size },
	{ g03_tex_addr, g03_tex_size },
	{ g04_tex_addr, g04_tex_size },
	{ g05_tex_addr, g05_tex_size },
	{ g06_tex_addr, g06_tex_size },
	{ g07_tex_addr, g07_tex_size },
	{ g08_tex_addr, g08_tex_size },
	{ g09_tex_addr, g09_tex_size },
	{ g10_tex_addr, g10_tex_size },
	{ g11_tex_addr, g11_tex_size },
	{ g12_tex_addr, g12_tex_size },
	{ g13_tex_addr, g13_tex_size },
	{ NULL, NULL }, //{ g14_tex_addr, g14_tex_size },
	{ NULL, NULL }, //{ g15_tex_addr, g15_tex_size },
	{ g16_tex_addr, g16_tex_size },
	{ g17_tex_addr, g17_tex_size },
	{ g18_tex_addr, g18_tex_size },
	{ g19_tex_addr, g19_tex_size },
	{ g20_tex_addr, g20_tex_size },
	{ g21_tex_addr, g21_tex_size },
	{ g22_tex_addr, g22_tex_size },
	{ g23_tex_addr, g23_tex_size },
	{ g24_tex_addr, g24_tex_size },
	{ g25_tex_addr, g25_tex_size },
	{ g26_tex_addr, g26_tex_size },
	{ g27_tex_addr, g27_tex_size },
	{ g28_tex_addr, g28_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };

//////////////////////////////////////////////////////////////////////////////
// タイムシートのテキスト化・マッド　シタイン
// 通常のアニメーション
// 攻撃のアニメーション
// ダメージのアニメーション
// 勝ちのアニメーション　2000.11.19 0709AM更新　H.Yoshikawa
// 負けのアニメーション

// 通常のアニメーション
static unsigned char anime0[] = {
	ASEQ_SET_LOOP(),
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,
		ASEQ_SET_REPEAT(4),
			3,3,4,4,5,5,4,4,
		ASEQ_EXE_REPEAT(),
		3,3,
	ASEQ_EXE_LOOP(100),
};

// 攻撃のアニメーション
static unsigned char anime1[] = {
//	1,1,1,1,1,1,1,1,1,1,1,1,2,2,
	ASEQ_SET_REPEAT(4),
		3,3,4,4,5,5,4,4,
	ASEQ_EXE_REPEAT(),
	3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	2,2,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	ASEQ_GOTO(ASEQ_NORMAL),
};

// ダメージのアニメーション
static unsigned char anime2[] = {
//	1,1,1,1,1,1,1,1,1,1,1,1,
	ASEQ_SET_REPEAT(4),
		3,3,4,4,5,5,4,4,
	ASEQ_EXE_REPEAT(),
	3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,7,7,
	ASEQ_SET_REPEAT(4),
		8,8,8,9,9,10,10,10,9,9,
	ASEQ_EXE_REPEAT(),
	8,8,
	ASEQ_GOTO(ASEQ_NORMAL),
};

// 勝ちのアニメーション　2000.11.19 0709AM更新　H.Yoshikawa
static unsigned char anime3[] = {
	ASEQ_SET_REPEAT(6),
	1,1,1,1,1,
	ASEQ_EXE_REPEAT(),
	ASEQ_SET_REPEAT(3),
	12,12,12,1,1,1,
	ASEQ_EXE_REPEAT(),
	ASEQ_SET_REPEAT(4),
	12,1,
	ASEQ_EXE_REPEAT(),
	ASEQ_SET_REPEAT(10),
	16,16,16,
	ASEQ_EXE_REPEAT(),
	2,2,2,
	ASEQ_SET_LOOP(),
		ASEQ_SET_REPEAT(8),
		11,11,11,
		ASEQ_EXE_REPEAT(),
		ASEQ_SET_REPEAT(4),
		6,6,6,
		ASEQ_EXE_REPEAT(),
		ASEQ_SET_REPEAT(8),
		11,11,11,
		ASEQ_EXE_REPEAT(),
		ASEQ_SET_REPEAT(4),
		3,3,3,4,5,5,4,4,
		ASEQ_EXE_REPEAT(),
	ASEQ_EXE_LOOP(100),
};

// 負けのアニメーション
static unsigned char anime4[] = {
	ASEQ_SET_REPEAT(6),
	1,1,1,1,1,
	ASEQ_EXE_REPEAT(),
	ASEQ_SET_REPEAT(3),
	12,12,12,1,1,1,
	ASEQ_EXE_REPEAT(),
	ASEQ_SET_REPEAT(4),
	12,1,
	ASEQ_EXE_REPEAT(),
	13,13,13,17,17,18,18,18,19,19,19,20,20,20,20,21,21,21,21,
	22,22,22,22,23,23,23,23,24,24,24,24,25,25,25,25,25,25,25,
	ASEQ_SET_LOOP(),
	26,26,26,27,27,27,28,28,28,27,27,27,
	ASEQ_EXE_LOOP(100),
};

// アニメーションデータテーブル
static unsigned char *animeTbl[] = {
	anime0, anime1, anime2, anime3, anime4,
};
static int animeTblSize[] = { sizeof(animeTbl) / sizeof(*animeTbl) };
