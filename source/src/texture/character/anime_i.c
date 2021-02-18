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

#include "i/ani/i01.txt"
#define NON_PALETTE
#include "i/ani/i02.txt"
#include "i/ani/i03.txt"
#include "i/ani/i04.txt"
#include "i/ani/i05.txt"
#include "i/ani/i06.txt"
#include "i/ani/i07.txt"
#include "i/ani/i08.txt"
#include "i/ani/i09.txt"
#include "i/ani/i10.txt"
#include "i/ani/i11.txt"
#include "i/ani/i12.txt"
#include "i/ani/i13.txt"
#include "i/ani/i14.txt"
#include "i/ani/i15.txt"
#include "i/ani/i16.txt"
#include "i/ani/i17.txt"
#include "i/ani/i18.txt"
#include "i/ani/i19.txt"
#include "i/ani/i20.txt"
#include "i/ani/i21.txt"
#include "i/ani/i22.txt"
#include "i/ani/i23.txt"
#include "i/ani/i24.txt"
#include "i/ani/i25.txt"
#include "i/ani/i26.txt"
#include "i/ani/i27.txt"
#include "i/ani/i28.txt"
#include "i/ani/i29.txt"
#include "i/ani/i30.txt"
#include "i/ani/i31.txt"
#include "i/ani/i32.txt"
#include "i/ani/i33.txt"
#include "i/ani/i34.txt"
#undef NON_PALETTE

static STexInfo texTbl[] = {
	{ i01_tex_addr, i01_tex_size },
	{ i02_tex_addr, i02_tex_size },
	{ i03_tex_addr, i03_tex_size },
	{ i04_tex_addr, i04_tex_size },
	{ i05_tex_addr, i05_tex_size },
	{ i06_tex_addr, i06_tex_size },
	{ i07_tex_addr, i07_tex_size },
	{ i08_tex_addr, i08_tex_size },
	{ i09_tex_addr, i09_tex_size },
	{ i10_tex_addr, i10_tex_size },
	{ i11_tex_addr, i11_tex_size },
	{ i12_tex_addr, i12_tex_size },
	{ i13_tex_addr, i13_tex_size },
	{ i14_tex_addr, i14_tex_size },
	{ i15_tex_addr, i15_tex_size },
	{ i16_tex_addr, i16_tex_size },
	{ i17_tex_addr, i17_tex_size },
	{ i18_tex_addr, i18_tex_size },
	{ i19_tex_addr, i19_tex_size },
	{ i20_tex_addr, i20_tex_size },
	{ i21_tex_addr, i21_tex_size },
	{ i22_tex_addr, i22_tex_size },
	{ i23_tex_addr, i23_tex_size },
	{ i24_tex_addr, i24_tex_size },
	{ i25_tex_addr, i25_tex_size },
	{ i26_tex_addr, i26_tex_size },
	{ i27_tex_addr, i27_tex_size },
	{ i28_tex_addr, i28_tex_size },
	{ i29_tex_addr, i29_tex_size },
	{ i30_tex_addr, i30_tex_size },
	{ i31_tex_addr, i31_tex_size },
	{ i32_tex_addr, i32_tex_size },
	{ i33_tex_addr, i33_tex_size },
	{ i34_tex_addr, i34_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };

//////////////////////////////////////////////////////////////////////////////
// タイムシートのテキスト化・ヤリマル

// 通常のアニメーション
static unsigned char anime0[] = {
	ASEQ_SET_LOOP(),
		1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,3,3,3,4,4,4,
		5,5,5,5,5,5,5,5,5,5,5,5,6,6,6,7,7,7,8,8,8,
	ASEQ_EXE_LOOP(100),
};

// 攻撃のアニメーション
static unsigned char anime1[] = {
//	1,1,1,
	9,9,9,10,10,10,11,11,11,12,12,12,13,13,13,13,14,15,15,
	16,13,13,14,15,15,16,13,13,14,15,15,16,13,13,13,13,11,11,11,
	ASEQ_GOTO(ASEQ_NORMAL),
};

// ダメージのアニメーション
static unsigned char anime2[] = {
//	1,1,1,
	17,17,17,
	ASEQ_SET_REPEAT(3),
		18,18,19,19,20,20,
	ASEQ_EXE_REPEAT(),
	18,18,18,
	ASEQ_GOTO(ASEQ_NORMAL),
};

// 勝ちのアニメーション
static unsigned char anime3[] = {
//	1,1,1,
	21,21,21,
	ASEQ_SET_LOOP(),
		22,22,22,23,23,23,24,24,24,25,25,25,22,22,22,26,26,26,27,27,27,25,25,25,
	ASEQ_EXE_LOOP(100),
};

// 負けのアニメーション
static unsigned char anime4[] = {
//	1,1,1,
	28,28,28,28,28,28,29,29,29,30,30,30,30,30,30,31,31,31,
	ASEQ_SET_LOOP(),
		32,32,32,33,33,33,34,34,34,
	ASEQ_EXE_LOOP(100),
};

// アニメーションデータテーブル
static unsigned char *animeTbl[] = {
	anime0, anime1, anime2, anime3, anime4,
};
static int animeTblSize[] = { sizeof(animeTbl) / sizeof(*animeTbl) };
