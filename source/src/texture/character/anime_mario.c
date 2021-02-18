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

#include "mario/ani/mario_001.txt"
#define NON_PALETTE
#include "mario/ani/mario_002.txt"
#include "mario/ani/mario_003.txt"
#include "mario/ani/mario_004.txt"
#include "mario/ani/mario_005.txt"
#include "mario/ani/mario_006.txt"
//#include "mario/ani/mario_007.txt"
//#include "mario/ani/mario_008.txt"
//#include "mario/ani/mario_009.txt"
//#include "mario/ani/mario_010.txt"
//#include "mario/ani/mario_011.txt"
#include "mario/ani/mario_063.txt"
#include "mario/ani/mario_064.txt"
#include "mario/ani/mario_065.txt"
#include "mario/ani/mario_066.txt"
#include "mario/ani/mario_067.txt"
#include "mario/ani/mario_068.txt"
#include "mario/ani/mario_069.txt"
#include "mario/ani/mario_070.txt"
#include "mario/ani/mario_071.txt"
#include "mario/ani/mario_072.txt"
#include "mario/ani/mario_073.txt"
#include "mario/ani/mario_074.txt"
#include "mario/ani/mario_075.txt"
#include "mario/ani/mario_076.txt"
#include "mario/ani/mario_077.txt"
#include "mario/ani/mario_078.txt"
#include "mario/ani/mario_079.txt"
#include "mario/ani/mario_080.txt"
#include "mario/ani/mario_081.txt"
#include "mario/ani/mario_082.txt"
#include "mario/ani/mario_083.txt"
#include "mario/ani/mario_084.txt"
#include "mario/ani/mario_085.txt"
#include "mario/ani/mario_086.txt"
#include "mario/ani/mario_087.txt"
#include "mario/ani/mario_088.txt"
#include "mario/ani/mario_089.txt"
#include "mario/ani/mario_090.txt"
#include "mario/ani/mario_091.txt"
#include "mario/ani/mario_092.txt"
#include "mario/ani/mario_093.txt"
#include "mario/ani/mario_094.txt"
#include "mario/ani/mario_095.txt"
#include "mario/ani/mario_096.txt"
#include "mario/ani/mario_097.txt"
//#include "mario/ani/mario_098.txt"
#include "mario/ani/mario_099.txt"
#include "mario/ani/mario_102.txt"
//#include "mario/ani/mario_103.txt"
//#include "mario/ani/mario_104.txt"
#include "mario/ani/mario_105.txt"
#include "mario/ani/mario_106.txt"
#undef NON_PALETTE

static STexInfo texTbl[] = {
	{ mario_001_tex_addr, mario_001_tex_size },
	{ mario_002_tex_addr, mario_002_tex_size },
	{ mario_003_tex_addr, mario_003_tex_size },
	{ mario_004_tex_addr, mario_004_tex_size },
	{ mario_005_tex_addr, mario_005_tex_size },
	{ mario_006_tex_addr, mario_006_tex_size },
	{ NULL, NULL }, //{ mario_007_tex_addr, mario_007_tex_size },
	{ NULL, NULL }, //{ mario_008_tex_addr, mario_008_tex_size },
	{ NULL, NULL }, //{ mario_009_tex_addr, mario_009_tex_size },
	{ NULL, NULL }, //{ mario_010_tex_addr, mario_010_tex_size },
	{ NULL, NULL }, //{ mario_011_tex_addr, mario_011_tex_size },

	{ mario_063_tex_addr, mario_063_tex_size },
	{ mario_064_tex_addr, mario_064_tex_size },
	{ mario_065_tex_addr, mario_065_tex_size },
	{ mario_066_tex_addr, mario_066_tex_size },
	{ mario_067_tex_addr, mario_067_tex_size },
	{ mario_068_tex_addr, mario_068_tex_size },
	{ mario_069_tex_addr, mario_069_tex_size },
	{ mario_070_tex_addr, mario_070_tex_size },
	{ mario_071_tex_addr, mario_071_tex_size },
	{ mario_072_tex_addr, mario_072_tex_size },
	{ mario_073_tex_addr, mario_073_tex_size },
	{ mario_074_tex_addr, mario_074_tex_size },
	{ mario_075_tex_addr, mario_075_tex_size },
	{ mario_076_tex_addr, mario_076_tex_size },
	{ mario_077_tex_addr, mario_077_tex_size },
	{ mario_078_tex_addr, mario_078_tex_size },
	{ mario_079_tex_addr, mario_079_tex_size },
	{ mario_080_tex_addr, mario_080_tex_size },
	{ mario_081_tex_addr, mario_081_tex_size },
	{ mario_082_tex_addr, mario_082_tex_size },
	{ mario_083_tex_addr, mario_083_tex_size },
	{ mario_084_tex_addr, mario_084_tex_size },
	{ mario_085_tex_addr, mario_085_tex_size },
	{ mario_086_tex_addr, mario_086_tex_size },
	{ mario_087_tex_addr, mario_087_tex_size },
	{ mario_088_tex_addr, mario_088_tex_size },
	{ mario_089_tex_addr, mario_089_tex_size },
	{ mario_090_tex_addr, mario_090_tex_size },
	{ mario_091_tex_addr, mario_091_tex_size },
	{ mario_092_tex_addr, mario_092_tex_size },
	{ mario_093_tex_addr, mario_093_tex_size },
	{ mario_094_tex_addr, mario_094_tex_size },
	{ mario_095_tex_addr, mario_095_tex_size },
	{ mario_096_tex_addr, mario_096_tex_size },
	{ mario_097_tex_addr, mario_097_tex_size },
	{ NULL, NULL }, //{ mario_098_tex_addr, mario_098_tex_size },
	{ mario_099_tex_addr, mario_099_tex_size },

	{ mario_102_tex_addr, mario_102_tex_size },
	{ NULL, NULL }, //{ mario_103_tex_addr, mario_103_tex_size },
	{ NULL, NULL }, //{ mario_104_tex_addr, mario_104_tex_size },
	{ mario_105_tex_addr, mario_105_tex_size },
	{ mario_106_tex_addr, mario_106_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };

//////////////////////////////////////////////////////////////////////////////
// マ～リオ

#define D 0
#define W (D+12-63)
#define N (D+49-102)

// 通常のアニメーション(カプセルが落下している間のアニメーション)
static unsigned char anime0[] = {
	ASEQ_SET_LOOP(),
		1+D,
		1+D,
		1+D,
		1+D,
		1+D,
		1+D,
		1+D,
		1+D,
		1+D,
		2+D,
		2+D,
		3+D,
		3+D,
		4+D,
		4+D,
		5+D,
		5+D,
		6+D,
		6+D,
		6+D,
		6+D,
		6+D,
		6+D,
		6+D,
		6+D,
		6+D,
		5+D,
		5+D,
		4+D,
		4+D,
		3+D,
		3+D,
		2+D,
		2+D,
	ASEQ_EXE_LOOP(100),
};

// 攻撃のアニメーション(カプセルを投げ込む時のアニメーション)
static unsigned char anime1[] = {
	102+N,
	106+N,
	106+N,
	106+N,
	106+N,
	105+N,
	105+N,
	105+N,
	102+N,
	102+N,
	102+N,
	ASEQ_GOTO(ASEQ_NORMAL),
};

// ダメージのアニメーション(pause中のアニメーション)
static unsigned char anime2[] = {
	ASEQ_SET_LOOP(),
		1+D,
		1+D,
		1+D,
		1+D,
		1+D,
		1+D,
		1+D,
		1+D,
		1+D,
		2+D,
		2+D,
		3+D,
		3+D,
		4+D,
		4+D,
		5+D,
		5+D,
		6+D,
		6+D,
		6+D,
		6+D,
		6+D,
		6+D,
		6+D,
		6+D,
		6+D,
		5+D,
		5+D,
		4+D,
		4+D,
		3+D,
		3+D,
		2+D,
		2+D,
	ASEQ_EXE_LOOP(100),
};

// 勝ちのアニメーション(勝利のアニメーション)
static unsigned char anime3[] = {
	ASEQ_SET_LOOP(),
	63+W,
	63+W,
	64+W,
	64+W,
	65+W,
	65+W,
	66+W,
	66+W,
	67+W,
	67+W,
	68+W,
	68+W,
	69+W,
	69+W,
	69+W,
	70+W,
	70+W,
	70+W,
	71+W,
	71+W,
	71+W,
	71+W,
	72+W,
	72+W,
	72+W,
	72+W,
	73+W,
	73+W,
	74+W,
	74+W,
		ASEQ_SET_REPEAT(10),
		75+W,
		75+W,
		75+W,
		75+W,
		ASEQ_EXE_REPEAT(),
	ASEQ_EXE_LOOP(100),
};

// 負けのアニメーション(GAME OVERのアニメーション)
static unsigned char anime4[] = {
	3+D,
	3+D,
	3+D,
	76+W,
	76+W,
	76+W,
	77+W,
	77+W,
	77+W,
	78+W,
	78+W,
	78+W,
	79+W,
	79+W,
	79+W,
	80+W,
	80+W,
	80+W,
	81+W,
	81+W,
	81+W,
	82+W,
	82+W,
	82+W,
	83+W,
	83+W,
	83+W,
	84+W,
	84+W,
	84+W,
	85+W,
	85+W,
	85+W,
	86+W,
	86+W,
	86+W,
	87+W,
	87+W,
	87+W,
	88+W,
	88+W,
	88+W,
	89+W,
	89+W,
	89+W,
	90+W,
	90+W,
	90+W,
	91+W,
	91+W,
	91+W,
	92+W,
	92+W,
	92+W,
	92+W,
	92+W,
	92+W,
	92+W,
	92+W,
	92+W,
	93+W,
	93+W,
	93+W,
	94+W,
	94+W,
	94+W,
	ASEQ_SET_LOOP(),
	95+W,
	95+W,
	95+W,
	96+W,
	96+W,
		ASEQ_SET_REPEAT(12),
		97+W,
		97+W,
		97+W,
		97+W,
		97+W,
		ASEQ_EXE_REPEAT(),
	96+W,
	96+W,
	96+W,
	ASEQ_EXE_LOOP(100),
};

// アニメーションデータテーブル
static unsigned char *animeTbl[] = {
	anime0, anime1, anime2, anime3, anime4,
};
static int animeTblSize[] = { sizeof(animeTbl) / sizeof(*animeTbl) };
