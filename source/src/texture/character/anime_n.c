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

#include "n/ani/n01.txt"
#define NON_PALETTE
#include "n/ani/n02.txt"
#include "n/ani/n03.txt"
#include "n/ani/n04.txt"
#include "n/ani/n05.txt"
#include "n/ani/n06.txt"
#include "n/ani/n07.txt"
#include "n/ani/n08.txt"
#include "n/ani/n09.txt"
#include "n/ani/n10.txt"
#include "n/ani/n11.txt"
#include "n/ani/n12.txt"
#include "n/ani/n13.txt"
#include "n/ani/n14.txt"
#include "n/ani/n15.txt"
#include "n/ani/n16.txt"
#include "n/ani/n17.txt"
#include "n/ani/n18.txt"
#include "n/ani/n19.txt"
#include "n/ani/n20.txt"
#include "n/ani/n21.txt"
#include "n/ani/n22.txt"
#include "n/ani/n23.txt"
#include "n/ani/n24.txt"
#include "n/ani/n25.txt"
#include "n/ani/n26.txt"
#include "n/ani/n27.txt"
#include "n/ani/n28.txt"
#include "n/ani/n29.txt"
#include "n/ani/n30.txt"
#undef NON_PALETTE

static STexInfo texTbl[] = {
	{ n01_tex_addr, n01_tex_size },
	{ n02_tex_addr, n02_tex_size },
	{ n03_tex_addr, n03_tex_size },
	{ n04_tex_addr, n04_tex_size },
	{ n05_tex_addr, n05_tex_size },
	{ n06_tex_addr, n06_tex_size },
	{ n07_tex_addr, n07_tex_size },
	{ n08_tex_addr, n08_tex_size },
	{ n09_tex_addr, n09_tex_size },
	{ n10_tex_addr, n10_tex_size },
	{ n11_tex_addr, n11_tex_size },
	{ n12_tex_addr, n12_tex_size },
	{ n13_tex_addr, n13_tex_size },
	{ n14_tex_addr, n14_tex_size },
	{ n15_tex_addr, n15_tex_size },
	{ n16_tex_addr, n16_tex_size },
	{ n17_tex_addr, n17_tex_size },
	{ n18_tex_addr, n18_tex_size },
	{ n19_tex_addr, n19_tex_size },
	{ n20_tex_addr, n20_tex_size },
	{ n21_tex_addr, n21_tex_size },
	{ n22_tex_addr, n22_tex_size },
	{ n23_tex_addr, n23_tex_size },
	{ n24_tex_addr, n24_tex_size },
	{ n25_tex_addr, n25_tex_size },
	{ n26_tex_addr, n26_tex_size },
	{ n27_tex_addr, n27_tex_size },
	{ n28_tex_addr, n28_tex_size },
	{ n29_tex_addr, n29_tex_size },
	{ n30_tex_addr, n30_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };

//////////////////////////////////////////////////////////////////////////////
// タイムシートのテキスト化・ワリオ

// 通常のアニメーション
static unsigned char anime0[] = {
	ASEQ_SET_LOOP(),
		1,1,1,2,2,3,3,4,4,5,5,5,4,4,3,3,2,2,
	ASEQ_EXE_LOOP(100),
};

// 攻撃のアニメーション
static unsigned char anime1[] = {
//	1,1,1,2,2,3,3,4,4,5,5,5,4,4,3,3,2,2,1,1,1,
	ASEQ_SET_REPEAT(2),
		6,6,6,7,7,7,8,8,8,9,9,9,10,10,11,11,11,12,12,13,13,14,14,
	ASEQ_EXE_REPEAT(),
	6,6,6,7,7,7,8,8,8,9,9,9,10,10,11,11,11,12,12,13,13,13,
	1,1,2,2,3,3,4,4,5,5,5,4,4,3,3,2,2,
	ASEQ_GOTO(ASEQ_NORMAL),
};

// ダメージのアニメーション
static unsigned char anime2[] = {
//	1,1,1,2,2,3,3,4,4,5,5,5,4,4,3,3,2,2,1,1,1,
	15,15,15,16,16,16,
	ASEQ_SET_REPEAT(4),
		17,17,18,18,19,19,18,18,
	ASEQ_EXE_REPEAT(),
	17,17,18,18,19,19,
	ASEQ_GOTO(ASEQ_NORMAL),
};

// 勝ちのアニメーション
static unsigned char anime3[] = {
//	1,1,1,2,2,3,3,4,4,5,5,5,4,4,3,3,2,2,1,1,1,
	25,25,25,26,26,26,27,27,27,
	ASEQ_SET_LOOP(),
		28,28,28,29,29,30,30,30,29,29,
	ASEQ_EXE_LOOP(100),
};

// 負けのアニメーション　2000.11.19 0709AM更新　H.Yoshikawa
static unsigned char anime4[] = {
//	1,1,1,2,2,3,3,4,4,5,5,5,4,4,3,3,2,2,1,1,1,
	ASEQ_SET_REPEAT(4),
		27,27,27,
	ASEQ_EXE_REPEAT(),
	20,20,20,21,21,21,22,22,22,
	ASEQ_SET_REPEAT(4),
		23,23,23,
	ASEQ_EXE_REPEAT(),
	ASEQ_SET_LOOP(),
		26,26,26,27,27,27,
	ASEQ_EXE_LOOP(100),
//	ASEQ_END(),
};

// アニメーションデータテーブル
static unsigned char *animeTbl[] = {
	anime0, anime1, anime2, anime3, anime4,
};
static int animeTblSize[] = { sizeof(animeTbl) / sizeof(*animeTbl) };
