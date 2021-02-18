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

#define NON_PALETTE
#include "smog/ani/puff0001.txt"
#include "smog/ani/puff0002.txt"
#include "smog/ani/puff0003.txt"
#include "smog/ani/puff0004.txt"
#include "smog/ani/puff0005.txt"
#include "smog/ani/puff0006.txt"
#include "smog/ani/puff0007.txt"
#include "smog/ani/puff0008.txt"
#include "smog/ani/puff0009.txt"
#undef NON_PALETTE

static STexInfo texTbl[] = {
	{ puff0001_tex_addr, puff0001_tex_size },
	{ puff0002_tex_addr, puff0002_tex_size },
	{ puff0003_tex_addr, puff0003_tex_size },
	{ puff0004_tex_addr, puff0004_tex_size },
	{ puff0005_tex_addr, puff0005_tex_size },
	{ puff0006_tex_addr, puff0006_tex_size },
	{ puff0007_tex_addr, puff0007_tex_size },
	{ puff0008_tex_addr, puff0008_tex_size },
	{ puff0009_tex_addr, puff0009_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };

//////////////////////////////////////////////////////////////////////////////
// 煙

// 通常のアニメーション
static unsigned char anime0[] = {
	1, 1, 1,
	2, 2, 2,
	3, 3, 3,
	4, 4, 4,
	5, 5, 5,
	6, 6, 6,
	7, 7, 7,
	8, 8, 8,
	9, 9, 9,
	0,
	ASEQ_END(),
};

// 攻撃のアニメーション
static unsigned char anime1[] = {
	0,
	ASEQ_END(),
};

// ダメージのアニメーション
static unsigned char anime2[] = {
	0,
	ASEQ_END(),
};

// 勝ちのアニメーション(笑い)
static unsigned char anime3[] = {
	0,
	ASEQ_END(),
};

// 負けのアニメーション(消滅)
static unsigned char anime4[] = {
	0,
	ASEQ_END(),
};

// アニメーションデータテーブル
static unsigned char *animeTbl[] = {
	anime0, anime1, anime2, anime3, anime4,
};
static int animeTblSize[] = { sizeof(animeTbl) / sizeof(*animeTbl) };
