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

#include "h/ani/h01.txt"
#define NON_PALETTE
#include "h/ani/h02.txt"
#include "h/ani/h03.txt"
#include "h/ani/h04.txt"
#include "h/ani/h05.txt"
#include "h/ani/h06.txt"
#include "h/ani/h07.txt"
#include "h/ani/h08.txt"
#include "h/ani/h09.txt"
#include "h/ani/h10.txt"
#include "h/ani/h11.txt"
#include "h/ani/h12.txt"
#include "h/ani/h13.txt"
#include "h/ani/h14.txt"
#include "h/ani/h15.txt"
#include "h/ani/h16.txt"
#include "h/ani/h17.txt"
#include "h/ani/h18.txt"
#include "h/ani/h19.txt"
#include "h/ani/h20.txt"
#include "h/ani/h21.txt"
#include "h/ani/h22.txt"
#include "h/ani/h23.txt"
#include "h/ani/h24.txt"
#include "h/ani/h25.txt"
#include "h/ani/h26.txt"
#include "h/ani/h27.txt"
#include "h/ani/h28.txt"
#include "h/ani/h29.txt"
#include "h/ani/h30.txt"
#include "h/ani/h31.txt"
#undef NON_PALETTE

static STexInfo texTbl[] = {
	{ h01_tex_addr, h01_tex_size },
	{ h02_tex_addr, h02_tex_size },
	{ h03_tex_addr, h03_tex_size },
	{ h04_tex_addr, h04_tex_size },
	{ h05_tex_addr, h05_tex_size },
	{ h06_tex_addr, h06_tex_size },
	{ h07_tex_addr, h07_tex_size },
	{ h08_tex_addr, h08_tex_size },
	{ h09_tex_addr, h09_tex_size },
	{ h10_tex_addr, h10_tex_size },
	{ h11_tex_addr, h11_tex_size },
	{ h12_tex_addr, h12_tex_size },
	{ h13_tex_addr, h13_tex_size },
	{ h14_tex_addr, h14_tex_size },
	{ h15_tex_addr, h15_tex_size },
	{ h16_tex_addr, h16_tex_size },
	{ h17_tex_addr, h17_tex_size },
	{ h18_tex_addr, h18_tex_size },
	{ h19_tex_addr, h19_tex_size },
	{ h20_tex_addr, h20_tex_size },
	{ h21_tex_addr, h21_tex_size },
	{ h22_tex_addr, h22_tex_size },
	{ h23_tex_addr, h23_tex_size },
	{ h24_tex_addr, h24_tex_size },
	{ h25_tex_addr, h25_tex_size },
	{ h26_tex_addr, h26_tex_size },
	{ h27_tex_addr, h27_tex_size },
	{ h28_tex_addr, h28_tex_size },
	{ h29_tex_addr, h29_tex_size },
	{ h30_tex_addr, h30_tex_size },
	{ h31_tex_addr, h31_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };

//////////////////////////////////////////////////////////////////////////////
// タイムシートのテキスト化・まゆぴー

// 通常のアニメーション
static unsigned char anime0[] = {
	ASEQ_SET_LOOP(),
		1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,
		5,5,5,5,4,4,4,4,3,3,3,3,2,2,2,2,
	ASEQ_EXE_LOOP(100),
};

// 攻撃のアニメーション
static unsigned char anime1[] = {
//	1,1,1,1,
	6,6,6,6,7,7,7,8,8,8,9,9,9,10,10,10,11,11,11,12,12,12,7,7,7,
	8,8,8,9,9,9,10,10,11,11,11,12,12,12,12,7,7,7,13,13,13,14,14,14,
	ASEQ_GOTO(ASEQ_NORMAL),
};

// ダメージのアニメーション
static unsigned char anime2[] = {
//	1,1,1,
	15,15,15,16,16,16,17,17,17,18,18,18,19,19,19,
	18,18,18,17,17,17,19,19,19,20,20,20,21,21,21,
	ASEQ_GOTO(ASEQ_NORMAL),
};

// 勝ちのアニメーション
static unsigned char anime3[] = {
//	1,1,1,
	22,22,22,23,23,23,
	ASEQ_SET_LOOP(),
		24,24,24,25,25,25,26,26,26,27,27,27,26,26,26,
	ASEQ_EXE_LOOP(100),
};

// 負けのアニメーション
static unsigned char anime4[] = {
//	1,1,1,
	28,28,28,29,29,29,
	ASEQ_SET_LOOP(),
		30,30,30,30,30,30,31,31,31,30,30,30,31,31,31,31,
	ASEQ_EXE_LOOP(100),
};

// アニメーションデータテーブル
static unsigned char *animeTbl[] = {
	anime0, anime1, anime2, anime3, anime4,
};
static int animeTblSize[] = { sizeof(animeTbl) / sizeof(*animeTbl) };
