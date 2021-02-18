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

#include "o/ani/o01.txt"
#define NON_PALETTE
#include "o/ani/o02.txt"
#include "o/ani/o03.txt"
#include "o/ani/o04.txt"
#include "o/ani/o05.txt"
#include "o/ani/o06.txt"
#include "o/ani/o07.txt"
#include "o/ani/o08.txt"
#include "o/ani/o09.txt"
#include "o/ani/o10.txt"
#include "o/ani/o11.txt"
#include "o/ani/o12.txt"
#include "o/ani/o13.txt"
#include "o/ani/o14.txt"
#include "o/ani/o15.txt"
#include "o/ani/o16.txt"
#include "o/ani/o17.txt"
#include "o/ani/o18.txt"
#include "o/ani/o19.txt"
#include "o/ani/o20.txt"
#include "o/ani/o21.txt"
#include "o/ani/o22.txt"
#include "o/ani/o23.txt"
#include "o/ani/o24.txt"
#include "o/ani/o25.txt"
#include "o/ani/o26.txt"
#include "o/ani/o27.txt"
#include "o/ani/o28.txt"
#include "o/ani/o29.txt"
#include "o/ani/o30.txt"
#include "o/ani/o31.txt"
#include "o/ani/o32.txt"
#include "o/ani/o33.txt"
#include "o/ani/o34.txt"
#undef NON_PALETTE

static STexInfo texTbl[] = {
	{ o01_tex_addr, o01_tex_size },
	{ o02_tex_addr, o02_tex_size },
	{ o03_tex_addr, o03_tex_size },
	{ o04_tex_addr, o04_tex_size },
	{ o05_tex_addr, o05_tex_size },
	{ o06_tex_addr, o06_tex_size },
	{ o07_tex_addr, o07_tex_size },
	{ o08_tex_addr, o08_tex_size },
	{ o09_tex_addr, o09_tex_size },
	{ o10_tex_addr, o10_tex_size },
	{ o11_tex_addr, o11_tex_size },
	{ o12_tex_addr, o12_tex_size },
	{ o13_tex_addr, o13_tex_size },
	{ o14_tex_addr, o14_tex_size },
	{ o15_tex_addr, o15_tex_size },
	{ o16_tex_addr, o16_tex_size },
	{ o17_tex_addr, o17_tex_size },
	{ o18_tex_addr, o18_tex_size },
	{ o19_tex_addr, o19_tex_size },
	{ o20_tex_addr, o20_tex_size },
	{ o21_tex_addr, o21_tex_size },
	{ o22_tex_addr, o22_tex_size },
	{ o23_tex_addr, o23_tex_size },
	{ o24_tex_addr, o24_tex_size },
	{ o25_tex_addr, o25_tex_size },
	{ o26_tex_addr, o26_tex_size },
	{ o27_tex_addr, o27_tex_size },
	{ o28_tex_addr, o28_tex_size },
	{ o29_tex_addr, o29_tex_size },
	{ o30_tex_addr, o30_tex_size },
	{ o31_tex_addr, o31_tex_size },
	{ o32_tex_addr, o32_tex_size },
	{ o33_tex_addr, o33_tex_size },
	{ o34_tex_addr, o34_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };

//////////////////////////////////////////////////////////////////////////////
// タイムシートのテキスト化・メタルマリオ
// 通常のアニメーション
// 攻撃のアニメーション
// ダメージのアニメーション
// 勝ちのアニメーション　2000.11.20 1333PM更新　H.Yoshikawa
// 負けのアニメーション　2000.11.20 1333PM更新　H.Yoshikawa

// 通常のアニメーション
static unsigned char anime0[] = {
	ASEQ_SET_LOOP(),
		ASEQ_SET_LOOP(),
			1,1,1,2,2,3,3,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,6,6,6,
		ASEQ_EXE_LOOP(75),
		7,7,7,7,7,7,8,8,8,8,8,8,7,7,7,7,7,7,8,8,8,8,8,8,7,7,7,7,7,7,
	ASEQ_EXE_LOOP(100),
};

// 攻撃のアニメーション
static unsigned char anime1[] = {
//	1,1,1,
	9,9,9,10,10,10,10,10,10,11,11,11,12,12,12,13,13,
	13,12,12,12,11,11,11,14,14,14,15,15,15,10,10,10,9,9,9,
	ASEQ_GOTO(ASEQ_NORMAL),
};

// ダメージのアニメーション
static unsigned char anime2[] = {
//	1,1,1,
	24,24,24,25,25,25,25,25,25,
	ASEQ_SET_REPEAT(3),
		26,26,27,27,28,28,27,27,26,26,
	ASEQ_EXE_REPEAT(),
	25,25,25,
	ASEQ_GOTO(ASEQ_NORMAL),
};

// 勝ちのアニメーション
static unsigned char anime3[] = {
	ASEQ_SET_LOOP(),
		16,16,16,17,17,17,18,18,18,19,19,19,20,20,20,21,21,21,22,22,22,
			ASEQ_SET_REPEAT(14),
			23,23,23,
			ASEQ_EXE_REPEAT(),
	//			ASEQ_SET_REPEAT(2),
	//			5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,6,6,6,1,1,1,2,2,3,3,4,4,4,
	//			ASEQ_EXE_REPEAT(),
	//			5,5,5,5,5,5,
	ASEQ_EXE_LOOP(100),
};

// 負けのアニメーション
static unsigned char anime4[] = {
	24,24,24,25,25,25,29,29,29,30,30,30,31,
	31,31,31,31,31,31,31,31,32,32,32,
	ASEQ_SET_LOOP(),
		33,33,33,33,33,33,
			ASEQ_SET_REPEAT(10),
			34,34,34,34,
			ASEQ_EXE_REPEAT(),
	ASEQ_EXE_LOOP(100),
};

// アニメーションデータテーブル
static unsigned char *animeTbl[] = {
	anime0, anime1, anime2, anime3, anime4,
};
static int animeTblSize[] = { sizeof(animeTbl) / sizeof(*animeTbl) };
