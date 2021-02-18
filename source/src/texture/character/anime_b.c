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

#include "b/ani/b01.txt"
#define NON_PALETTE
#include "b/ani/b02.txt"
#include "b/ani/b03.txt"
#include "b/ani/b04.txt"
#include "b/ani/b05.txt"
#include "b/ani/b06.txt"
#include "b/ani/b07.txt"
#include "b/ani/b08.txt"
#include "b/ani/b09.txt"
#include "b/ani/b10.txt"
#include "b/ani/b11.txt"
#include "b/ani/b12.txt"
#include "b/ani/b13.txt"
#include "b/ani/b14.txt"
#include "b/ani/b15.txt"
#include "b/ani/b16.txt"
#include "b/ani/b17.txt"
#include "b/ani/b18.txt"
#include "b/ani/b19.txt"
#include "b/ani/b20.txt"
#include "b/ani/b21.txt"
#include "b/ani/b22.txt"
#include "b/ani/b23.txt"
#include "b/ani/b24.txt"
#include "b/ani/b25.txt"
#undef NON_PALETTE

static STexInfo texTbl[] = {
	{ b01_tex_addr, b01_tex_size },
	{ b02_tex_addr, b02_tex_size },
	{ b03_tex_addr, b03_tex_size },
	{ b04_tex_addr, b04_tex_size },
	{ b05_tex_addr, b05_tex_size },
	{ b06_tex_addr, b06_tex_size },
	{ b07_tex_addr, b07_tex_size },
	{ b08_tex_addr, b08_tex_size },
	{ b09_tex_addr, b09_tex_size },
	{ b10_tex_addr, b10_tex_size },
	{ b11_tex_addr, b11_tex_size },
	{ b12_tex_addr, b12_tex_size },
	{ b13_tex_addr, b13_tex_size },
	{ b14_tex_addr, b14_tex_size },
	{ b15_tex_addr, b15_tex_size },
	{ b16_tex_addr, b16_tex_size },
	{ b17_tex_addr, b17_tex_size },
	{ b18_tex_addr, b18_tex_size },
	{ b19_tex_addr, b19_tex_size },
	{ b20_tex_addr, b20_tex_size },
	{ b21_tex_addr, b21_tex_size },
	{ b22_tex_addr, b22_tex_size },
	{ b23_tex_addr, b23_tex_size },
	{ b24_tex_addr, b24_tex_size },
	{ b25_tex_addr, b25_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };

//////////////////////////////////////////////////////////////////////////////
// タイムシートのテキスト化・キグモン

// 通常のアニメーション
static unsigned char anime0[] = {
	ASEQ_SET_LOOP(),
		1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,7,7,6,6,5,5,4,4,3,3,2,2,
	ASEQ_EXE_LOOP(100),
};

// 攻撃のアニメーション
static unsigned char anime1[] = {
	4,4,4,9,9,9,10,10,10,11,11,11,11,11,11,12,12,13,13,11,11,
	12,12,13,13,11,11,12,12,13,13,11,11,11,10,10,10,9,9,9,
	ASEQ_GOTO(ASEQ_NORMAL),
};

// ダメージのアニメーション
static unsigned char anime2[] = {
	4,4,4,
	ASEQ_SET_REPEAT(3),
		14,14,15,15,16,16,17,17,18,18,
	ASEQ_EXE_REPEAT(),
	ASEQ_GOTO(ASEQ_NORMAL),
};

// 勝ちのアニメーション
static unsigned char anime3[] = {
	4,4,4,19,19,19,
	ASEQ_SET_LOOP(),
		20,20,20,21,21,21,22,22,22,
	ASEQ_EXE_LOOP(100),
};

// 負けのアニメーション
static unsigned char anime4[] = {
//	1,1,1,
	23,23,23,24,24,24,24,24,24,24,24,24,25,25,
	ASEQ_SET_LOOP(),
		24,24,24,24,24,25,25,24,24,25,25,
	ASEQ_EXE_LOOP(100),
};

// アニメーションデータテーブル
static unsigned char *animeTbl[] = {
	anime0, anime1, anime2, anime3, anime4,
};
static int animeTblSize[] = { sizeof(animeTbl) / sizeof(*animeTbl) };
