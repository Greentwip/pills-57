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

#include "f/ani/f01.txt"
#define NON_PALETTE
#include "f/ani/f02.txt"
#include "f/ani/f03.txt"
#include "f/ani/f04.txt"
#include "f/ani/f05.txt"
#include "f/ani/f06.txt"
#include "f/ani/f07.txt"
#include "f/ani/f08.txt"
#include "f/ani/f09.txt"
#include "f/ani/f10.txt"
#include "f/ani/f11.txt"
#include "f/ani/f12.txt"
#include "f/ani/f13.txt"
#include "f/ani/f14.txt"
#include "f/ani/f15.txt"
#include "f/ani/f16.txt"
#include "f/ani/f17.txt"
#include "f/ani/f18.txt"
#include "f/ani/f19.txt"
#include "f/ani/f20.txt"
#include "f/ani/f21.txt"
#include "f/ani/f22.txt"
#include "f/ani/f23.txt"
#include "f/ani/f24.txt"
#include "f/ani/f25.txt"
#include "f/ani/f26.txt"
#include "f/ani/f27.txt"
#include "f/ani/f28.txt"
#include "f/ani/f29.txt"
#include "f/ani/f30.txt"
#include "f/ani/f31.txt"
#include "f/ani/f32.txt"
#include "f/ani/f33.txt"
#include "f/ani/f34.txt"
#undef NON_PALETTE

static STexInfo texTbl[] = {
	{ f01_tex_addr, f01_tex_size },
	{ f02_tex_addr, f02_tex_size },
	{ f03_tex_addr, f03_tex_size },
	{ f04_tex_addr, f04_tex_size },
	{ f05_tex_addr, f05_tex_size },
	{ f06_tex_addr, f06_tex_size },
	{ f07_tex_addr, f07_tex_size },
	{ f08_tex_addr, f08_tex_size },
	{ f09_tex_addr, f09_tex_size },
	{ f10_tex_addr, f10_tex_size },
	{ f11_tex_addr, f11_tex_size },
	{ f12_tex_addr, f12_tex_size },
	{ f13_tex_addr, f13_tex_size },
	{ f14_tex_addr, f14_tex_size },
	{ f15_tex_addr, f15_tex_size },
	{ f16_tex_addr, f16_tex_size },
	{ f17_tex_addr, f17_tex_size },
	{ f18_tex_addr, f18_tex_size },
	{ f19_tex_addr, f19_tex_size },
	{ f20_tex_addr, f20_tex_size },
	{ f21_tex_addr, f21_tex_size },
	{ f22_tex_addr, f22_tex_size },
	{ f23_tex_addr, f23_tex_size },
	{ f24_tex_addr, f24_tex_size },
	{ f25_tex_addr, f25_tex_size },
	{ f26_tex_addr, f26_tex_size },
	{ f27_tex_addr, f27_tex_size },
	{ f28_tex_addr, f28_tex_size },
	{ f29_tex_addr, f29_tex_size },
	{ f30_tex_addr, f30_tex_size },
	{ f31_tex_addr, f31_tex_size },
	{ f32_tex_addr, f32_tex_size },
	{ f33_tex_addr, f33_tex_size },
	{ f34_tex_addr, f34_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };

//////////////////////////////////////////////////////////////////////////////
// タイムシートのテキスト化・ふっくらげ

// 通常のアニメーション
static unsigned char anime0[] = {
	ASEQ_SET_LOOP(),
		1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,7,7,7,7,8,8,8,8,9,9,9,9,
		10,10,10,10,10,11,11,11,12,12,12,12,13,13,13,13,14,14,14,14,
	ASEQ_EXE_LOOP(100),
};

// 攻撃のアニメーション
static unsigned char anime1[] = {
	15,15,15,15,16,16,16,16,
	ASEQ_SET_REPEAT(7),
		17,17,17,18,18,18,
	ASEQ_EXE_REPEAT(),
	ASEQ_GOTO(ASEQ_NORMAL),
};

// ダメージのアニメーション
static unsigned char anime2[] = {
	19,19,19,19,20,20,20,20,
	ASEQ_SET_REPEAT(7),
		21,21,21,22,22,22,
	ASEQ_EXE_REPEAT(),
	ASEQ_GOTO(ASEQ_NORMAL),
};

// 勝ちのアニメーション
static unsigned char anime3[] = {
	ASEQ_SET_LOOP(),
		23,23,23,24,24,24,23,23,23,25,25,25,
	ASEQ_EXE_LOOP(100),
};

// 負けのアニメーション　2000.11.19 0709AM更新　H.Yoshikawa
static unsigned char anime4[] = {
	26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,
	27,27,27,27,28,28,28,28,29,29,29,29,30,30,30,30,
	31,31,31,31,31,32,32,32,33,33,33,33,34,
	ASEQ_SET_LOOP(),
		34,34,33,33,33,33,32,32,32,33,33,33,33,34,
	ASEQ_EXE_LOOP(100),
//	ASEQ_END(),
};

// アニメーションデータテーブル
static unsigned char *animeTbl[] = {
	anime0, anime1, anime2, anime3, anime4,
};
static int animeTblSize[] = { sizeof(animeTbl) / sizeof(*animeTbl) };
