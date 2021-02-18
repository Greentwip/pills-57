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

#include "virus_y/ani/y_l1.txt"
#define NON_PALETTE
#include "virus_y/ani/y_l2.txt"
#include "virus_y/ani/y_l3.txt"
#include "virus_y/ani/y_l4.txt"

#include "virus_y/ani/y_n1.txt"
#include "virus_y/ani/y_n2.txt"
#include "virus_y/ani/y_n3.txt"

#include "virus_y/ani/y_w1.txt"
#include "virus_y/ani/y_w2.txt"
#include "virus_y/ani/y_w3.txt"
#undef NON_PALETTE

static STexInfo texTbl[] = {
	{ y_l1_tex_addr, y_l1_tex_size },
	{ y_l2_tex_addr, y_l2_tex_size },
	{ y_l3_tex_addr, y_l3_tex_size },
	{ y_l4_tex_addr, y_l4_tex_size },

	{ y_n1_tex_addr, y_n1_tex_size },
	{ y_n2_tex_addr, y_n2_tex_size },
	{ y_n3_tex_addr, y_n3_tex_size },

	{ y_w1_tex_addr, y_w1_tex_size },
	{ y_w2_tex_addr, y_w2_tex_size },
	{ y_w3_tex_addr, y_w3_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };

//////////////////////////////////////////////////////////////////////////////
// ウィルス黄

#define L(n) ( 0+(n))
#define N(n) ( 4+(n))
#define W(n) ( 7+(n))

// 通常のアニメーション
static unsigned char anime0[] = {
	ASEQ_SET_LOOP(),
		N(1),N(1),N(1),N(1),
		N(2),N(2),N(2),N(2),
		N(3),N(3),N(3),N(3),
	ASEQ_EXE_LOOP(100),
};

// 攻撃のアニメーション
static unsigned char anime1[] = {
	ASEQ_GOTO(ASEQ_NORMAL),
};

// ダメージのアニメーション
static unsigned char anime2[] = {
	L(1),L(1),L(1),L(1),
	L(2),L(2),L(2),L(2),
	ASEQ_SET_REPEAT(10),
		L(3),L(3),L(3),L(3),
		L(4),L(4),L(4),L(4),
	ASEQ_EXE_REPEAT(),
	ASEQ_GOTO(ASEQ_NORMAL),
};

// 勝ちのアニメーション(笑い)
static unsigned char anime3[] = {
	ASEQ_SET_LOOP(),
		W(1),W(1),W(1),W(1),
		W(2),W(2),W(2),W(2),
		W(3),W(3),W(3),W(3),
		W(3),W(3),W(3),W(3),
		W(2),W(2),W(2),W(2),
	ASEQ_EXE_LOOP(100),
};

// 負けのアニメーション(消滅)
static unsigned char anime4[] = {
	L(1),L(1),L(1),L(1),
	L(2),L(2),L(2),L(2),
	ASEQ_SET_REPEAT(10),
		L(3),L(3),L(3),L(3),
		L(4),L(4),L(4),L(4),
	ASEQ_EXE_REPEAT(),
	0,
	ASEQ_END(),
};

// アニメーションデータテーブル
static unsigned char *animeTbl[] = {
	anime0, anime1, anime2, anime3, anime4,
};
static int animeTblSize[] = { sizeof(animeTbl) / sizeof(*animeTbl) };
