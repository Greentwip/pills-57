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

#include "j/ani/j01.txt"
#define NON_PALETTE
#include "j/ani/j02.txt"
#include "j/ani/j03.txt"
#include "j/ani/j04.txt"
#include "j/ani/j05.txt"
#include "j/ani/j06.txt"
#include "j/ani/j07.txt"
#include "j/ani/j08.txt"
#include "j/ani/j09.txt"
#include "j/ani/j10.txt"
#include "j/ani/j11.txt"
#include "j/ani/j12.txt"
#include "j/ani/j13.txt"
#include "j/ani/j14.txt"
#include "j/ani/j15.txt"
#include "j/ani/j16.txt"
#include "j/ani/j17.txt"
#include "j/ani/j18.txt"
#include "j/ani/j19.txt"
#include "j/ani/j20.txt"
#include "j/ani/j21.txt"
#include "j/ani/j22.txt"
#include "j/ani/j23.txt"
#include "j/ani/j24.txt"
#include "j/ani/j25.txt"
#include "j/ani/j26.txt"
#include "j/ani/j27.txt"
#include "j/ani/j28.txt"
#include "j/ani/j29.txt"
#include "j/ani/j30.txt"
#include "j/ani/j31.txt"
#include "j/ani/j32.txt"
#include "j/ani/j33.txt"
#undef NON_PALETTE

static STexInfo texTbl[] = {
	{ j01_tex_addr, j01_tex_size },
	{ j02_tex_addr, j02_tex_size },
	{ j03_tex_addr, j03_tex_size },
	{ j04_tex_addr, j04_tex_size },
	{ j05_tex_addr, j05_tex_size },
	{ j06_tex_addr, j06_tex_size },
	{ j07_tex_addr, j07_tex_size },
	{ j08_tex_addr, j08_tex_size },
	{ j09_tex_addr, j09_tex_size },
	{ j10_tex_addr, j10_tex_size },
	{ j11_tex_addr, j11_tex_size },
	{ j12_tex_addr, j12_tex_size },
	{ j13_tex_addr, j13_tex_size },
	{ j14_tex_addr, j14_tex_size },
	{ j15_tex_addr, j15_tex_size },
	{ j16_tex_addr, j16_tex_size },
	{ j17_tex_addr, j17_tex_size },
	{ j18_tex_addr, j18_tex_size },
	{ j19_tex_addr, j19_tex_size },
	{ j20_tex_addr, j20_tex_size },
	{ j21_tex_addr, j21_tex_size },
	{ j22_tex_addr, j22_tex_size },
	{ j23_tex_addr, j23_tex_size },
	{ j24_tex_addr, j24_tex_size },
	{ j25_tex_addr, j25_tex_size },
	{ j26_tex_addr, j26_tex_size },
	{ j27_tex_addr, j27_tex_size },
	{ j28_tex_addr, j28_tex_size },
	{ j29_tex_addr, j29_tex_size },
	{ j30_tex_addr, j30_tex_size },
	{ j31_tex_addr, j31_tex_size },
	{ j32_tex_addr, j32_tex_size },
	{ j33_tex_addr, j33_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };

//////////////////////////////////////////////////////////////////////////////
// タイムシートのテキスト化・りんごろう

// 通常のアニメーション
static unsigned char anime0[] = {
	ASEQ_SET_LOOP(),
		1,1,1,2,2,2,3,3,3,4,4,4,3,3,3,2,2,2,
	ASEQ_EXE_LOOP(100),
};

// 攻撃のアニメーション
static unsigned char anime1[] = {
//	1,1,1,
	5,5,5,6,6,6,7,7,7,8,8,8,8,8,8,9,9,9,
	ASEQ_SET_REPEAT(2),
		10,10,10,11,11,11,12,12,12,12,12,12,33,33,33,
	ASEQ_EXE_REPEAT(),
	10,10,10,13,13,13,14,14,14,14,15,15,15,
	ASEQ_GOTO(ASEQ_NORMAL),
};

// ダメージのアニメーション
static unsigned char anime2[] = {
//	1,1,1,
	16,16,16,
	ASEQ_SET_REPEAT(2),
		17,17,17,18,18,18,19,19,19,20,20,20,
	ASEQ_EXE_REPEAT(),
	17,17,17,21,21,21,
	ASEQ_GOTO(ASEQ_NORMAL),
};

// 勝ちのアニメーション
static unsigned char anime3[] = {
//	1,1,1,
	22,22,22,23,23,23,
	ASEQ_SET_LOOP(),
		24,24,24,24,24,24,24,24,24,25,25,25,25,25,25,25,25,
	ASEQ_EXE_LOOP(100),
};

// 負けのアニメーション
static unsigned char anime4[] = {
//	1,1,1,
	26,26,26,27,27,27,
	ASEQ_SET_LOOP(),
		28,28,28,29,29,29,30,30,30,30,29,29,29,28,28,28,31,31,31,32,32,32,32,31,31,31,
	ASEQ_EXE_LOOP(100),
};

// アニメーションデータテーブル
static unsigned char *animeTbl[] = {
	anime0, anime1, anime2, anime3, anime4,
};
static int animeTblSize[] = { sizeof(animeTbl) / sizeof(*animeTbl) };
