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

#include "m/ani/m01.txt"
#define NON_PALETTE
#include "m/ani/m02.txt"
#include "m/ani/m03.txt"
#include "m/ani/m04.txt"
#include "m/ani/m05.txt"
#include "m/ani/m06.txt"
#include "m/ani/m07.txt"
#include "m/ani/m08.txt"
#include "m/ani/m09.txt"
#include "m/ani/m10.txt"
#include "m/ani/m11.txt"
#include "m/ani/m12.txt"
#include "m/ani/m13.txt"
#include "m/ani/m14.txt"
#include "m/ani/m15.txt"
#include "m/ani/m16.txt"
#include "m/ani/m17.txt"
#include "m/ani/m18.txt"
#include "m/ani/m19.txt"
#include "m/ani/m20.txt"
#include "m/ani/m21.txt"
#include "m/ani/m22.txt"
#include "m/ani/m23.txt"
#include "m/ani/m24.txt"
#include "m/ani/m25.txt"
#include "m/ani/m26.txt"
#include "m/ani/m27.txt"
#include "m/ani/m28.txt"
#include "m/ani/m29.txt"
#include "m/ani/m30.txt"
#include "m/ani/m31.txt"
#include "m/ani/m32.txt"
#include "m/ani/m33.txt"
#include "m/ani/m34.txt"
#undef NON_PALETTE

static STexInfo texTbl[] = {
	{ m01_tex_addr, m01_tex_size },
	{ m02_tex_addr, m02_tex_size },
	{ m03_tex_addr, m03_tex_size },
	{ m04_tex_addr, m04_tex_size },
	{ m05_tex_addr, m05_tex_size },
	{ m06_tex_addr, m06_tex_size },
	{ m07_tex_addr, m07_tex_size },
	{ m08_tex_addr, m08_tex_size },
	{ m09_tex_addr, m09_tex_size },
	{ m10_tex_addr, m10_tex_size },
	{ m11_tex_addr, m11_tex_size },
	{ m12_tex_addr, m12_tex_size },
	{ m13_tex_addr, m13_tex_size },
	{ m14_tex_addr, m14_tex_size },
	{ m15_tex_addr, m15_tex_size },
	{ m16_tex_addr, m16_tex_size },
	{ m17_tex_addr, m17_tex_size },
	{ m18_tex_addr, m18_tex_size },
	{ m19_tex_addr, m19_tex_size },
	{ m20_tex_addr, m20_tex_size },
	{ m21_tex_addr, m21_tex_size },
	{ m22_tex_addr, m22_tex_size },
	{ m23_tex_addr, m23_tex_size },
	{ m24_tex_addr, m24_tex_size },
	{ m25_tex_addr, m25_tex_size },
	{ m26_tex_addr, m26_tex_size },
	{ m27_tex_addr, m27_tex_size },
	{ m28_tex_addr, m28_tex_size },
	{ m29_tex_addr, m29_tex_size },
	{ m30_tex_addr, m30_tex_size },
	{ m31_tex_addr, m31_tex_size },
	{ m32_tex_addr, m32_tex_size },
	{ m33_tex_addr, m33_tex_size },
	{ m34_tex_addr, m34_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };

//////////////////////////////////////////////////////////////////////////////
// タイムシートのテキスト化・マリオ
// 通常のアニメーション
// 攻撃のアニメーション
// ダメージのアニメーション
// 勝ちのアニメーション　2000.11.20 1253AM更新　H.Yoshikawa
// 負けのアニメーション　2000.11.20 1251PM更新　H.Yoshikawa


// 通常のアニメーション
static unsigned char anime0[] = {
	ASEQ_SET_LOOP(),
		ASEQ_SET_LOOP(),
			1,1,1,2,2,3,3,4,4,4,5,5,5,5,
			5,5,5,5,5,5,5,5,5,5,5,6,6,6,
		ASEQ_EXE_LOOP(75),
		7,7,7,7,7,7,8,8,8,8,8,8,7,7,7,
		7,7,7,8,8,8,8,8,8,7,7,7,7,7,7,
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
		26,26,27,27,28,28,27,27,26,
	ASEQ_EXE_REPEAT(),
	26,25,25,25,
	ASEQ_GOTO(ASEQ_NORMAL),
};

// 勝ちのアニメーション
static unsigned char anime3[] = {
	ASEQ_SET_LOOP(),
		16,16,16,17,17,17,18,18,18,19,19,19,20,20,20,21,21,21,22,22,22,
			ASEQ_SET_REPEAT(14),
			23,23,23,
			ASEQ_EXE_REPEAT(),
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
