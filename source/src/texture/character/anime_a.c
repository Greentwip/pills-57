#include <ultra64.h>
#include "local.h"
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

#if LOCAL==JAPAN

#include "a/ani/a01.txt"
#define NON_PALETTE
#include "a/ani/a02.txt"
#include "a/ani/a03.txt"
#include "a/ani/a04.txt"
#include "a/ani/a05.txt"
#include "a/ani/a06.txt"
#include "a/ani/a07.txt"
#include "a/ani/a08.txt"
#include "a/ani/a09.txt"
#include "a/ani/a10.txt"
#include "a/ani/a11.txt"
#include "a/ani/a12.txt"
#include "a/ani/a13.txt"
#include "a/ani/a14.txt"
#include "a/ani/a15.txt"
#include "a/ani/a16.txt"
#include "a/ani/a17.txt"
#include "a/ani/a18.txt"
#include "a/ani/a19.txt"
#include "a/ani/a20.txt"
#include "a/ani/a21.txt"
#include "a/ani/a22.txt"
#include "a/ani/a23.txt"
#include "a/ani/a24.txt"
#include "a/ani/a25.txt"
#include "a/ani/a26.txt"
#include "a/ani/a27.txt"
#include "a/ani/a28.txt"
#include "a/ani/a29.txt"
#include "a/ani/a30.txt"
#include "a/ani/a31.txt"
#include "a/ani/a32.txt"
#include "a/ani/a33.txt"
#include "a/ani/a34.txt"
#undef NON_PALETTE

#elif LOCAL==AMERICA

#include "a2/ani/a01.txt"
#define NON_PALETTE
#include "a2/ani/a02.txt"
#include "a2/ani/a03.txt"
#include "a2/ani/a04.txt"
#include "a2/ani/a05.txt"
#include "a2/ani/a06.txt"
#include "a2/ani/a07.txt"
#include "a2/ani/a08.txt"
#include "a2/ani/a09.txt"
#include "a2/ani/a10.txt"
#include "a2/ani/a11.txt"
#include "a2/ani/a12.txt"
#include "a2/ani/a13.txt"
#include "a2/ani/a14.txt"
#include "a2/ani/a15.txt"
#include "a2/ani/a16.txt"
#include "a2/ani/a17.txt"
#include "a2/ani/a18.txt"
#include "a2/ani/a19.txt"
#include "a2/ani/a20.txt"
#include "a2/ani/a21.txt"
#include "a2/ani/a22.txt"
#include "a2/ani/a23.txt"
#include "a2/ani/a24.txt"
#include "a2/ani/a25.txt"
#include "a2/ani/a26.txt"
#include "a2/ani/a27.txt"
#include "a2/ani/a28.txt"
#include "a2/ani/a29.txt"
#include "a2/ani/a30.txt"
#include "a2/ani/a31.txt"
#include "a2/ani/a32.txt"
#include "a2/ani/a33.txt"
#include "a2/ani/a34.txt"
#undef NON_PALETTE

#endif

static STexInfo texTbl[] = {
	{ a01_tex_addr, a01_tex_size },
	{ a02_tex_addr, a02_tex_size },
	{ a03_tex_addr, a03_tex_size },
	{ a04_tex_addr, a04_tex_size },
	{ a05_tex_addr, a05_tex_size },
	{ a06_tex_addr, a06_tex_size },
	{ a07_tex_addr, a07_tex_size },
	{ a08_tex_addr, a08_tex_size },
	{ a09_tex_addr, a09_tex_size },
	{ a10_tex_addr, a10_tex_size },
	{ a11_tex_addr, a11_tex_size },
	{ a12_tex_addr, a12_tex_size },
	{ a13_tex_addr, a13_tex_size },
	{ a14_tex_addr, a14_tex_size },
	{ a15_tex_addr, a15_tex_size },
	{ a16_tex_addr, a16_tex_size },
	{ a17_tex_addr, a17_tex_size },
	{ a18_tex_addr, a18_tex_size },
	{ a19_tex_addr, a19_tex_size },
	{ a20_tex_addr, a20_tex_size },
	{ a21_tex_addr, a21_tex_size },
	{ a22_tex_addr, a22_tex_size },
	{ a23_tex_addr, a23_tex_size },
	{ a24_tex_addr, a24_tex_size },
	{ a25_tex_addr, a25_tex_size },
	{ a26_tex_addr, a26_tex_size },
	{ a27_tex_addr, a27_tex_size },
	{ a28_tex_addr, a28_tex_size },
	{ a29_tex_addr, a29_tex_size },
	{ a30_tex_addr, a30_tex_size },
	{ a31_tex_addr, a31_tex_size },
	{ a32_tex_addr, a32_tex_size },
	{ a33_tex_addr, a33_tex_size },
	{ a34_tex_addr, a34_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };

//////////////////////////////////////////////////////////////////////////////
// タイムシートのテキスト化・イカタコテング

// 通常のアニメーション
static unsigned char anime0[] = {
	ASEQ_SET_LOOP(),
		1,1,1,2,2,2,3,3,3,4,4,4,5,5,5,6,6,6,
	ASEQ_EXE_LOOP(100),
};

// 攻撃のアニメーション
static unsigned char anime1[] = {
//	1,1,1,2,2,2,3,3,3,4,4,4,5,5,5,6,6,6,1,1,1,
	7,7,7,8,8,8,9,9,9,
	10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,11,11,11,
	12,12,13,13,14,14,15,15,15,16,16,16,17,17,17,18,18,18,19,19,
	20,20,20,21,21,21,22,22,22,23,23,23,
	ASEQ_GOTO(ASEQ_NORMAL),
};

// ダメージのアニメーション
static unsigned char anime2[] = {
//	1,1,1,2,2,2,3,3,3,4,4,4,5,5,5,6,6,6,1,1,1,
	24,24,24,25,25,25,26,26,26,27,27,27,28,28,
	28,29,29,29,29,29,29,29,29,29,29,29,29,29,
	29,29,29,29,29,29,29,29,29,29,29,29,29,29,
	29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,
	ASEQ_GOTO(ASEQ_NORMAL),
};

// 勝ちのアニメーション
static unsigned char anime3[] = {
//	1,1,1,2,2,2,3,3,3,4,4,4,5,5,5,6,6,6,
	ASEQ_SET_LOOP(),
		30,30,30,30,30,30,30,30,30,30,30,30,
		30,30,30,30,30,30,30,30,30,31,31,31,
		31,31,31,30,30,30,30,30,30,30,30,30,
		30,30,31,31,31,31,31,31,
	ASEQ_EXE_LOOP(100),
};

// 負けのアニメーション
static unsigned char anime4[] = {
//	1,1,1,2,2,2,3,3,3,4,4,4,5,5,5,6,6,6,1,1,1,
	24,24,24,25,25,25,32,32,32,
	ASEQ_SET_LOOP(),
		33,33,33,34,34,34,34,34,34,34,34,34,33,33,33,
		32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
	ASEQ_EXE_LOOP(100),
};

// アニメーションデータテーブル
static unsigned char *animeTbl[] = {
	anime0, anime1, anime2, anime3, anime4,
};
static int animeTblSize[] = { sizeof(animeTbl) / sizeof(*animeTbl) };
