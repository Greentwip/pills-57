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

#include "k/ani/k01.txt"
#define NON_PALETTE
#include "k/ani/k02.txt"
#include "k/ani/k03.txt"
#include "k/ani/k04.txt"
#include "k/ani/k05.txt"
#include "k/ani/k06.txt"
#include "k/ani/k07.txt"
#include "k/ani/k08.txt"
#include "k/ani/k09.txt"
#include "k/ani/k10.txt"
#include "k/ani/k11.txt"
#include "k/ani/k12.txt"
#include "k/ani/k13.txt"
#include "k/ani/k14.txt"
#include "k/ani/k15.txt"
#include "k/ani/k16.txt"
#include "k/ani/k17.txt"
#include "k/ani/k18.txt"
#include "k/ani/k19.txt"
#include "k/ani/k20.txt"
#include "k/ani/k21.txt"
#undef NON_PALETTE

static STexInfo texTbl[] = {
	{ k01_tex_addr, k01_tex_size },
	{ k02_tex_addr, k02_tex_size },
	{ k03_tex_addr, k03_tex_size },
	{ k04_tex_addr, k04_tex_size },
	{ k05_tex_addr, k05_tex_size },
	{ k06_tex_addr, k06_tex_size },
	{ k07_tex_addr, k07_tex_size },
	{ k08_tex_addr, k08_tex_size },
	{ k09_tex_addr, k09_tex_size },
	{ k10_tex_addr, k10_tex_size },
	{ k11_tex_addr, k11_tex_size },
	{ k12_tex_addr, k12_tex_size },
	{ k13_tex_addr, k13_tex_size },
	{ k14_tex_addr, k14_tex_size },
	{ k15_tex_addr, k15_tex_size },
	{ k16_tex_addr, k16_tex_size },
	{ k17_tex_addr, k17_tex_size },
	{ k18_tex_addr, k18_tex_size },
	{ k19_tex_addr, k19_tex_size },
	{ k20_tex_addr, k20_tex_size },
	{ k21_tex_addr, k21_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };

//////////////////////////////////////////////////////////////////////////////
// タイムシートのテキスト化・なぞのぞう
// 通常のアニメーション
// 攻撃のアニメーション
// ダメージのアニメーション
// 勝ちのアニメーション
// 負けのアニメーション　2000.11.20 1331PM更新　H.Yoshikawa

// 通常のアニメーション
static unsigned char anime0[] = {
	ASEQ_SET_LOOP(),
		1,1,1,2,2,3,3,4,4,4,3,3,2,2,
	ASEQ_EXE_LOOP(100),
};

// 攻撃のアニメーション
static unsigned char anime1[] = {
//	1,1,1,2,2,3,3,4,4,4,3,3,2,2,1,1,1,
	5,5,5,6,6,7,7,8,8,8,9,9,9,
	10,10,11,11,11,12,12,12,12,12,12,12,12,12,12,12,12,12,
	ASEQ_GOTO(ASEQ_NORMAL),
};

// ダメージのアニメーション
static unsigned char anime2[] = {
//	1,1,1,2,2,3,3,4,4,4,3,3,2,2,1,1,1,
	13,13,13,13,13,13,13,13,
	ASEQ_GOTO(ASEQ_NORMAL),
};

// 勝ちのアニメーション
static unsigned char anime3[] = {
//	1,1,1,2,2,3,3,4,4,4,3,3,2,2,1,1,1,
	ASEQ_SET_LOOP(),
		14,14,14,15,15,16,16,16,15,15,
	ASEQ_EXE_LOOP(100),
};

// 負けのアニメーション
static unsigned char anime4[] = {
	ASEQ_SET_LOOP(),
	17,17,17,18,18,18,19,19,19,19,19,19,18,18,18,17,17,17,
	17,17,17,18,18,18,19,19,19,19,19,19,18,18,18,17,17,17,
	17,17,17,18,18,18,19,19,19,19,20,20,20,20,20,20,19,19,19,
	20,20,20,20,20,20,20,20,19,19,19,18,18,18,
	ASEQ_EXE_LOOP(100),
};

// アニメーションデータテーブル
static unsigned char *animeTbl[] = {
	anime0, anime1, anime2, anime3, anime4,
};
static int animeTblSize[] = { sizeof(animeTbl) / sizeof(*animeTbl) };
