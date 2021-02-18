
#include <ultra64.h>
#include <math.h>
#include "def.h"
#include "debug.h"
#include "util.h"
#include "tex_func.h"
#include "char_anime.h"
#include "dmacopy.h"
#include "vr_init.h"
#include "local.h"

//////////////////////////////////////////////////////////////////////////////

void animeSeq_init(SAnimeSeq *st, unsigned char **seqArray, int animeNo)
{
	st->stackDepth = -1;

	st->seqArray = seqArray;

	st->animeNo = animeNo;

	st->seqCount = 0;

	st->textureNo = -1;

	animeSeq_update(st, 0);
}

void animeSeq_set(SAnimeSeq *st, int animeNo)
{
	animeSeq_init(st, st->seqArray, animeNo);
}

void animeSeq_update(SAnimeSeq *st, int step)
{
	unsigned char *seqDat, *seqPtr;

	while(step >= 0) {
		seqDat = st->seqArray[st->animeNo];
		seqPtr = &seqDat[st->seqCount];

		switch(seqPtr[0]) {
		case _ASEQ_SET_REPEAT:
			// 繰り返し位置を設定
			st->seqCount += 2;
			st->stackDepth++;
			st->labelStack[st->stackDepth] = st->seqCount;
			st->countStack[st->stackDepth] = seqPtr[1];
			break;

		case _ASEQ_EXE_REPEAT:
			if(st->countStack[st->stackDepth] != 0xff) {
				st->countStack[st->stackDepth]--;
			}

			if(st->countStack[st->stackDepth] > 0 && seqPtr[1] > rand() % 100) {
				// 繰り返し再生を行う
				st->seqCount = st->labelStack[st->stackDepth];
			}
			else {
				// 繰り返し再生を終了する
				st->seqCount += 2;
				st->stackDepth--;
			}
			break;

		case _ASEQ_GOTO:
			// 他のアニメーションを再生
			st->seqCount = 0;
			st->animeNo = seqPtr[1];
			st->stackDepth = -1;
			break;

		case _ASEQ_END:
			// アニメーションを終了
			step = -1;
			break;

		default:
			if(step > 0) {
				st->seqCount++;
			}
			st->textureNo = seqPtr[0] - 1;
			step--;
			break;
		}
	}
}

int animeSeq_isEnd(SAnimeSeq *st)
{
	return st->seqArray[st->animeNo][st->seqCount] == _ASEQ_END;
}

//////////////////////////////////////////////////////////////////////////////

#include "boot_data.h"
#if LOCAL==AMERICA
#include "texture/character/AMERICA/segsize.h"
#elif LOCAL==CHINA
#include "texture/character/AMERICA/segsize.h"
#else
#include "texture/character/JAPAN/segsize.h"
#endif

int animeState_getDataSize(int charNo)
{
	static const int _size[] = {
		_anime_mSegmentSize, // 00:マリオ
		_anime_nSegmentSize, // 01:ワリオ
		_anime_hSegmentSize, // 02:マユピー
		_anime_iSegmentSize, // 03:ヤリマル
		_anime_jSegmentSize, // 04:りんごろう
		_anime_dSegmentSize, // 05:フウセンまじん
		_anime_eSegmentSize, // 06:つまりカエル
		_anime_fSegmentSize, // 07:ふっくらげ
		_anime_aSegmentSize, // 08:イカタコテング
		_anime_bSegmentSize, // 09:キグモン
		_anime_cSegmentSize, // 10:ハンマーロボ
		_anime_gSegmentSize, // 11:マッド＝シタイン
		_anime_kSegmentSize, // 12:なぞのぞう
		_anime_lSegmentSize, // 13:バンパイアワリオ
		_anime_oSegmentSize, // 14:メタルマリオ
		_anime_marioSegmentSize, // 15:デカマリオ
		_anime_virus_rSegmentSize, // 16:ウィルス赤
		_anime_virus_ySegmentSize, // 17:ウィルス黄
		_anime_virus_bSegmentSize, // 18:ウィルス青
		_anime_smogSegmentSize, // 19:煙
	};
	return _size[charNo];
}

void animeState_load(SAnimeState *st, void **hpp, int charNo)
{
	// アニメーションデータのＲＯＭアドレス
	static const int _addrTbl[] = {
		_anime_m, // 00:マリオ
		_anime_n, // 01:ワリオ
		_anime_h, // 02:マユピー
		_anime_i, // 03:ヤリマル
		_anime_j, // 04:りんごろう
		_anime_d, // 05:フウセンまじん
		_anime_e, // 06:つまりカエル
		_anime_f, // 07:ふっくらげ
		_anime_a, // 08:イカタコテング
		_anime_b, // 09:キグモン
		_anime_c, // 10:ハンマーロボ
		_anime_g, // 11:マッド＝シタイン
		_anime_k, // 12:なぞのぞう
		_anime_l, // 13:バンパイアワリオ
		_anime_o, // 14:メタルマリオ
		_anime_mario, // 15:デカマリオ
		_anime_virus_r, // 16:ウィルス赤
		_anime_virus_y, // 17:ウィルス黄
		_anime_virus_b, // 18:ウィルス青
		_anime_smog, // 19:煙
	};
	// テクスチャの中心座標
	static const int _centerTbl[][2] = {
		{ 16, 43 }, // 00:マリオ
		{ 25, 44 }, // 01:ワリオ
		{ 16, 41 }, // 02:マユピー
		{ 22, 40 }, // 03:ヤリマル
		{ 26, 51 }, // 04:りんごろう
		{ 24, 40 }, // 05:フウセンまじん
		{ 24, 50 }, // 06:つまりカエル
		{ 19, 40 }, // 07:ふっくらげ
		{ 32, 50 }, // 08:イカタコテング
		{ 18, 47 }, // 09:キグモン
		{ 32, 62 }, // 10:ハンマーロボ
		{ 35, 60 }, // 11:マッド＝シタイン
		{ 20, 47 }, // 12:なぞのぞう
		{ 30, 49 }, // 13:バンパイアワリオ
		{ 16, 43 }, // 14:メタルマリオ
		{ 32, 64 }, // 15:デカマリオ
		{ 16, 16 }, // 16:ウィルス赤
		{ 16, 16 }, // 17:ウィルス黄
		{ 16, 16 }, // 18:ウィルス青
		{ 16, 16 }, // 19:煙
	};

	unsigned char **seqArray;
	STexInfo *texArray;

	loadAnimeSeq(hpp, &texArray, &seqArray,
		_romDataTbl[_addrTbl[charNo]][0], _romDataTbl[_addrTbl[charNo]][1]);

	animeState_init(st, seqArray, texArray,
		_centerTbl[charNo][0], _centerTbl[charNo][1], charNo);
}

void animeState_init(SAnimeState *st, unsigned char **seqArray, STexInfo *texArray, int cx, int cy, int charNo)
{
	animeSeq_init(st->animeSeq, seqArray, ASEQ_NORMAL);
	st->texArray = texArray;
	st->frameCount = 0;
	st->center[0] = cx;
	st->center[1] = cy;
	st->charNo = charNo;

	st->color[0] = 255;
	st->color[1] = 255;
	st->color[2] = 255;
	st->color[3] = 255;
}

// アニメーション番号を設定
void animeState_set(SAnimeState *st, int animeNo)
{
	st->frameCount = 0;
	animeSeq_set(st->animeSeq, animeNo);
}

// アニメーションを更新
void animeState_update(SAnimeState *st)
{
	animeSeq_update(st->animeSeq, st->frameCount & 1);
	st->frameCount++;
}

// アニメーションが終了している? の確認
int animeState_isEnd(SAnimeState *st)
{
	return animeSeq_isEnd(st->animeSeq);
}

// ディスプレイリストを初期化
void animeState_initDL(SAnimeState *st, Gfx **gpp)
{
	Gfx *gp = *gpp;

	gSPDisplayList(gp++, normal_texture_init_dl);
	gDPSetCombineMode(gp++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
	gDPSetPrimColor(gp++, 0,0, st->color[0],st->color[1],st->color[2],st->color[3]);

	if(st->color[3] < 255) {
		gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
	}

	*gpp = gp;
}

// ディスプレイリストを初期化
void animeState_initDL2(SAnimeState *st, Gfx **gpp)
{
	Gfx *gp = *gpp;

	gSPDisplayList(gp++, normal_texture_init_dl);
//	gDPSetCombineMode(gp++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
	gDPSetCombineLERP(gp++,
		TEXEL0,ENVIRONMENT,PRIMITIVE,ENVIRONMENT, TEXEL0,0,PRIMITIVE,0,
		TEXEL0,ENVIRONMENT,PRIMITIVE,ENVIRONMENT, TEXEL0,0,PRIMITIVE,0);
	gDPSetPrimColor(gp++, 0,0, st->color[0],st->color[1],st->color[2],st->color[3]);
//	gDPSetEnvColor(gp++, 255, 255, 255, 255);
	gDPSetEnvColor(gp++, 0, 0, 0, 255);

	if(st->color[3] < 255) {
		gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
	}

	*gpp = gp;
}

// ディスプレイリストを初期化
void animeState_initIntensityDL(SAnimeState *st, Gfx **gpp)
{
	Gfx *gp = *gpp;

	gSPDisplayList(gp++, normal_texture_init_dl);
	gDPSetTextureLUT(gp++, G_TT_NONE);
	gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
	gDPSetCombineLERP(gp++,
		PRIMITIVE,ENVIRONMENT,TEXEL0,ENVIRONMENT, TEXEL0,0,PRIMITIVE,0,
		PRIMITIVE,ENVIRONMENT,TEXEL0,ENVIRONMENT, TEXEL0,0,PRIMITIVE,0);
	gDPSetPrimColor(gp++, 0,0, st->color[0],st->color[1],st->color[2],st->color[3]);
	gDPSetEnvColor(gp++, 0, 0, 0, 255);

	*gpp = gp;
}

// アニメーションを描画
void animeState_draw(SAnimeState *st, Gfx **gpp, float x, float y, float sx, float sy)
{
	Gfx *gp = *gpp;
	STexInfo *texC, *texI, *pal;

	if(st->animeSeq->textureNo < 0) {
		return;
	}

	pal  = &st->texArray[0];
	texC = &st->texArray[st->animeSeq->textureNo];

	if(sx < 0) {
		x -= (st->center[0] - texC->size[0]) * sx;
	}
	else {
		x -= st->center[0] * sx;
	}

	if(sy < 0) {
		y -= (st->center[1] - texC->size[1]) * sy;
	}
	else {
		y -= st->center[1] * sy;
	}

	// デカマリオだけ特殊処理
	if(st->charNo == 15) {
		y -= (texC->size[1] - 64) * sy;
	}

	switch(texC->size[2]) {
	case 4:
		StretchTexTile4(&gp,
			texC->size[0], texC->size[1],
			pal->addr[0], texC->addr[1],
			0, 0, texC->size[0], texC->size[1],
			x, y, texC->size[0] * sx, texC->size[1] * sy);
		break;
	case 8:
		StretchTexTile8(&gp,
			texC->size[0], texC->size[1],
			pal->addr[0], texC->addr[1],
			0, 0, texC->size[0], texC->size[1],
			x, y, texC->size[0] * sx, texC->size[1] * sy);
		break;
	}

	*gpp = gp;
}

// アニメーションを描画 ( インテンシティテクスチャ用 )
void animeState_drawI(SAnimeState *st, Gfx **gpp, float x, float y, float sx, float sy)
{
	Gfx *gp = *gpp;
	STexInfo *texC;

	if(st->animeSeq->textureNo < 0) {
		return;
	}

	texC = &st->texArray[st->animeSeq->textureNo];

	if(sx < 0) {
		x -= (st->center[0] - texC->size[0]) * sx;
	}
	else {
		x -= st->center[0] * sx;
	}

	if(sy < 0) {
		y -= (st->center[1] - texC->size[1]) * sy;
	}
	else {
		y -= st->center[1] * sy;
	}

	StretchTexTile4i(&gp,
		texC->size[0], texC->size[1], texC->addr[1],
		0, 0, texC->size[0], texC->size[1],
		x, y, texC->size[0] * sx, texC->size[1] * sy);

	*gpp = gp;
}

//////////////////////////////////////////////////////////////////////////////

void animeSmog_init(SAnimeSmog *st, SAnimeState *state)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(st->animeState); i++) {
		animeState_init(&st->animeState[i], state->animeSeq->seqArray, state->texArray,
			state->center[0], state->center[1], 19);
		animeState_set(&st->animeState[i], ASEQ_DAMAGE);
		st->pos[i][0] = st->pos[i][1] = 0;
	}

	st->frameCount = 180;
}

void animeSmog_load(SAnimeSmog *st, void **hpp)
{
	SAnimeState *state;
	int i;

	state = &st->animeState[0];
	animeState_load(state, hpp, 19);

	animeSmog_init(st, state);
}

void animeSmog_start(SAnimeSmog *st)
{
	st->frameCount = 0;
}

void animeSmog_stop(SAnimeSmog *st)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(st->animeState); i++) {
		animeState_set(&st->animeState[i], ASEQ_DAMAGE);
	}

	st->frameCount = 180;
}

void animeSmog_update(SAnimeSmog *st)
{
	SAnimeState *state;
	int i;

	for(i = 0; i < ARRAY_SIZE(st->animeState); i++) {
		state = &st->animeState[i];
		animeState_update(state);

		if(st->frameCount < 180 && animeState_isEnd(state) && rand() % 16 == 0) {
			animeState_set(state, ASEQ_NORMAL);
			st->pos[i][0] = rand() % 20 - 10;
			st->pos[i][1] = rand() % 20 - 10;
		}
	}

	st->frameCount = MIN(180, st->frameCount + 1);
}

void animeSmog_draw(SAnimeSmog *st, Gfx **gpp, float x, float y, float sx, float sy)
{
	Gfx *gp = *gpp;
	SAnimeState *state;
	int i;

	for(i = 0; i < ARRAY_SIZE(st->animeState); i++) {
		state = &st->animeState[i];
		animeState_initIntensityDL(state, &gp);

		if(!animeState_isEnd(state)) {
			animeState_drawI(state, &gp,
				x + st->pos[i][0] * sx, y + st->pos[i][1] * sy, sx, sy);
		}
	}

	*gpp = gp;
}

//////////////////////////////////////////////////////////////////////////////

// ROMから読み込んだ状態のシーケンスデータをメモリ上にマッピング
void mappingAnimeSeq(unsigned char **seqArray, int count, u32 segGap)
{
	int i;

	for(i = 0; i < count; i++) {
		(u8 *)seqArray[i] += segGap;
	}
}

// シーケンスデータを読み込む
void loadAnimeSeq(void **hpp,
	STexInfo **texArray, unsigned char ***seqArray,
	void *romStart, void *romEnd)
{
	void *hp = (void *)ALIGN_16(*hpp);
	u32 addr, size;

//	*hpp = (void *)ExpandRomData(romStart, hp, (u32)romEnd - (u32)romStart);
	*hpp = (void *)ExpandGZip(romStart, hp, (u32)romEnd - (u32)romStart);

	addr = ( ((u32 *)hp)[0] += (u32)hp );
	size = ( ((u32 *)hp)[1] += (u32)hp );
	*texArray = (STexInfo *)addr;
	tiMappingAddr(*texArray, *(u32 *)size, (u32)hp);

	addr = ( ((u32 *)hp)[2] += (u32)hp );
	size = ( ((u32 *)hp)[3] += (u32)hp );
	*seqArray = (unsigned char **)addr;
	mappingAnimeSeq(*seqArray, *(u32 *)size, (u32)hp);
}

//////////////////////////////////////////////////////////////////////////////
