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

#include "virus_b/ani/b_l1.txt"
#define NON_PALETTE
#include "virus_b/ani/b_l2.txt"
#include "virus_b/ani/b_l3.txt"
#include "virus_b/ani/b_l4.txt"

#include "virus_b/ani/b_n1.txt"
#include "virus_b/ani/b_n2.txt"
#include "virus_b/ani/b_n3.txt"
#include "virus_b/ani/b_n4.txt"
#include "virus_b/ani/b_n5.txt"

#include "virus_b/ani/b_w1.txt"
#include "virus_b/ani/b_w2.txt"
#include "virus_b/ani/b_w3.txt"
#include "virus_b/ani/b_w4.txt"
#undef NON_PALETTE

static STexInfo texTbl[] = {
	{ b_l1_tex_addr, b_l1_tex_size },
	{ b_l2_tex_addr, b_l2_tex_size },
	{ b_l3_tex_addr, b_l3_tex_size },
	{ b_l4_tex_addr, b_l4_tex_size },

	{ b_n1_tex_addr, b_n1_tex_size },
	{ b_n2_tex_addr, b_n2_tex_size },
	{ b_n3_tex_addr, b_n3_tex_size },
	{ b_n4_tex_addr, b_n4_tex_size },
	{ b_n5_tex_addr, b_n5_tex_size },

	{ b_w1_tex_addr, b_w1_tex_size },
	{ b_w2_tex_addr, b_w2_tex_size },
	{ b_w3_tex_addr, b_w3_tex_size },
	{ b_w4_tex_addr, b_w4_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };

//////////////////////////////////////////////////////////////////////////////
// ウィルス青

#define L(n) ( 0+(n))
#define N(n) ( 4+(n))
#define W(n) ( 9+(n))

// 通常のアニメーション
static unsigned char anime0[] = {
	ASEQ_SET_LOOP(),
		N(1),N(1),N(1),N(1),
		N(2),N(2),N(2),N(2),
		N(3),N(3),N(3),N(3),
		N(2),N(2),N(2),N(2),
		N(1),N(1),N(1),N(1),
		N(4),N(4),N(4),N(4),
		N(5),N(5),N(5),N(5),
		N(4),N(4),N(4),N(4),
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
		W(4),W(4),W(4),W(4),
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
