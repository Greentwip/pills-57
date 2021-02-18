
#include <ultra64.h>
#include <math.h>
#include <ctype.h>
#include "debug.h"
#include "def.h"
#include "nnsched.h"
#include "util.h"
#include "tex_func.h"
#include "char_anime.h"
#include "msgwnd.h"
#include "font.h"
#include "char_anime.h"
#include "cpu_task.h"
#include "main_story.h"
#include "aidebug.h"
#include "local.h"

//////////////////////////////////////////////////////////////////////////////
//{### マクロ

#define CHOICE_A_SIZE 47
#define WINDOW_ALPHA 160
#define BGM_FADE_FRAME 20
#define FRAME_MOVE_STEP (1.0 / 20.0)
#define BLINK_SPEED (1.0 / 32.0)

//////////////////////////////////////////////////////////////////////////////
//{### 項目管理ルーチン

typedef struct SMenuItem SMenuItem;

struct SMenuItem {
	float center[2];

	void (*transFunc)(SMenuItem *st);
	float trans[2];
	float transTime;
	float transStep;
	float transRange[2][2];

	void (*scaleFunc)(SMenuItem *st);
	float scale[2];
	float scaleTime;
	float scaleStep;
	float scaleRange[2][2];

	void (*colorFunc)(SMenuItem *st);
	float color[4];
	float colorTime;
	float colorStep;
	float colorRange[2][4];

	struct {
		unsigned localColor : 1;
		unsigned localAlpha : 1;
	} flags;
};

static float timeFunc_update(float time, float step)
{
	time += step;

	if(time < 0) time = 0;
	else if(time > 1) time = 1;

	return time;
}

static float timeFunc_loop(float time, float step)
{
	return WrapF(0, 1, time + step);
}

static float coordFunc_curve(float time)
{
#if 1
	if(time < 0.5) {
		time = time * time;
	}
	else {
		time = 1.0 - time;
		time = 0.5 - time * time;
	}
	return time + time;
#else
	if(time < 0.5) {
		time = (time * time * time) * 4;
	}
	else {
		time = time - 1.0;
		time = (time * time * time) * 4 + 1.0;
	}
	return time;
#endif
}

static void transFunc_linear(SMenuItem *st)
{
	float time = timeFunc_update(st->transTime, st->transStep);
	int i;

	st->transTime = time;

	for(i = 0; i < ARRAY_SIZE(st->trans); i++) {
		st->trans[i] = st->transRange[0][i] +
			(st->transRange[1][i] - st->transRange[0][i]) * time;
	}
}

static void transFunc_curve(SMenuItem *st)
{
	float time = timeFunc_update(st->transTime, st->transStep);
	float coord = coordFunc_curve(time);
	int i;

	st->transTime = time;

	for(i = 0; i < ARRAY_SIZE(st->trans); i++) {
		st->trans[i] = st->transRange[0][i] +
			(st->transRange[1][i] - st->transRange[0][i]) * coord;
	}
}

static void transFunc_finger(SMenuItem *st)
{
	float time = timeFunc_loop(st->transTime, st->transStep);
	float coord = (sinf(M_PI * 2 * time) + 1.0) * 0.5;
	int i;

	st->transTime = time;

	for(i = 0; i < ARRAY_SIZE(st->trans); i++) {
		st->trans[i] = st->transRange[0][i] +
			(st->transRange[1][i] - st->transRange[0][i]) * coord;
	}
}

static void scaleFunc_linear(SMenuItem *st)
{
	float time = timeFunc_update(st->scaleTime, st->scaleStep);
	int i;

	st->scaleTime = time;

	for(i = 0; i < ARRAY_SIZE(st->scale); i++) {
		st->scale[i] = st->scaleRange[0][i] +
			(st->scaleRange[1][i] - st->scaleRange[0][i]) * time;
	}
}

static void scaleFunc_curve(SMenuItem *st)
{
	float time = timeFunc_update(st->scaleTime, st->scaleStep);
	float coord = coordFunc_curve(time);
	int i;

	st->scaleTime = time;

	for(i = 0; i < ARRAY_SIZE(st->scale); i++) {
		st->scale[i] = st->scaleRange[0][i] +
			(st->scaleRange[1][i] - st->scaleRange[0][i]) * coord;
	}
}

static void colorFunc_linear(SMenuItem *st)
{
	float time = timeFunc_update(st->colorTime, st->colorStep);
	int i;

	st->colorTime = time;

	for(i = 0; i < ARRAY_SIZE(st->color); i++) {
		st->color[i] = st->colorRange[0][i] +
			(st->colorRange[1][i] - st->colorRange[0][i]) * time;
	}
}

static void colorFunc_curve(SMenuItem *st)
{
	float time = timeFunc_update(st->colorTime, st->colorStep);
	float coord = coordFunc_curve(time);
	int i;

	st->colorTime = time;

	for(i = 0; i < ARRAY_SIZE(st->color); i++) {
		st->color[i] = st->colorRange[0][i] +
			(st->colorRange[1][i] - st->colorRange[0][i]) * coord;
	}
}

static void colorFunc_cursor(SMenuItem *st)
{
	float time = timeFunc_loop(st->colorTime, st->colorStep);
	float coord = (sinf(M_PI * 2 * time) + 1.0) * 0.5;
//	float coord = ((time < 0.5) ? time : (1.0 - time)) * 2;
	int i;

	st->colorTime = time;

	for(i = 0; i < ARRAY_SIZE(st->color); i++) {
		st->color[i] = st->colorRange[0][i] +
			(st->colorRange[1][i] - st->colorRange[0][i]) * coord;
	}
}

static void menuItem_init(SMenuItem *st, int x, int y)
{
	static const float color[] = { 0.5, 0.5, 0.5, 1.0 };
	int i;

	st->center[0] = 0;
	st->center[1] = 0;

	st->transFunc = transFunc_curve;
	st->transTime = 1;
	st->transStep = 1.0 / 8.0;
	st->trans[0] = st->transRange[0][0] = st->transRange[1][0] = x;
	st->trans[1] = st->transRange[0][1] = st->transRange[1][1] = y;

	st->scaleFunc = scaleFunc_curve;
	st->scaleTime = 1;
	st->scaleStep = 1.0 / 8.0;
	for(i = 0; i < ARRAY_SIZE(st->scale); i++) {
		st->scale[i] = 1;
		st->scaleRange[0][i] = 0;
		st->scaleRange[1][i] = 1;
	}

	st->colorFunc = colorFunc_curve;
	st->colorTime = 1;
	st->colorStep = 1.0 / 8.0;
	for(i = 0; i < ARRAY_SIZE(st->color); i++) {
		st->color[i] = 1;
		st->colorRange[0][i] = color[i];
		st->colorRange[1][i] = 1;
	}

	st->flags.localColor = 0;
	st->flags.localAlpha = 0;
}

static void menuItem_updateTransScale(SMenuItem *st, SMenuItem *parent)
{
	int i;

	st->transFunc(st);
	st->scaleFunc(st);

	if(parent) {
		for(i = 0; i < ARRAY_SIZE(st->trans); i++) {
			st->trans[i] = st->trans[i] * parent->scale[i] + parent->trans[i];
			st->scale[i] *= parent->scale[i];
			st->trans[i] -= st->center[i] * st->scale[i];
		}
	}
}

static void menuItem_updateColor(SMenuItem *st, SMenuItem *parent)
{
	int i;

	st->colorFunc(st);

	if(parent) {
		if(!st->flags.localColor) {
			for(i = 0; i < ARRAY_SIZE(st->color) - 1; i++) {
				st->color[i] *= parent->color[i];
			}
		}
		if(!st->flags.localAlpha) {
			for(i = ARRAY_SIZE(st->color) - 1; i < ARRAY_SIZE(st->color); i++) {
				st->color[i] *= parent->color[i];
			}
		}
	}
}

static void menuItem_update(SMenuItem *st, SMenuItem *parent)
{
	menuItem_updateTransScale(st, parent);
	menuItem_updateColor(st, parent);
}

static void menuItem_updateN(SMenuItem *array, int count, SMenuItem *parent)
{
	int i;

	for(i = 0; i < count; i++) {
		menuItem_update(&array[i], parent);
	}
}

static void menuItem_setTransDir(SMenuItem *st, int dir)
{
	if((dir < 0 && st->transStep > 0) || (dir > 0 && st->transStep < 0)) {
		st->transStep = -st->transStep;
	}
}

static void menuItem_setScaleDir(SMenuItem *st, int dir)
{
	if((dir < 0 && st->scaleStep > 0) || (dir > 0 && st->scaleStep < 0)) {
		st->scaleStep = -st->scaleStep;
	}
}

static void menuItem_setColorDir(SMenuItem *st, int dir)
{
	if((dir < 0 && st->colorStep > 0) || (dir > 0 && st->colorStep < 0)) {
		st->colorStep = -st->colorStep;
	}
}

static void menuItem_setTransLow(SMenuItem *item, float x, float y)
{
	item->transRange[0][0] = x;
	item->transRange[0][1] = y;
}

static void menuItem_setTransHi(SMenuItem *item, float x, float y)
{
	item->transRange[1][0] = x;
	item->transRange[1][1] = y;
}

static void menuItem_setScaleLow(SMenuItem *item, float x, float y)
{
	item->scaleRange[0][0] = x;
	item->scaleRange[0][1] = y;
}

static void menuItem_setScaleHi(SMenuItem *item, float x, float y)
{
	item->scaleRange[1][0] = x;
	item->scaleRange[1][1] = y;
}

static void menuItem_setColorLow(SMenuItem *item, float bright, float alpha)
{
	item->colorRange[0][0] = item->colorRange[0][1] = item->colorRange[0][2] = bright;
	item->colorRange[0][3] = alpha;
}

static void menuItem_setColorHi(SMenuItem *item, float bright, float alpha)
{
	item->colorRange[1][0] = item->colorRange[1][1] = item->colorRange[1][2] = bright;
	item->colorRange[1][3] = alpha;
}

static void menuItem_setColor_cursor(SMenuItem *item)
{
	item->colorFunc = colorFunc_cursor;
	item->colorStep = BLINK_SPEED;
	menuItem_setColorLow(item, 0.5, 1.0);
	menuItem_setColorHi(item, 1.0, 1.0);
}

static void menuItem_setColor_cursor2(SMenuItem *item,
	float red, float green, float blue)
{
	float rgb[] = { red, green, blue };
	int i;
	item->colorFunc = colorFunc_cursor;
	item->colorStep = BLINK_SPEED;
	menuItem_setColorLow(item, 0.5, 1.0);
	menuItem_setColorHi(item, 1.0, 1.0);
	for(i = 0; i < 3; i++) {
		item->colorRange[0][i] *= rgb[i];
		item->colorRange[1][i] *= rgb[i];
	}
}

static void menuItem_setColor_fade(SMenuItem *item)
{
	item->colorFunc = colorFunc_curve;
	item->colorStep = 1.0 / 8.0;
	menuItem_setColorLow(item, 1.0, 0.0);
	menuItem_setColorHi(item, 1.0, 1.0);
}

static void menuItem_setPrim(SMenuItem *st, Gfx **gpp)
{
	Gfx *gp = *gpp;
	int color[] = {
		(int)(CLAMP(0, 1, st->color[0]) * 255),
		(int)(CLAMP(0, 1, st->color[1]) * 255),
		(int)(CLAMP(0, 1, st->color[2]) * 255),
		(int)(CLAMP(0, 1, st->color[3]) * 255),
	};

//	gDPPipeSync(gp++);
//	if(color[3] < 255) {
//		gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
//	}
	gDPSetPrimColor(gp++, 0, 0, color[0], color[1], color[2], color[3]);

	*gpp = gp;
}

static bool menuItem_outOfScreen(SMenuItem *st, int width, int height)
{
	if(st->color[3] <= 0
	|| st->trans[0] >= SCREEN_WD
	|| st->trans[1] >= SCREEN_HT
	|| st->trans[0] + width  * st->scale[0] < 0
	|| st->trans[1] + height * st->scale[1] < 0
	|| st->scale[0] == 0
	|| st->scale[1] == 0
	)
	{
		return true;
	}
	return false;
}

static int menuItem_drawTex(SMenuItem *st, Gfx **gpp, STexInfo *tex, int cached)
{
	if(menuItem_outOfScreen(st, tex->size[0], tex->size[1])) {
		return 0;
	}

	menuItem_setPrim(st, gpp);

	if(tex->size[3] & 1) {
		tiStretchTexBlock(gpp, tex, cached,
			st->trans[0], st->trans[1],
			tex->size[0] * st->scale[0],
			tex->size[1] * st->scale[1]);
	}
	else {
		tiStretchTexTile(gpp, tex, cached,
			0, 0, tex->size[0], tex->size[1],
			st->trans[0], st->trans[1],
			tex->size[0] * st->scale[0],
			tex->size[1] * st->scale[1]);
	}

	return 1;
}
static int menuItem_drawTex2(SMenuItem *st, Gfx **gpp, STexInfo *tex, int cached,
	float x, float y, float sx, float sy)
{
	float bak[] = { st->trans[0], st->trans[1], st->scale[0], st->scale[1] };
	st->trans[0] += x;
	st->trans[1] += y;
	st->scale[0] *= sx;
	st->scale[1] *= sy;
	menuItem_drawTex(st, gpp, tex, cached);
	st->trans[0] = bak[0];
	st->trans[1] = bak[1];
	st->scale[0] = bak[2];
	st->scale[1] = bak[3];
}

static int menuItem_drawAlphaTex(SMenuItem *st, Gfx **gpp,
	STexInfo *texC, STexInfo *texA, int cached)
{
	int width, height;
	void *tex[2];

	width = MIN(texC->size[0], texA->size[0]);
	height = MIN(texC->size[1], texA->size[1]);

	if(menuItem_outOfScreen(st, width, height)) {
		return 0;
	}

	menuItem_setPrim(st, gpp);

	if(cached) {
		tex[0] = NULL;
		tex[1] = NULL;
	}
	else {
		tex[0] = texC->addr[1];
		tex[1] = texA->addr[1];
	}

	if((texC->size[3] & 1) && (texA->size[3] & 1)) {
		StretchAlphaTexBlock(gpp, width, height,
			tex[0], texC->size[0], tex[1], texA->size[0],
			st->trans[0], st->trans[1],
			width * st->scale[0],
			height * st->scale[1]);
	}
	else {
		StretchAlphaTexTile(gpp, width, height,
			texC->addr[1], texC->size[0], texA->addr[1], texA->size[0],
			0, 0, width, texC->size[1],
			st->trans[0], st->trans[1],
			width * st->scale[0],
			height * st->scale[1]);
	}

	return 1;
}

static int menuItem_drawItem(SMenuItem *st, Gfx **gpp,
	STexInfo *tex, int cached, int itemCount, int itemIndex)
{
	int height;

	height = tex->size[1] / itemCount;

	if(menuItem_outOfScreen(st, tex->size[0], height)) {
		return 0;
	}

	menuItem_setPrim(st, gpp);

	tiStretchTexItem(gpp, tex, cached, itemCount, itemIndex,
		st->trans[0], st->trans[1],
		tex->size[0] * st->scale[0], height * st->scale[1]);

	return 1;
}
static int menuItem_drawItem2(SMenuItem *st, Gfx **gpp, STexInfo *tex, int cached,
	int itemCount, int itemIndex, float x, float y, float sx, float sy)
{
	float bak[] = { st->trans[0], st->trans[1], st->scale[0], st->scale[1] };
	st->trans[0] += x;
	st->trans[1] += y;
	st->scale[0] *= sx;
	st->scale[1] *= sy;
	menuItem_drawItem(st, gpp, tex, cached, itemCount, itemIndex);
	st->trans[0] = bak[0];
	st->trans[1] = bak[1];
	st->scale[0] = bak[2];
	st->scale[1] = bak[3];
}

static int menuItem_drawAlphaItem(SMenuItem *st, Gfx **gpp,
	STexInfo *texC, STexInfo *texA, int cached, int itemCount, int itemIndex)
{
	int width, height;

	width = MIN(texC->size[0], texA->size[0]);
	height = MIN(texC->size[1], texA->size[1]) / itemCount;

	if(menuItem_outOfScreen(st, width, height)) {
		return 0;
	}

	menuItem_setPrim(st, gpp);

	tiStretchAlphaTexItem(gpp, texC, texA, cached, itemCount, itemIndex,
		st->trans[0], st->trans[1],
		width * st->scale[0], height * st->scale[1]);

	return 1;
}

static int menuItem_drawAlphaItem2(SMenuItem *st, Gfx **gpp,
	STexInfo *texC, STexInfo *texA, int cached, int itemCount, int itemIndex)
{
	int width, height;

	width = MIN(texC->size[0], texA->size[0]);
	height = texC->size[1] / itemCount;
	height = MIN(height, texA->size[1]);

	if(menuItem_outOfScreen(st, width, height)) {
		return 0;
	}

	menuItem_setPrim(st, gpp);

	tiStretchAlphaTexItem2(gpp, texC, texA, cached, itemCount, itemIndex,
		st->trans[0], st->trans[1],
		width * st->scale[0], height * st->scale[1]);

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
//{### 初期化DL

static const Gfx fade_normal_texture_init_dl[] = {
	gsSPDisplayList(normal_texture_init_dl),
	gsDPSetCombineLERP(
		TEXEL0,ENVIRONMENT,PRIMITIVE,ENVIRONMENT, TEXEL0,0,PRIMITIVE,0,
		TEXEL0,ENVIRONMENT,PRIMITIVE,ENVIRONMENT, TEXEL0,0,PRIMITIVE,0),
	gsSPEndDisplayList(),
};

static const Gfx fade_alpha_texture_init_dl[] = {
	gsSPDisplayList(alpha_texture_init_dl),
	gsDPSetCombineLERP(
		TEXEL0,ENVIRONMENT,PRIMITIVE,ENVIRONMENT, TEXEL1,0,PRIMITIVE,0,
		0, 0, 0, COMBINED, 0, 0, 0, COMBINED),
	gsSPEndDisplayList(),
};

static const Gfx fade_intensity_texture_init_dl[] = {
	gsSPDisplayList(normal_texture_init_dl),
	gsDPSetCombineLERP(
		PRIMITIVE,ENVIRONMENT,PRIMITIVE,ENVIRONMENT, TEXEL0,0,PRIMITIVE,0,
		PRIMITIVE,ENVIRONMENT,PRIMITIVE,ENVIRONMENT, TEXEL0,0,PRIMITIVE,0),
	gsDPSetRenderMode(G_RM_XLU_SURF, G_RM_XLU_SURF2),
	gsDPSetTextureLUT(G_TT_NONE),
	gsSPEndDisplayList(),
};

static const Gfx fade_fillrect_init_dl[] = {
	gsSPDisplayList(normal_texture_init_dl),
	gsDPSetCombineLERP(
		PRIMITIVE,ENVIRONMENT,PRIMITIVE,ENVIRONMENT, 0,0,0,PRIMITIVE,
		PRIMITIVE,ENVIRONMENT,PRIMITIVE,ENVIRONMENT, 0,0,0,PRIMITIVE),
	gsDPSetRenderMode(G_RM_XLU_SURF, G_RM_XLU_SURF2),
	gsDPSetTextureLUT(G_TT_NONE),
	gsSPEndDisplayList(),
};

static const Gfx fade_shadow_texture_init_dl[] = {
	gsSPDisplayList(normal_texture_init_dl),
	gsDPSetCombineLERP(
		PRIMITIVE,ENVIRONMENT,PRIMITIVE,ENVIRONMENT, TEXEL0,0,PRIMITIVE,0,
		PRIMITIVE,ENVIRONMENT,PRIMITIVE,ENVIRONMENT, TEXEL0,0,PRIMITIVE,0),
	gsDPSetRenderMode(G_RM_XLU_SURF, G_RM_XLU_SURF2),
	gsSPEndDisplayList(),
};

//////////////////////////////////////////////////////////////////////////////
//{### リソースなど

#include "texture/menu/menu_char.h"
#include "texture/menu/menu_common.h"
#include "texture/menu/menu_level.h"
#include "texture/menu/menu_main.h"
#include "texture/menu/menu_name.h"
#include "texture/menu/menu_p2.h"
#include "texture/menu/menu_p4.h"
#include "texture/menu/menu_rank.h"
#include "texture/menu/menu_setup.h"
#include "texture/menu/menu_story.h"
#include "texture/game/game_al.h"
#include "texture/game/game_p1.h"
#include "texture/menu/menu_cont.h"
#include "texture/tutorial/tutorial_data.h"
#include "texture/menu/menu_kasa.h"

static void _eepWritePlayer(void *global);
static void _eepErasePlayer(void *global);
static void _eepEraseData(void *global);
static void _waitRetrace(void *global);
static Mtx **_getMtxPtr(void *global);
static Vtx **_getVtxPtr(void *global);
static STexInfo *_getTexChar(void *global, int index);
static STexInfo *_getTexCommon(void *global, int index);
static STexInfo *_getTexCredit(void *global, int index);
static STexInfo *_getTexLevel(void *global, int index);
static STexInfo *_getTexMain(void *global, int index);
static STexInfo *_getTexName(void *global, int index);
static STexInfo *_getTexP2(void *global, int index);
static STexInfo *_getTexP4(void *global, int index);
static STexInfo *_getTexRank(void *global, int index);
static STexInfo *_getTexSetup(void *global, int index);
static STexInfo *_getTexStory(void *global, int index);
static STexInfo *_getTexGameAl(void *global, int index);
static STexInfo *_getTexGameP1(void *global, int index);
static STexInfo *_getTexCont(void *global, int index);
static STexInfo *_getTexTutol(void *global, int index);
static STexInfo *_getTexKasa(void *global, int index);

static SMenuItem *_getRootItem(void *global);

static int _getKeyLvl(void *global, int contNo);
static int _getKeyTrg(void *global, int contNo);
static int _getKeyRep(void *global, int contNo);

typedef enum {
	MODE_MAIN,
		MODE_PLAY1,
			MODE_STORY_NS,
				MODE_STORY_NE,
					MODE_STORY,
			MODE_LVSEL_NS,
				MODE_LVSEL_NE,
					MODE_LVSEL,
			MODE_LVSEL_TQ_NS,
				MODE_LVSEL_TQ_NE,
					MODE_LVSEL_TQ,
			MODE_LVSEL_TA_NS,
				MODE_LVSEL_TA_NE,
					MODE_LVSEL_TA,
			MODE_VSCOM_NS,
				MODE_VSCOM_NE,
					MODE_VSCOM_CH,
						MODE_VSCOM,
			MODE_VSCOM_FL_NS,
				MODE_VSCOM_FL_NE,
					MODE_VSCOM_FL_CH,
						MODE_VSCOM_FL,
		MODE_PLAY2,
			MODE_VSMAN_NS,
				MODE_VSMAN_NE,
					MODE_VSMAN_CH,
						MODE_VSMAN,
			MODE_VSMAN_FL_NS,
				MODE_VSMAN_FL_NE,
					MODE_VSMAN_FL_CH,
						MODE_VSMAN_FL,
			MODE_VSMAN_TA_NS,
				MODE_VSMAN_TA_NE,
					MODE_VSMAN_TA_CH,
						MODE_VSMAN_TA,
		MODE_PLAY4,
			MODE_PLAY4_TYPE1,
			MODE_PLAY4_TYPE2,
			MODE_PLAY4_TYPE3,
			MODE_PLAY4_TYPE4,
				MODE_PLAY4_CH,
					MODE_PLAY4_LV,
				MODE_PLAY4_TB_CH,
					MODE_PLAY4_TB_LV,
				MODE_PLAY4_FL_CH,
					MODE_PLAY4_FL_LV,
		MODE_OPTION,
			MODE_RECORD_MS,
				MODE_RECORD_PLAY1,
					MODE_RECORD_ST,
					MODE_RECORD_LS,
					MODE_RECORD_LS_TQ,
					MODE_RECORD_LS_TA,
					MODE_RECORD_VC,
					MODE_RECORD_VC_FL,
				MODE_RECORD_PLAY2,
					MODE_RECORD_VM,
					MODE_RECORD_VM_FL,
					MODE_RECORD_VM_TA,
			MODE_TUTORIAL,
				MODE_CONT,
			MODE_NAME_NS,
				MODE_NAME,
				MODE_NAME_NE,
					MODE_NAME_NE2,
					MODE_NAME_DEL_YN,
						MODE_NAME_DEL_MES,
			MODE_BACKUP,
				MODE_BACKUP_YN,
					MODE_BACKUP_MES,
			MODE_MISC,
				MODE_SOUND,
				MODE_COUNT,
				MODE_SCORE,
} MODE;

static MODE _getMode(void *global); // <- menuXXXX_draw で呼んじゃだめ
static MODE _getModeOld(void *global);
static void _setMode(void *global, MODE mode);
static void _setNextMain(void *global, int nextMain);
static void _setTitle(void *global, MODE mode);
static void _setFadeDir(void *global, int dir);

#define HI 0.65
#define LO 0.5
static const float _color_panelDark[][3] = {
	{ HI, LO, LO }, { LO, LO, HI },
	{ LO, HI, LO }, { HI, HI, LO }, { 0.5, 0.5, 0.5 },
};
#undef HI
#undef LO

// タイムアタックモードのウィルスレベル
static const u8 _timeAttack_levelTable[] = { 6, 9, 12 };

// サウンド
#include "sound.h"

// 各種設定
#include "evsworks.h"
#include "record.h"
#include "dm_game_main.h"

// メッセージ
#if LOCAL==JAPAN
#include "main_menu.mes"
#elif LOCAL==AMERICA
#include "main_menu_am.mes"
#elif LOCAL==CHINA
#include "main_menu_zh.mes"
#endif

#if (LOCAL == CHINA)
#include "record_zh.mes"
#else
#include "record.mes"
#endif

// ゲームレベル項目の色
static const u8 _gameLvItemColor[3][2][4] = {
	// easy
	{ {   0,   0,   0,  80 },   // 非選択時
	  { 128, 160, 255, 200 } }, // 　選択時
	// normal
	{ {   0,   0,   0,  80 },   // 非選択時
	  { 255, 255,  64, 200 } }, // 　選択時
	// hard
	{ {   0,   0,   0,  80 },   // 非選択時
	  { 255, 144,  64, 200 } }, // 　選択時
};

// BGM項目の色
static const u8 _musicItemColor[2][4] = {
	{   0,   0,   0,  80 }, // 非選択時
	{ 255, 160, 200, 200 }, // 　選択時
};

// キー
#define CONT_OK     DM_KEY_OK
#define CONT_CANCEL (B_BUTTON)
#define CONT_ANY    DM_ANY_KEY

//////////////////////////////////////////////////////////////////////////////
//{### 表示物

//////////////////////////////////////
//## タイトル

typedef struct {
	void *global;
	int current;
	int titleNo[2];
	SMenuItem miBase[2];
} SMenuTitle;

// タイトルを設定
static void menuTitle_setTitle(SMenuTitle *st, MODE mode)
{
	int titleNo = -1;

	switch(mode) {
	// モードセレクト
	case MODE_MAIN:
	case MODE_PLAY1:
	case MODE_PLAY2:

	case MODE_PLAY4:
	case MODE_PLAY4_TYPE1:
	case MODE_PLAY4_TYPE2:
	case MODE_PLAY4_TYPE3:
	case MODE_PLAY4_TYPE4:
		titleNo = 0;
		break;

	// オプション
	case MODE_OPTION:
	case MODE_TUTORIAL:
	case MODE_CONT:
	case MODE_NAME_NS:
	case MODE_NAME:
	case MODE_NAME_NE:
	case MODE_NAME_NE2:
	case MODE_NAME_DEL_YN:
	case MODE_MISC:
	case MODE_SCORE:
	case MODE_COUNT:
	case MODE_SOUND:
	case MODE_BACKUP:
	case MODE_BACKUP_YN:
		titleNo = 1;
		break;

	// タイムアタック
	case MODE_LVSEL_TA_NS:
	case MODE_LVSEL_TA_NE:
	case MODE_LVSEL_TA:

	case MODE_VSMAN_TA_NS:
	case MODE_VSMAN_TA_NE:
	case MODE_VSMAN_TA_CH:
	case MODE_VSMAN_TA:
		titleNo = 2;
		break;

	// タッグバトル
	case MODE_PLAY4_TB_CH:
	case MODE_PLAY4_TB_LV:
		titleNo = 3;
		break;

	// 記録
	case MODE_RECORD_MS:
	case MODE_RECORD_PLAY1:
	case MODE_RECORD_PLAY2:
	case MODE_RECORD_ST:
	case MODE_RECORD_LS:
	case MODE_RECORD_LS_TQ:
	case MODE_RECORD_LS_TA:
	case MODE_RECORD_VC:
	case MODE_RECORD_VC_FL:
	case MODE_RECORD_VM:
	case MODE_RECORD_VM_FL:
	case MODE_RECORD_VM_TA:
		titleNo = 4;
		break;

	// エンドレス
	case MODE_LVSEL_NS:
	case MODE_LVSEL_NE:
	case MODE_LVSEL:
		titleNo = 5;
		break;

	// お話
	case MODE_STORY_NS:
	case MODE_STORY_NE:
	case MODE_STORY:
		titleNo = 6;
		break;

	// VS 4P
	case MODE_PLAY4_CH:
	case MODE_PLAY4_LV:
		titleNo = 7;
		break;

	// VS COM
	case MODE_VSCOM_NS:
	case MODE_VSCOM_NE:
	case MODE_VSCOM_CH:
	case MODE_VSCOM:
		titleNo = 8;
		break;

	// VS 2P
	case MODE_VSMAN_NS:
	case MODE_VSMAN_NE:
	case MODE_VSMAN_CH:
	case MODE_VSMAN:
		titleNo = 9;
		break;

	// フラッシュ
	case MODE_VSCOM_FL_NS:
	case MODE_VSCOM_FL_NE:
	case MODE_VSCOM_FL_CH:
	case MODE_VSCOM_FL:

	case MODE_VSMAN_FL_NS:
	case MODE_VSMAN_FL_NE:
	case MODE_VSMAN_FL_CH:
	case MODE_VSMAN_FL:

	case MODE_PLAY4_FL_CH:
	case MODE_PLAY4_FL_LV:
		titleNo = 10;
		break;

	// 耐久
	case MODE_LVSEL_TQ_NS:
	case MODE_LVSEL_TQ_NE:
	case MODE_LVSEL_TQ:
		titleNo = 11;
		break;
	}

	if(st->titleNo[st->current] != titleNo) {
		st->current = (st->current + 1) % ARRAY_SIZE(st->miBase);
		st->miBase[st->current].colorTime = 0;
		st->titleNo[st->current] = titleNo;
	}
}

// 初期化
static void menuTitle_init(SMenuTitle *st, void *global, int x, int y)
{
	SMenuItem *item;
	int i;

	st->global = global;
	st->current = 0;
	st->titleNo[0] = -1;
	st->titleNo[1] = -1;

	// 下地
	for(i = 0; i < ARRAY_SIZE(st->miBase); i++) {
		item = &st->miBase[i];
		menuItem_init(item, x, y);
		menuItem_setColor_fade(item);
	}
}

// 更新
static void menuTitle_update(SMenuTitle *st, SMenuItem *parent)
{
	// 下地
	menuItem_updateN(st->miBase, ARRAY_SIZE(st->miBase), parent);
}

// 描画
static void menuTitle_draw(SMenuTitle *st, Gfx **gpp)
{
	Gfx *gp = *gpp;
	SMenuItem *item;
	STexInfo *texC, *texA;
	int i, j;

	gSPDisplayList(gp++, fade_normal_texture_init_dl);

	// 下地
	texC = _getTexCommon(st->global, TEX_menu_title);
	for(i = 0; i < ARRAY_SIZE(st->miBase); i++) {
		j = (st->current + i + 1) % ARRAY_SIZE(st->miBase);
		if(st->titleNo[j] < 0) continue;

		item = &st->miBase[j];

		if(item->color[3] == 1) {
			gDPSetRenderMode(gp++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
		}
		else {
			gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
		}

		menuItem_drawItem(&st->miBase[j], &gp, texC, false, 12, st->titleNo[j]);
	}

	*gpp = gp;
}

//////////////////////////////////////
//## カーソル

typedef enum {
	CURSOR_ITEM,
	CURSOR_PANEL,
	CURSOR_WINDOW,
	CURSOR_THICK,
	CURSOR_NEW_4P,
	CURSOR_NEW_MUSIC,
	CURSOR_NEW_SPEED,
	CURSOR_NEW_VIRUS_LV_L,
	CURSOR_NEW_VIRUS_LV_S,
	CURSOR_TYPE_SUM,
} CURSOR_TYPE;

typedef struct {
	void *global;
	CURSOR_TYPE type;
	int playerCount;
	int cpuCount;
	int playerNo;
	int size[2];
//	int frameCount;
	struct {
		unsigned cursor : 1;
		unsigned finger : 1;
		unsigned player : 1;
		unsigned blink  : 1;
	} flags;
	SMenuItem miBase[1];
	SMenuItem miCursor[1];
	SMenuItem miFinger[1];
	SMenuItem miPlayer[1];
} SMenuCursor;

static const int _menuCursor_cursor_4_pattern[] = {
	0,0,0,0,0,0, 1,1,1,1,1,1, 2,2,2,2,2,2, 1,1,1,1,1,1, 0,0,0,0,0,0,
};

// 指の位置を指定
static void menuCursor_setFingerPos(SMenuCursor *st, int x, int y)
{
	SMenuItem *item = st->miFinger;
	item->transFunc = transFunc_finger;
	item->transStep = BLINK_SPEED;
	menuItem_setTransHi(item, x, y);
	menuItem_setTransLow(item, x - 8, y);
}

// プレイヤー番号の位置を指定
static void menuCursor_setPlayerPos(SMenuCursor *st, int x, int y)
{
	SMenuItem *item = st->miPlayer;
	menuItem_setTransHi(item, x, y);
	menuItem_setTransLow(item, x, y);
}

// 初期化
static void menuCursor_init2(SMenuCursor *st, void *global,
	CURSOR_TYPE type, int playerCount, int cpuCount, int playerNo, int x, int y, int w, int h)
{
	static const float _color[][3] = {
		{ 1.0, 0.3, 0.3 }, // 赤
		{ 0.3, 0.3, 1.0 }, // 青
		{ 1.0, 1.0, 0.0 }, // 黄
		{ 0.0, 1.0, 0.0 }, // 緑
	};
	SMenuItem *item;
	int i;

	switch(type) {
	case CURSOR_ITEM:
	case CURSOR_PANEL:
		type = 3;
		break;
	}

	st->global = global;
	st->type = type;
	st->playerCount = playerCount;
	st->cpuCount = cpuCount;
	st->playerNo = playerNo;
	st->size[0] = w;
	st->size[1] = h;
//	st->frameCount = 0;

	st->flags.cursor = 1;
	st->flags.finger = 0;
	st->flags.player = 0;
	st->flags.blink = 1;

	// 下地
	menuItem_init(st->miBase, x, y);

	// カーソル
	menuItem_init(st->miCursor, 0, 0);
	menuItem_setColor_cursor2(st->miCursor,
		_color[playerNo & 3][0], _color[playerNo & 3][1], _color[playerNo & 3][2]);

	if(type == CURSOR_THICK) {
//		st->miCursor->colorFunc = colorFunc_curve;
		item = st->miCursor;
		for(i = 0; i < ARRAY_SIZE(item->colorRange[0]); i++) {
			item->colorRange[0][i] = item->colorRange[1][i];
		}
	}

	// 指
	menuItem_init(st->miFinger, 0, 0);
	menuCursor_setFingerPos(st, -20, h - 14);

	// プレイヤー番号
	menuItem_init(st->miPlayer, 0, 0);
	menuCursor_setPlayerPos(st, -16, h - 22);
}
static void menuCursor_init(SMenuCursor *st, void *global,
	CURSOR_TYPE type, int playerNo, int x, int y, int w, int h)
{
	menuCursor_init2(st, global, type, 4, 0, playerNo, x, y, w, h);
}

// 更新
static void menuCursor_update(SMenuCursor *st, SMenuItem *parent)
{
	SMenuItem *item;
	int i;

	// 下地
	menuItem_update(st->miBase, parent);

	// カーソル
	menuItem_update(st->miCursor, st->miBase);

	if(!st->flags.blink) {
		item = st->miCursor;
		for(i = 0; i < ARRAY_SIZE(item->color); i++) {
			item->color[i] = item->colorRange[1][i] * parent->color[i];
		}
	}

	// 指
	menuItem_update(st->miFinger, st->miBase);

	// プレイヤー番号
	menuItem_update(st->miPlayer, st->miBase);

	// フレームカウント
//	st->frameCount = WrapI(0, ARRAY_SIZE(_menuCursor_cursor_4_pattern), st->frameCount + 1);
}

// 描画(枠)
static void menuCursor_draw1(SMenuCursor **stAry, int count, Gfx **gpp)
{
	static const int _pnts[][8] = {
		{ 0,0,1,1,0,0,2,2 }, { 1,0,2,1,2,0,1,2 }, { 2,0,3,1,1,0,0,2 },
		{ 0,1,1,2,0,2,2,1 }, { 1,1,2,2,2,2,1,1 }, { 2,1,3,2,1,2,0,1 },
		{ 0,2,1,3,0,1,2,0 }, { 1,2,2,3,2,1,1,0 }, { 2,2,3,3,1,1,0,0 },
	};

	Gfx *gp = *gpp;
	SMenuCursor *st;
	SMenuItem *item;
	STexInfo *texC, *texA;
	int i, j, k, cached;

	gSPDisplayList(gp++, fade_normal_texture_init_dl);
	gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
	gDPSetTextureLUT(gp++, G_TT_NONE);

	// カーソルを描画
	for(i = 0; i < CURSOR_TYPE_SUM; i++) {
		int kado, pattern;

		// テクスチャをロード
		switch(i) {
		// 選択項目用
		case CURSOR_ITEM:
			k = TEX_item_cursor;
			kado = 4;
			break;

		// パネル用
		case CURSOR_PANEL:
			k = TEX_constituent_cursor;
			kado = 3;
			break;

		// ウィンドウ用
		case CURSOR_WINDOW:
			k = TEX_constituent_cursor2;
			kado = 16;
			break;

		// 太め
		case CURSOR_THICK: {
			kado = 16;
			pattern = -1;
			} break;

		case CURSOR_NEW_4P:
			k = TEX_new_cursor_4p;
			break;

		case CURSOR_NEW_MUSIC:
			k = TEX_new_cursor_music;
			break;

		case CURSOR_NEW_SPEED:
			k = TEX_new_cursor_speed;
			break;

		case CURSOR_NEW_VIRUS_LV_L:
			k = TEX_new_cursor_viruslv_l;
			break;

		case CURSOR_NEW_VIRUS_LV_S:
			k = TEX_new_cursor_viruslv_s;
			break;
		}
		cached = 0;

		for(j = 0; j < count; j++) {
			int px[4], py[4], tx[3], ty[3], dx[3], size;

			st = stAry[j];
			item = st->miCursor;

			if(!st->flags.cursor || i != st->type ||
				menuItem_outOfScreen(item, st->size[0], st->size[1]))
			{
				continue;
			}

			if(i == CURSOR_THICK) {
				if(st->flags.blink) {
//					k += _menuCursor_cursor_4_pattern[st->frameCount];
					k = item->colorTime * (ARRAY_SIZE(_menuCursor_cursor_4_pattern) - 1);
					k = TEX_cursor_low + _menuCursor_cursor_4_pattern[k];
				}
				else {
					k = TEX_cursor_low;
				}

				if(k != pattern) {
					pattern = k;
					cached = 0;
				}
			}

			if(!cached) {
				texC = _getTexCommon(st->global, k);
				gDPLoadTextureBlock_4b(gp++, texC->addr[1], G_IM_FMT_I,
					texC->size[0], texC->size[1], 0,
					G_TX_CLAMP, G_TX_CLAMP,
					G_TX_NOMASK, G_TX_NOMASK,
					G_TX_NOLOD, G_TX_NOLOD);
				cached++;
			}

			menuItem_setPrim(item, &gp);

			if(i == CURSOR_THICK) {
				size = st->size[0] + kado;
				px[0] = (int)(item->trans[0] * 4 - kado * 2);
			}
			else {
				size = st->size[0];
				px[0] = (int)(item->trans[0] * 4);
			}

			k = MIN(kado, size / 2);

			px[3] = px[0] + (int)(item->scale[0] * 4 * size);
			px[1] = px[0] + (int)(item->scale[0] * 4 * k);
			px[2] = px[3] - (int)(item->scale[0] * 4 * k);

			tx[0] = 32 * 0;
			tx[1] = 32 * (k - 1);
			tx[2] = 32 * k;

			if(i == CURSOR_THICK) {
				size = st->size[1] + kado;
				py[0] = (int)(item->trans[1] * 4 - kado * 2);
			}
			else {
				size = st->size[1];
				py[0] = (int)(item->trans[1] * 4);
			}

			k = MIN(kado, size / 2);

			py[3] = py[0] + (int)(item->scale[1] * 4 * size);
			py[1] = py[0] + (int)(item->scale[1] * 4 * k);
			py[2] = py[3] - (int)(item->scale[1] * 4 * k);

			ty[0] = 32 * 0;
			ty[1] = 32 * (k - 1);
			ty[2] = 32 * k;

			dx[0] = -1 * 1024;
			dx[1] =  0 * 1024;
			dx[2] =  1 * 1024;

			if(i < CURSOR_NEW_4P) {
				for(k = 0; k < ARRAY_SIZE(_pnts); k++) {
					int *pnts = _pnts[k];

					gSPScisTextureRectangle(gp++,
						px[pnts[0]], py[pnts[1]],
						px[pnts[2]], py[pnts[3]], G_TX_RENDERTILE,
						tx[pnts[4]], ty[pnts[5]],
						dx[pnts[6]], dx[pnts[7]]);
				}
			}
			else {
				int x0, y0, x1, y1;
				x0 = item->trans[0];
				y0 = item->trans[1];
				x1 = x0 + texC->size[0];
				y1 = y0 + texC->size[1];
				gSPScisTextureRectangle(gp++,
					x0 << 2, y0 << 2, x1 << 2, y1 << 2, G_TX_RENDERTILE,
					0 << 5, 0 << 5, 1 << 10, 1 << 10);
			}
		}
	}

	*gpp = gp;
}

// 描画(指と番号)
static void menuCursor_draw2(SMenuCursor **stAry, int count, Gfx **gpp)
{
	static const int _type[][4] = {
		{ 0, 4, 7, 9 },
		{ 0, 1, 5, 8 },
		{ 0, 1, 2, 6 },
		{ 0, 1, 2, 3 },
	};

	Gfx *gp = *gpp;
	SMenuCursor *st;
	SMenuItem *item;
	STexInfo *texC, *texA;
	int i, j, k, cached;

	gSPDisplayList(gp++, fade_alpha_texture_init_dl);

	// 指
	for(i = cached = 0; i < count; i++) {
		st = stAry[i];
		if(!st->flags.finger) continue;
		texC = _getTexCommon(st->global, TEX_finger);
		texA = _getTexCommon(st->global, TEX_finger_alpha);
		cached += menuItem_drawAlphaTex(st->miFinger, &gp, texC, texA, cached);
	}

	// プレイヤー番号
	for(i = 0; cached = 0, i < 11; i++) {
	for(j = 0; j < count; j++) {
		st = stAry[j];

		if(st->playerNo == 1 && st->playerCount == 1 && st->cpuCount == 1) {
			k = 10;
		}
		else {
			k = _type[st->playerCount - 1][st->playerNo];
		}

		if(!st->flags.player || i != k) continue;

		texC = _getTexSetup(st->global, TEX_finger_sub);
		texA = _getTexSetup(st->global, TEX_finger_sub_alpha);

		cached += menuItem_drawAlphaItem(st->miPlayer, &gp, texC, texA, cached, 11, i);
	}}

	*gpp = gp;
}

// 描画
static void menuCursor_draw(SMenuCursor **stAry, int count, Gfx **gpp)
{
	menuCursor_draw1(stAry, count, gpp);
	menuCursor_draw2(stAry, count, gpp);
}

//////////////////////////////////////
//## ペットボトル

typedef struct {
	void *global;
	int level;
	SMenuItem miBase[1];
	SMenuItem miVirus[1];
} SMenuBottle;

// 初期化
static void menuBottle_init(SMenuBottle *st, void *global, int x, int y)
{
	st->global = global;
	st->level = 0;

	// 下地
	menuItem_init(st->miBase, x, y);

	// ウィルス
	menuItem_init(st->miVirus, 4, 16);
}

// 更新
static void menuBottle_update(SMenuBottle *st, SMenuItem *parent)
{
	menuItem_update(st->miBase, parent);
	menuItem_update(st->miVirus, st->miBase);
}

// 描画
static void menuBottle_draw(SMenuBottle *st, Gfx **gpp)
{
	#include "cap_pos.dat"
	Gfx *gp = *gpp;
	STexInfo *tex;
	int visible;

	gSPDisplayList(gp++, fade_normal_texture_init_dl);

	// ボトルを描画
//	tex = _getTexLevel(st->global, TEX_bottle);
//	visible = menuItem_drawTex(st->miBase, &gp, tex, false);

	// ウィルスを描画
	if(visible) {
		SMenuItem *item;
		u16 *caps;
		int c, x, y, px, py, ty, cap, color;

		item = st->miVirus;
		tex = _getTexLevel(st->global, TEX_bottle_virus);
		gDPLoadTLUT_pal16(gp++, 0, tex->addr[0]);
		gDPLoadTextureBlock_4b(gp++, tex->addr[1], G_IM_FMT_CI, tex->size[0], tex->size[1], 0,
			G_TX_CLAMP,G_TX_CLAMP, G_TX_NOMASK,G_TX_NOMASK, G_TX_NOLOD,G_TX_NOLOD);

		menuItem_setPrim(item, &gp);

		ty = 0;
		for(c = 1; c < 4; c++) {
			caps = (u16 *)&cap_pos[st->level][2];
			py = (int)item->trans[1] << 2;
			for(y = 2; y < 13; y++) {
				cap = *caps;
				px = (int)item->trans[0] << 2;
				for(x = 7; x >= 0; x--) {
					color = (cap >> (x + x)) & 0x3;
					if(color == c) {
						gSPScisTextureRectangle(gp++,
							px, py, px + (5 << 2), py + (5 << 2),
							G_TX_RENDERTILE, 0, ty, 1 << 10, 1 << 10);
					}
					px += 5 << 2;
				}
				py += 5 << 2;
				caps++;
			}
			ty += 5 << 5;
		}
	}

	*gpp = gp;
}

//////////////////////////////////////
//## レベルゲージ

typedef struct SMenuLvGauge {
	void *global;
	int size;
	int levelMax;
	int level;
	SMenuItem miBase[1];
	SMenuCursor cursor[1];
} SMenuLvGauge;

static const float _lvGauge_step[] = { 3.5, 3.5, 4.0 };

// 初期化
static void menuLvGauge_init(SMenuLvGauge *st, void *global,
	int size, int playerNo, int level, int x, int y)
{
	static const int _size[][2] = {
		{ 6, 10 }, { 6, 16 }, { 7, 16 },
	};
	static const CURSOR_TYPE _type[] = {
		CURSOR_NEW_VIRUS_LV_S,
		CURSOR_NEW_VIRUS_LV_L,
		CURSOR_NEW_VIRUS_LV_L,
	};

	st->global = global;
	st->size = size;
	st->levelMax = 20;
	st->level = level;

	// 下地
	menuItem_init(st->miBase, x, y);

	// カーソル
//	menuCursor_init(st->cursor, global, CURSOR_ITEM, playerNo,
//		-2, -2, _size[size][0], _size[size][1]);
	menuCursor_init(st->cursor, global, _type[size], playerNo,
		-2, -2, _size[size][0], _size[size][1]);
}

// 入力
static void menuLvGauge_input(SMenuLvGauge *st, int playerNo)
{
	int rep = _getKeyRep(st->global, playerNo);
	int vec = 0;
	int level;

	if(rep & L_JPAD) vec--;
	if(rep & R_JPAD) vec++;
	level = CLAMP(0, st->levelMax, st->level + vec);

	// サウンドを再生
	if(level != st->level) {
		dm_snd_play(SE_mLeftRight);
		st->level = level;
	}
}

// 更新
static void menuLvGauge_update(SMenuLvGauge *st, SMenuItem *parent)
{
	SMenuItem *item;

	// 下地
	menuItem_update(st->miBase, parent);

	// カーソル
	item = st->cursor->miBase;
	item->transRange[1][0] = item->transRange[0][0] + _lvGauge_step[st->size] * st->level;
	menuCursor_update(st->cursor, st->miBase);
}

// 描画
static void menuLvGauge_draw1(SMenuLvGauge **stAry, int count, Gfx **gpp)
{
	Gfx *gp = *gpp;
	SMenuLvGauge *st;
	SMenuItem *item;
	int i;

	gSPDisplayList(gp++, fade_fillrect_init_dl);

	// フィルター
	for(i = 0; i < count; i++) {
		int x, y, w, h;
		st = stAry[i];
		item = st->cursor->miBase;

		x = st->miBase->trans[0] + _lvGauge_step[st->size] * (st->level + 1);
		y = st->miBase->trans[1];
		w = (20 - st->level) * _lvGauge_step[st->size];
		h = st->cursor->size[1] - 3;

		if(st->size == 0) { y--; h++; }

		gDPSetPrimColor(gp++, 0,0, 0,0,0,160);
		gDPScisFillRectangle(gp++, x, y, x+w, y+h);

		if(st->levelMax > 20) {
			x = st->miBase->trans[0] + _lvGauge_step[st->size] * 21;
			y = st->miBase->trans[1] - 1;
			w = (st->levelMax - MAX(20, st->level)) * _lvGauge_step[st->size];
			h = st->cursor->size[1] - 1;

			gDPScisFillRectangle(gp++, x, y, x+w, y+h);
		}
	}

	*gpp = gp;
}

// 描画
static void menuLvGauge_draw2(SMenuLvGauge **stAry, int count, Gfx **gpp)
{
	Gfx *gp = *gpp;
	SMenuLvGauge *st;
	void *list[count];
	int i;

	// カーソル
	for(i = 0; i < count; i++) {
		st = stAry[i];
		list[i] = st->cursor;
	}
	menuCursor_draw((SMenuCursor **)list, count, &gp);

	*gpp = gp;
}

// 描画
static void menuLvGauge_draw(SMenuLvGauge **stAry, int count, Gfx **gpp)
{
	menuLvGauge_draw1(stAry, count, gpp);
	menuLvGauge_draw2(stAry, count, gpp);
}

//////////////////////////////////////////////////////////////////////////////
//{### 選択項目

//////////////////////////////////////
//## はい, いいえ

typedef struct {
	void *global;
	int select;
	SMenuItem miBase[1];
	SMenuItem miYN[2];
	SMenuCursor cursor[1];
	SMenuCursor window[1];
} SMenuYN;

// フェードインアウト
static void menuYN_setFade(SMenuYN *st, int dir, float time)
{
	SMenuItem *item = st->miBase;
	menuItem_setColorDir(item, dir);
	item->colorTime = time;
}

static void menuYN_setFadeDir(SMenuYN *st, int dir)
{
	menuYN_setFade(st, dir, st->miBase->colorTime);
}

// 初期化
static void menuYN_init(SMenuYN *st, void *global, int x, int y)
{
	static const int _yn[][2] = {
		{ 14, 12 }, { 14, 36 },
	};
	SMenuItem *item;
	int i;

	st->global = global;
	st->select = 0;

	// 下地
	menuItem_init(st->miBase, x, y);
	menuItem_setColor_fade(st->miBase);

	// YN
	for(i = 0; i < ARRAY_SIZE(_yn); i++) {
		menuItem_init(&st->miYN[i], _yn[i][0], _yn[i][1]);
	}

	// カーソル
	menuCursor_init(st->cursor, global, CURSOR_ITEM, 0, -4, -4, 60, 20);
	st->cursor->flags.finger = 1;

	// ウィンドウ
	menuCursor_init(st->window, global, CURSOR_WINDOW, 0, 0, 0, 80, 60);
	item = st->window->miCursor;
	item->colorFunc = colorFunc_curve;
	item->colorRange[0][0] = item->colorRange[1][0] = 0;
	item->colorRange[0][1] = item->colorRange[1][1] = 0;
	item->colorRange[0][2] = item->colorRange[1][2] = 0;
	item->colorRange[0][3] = 0;
	item->colorRange[1][3] = WINDOW_ALPHA / 255.0;

	// フェードアウト
	menuYN_setFade(st, -1, 0);
}

// 入力
static bool menuYN_input(SMenuYN *st, int playerNo)
{
	int trg = _getKeyTrg(st->global, playerNo);
	int sel = st->select;

	if(st->miBase->colorStep < 0 || st->miBase->colorTime < 1) {
		return false;
	}

	if(trg & U_JPAD) sel--;
	if(trg & D_JPAD) sel++;
	sel = WrapI(0, 2, sel);

	if(sel != st->select) {
		st->select = sel;
		dm_snd_play(SE_mUpDown);
	}

	return true;
}

// 更新
static void menuYN_update(SMenuYN *st, SMenuItem *parent)
{
	// 下地
	menuItem_update(st->miBase, parent);

	// YN
	menuItem_updateN(st->miYN, ARRAY_SIZE(st->miYN), st->miBase);

	// カーソル
	menuCursor_update(st->cursor, &st->miYN[st->select]);

	// ウィンドウ
	menuCursor_update(st->window, st->miBase);
}

// 描画
static void menuYN_draw(SMenuYN *st, Gfx **gpp)
{
	static char *_yn[] = {
#if LOCAL==JAPAN
		"\x82\xa2\x82\xa2\x82\xa6", // "いいえ",
		"\x82\xcd\x81\x40\x82\xa2", // "は　い",
#elif LOCAL==AMERICA
		"\x82\x6d\x82\x8f",         // "Ｎｏ",
//		"\x82\x6d\x81\x40\x82\x8f", // "Ｎ　ｏ",
		"\x82\x78\x82\x85\x82\x93", // "Ｙｅｓ",
#elif LOCAL==CHINA
		"\xc8\xa1\xcf\xfb", // "Ｙｅｓ",
//		"\x82\x6d\x81\x40\x82\x8f", // "Ｎ　ｏ",
		"\xc8\xb7\xb6\xa8",         // "Ｎｏ",
#endif
	};

	Gfx *gp = *gpp;
	SMenuItem *item = st->miBase;
	void *list[1];
	float x;
	int i, j;

	if(menuItem_outOfScreen(item, 80, 60)) {
		return;
	}

	// 下地
	list[0] = st->window;
	menuCursor_draw((SMenuCursor **)list, 1, &gp);

	// 文字
	gSPDisplayList(gp++, fade_intensity_texture_init_dl);
	gDPSetTextureFilter(gp++, G_TF_BILERP);
	gDPSetPrimColor(gp++, 0,0, 255,255,255, item->color[3] * 255);

	for(i = 0; i < ARRAY_SIZE(st->miYN); i++) {
		item = &st->miYN[i];
		x = item->trans[0];
		for(j = 0; _yn[i][j] != '\0'; j += 2) {
			fontXX_draw(&gp, x, item->trans[1], FONT_WORD_W, FONT_WORD_H, &_yn[i][j]);
			x += 20;
		}
	}

	// カーソル
	list[0] = st->cursor;
	menuCursor_draw((SMenuCursor **)list, 1, &gp);

	*gpp = gp;
}

//////////////////////////////////////
//## 注意文

typedef struct {
	void *global;
	SMenuItem miBase[1];
	SMsgWnd msgWnd[1];
	SMenuCursor window[1];
} SMenuMes;

// フェードインアウト
static void menuMes_setFade(SMenuMes *st, int dir, float time)
{
	menuItem_setColorDir(st->miBase, dir);
	st->miBase->colorTime = time;
}

static void menuMes_setFadeDir(SMenuMes *st, int dir)
{
	menuMes_setFade(st, dir, st->miBase->colorTime);
}

// 初期化
static void menuMes_init(SMenuMes *st, void *global, void **hpp,
	int col, int row, int x, int y)
{
	SMenuItem *item;
	void *hp = *hpp;

	st->global = global;

	// 下地
	menuItem_init(st->miBase, x, y);
	menuItem_setColor_fade(st->miBase);

	// メッセージ
	msgWnd_init(st->msgWnd, hpp, col, row, x, y);
#if LOCAL==AMERICA
	st->msgWnd->centering = 1;
#elif LOCAL==CHINA
	st->msgWnd->centering = 1;
#endif
	st->msgWnd->fntW = st->msgWnd->fntH = 12;
	st->msgWnd->colStep = 6;
	st->msgWnd->rowStep = 14;

	// ウィンドウ
	menuCursor_init(st->window, global, CURSOR_WINDOW, 0, -16, -16,
		st->msgWnd->colStep * col * 2 + 32,
		st->msgWnd->rowStep * row     + 32);
//	w = (wnd->colSize - 2) * wnd->colStep + wnd->fntW + 12;
//	h = (wnd->rowSize - 1) * wnd->rowStep + wnd->fntH + 12;

	item = st->window->miCursor;
	item->colorFunc = colorFunc_curve;
	item->colorRange[0][0] = item->colorRange[1][0] = 0;
	item->colorRange[0][1] = item->colorRange[1][1] = 0;
	item->colorRange[0][2] = item->colorRange[1][2] = 0;
	item->colorRange[0][3] = 0;
	item->colorRange[1][3] = WINDOW_ALPHA / 255.0;

	// フェードアウト
	menuMes_setFade(st, -1, 0);

	*hpp = hp;
}

// 文字列を設定
static void menuMes_setStr(SMenuMes *st, const char *str)
{
	msgWnd_clear(st->msgWnd);
	msgWnd_addStr(st->msgWnd, str);
	msgWnd_skip(st->msgWnd);
}

// 入力
//static bool menuMes_input(SMenuMes *st, int playerNo)
//{
//	SMenuItem *item = st->miBase;
//	return item->colorStep > 0 && item->colorTime == 1;
//}

// 更新
static void menuMes_update(SMenuMes *st, SMenuItem *parent)
{
	// 下地
	menuItem_update(st->miBase, parent);

	// メッセージ
	st->msgWnd->posX = st->miBase->trans[0];
	st->msgWnd->posY = st->miBase->trans[1];
	st->msgWnd->alpha = st->miBase->color[3] * 255;
	msgWnd_update(st->msgWnd);

	// ウィンドウ
	menuCursor_update(st->window, st->miBase);
}

// 描画
static void menuMes_draw(SMenuMes *st, Gfx **gpp)
{
	Gfx *gp = *gpp;
	SMenuItem *item = st->miBase;
	SMsgWnd *wnd = st->msgWnd;
	void *list[1];
	int x, y, w, h;

	x = item->trans[0] - 6;
	y = item->trans[1] - 6;
	w = (wnd->colSize - 2) * wnd->colStep + wnd->fntW + 12;
	h = (wnd->rowSize - 1) * wnd->rowStep + wnd->fntH + 12;

	if(menuItem_outOfScreen(item, w, h)) {
		return;
	}

	// 下地
	list[0] = st->window;
	menuCursor_draw((SMenuCursor **)list, 1, &gp);

	// メッセージ
	msgWnd_draw(wnd, &gp);

	*gpp = gp;
}

//////////////////////////////////////
//## [LOW] [MED] [HI] の確認

typedef struct {
	void *global;
	int size;
	int select;
	SMenuItem miBase[1];
} SMenuSpeedAsk;

// 初期化
static void menuSpeedAsk_init(SMenuSpeedAsk *st, void *global,
	int size, int speed, int x, int y)
{
	st->global = global;
	st->size = size;
	st->select = speed;

	// 下地
	menuItem_init(st->miBase, x, y);
}

// 更新
static void menuSpeedAsk_update(SMenuSpeedAsk *st, SMenuItem *parent)
{
	// 下地
	menuItem_update(st->miBase, parent);
}

// 描画
static void menuSpeedAsk_draw(SMenuSpeedAsk **stAry, int count, Gfx **gpp)
{
	static const int _tex[][2] = {
		{ TEX_speed_answer_b, TEX_speed_answer_b_alpha },
		{ TEX_speed_answer_a, TEX_speed_answer_a_alpha },
		{ TEX_gamelv_answer_b, TEX_gamelv_answer_b_alpha },
		{ TEX_gamelv_answer, TEX_gamelv_answer_alpha },
	};
	static const int _row[] = { 3, 3, 3, 4 };
	Gfx *gp = *gpp;
	SMenuSpeedAsk *st;
	STexInfo *texC, *texA;
	int i, j, k, cached;

	gSPDisplayList(gp++, fade_alpha_texture_init_dl);

	for(i = 0; i < 4; i++) {
	for(j = 0; cached = 0, j < _row[i]; j++) {
	for(k = 0; k < count; k++) {
		st = stAry[k];
		if(i != st->size || j != st->select) continue;
		texC = _getTexSetup(st->global, _tex[st->size][0]);
		texA = _getTexSetup(st->global, _tex[st->size][1]);
		cached += menuItem_drawAlphaItem(st->miBase, &gp, texC, texA, cached, _row[i], j);
	}}}

	*gpp = gp;
}

//////////////////////////////////////
//## [LOW] [MED] [HI] の選択

typedef struct {
	void *global;
	int size;
	int select;
	struct {
		unsigned blink;
		unsigned special;
	} flags;
	SMenuItem miBase[1];
	SMenuItem miLabel[3];
	SMenuItem miMask[3];
	SMenuCursor cursor[1];
} SMenuSpeedItem;

// 初期化
static void menuSpeedItem_init(SMenuSpeedItem *st, void *global,
	int size, int playerNo, int speed, int x, int y, int step)
{
	static const int _size[][2] = { { 36, 16 }, { 36, 24 }, { 36, 16 }, { 36, 24 } };
	static CURSOR_TYPE _type[] = { CURSOR_NEW_MUSIC, CURSOR_NEW_SPEED, CURSOR_NEW_MUSIC, CURSOR_NEW_SPEED };
	SMenuItem *item;
	int i, j, k;

	st->global = global;
	st->size = size;
	st->select = speed;
	st->flags.blink = 1;
	st->flags.special = 0;

	// 下地
	menuItem_init(st->miBase, x, y);

	// ラベル
	for(i = 0; i < ARRAY_SIZE(st->miLabel); i++) {
		menuItem_init(&st->miLabel[i], step * i, 0);
	}

	// マスク
	for(i = 0; i < ARRAY_SIZE(st->miMask); i++) {
		static const s8 _pos[][2] = { { -3,-2 }, { -3,-3 }, { -3,-2 }, { -3,-3 } };
		item = &st->miMask[i];
		menuItem_init(item, _pos[size][0], _pos[size][1]);
		item->colorFunc = colorFunc_cursor;
		item->colorStep = BLINK_SPEED;
		for(j = 0; j < ARRAY_SIZE(item->colorRange); j++) {
			for(k = 0; k < ARRAY_SIZE(item->colorRange[j]); k++) {
				item->colorRange[j][k] = _gameLvItemColor[i][j][k] * (1.0 / 255.0);
			}
		}
	}

	// カーソル
//	menuCursor_init(st->cursor, global, CURSOR_ITEM, playerNo,
//		0, 0, _size[size][0], _size[size][1]);
	menuCursor_init(st->cursor, global, _type[size], playerNo,
		-1, -1, _size[size][0], _size[size][1]);
}

// 入力
static void menuSpeedItem_input(SMenuSpeedItem *st, int playerNo)
{
	int rep = _getKeyRep(st->global, playerNo);
	int vec = 0;
	int select;

	if(rep & L_JPAD) vec--;
	if(rep & R_JPAD) vec++;
	select = CLAMP(0, ARRAY_SIZE(st->miLabel) - 1, st->select + vec);

	// サウンドを再生
	if(select != st->select) {
		dm_snd_play(SE_mLeftRight);
		st->select = select;
	}
}

// 更新
static void menuSpeedItem_update(SMenuSpeedItem *st, SMenuItem *parent)
{
	SMenuItem *item;
	float *color;
	int i, j;

	// 下地
	menuItem_update(st->miBase, parent);

	// ラベル
	menuItem_updateN(st->miLabel, ARRAY_SIZE(st->miLabel), st->miBase);

	// マスク
	for(i = 0; i < ARRAY_SIZE(st->miMask); i++) {
		item = &st->miMask[i];

		menuItem_update(&st->miMask[i], &st->miLabel[i]);

		if(i == st->select) {
			if(!st->flags.blink) {
				color = item->colorRange[1];
			}
			else {
				color = NULL;
			}

			for(j = 0; j < ARRAY_SIZE(item->colorRange[0]); j++) {
				item->colorRange[0][j] = _gameLvItemColor[i][1][j] * (1.0 / 511.0);
			}
		}
		else {
			color = item->colorRange[0];

			for(j = 0; j < ARRAY_SIZE(item->colorRange[0]); j++) {
				item->colorRange[0][j] = _gameLvItemColor[i][0][j] * (1.0 / 255.0);
			}
		}

		if(color != NULL) {
			for(j = 0; j < ARRAY_SIZE(item->color); j++) {
				item->color[j] = color[j] * parent->color[j];
			}
		}
	}

	// カーソル
	menuCursor_update(st->cursor, &st->miLabel[st->select]);
}

// 描画1
static void menuSpeedItem_draw1(SMenuSpeedItem **stAry, int count, Gfx **gpp)
{
	Gfx *gp = *gpp;
	SMenuSpeedItem *st;
	SMenuItem *item;
	STexInfo *texC;
	int i, j, k, cached;

	gSPDisplayList(gp++, fade_normal_texture_init_dl);

	for(j = cached = 0; j < count; j++) {
		st = stAry[j];
		item = &st->miLabel[2];

		if(st->size != 3 || !st->flags.special) {
			continue;
		}

		texC = _getTexSetup(st->global, TEX_ohanashi_panel_shard);

		cached += menuItem_drawTex(item, &gp, texC, cached);
	}

	gSPDisplayList(gp++, fade_intensity_texture_init_dl);

	for(j = cached = 0; j < count; j++) {
		st = stAry[j];
		item = &st->miMask[2];

		if(st->size != 3 || !st->flags.special) {
			continue;
		}

		texC = _getTexSetup(st->global, TEX_new_cursor_gamelv_shard);

		cached += menuItem_drawTex(item, &gp, texC, cached);
	}

	// マスク
	for(k = 0; k < 4; k++) {
		for(i = 0; i < ARRAY_SIZE(st->miMask); i++) {
			cached = 0;

			if(i == 2 && st->size == 3 && st->flags.special) {
				continue;
			}

			for(j = 0; j < count; j++) {
				st = stAry[j];
				item = &st->miMask[i];

				if(st->size != k) {
					continue;
				}

				switch(k) {
				case 0:
					texC = _getTexP4(st->global, TEX_new_cursor_speed_4p_low + i);
					break;
				case 1:
					texC = _getTexSetup(st->global, TEX_new_cursor_speed_low + i);
					break;
				case 2:
					texC = _getTexP4(st->global, TEX_new_cursor_gamelv_4p_easy + i);
					break;
				case 3:
					texC = _getTexSetup(st->global, TEX_new_cursor_gamelv_easy + i);
					break;
				}

				cached += menuItem_drawTex(item, &gp, texC, cached);
			}
		}
	}

	*gpp = gp;
}

// 描画2
static void menuSpeedItem_draw2(SMenuSpeedItem **stAry, int count, Gfx **gpp)
{
	Gfx *gp = *gpp;
	SMenuSpeedItem *st;
	void *list[count];
	int i;

	// カーソル
	for(i = 0; i < count; i++) {
		st = stAry[i];
		list[i] = st->cursor;
	}
	menuCursor_draw2((SMenuCursor **)list, count, &gp);

	*gpp = gp;
}

// 描画
static void menuSpeedItem_draw(SMenuSpeedItem **stAry, int count, Gfx **gpp)
{
	menuSpeedItem_draw1(stAry, count, gpp);
	menuSpeedItem_draw2(stAry, count, gpp);
}

//////////////////////////////////////
//## [FEVER] [CHILL] [DIZZY] の選択

typedef struct {
	void *global;
	int select;
	int playNo;
	struct {
		unsigned blink : 1;
	} flags;
	SMenuItem miBase[1];
	SMenuItem miLabel[5];
	SMenuItem miMask[5];
	SMenuCursor cursor[1];
} SMenuMusicItem;

// 初期化
static void menuMusicItem_init(SMenuMusicItem *st, void *global, int musicNo, int x, int y)
{
	SMenuItem *item;
	int i, j, k;

	st->global = global;
	st->select = musicNo;
	st->playNo = -1;
	st->flags.blink = 1;

	// 下地
	menuItem_init(st->miBase, x, y);

	// ラベル
	for(i = 0; i < ARRAY_SIZE(st->miLabel); i++) {
		menuItem_init(&st->miLabel[i], 39 * i, 0);
	}

	// マスク
	for(i = 0; i < ARRAY_SIZE(st->miMask); i++) {
		item = &st->miMask[i];
		menuItem_init(&st->miMask[i], -2, -2);
		item->colorFunc = colorFunc_cursor;
		item->colorStep = BLINK_SPEED;
		for(j = 0; j < ARRAY_SIZE(item->colorRange); j++) {
			for(k = 0; k < ARRAY_SIZE(item->colorRange[j]); k++) {
				item->colorRange[j][k] = _musicItemColor[j][k] * (1.0 / 255.0);
			}
		}
	}

	// カーソル
//	menuCursor_init(st->cursor, global, CURSOR_ITEM, 0, 0, 0, 37, 16);
	menuCursor_init(st->cursor, global, CURSOR_NEW_MUSIC, 0, -1, -1, 37, 16);
	st->cursor->flags.cursor = 0;
}

// 入力
static void menuMusicItem_input(SMenuMusicItem *st, int playerNo)
{
	int rep = _getKeyRep(st->global, playerNo);
	int vec = 0;
	int select;

	if(st->playNo != st->select) {
		static const int _seq[] = {
			SEQ_Fever, SEQ_Chill, SEQ_Game_C, SEQ_Game_D,
		};

		if(st->select < ARRAY_SIZE(_seq)) {
			dm_seq_play_fade( _seq[ st->select ], BGM_FADE_FRAME );
		}
		else {
			dm_seq_stop();
		}

		st->playNo = st->select;
	}

	if(rep & L_JPAD) vec--;
	if(rep & R_JPAD) vec++;
	select = CLAMP(0, 4, st->select + vec);

	// サウンドを再生
	if(select != st->select) {
		dm_snd_play(SE_mLeftRight);
		st->select = select;
	}
}

// 更新
static void menuMusicItem_update(SMenuMusicItem *st, SMenuItem *parent)
{
	SMenuItem *item;
	float *color;
	int i, j;

	// 下地
	menuItem_update(st->miBase, parent);

	// ラベル
	menuItem_updateN(st->miLabel, ARRAY_SIZE(st->miLabel), st->miBase);

	// マスク
	for(i = 0; i < ARRAY_SIZE(st->miMask); i++) {
		item = &st->miMask[i];

		menuItem_update(item, &st->miLabel[i]);

		if(i == st->select) {
			if(!st->flags.blink) {
				color = item->colorRange[1];
			}
			else {
				color = NULL;
			}

			for(j = 0; j < ARRAY_SIZE(item->colorRange[0]); j++) {
				item->colorRange[0][j] = _musicItemColor[1][j] * (1.0 / 511.0);
			}
		}
		else {
			color = item->colorRange[0];

			for(j = 0; j < ARRAY_SIZE(item->colorRange[0]); j++) {
				item->colorRange[0][j] = _musicItemColor[0][j] * (1.0 / 255.0);
			}
		}

		if(color != NULL) {
			for(j = 0; j < ARRAY_SIZE(item->color); j++) {
				item->color[j] = color[j] * parent->color[j];
			}
		}
	}

	// カーソル
	menuCursor_update(st->cursor, &st->miLabel[st->select]);
}

// 描画
static void menuMusicItem_draw1(SMenuMusicItem **stAry, int count, Gfx **gpp)
{
	Gfx *gp = *gpp;
	SMenuMusicItem *st;
	SMenuItem *item;
	STexInfo *texC;
	int i, j, cached;

	gSPDisplayList(gp++, fade_intensity_texture_init_dl);

	// マスク
	for(i = 0; i < ARRAY_SIZE(st->miMask); i++) {
		cached = 0;

		for(j = 0; j < count; j++) {
			st = stAry[j];
			item = &st->miMask[i];

			texC = _getTexSetup(st->global, TEX_new_cursor_music_fever + i);
			cached += menuItem_drawTex(item, &gp, texC, cached);
		}
	}

	*gpp = gp;
}

// 描画2
static void menuMusicItem_draw2(SMenuMusicItem **stAry, int count, Gfx **gpp)
{
	Gfx *gp = *gpp;
	SMenuMusicItem *st;
	void *list[count];
	int i;

	// カーソル
	for(i = 0; i < count; i++) {
		st = stAry[i];
		list[i] = st->cursor;
	}
	menuCursor_draw((SMenuCursor **)list, count, &gp);

	*gpp = gp;
}

// 描画
static void menuMusicItem_draw(SMenuMusicItem **stAry, int count, Gfx **gpp)
{
	menuMusicItem_draw1(stAry, count, gpp);
	menuMusicItem_draw2(stAry, count, gpp);
}

//////////////////////////////////////////////////////////////////////////////
//{### 数値選択

//////////////////////////////////////
//## 数値

typedef struct {
	void *global;
	int size;
	int column;
	int number;
	struct {
		unsigned zero : 1;
	} flags;
	char columns[8];
	SMenuItem miBase[1];
} SMenuNumber;

// 初期化
static void menuNumber_init(SMenuNumber *st, void *global,
	int size, int column, int number, int x, int y)
{
	int i;

	st->global = global;
	st->size = size;
	st->column = column;
	st->number = number;
	st->flags.zero = 1;
	bzero(st->columns, sizeof(st->columns));

	// 下地
	menuItem_init(st->miBase, x, y);
}

// 更新
static void menuNumber_update(SMenuNumber *st, SMenuItem *parent)
{
	int i, n;

	n = st->number;
	for(i = st->column - 1; i >= 0; i--) {
		st->columns[i] = n % 10;
		n /= 10;
	}

	// 下地
	menuItem_update(st->miBase, parent);
}

// 描画
static void menuNumber_draw(SMenuNumber **stAry, int count, Gfx **gpp)
{
	static const int _tbl[] = { 9,0,1,2,3,4,5,6,7,8 };
	static const int _step[] = { 10, 14 };
	Gfx *gp = *gpp;
	SMenuNumber *st;
	SMenuItem *item;
	STexInfo *texC, *texA;
	int i, j, k, s, width, height, cached, zero;

	gSPDisplayList(gp++, fade_alpha_texture_init_dl);

	for(s = 0; s < 2; s++) {
	for(i = 0; cached = 0, i < 10; i++) {
	for(j = 0; j < count; j++) {
		st = stAry[j];
		item = st->miBase;

		zero = st->flags.zero;

		switch(st->size) {
		case 0:
			texC = _getTexSetup(st->global, TEX_answer_no);
			texA = _getTexSetup(st->global, TEX_answer_no_alpha);
			break;
		case 1:
			texC = _getTexGameAl(st->global, TEX_AL_VIRUS_NUMBER);
			texA = _getTexGameAl(st->global, TEX_AL_VIRUS_NUMBER_ALPHA);
			break;
		}

		width = MIN(texC->size[0], texA->size[0]);
		height = MIN(texC->size[1], texA->size[1]) / 10;

		if(menuItem_outOfScreen(item, width * st->column, height)) {
			continue;
		}

		menuItem_setPrim(item, &gp);

		for(k = 0; k < st->column; k++) {
			if(zero == 0 && st->columns[k] == 0) {
				continue;
			}
			zero = 1;

			if(s != st->size || i != st->columns[k]) {
				continue;
			}

			tiStretchAlphaTexItem(&gp, texC, texA, cached++, 10, _tbl[i],
				item->trans[0] + _step[s] * item->scale[0] * k, item->trans[1],
				width * item->scale[0], height * item->scale[1]);
		}
	}}}

	*gpp = gp;
}

//////////////////////////////////////////////////////////////////////////////
//{### 各種パネル

//////////////////////////////////////
//## COM LV

typedef struct {
	void *global;
	int level;
	SMenuItem miBase[1];
	SMenuItem miStar[1];
} SMenuComLvPanel;

// フェード
static void menuComLvPanel_setFade(SMenuComLvPanel *st, int dir, float time)
{
	SMenuItem *item = st->miBase;
	menuItem_setColor_fade(item);
	menuItem_setColorDir(item, dir);
	item->colorTime = time;
}

static void menuComLvPanel_setFadeDir(SMenuComLvPanel *st, int dir)
{
	menuComLvPanel_setFade(st, dir, st->miBase->colorTime);
}

//
static void menuComLvPanel_setPos(SMenuComLvPanel *st, int x, int y)
{
	SMenuItem *item = st->miBase;
	item->transRange[0][0] = item->transRange[1][0] = x;
	item->transRange[0][1] = item->transRange[1][1] = y;
}

// 初期化
static void menuComLvPanel_init(SMenuComLvPanel *st, void *global, int level, int x, int y)
{
	st->global = global;
	st->level = level;

	// 下地
	menuItem_init(st->miBase, x, y);

	// ☆
	menuItem_init(st->miStar, 0, 10);
}

// 更新
static void menuComLvPanel_update(SMenuComLvPanel *st, SMenuItem *parent)
{
	// 下地
	menuItem_update(st->miBase, parent);

	// ☆
	menuItem_update(st->miStar, st->miBase);
}

// 描画
static void menuComLvPanel_draw(SMenuComLvPanel **stAry, int count, Gfx **gpp)
{
	Gfx *gp = *gpp;
	SMenuComLvPanel *st;
	SMenuItem *item;
	STexInfo *texC;
	int i, j, cached;

	gSPDisplayList(gp++, fade_normal_texture_init_dl);

	for(i = cached = 0; i < count; i++) {
		st = stAry[i];
		item = st->miBase;
		texC = _getTexChar(st->global, TEX_character_lv_panel);

		if(menuItem_outOfScreen(item, texC->size[0], texC->size[1])) {
			continue;
		}

		if(item->color[3] < 1) {
			gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
		}
		else {
			gDPSetRenderMode(gp++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
		}

		menuItem_setPrim(item, &gp);

		tiStretchTexTile(&gp, texC, cached,
			0, 0, texC->size[0], 20,
			item->trans[0], item->trans[1],
			item->scale[0] * texC->size[0],
			item->scale[1] * 20);

		cached++;
	}

	cached = 0;
	for(j = 7; j > 0; j--) {
	for(i = 0; i < count; i++) {
		st = stAry[i];

		if(j != st->level) {
			continue;
		}

		item = st->miStar;
		texC = _getTexChar(st->global, TEX_character_lv_panel);

		if(menuItem_outOfScreen(st->miBase, texC->size[0], texC->size[1])) {
			continue;
		}

		if(item->color[3] < 1) {
			gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
		}
		else {
			gDPSetRenderMode(gp++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
		}

		menuItem_setPrim(item, &gp);

		tiStretchTexTile(&gp, texC, cached,
			0, 20, texC->size[0] * j / 7, 10,
			item->trans[0], item->trans[1],
			item->scale[0] * texC->size[0] * j / 7,
			item->scale[1] * 10);
	}}

	*gpp = gp;
}

//////////////////////////////////////
//## 操作の仕方

typedef struct {
	void *global;
	SMenuItem miBase[1];
	SMenuItem miDesc[5];
	SMenuItem miLine[1];
	SMenuItem miLabel[1];
	SMenuItem miPushAnyKey[1];
} SMenuCont;

// フェードインアウト
static void menuCont_setFade(SMenuCont *st, int dir, float time)
{
	static const float _color[] = { 1, 0, 0 };
	SMenuItem *list[ARRAY_SIZE(st->miDesc) + ARRAY_SIZE(st->miLine) + ARRAY_SIZE(st->miLabel) + ARRAY_SIZE(st->miPushAnyKey)];
	SMenuItem *item;
	int i, j = 0;

	for(i = 0; i < ARRAY_SIZE(st->miDesc); i++) {
		list[j++] = &st->miDesc[i];
	}

	for(i = 0; i < ARRAY_SIZE(st->miLine); i++) {
		list[j++] = &st->miLine[i];
	}

	for(i = 0; i < ARRAY_SIZE(st->miLabel); i++) {
		list[j++] = &st->miLabel[i];
	}

	for(i = 0; i < ARRAY_SIZE(st->miPushAnyKey); i++) {
		list[j++] = &st->miPushAnyKey[i];
	}

	for(i = 0; i < j; i++) {
		item = list[i];
		menuItem_setColor_fade(item);
		item->colorStep = FRAME_MOVE_STEP;
		item->colorTime = time;
		menuItem_setColorDir(item, dir);
	}

	for(i = 0; i < ARRAY_SIZE(st->miLine); i++) {
		item = &st->miLine[i];
		for(j = 0; j < 3; j++) {
			item->colorRange[0][j] = item->colorRange[1][j] = _color[j];
		}
	}
}

// フレームインアウト
static void menuCont_setFrame(SMenuCont *st, int dir, int time)
{
	SMenuItem *item = st->miBase;
	item->transRange[0][0] = item->transRange[1][0] - SCREEN_WD / 2;
	item->transStep = FRAME_MOVE_STEP;
	item->transTime = time;
	menuItem_setTransDir(item, dir);
}

// 初期化
static void menuCont_init(SMenuCont *st, void *global, int x, int y)
{
#if LOCAL==JAPAN
	static const int _posDesc[][2] = {
		{ -6, -42 }, { 100, 60 }, { 100, 101 }, { 100, 10 }, { 100, -40 },
	};
	static const int _posLine[][2] = {
		{ 4, -2 },
	};
#elif LOCAL==AMERICA
	static const int _posDesc[][2] = {
		{ 200, -62 }, { 102, -36 }, { 102, 14 }, { 102, 64 }, { 0, -44 },
	};
	static const int _posLine[][2] = {
		{ 14, -41 },
	};
#elif LOCAL==CHINA
	static const int _posDesc[][2] = {
		{ 200, -62 }, { 102, -36 }, { 102, 14 }, { 102, 64 }, { 0, -44 },
	};
	static const int _posLine[][2] = {
		{ 14, -41 },
	};
#endif

	SMenuItem *item;
	int i;

	st->global = global;

	// 下地
	menuItem_init(st->miBase, x, y);

	// 説明
	for(i = 0; i < ARRAY_SIZE(st->miDesc); i++) {
		item = &st->miDesc[i];
		menuItem_init(item, _posDesc[i][0], _posDesc[i][1]);
		menuItem_setColor_fade(item);
	}

	// 線
	for(i = 0; i < ARRAY_SIZE(st->miLine); i++) {
		item = &st->miLine[i];
		menuItem_init(item, _posLine[i][0], _posLine[i][1]);
		menuItem_setColor_fade(item);
	}

	// ラベル
#if LOCAL==JAPAN
	menuItem_init(st->miLabel, 203, 47);
#elif LOCAL==AMERICA
	menuItem_init(st->miLabel, 14, 47);
#elif LOCAL==CHINA
	menuItem_init(st->miLabel, 14, 47);
#endif

	// push any key
#if LOCAL==JAPAN
	menuItem_init(st->miPushAnyKey, 207, 202);
#elif LOCAL==AMERICA
	menuItem_init(st->miPushAnyKey, 214, 212);
#elif LOCAL==CHINA
	menuItem_init(st->miPushAnyKey, 214, 212);
#endif

	// フェードアウト
	menuCont_setFade(st, -1, 0);

	// フレームアウト
	menuCont_setFrame(st, -1, 0);
}

// 入力
static bool menuCont_input(SMenuCont *st, int playerNo)
{
	SMenuItem *item = st->miBase;
	return item->transStep > 0 && item->transTime == 1;
}

// 更新
static void menuCont_update(SMenuCont *st, SMenuItem *parent)
{
	// 下地
	menuItem_update(st->miBase, parent);

	// 説明
	menuItem_updateN(st->miDesc, ARRAY_SIZE(st->miDesc), st->miBase);

	// 線
	menuItem_updateN(st->miLine, ARRAY_SIZE(st->miLine), st->miBase);

	// ラベル
	menuItem_update(st->miLabel, parent);

	// push any key
	menuItem_update(st->miPushAnyKey, parent);
}

// 描画
static void menuCont_draw(SMenuCont *st, Gfx **gpp)
{
	static const int _line[] = { TEX_account_controller_line_b };
#if LOCAL==JAPAN
	static const int _desc[] = { TEX_r_triger, TEX_l_triger };
#elif LOCAL==AMERICA
	static const int _desc[] = { TEX_control_pad_a };
#elif LOCAL==CHINA
	static const int _desc[] = { TEX_control_pad_a };
#endif

	Gfx *gp = *gpp;
	SMenuItem *item;
	STexInfo *texC, *texA;
	int i;

	gSPDisplayList(gp++, fade_normal_texture_init_dl);

	// 下地
	item = st->miBase;
	texC = _getTexTutol(st->global, TEX_account_controller);
	menuItem_drawTex(item, &gp, texC, false);

	gSPDisplayList(gp++, fade_intensity_texture_init_dl);

	// 線
	for(i = 0; i < ARRAY_SIZE(st->miLine); i++) {
		item = &st->miLine[i];
		texC = _getTexCont(st->global, _line[i]);
		menuItem_drawTex(item, &gp, texC, false);
	}

	gSPDisplayList(gp++, fade_normal_texture_init_dl);
	gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);

	// 説明
#if LOCAL==JAPAN
	texC = _getTexCont(st->global, TEX_account_operation);
	for(i = 0; i < ARRAY_SIZE(st->miDesc) - 2; i++) {
		item = &st->miDesc[i];
		menuItem_drawItem(item, &gp, texC, false, ARRAY_SIZE(st->miDesc) - 2, i);
	}
#elif LOCAL==AMERICA
	texC = _getTexCont(st->global, TEX_control_pad_b);
	for(i = 0; i < ARRAY_SIZE(st->miDesc) - 1; i++) {
		item = &st->miDesc[i];
		menuItem_drawItem(item, &gp, texC, false, ARRAY_SIZE(st->miDesc) - 1, i);
	}
#elif LOCAL==CHINA
	texC = _getTexCont(st->global, TEX_control_pad_b);
	for(i = 0; i < ARRAY_SIZE(st->miDesc) - 1; i++) {
		item = &st->miDesc[i];
		menuItem_drawItem(item, &gp, texC, false, ARRAY_SIZE(st->miDesc) - 1, i);
	}
#endif

	for(; i < ARRAY_SIZE(st->miDesc); i++) {
		item = &st->miDesc[i];
		texC = _getTexCont(st->global, _desc[i - ARRAY_SIZE(st->miDesc) + 2]);
		menuItem_drawTex(item, &gp, texC, false);
	}

	// ラベル
	texC = _getTexCommon(st->global, TEX_choice_a);
	menuItem_drawItem(st->miLabel, &gp, texC, false, CHOICE_A_SIZE, 19);

	// push any key
	gSPDisplayList(gp++, fade_alpha_texture_init_dl);
	texC = _getTexTutol(st->global, TEX_b_button);
	texA = _getTexTutol(st->global, TEX_b_button_alpha);
	menuItem_drawAlphaTex(st->miPushAnyKey, &gp, texC, texA, false);

	*gpp = gp;
}

//////////////////////////////////////
//## モードセレクトのパネル

typedef struct {
	void *global;
	int size;
	int count;
	int select;
	int indexes[6];
	SMenuItem miBase[1];
	SMenuItem miPanel[6];
	SMenuCursor cursor[1];
} SMenuMainPanel;

// フレームインアウト
static void menuMainPanel_setFrame(SMenuMainPanel *st, int dir, float time)
{
	SMenuItem *item = st->miBase;
	item->transRange[0][0] = item->transRange[1][0] - SCREEN_WD;
	item->transStep = FRAME_MOVE_STEP;
	item->transTime = time;
	menuItem_setTransDir(item, dir);
}

// 初期化
static void menuMainPanel_init(SMenuMainPanel *st, void *global,
	int count, int select, const int *indexes, int x, int y)
{
	int i;

	st->global = global;
	st->count = count;
	st->select = select = WrapI(0, count, select);

	for(i = 0; i < ARRAY_SIZE(st->indexes); i++) {
		st->indexes[i] = indexes[i];
	}

	// 下地
	menuItem_init(st->miBase, x, y);

	// パネル群
	for(i = 0; i < count; i++) {
		menuItem_init(&st->miPanel[i], 11, 9 + i * 23);
	}

	// カーソル
	menuCursor_init(st->cursor, global, CURSOR_ITEM, 0, 0, 0, 106, 20);
	st->cursor->flags.finger = 1;
}

// 入力
static bool menuMainPanel_input(SMenuMainPanel *st, int playerNo)
{
	SMenuItem *item = &st->miPanel[st->select];
	int rep = _getKeyRep(st->global, playerNo);
	int vec = 0;
	int select;

	if(st->miBase->transStep < 0 || st->miBase->transTime < 1) {
		return false;
	}

	if(rep & U_JPAD) vec--;
	if(rep & D_JPAD) vec++;

	select = st->select;
#if defined(USE_HALT)
	do {
#endif
		select = WrapI(0, st->count, select + vec);
#if defined(USE_HALT)
	} while(vec != 0 && st->miPanel[select].colorTime == 0);
#endif

	if(select != st->select) {
		dm_snd_play(SE_mUpDown);
		st->select = select;
	}

	return true;
}

// 更新
static void menuMainPanel_update(SMenuMainPanel *st, SMenuItem *parent)
{
	menuItem_update(st->miBase, parent);
	menuItem_updateN(st->miPanel, st->count, st->miBase);
	menuCursor_update(st->cursor, &st->miPanel[st->select]);
}

// 描画
static void menuMainPanel_draw(SMenuMainPanel *st, Gfx **gpp)
{
	static const int _panel[] = {
		TEX_menu_panel_2p, TEX_menu_panel_3p, TEX_menu_panel_4p,
		TEX_menu_panel_5p, TEX_menu_panel_6p,
	};
	Gfx *gp = *gpp;
	STexInfo *texC, *texA;
	void *list[1];
	int i, j;
	float y;

	gSPDisplayList(gp++, fade_normal_texture_init_dl);

	// 下地
	texC = _getTexMain(st->global, _panel[st->count - 2]);
	menuItem_drawTex(st->miBase, &gp, texC, false);

	// パネル
	texC = _getTexCommon(st->global, TEX_choice_a);
	for(i = 0; i < st->count; i++) {
		menuItem_drawItem(&st->miPanel[i], &gp,
			texC, false, CHOICE_A_SIZE, st->indexes[i]);
	}

	// カーソル
	list[0] = st->cursor;
	menuCursor_draw((SMenuCursor **)list, 1, &gp);

	*gpp = gp;
}

//////////////////////////////////////
//## 名前選択パネル

typedef struct {
	void *global;
	int playerCount;
	int flow;
	int depth[2];
	int select[2];
	int column[2];
	struct {
		unsigned guest : 1;
	} flags;
	SMenuItem miBase[1];
	SMenuItem miLabel[9];
	SMenuCursor cursor[2];
} SMenuNameSelPanel;

// フレームインアウト
static void menuNameSelPanel_setFrame(SMenuNameSelPanel *st, int dir, float time)
{
	SMenuItem *item = st->miBase;
	item->transRange[0][0] = item->transRange[1][0] - SCREEN_WD;
	item->transStep = FRAME_MOVE_STEP;
	item->transTime = time;
	menuItem_setTransDir(item, dir);
}

// クリア
static void menuNameSelPanel_clear(SMenuNameSelPanel *st, int guestFlag, int playerCount)
{
	int i;

	st->playerCount = playerCount;
	st->flow = 0;

	if(evs_select_name_no[0] == evs_select_name_no[1]) {
		evs_select_name_no[1] = DM_MEM_GUEST;
	}

	for(i = 0; i < 2; i++) {
		st->depth[i] = 0;
		st->select[i] = evs_select_name_no[i];
		st->column[i] = evs_select_name_no[i] & 1;
	}

	st->flags.guest = guestFlag = (guestFlag ? 1 : 0);

	for(i = 0; i < ARRAY_SIZE(st->cursor); i++) {
		st->cursor[i].flags.finger = 1;
		st->cursor[i].flags.player = (st->playerCount > 1 ? 1 : 0);
	}
}

// 初期化
static void menuNameSelPanel_init(SMenuNameSelPanel *st, void *global,
	int guestFlag, int playerCount, int x, int y)
{
	int i;

	st->global = global;

	// 下地
	menuItem_init(st->miBase, x, y);

	// パネル
	for(i = 0; i < ARRAY_SIZE(st->miLabel) - 1; i++) {
		menuItem_init(&st->miLabel[i], 6 + i % 2 * 60, 31 + i / 2 * 23);
	}

	// ゲスト or 終了
	menuItem_init(&st->miLabel[i], 36, 8);

	// カーソル
	for(i = 0; i < ARRAY_SIZE(st->cursor); i++) {
		SMenuItem *item;

		menuCursor_init(&st->cursor[i], global, CURSOR_ITEM, i, 0, 0, 56, 20);

		if(i & 1) {
			item = st->cursor[i].miCursor;
			item->colorTime = WrapF(0, 1, item->colorTime + 0.5);

			item = st->cursor[i].miFinger;
			item->transTime = WrapF(0, 1, item->transTime + 0.5);
			item->transRange[0][0] += st->cursor[i].size[0] + 16;
			item->transRange[1][0] += st->cursor[i].size[0] + 16;
			item->scaleRange[0][0] = -item->scaleRange[0][0];
			item->scaleRange[1][0] = -item->scaleRange[1][0];

			item = st->cursor[i].miPlayer;
			item->transTime = WrapF(0, 1, item->transTime + 0.5);
			item->transRange[0][0] += st->cursor[i].size[0] + 16;
			item->transRange[1][0] += st->cursor[i].size[0] + 16;
		}
	}

	menuNameSelPanel_clear(st, guestFlag, playerCount);
}

// 選択済みの名前かどうかチェック
static int menuNameSelPanel_checkSelected(SMenuNameSelPanel *st, int playerNo, int nameNo)
{
	int i;

	if(nameNo != DM_MEM_GUEST) {
		for(i = 0; i < st->playerCount; i++) {
			if(i != playerNo && st->select[i] != DM_MEM_GUEST && st->select[i] == nameNo) {
				return 1;
			}
		}
	}

	return 0;
}

// 入力
static void menuNameSelPanel_input1(SMenuNameSelPanel *st, int playerNo)
{
	int rep = _getKeyRep(st->global, playerNo);
	int trg = _getKeyTrg(st->global, playerNo);
	int vec, sound = -1, select = st->select[playerNo];

	if(menuNameSelPanel_checkSelected(st, playerNo, select)) {
		select = st->select[playerNo] = DM_MEM_GUEST;
	}

	if(st->depth[playerNo] == 0) {
		// ゲスト or 終了を選択中
		if(select == DM_MEM_GUEST) {
			vec = 0;
			if(rep & L_JPAD) vec--;
			if(rep & R_JPAD) vec++;
			st->column[playerNo] = WrapI(0, 2, st->column[playerNo] + vec);

			vec = 0;
			if(rep & U_JPAD) vec--;
			if(rep & D_JPAD) vec++;

			if(vec > 0) {
				select = st->column[playerNo];

				while(select < 8 &&
					menuNameSelPanel_checkSelected(st, playerNo, select))
				{
					select += 2;
				}
			}
			else if(vec < 0) {
				select = st->column[playerNo] + 6;

				while(select > 0 &&
					menuNameSelPanel_checkSelected(st, playerNo, select))
				{
					select -= 2;
				}
			}

			if(select < 0 || select >= 8) {
				select = DM_MEM_GUEST;
			}
		}
		// 名前を選択中
		else {
			int ix, iy, vx, vy;

			ix = select & 1;
			iy = select >> 1;

			vx = vy = 0;
			if(rep & L_JPAD) vx--;
			if(rep & R_JPAD) vx++;
			if(rep & U_JPAD) vy--;
			if(rep & D_JPAD) vy++;

			if(vx || vy) {
				do {
					ix = WrapI(0, 2, ix + vx);
					iy += vy;
				} while(iy >= 0 && iy < 4 &&
					menuNameSelPanel_checkSelected(st, playerNo, ix + iy * 2));
			}

			if(iy < 0 || iy >= 4) {
				st->column[playerNo] = (select & 1);
				select = DM_MEM_GUEST;
			}
			else {
				select = ix + iy * 2;
			}
		}
	}

	if(select != st->select[playerNo]) {
		sound = SE_mUpDown;
		st->select[playerNo] = select;
	}

	if(trg & CONT_OK) {
		vec = MIN(1, st->depth[playerNo] + 1);

		if(vec != st->depth[playerNo]) {
			if(st->flags.guest || st->select[playerNo] != DM_MEM_GUEST) {
				sound = SE_mDecide;
			}
			st->depth[playerNo] = vec;
		}
	}
	else if(trg & CONT_CANCEL) {
		vec = MAX(-1, st->depth[playerNo] - 1);

		if(vec != st->depth[playerNo]) {
			sound = SE_mCANCEL;
			st->depth[playerNo] = vec;
		}
	}

	// サウンドを再生
	if(sound >= 0) {
		dm_snd_play(sound);
	}
}

// 入力
static bool menuNameSelPanel_input(SMenuNameSelPanel *st)
{
	int i, ok = 0;

	if(st->miBase->transStep < 0 || st->miBase->transTime < 1) {
		return false;
	}

	st->flow = 0;

	for(i = 0; i < st->playerCount; i++) {
		menuNameSelPanel_input1(st, i);

		if(st->depth[i] > 0) {
			ok++;
		}
		else if(st->depth[i] < 0) {
			st->depth[i] = 0;
			st->flow = -1;
			return true;
		}
	}

	if(ok == st->playerCount) {
		st->flow = 1;
	}

	return true;
}

// 更新
static void menuNameSelPanel_update(SMenuNameSelPanel *st, SMenuItem *parent)
{
	SMenuItem *item;
	int i, j;

	menuItem_update(st->miBase, parent);
	menuItem_updateN(st->miLabel, ARRAY_SIZE(st->miLabel), st->miBase);

	for(i = 0; i < st->playerCount; i++) {
		st->cursor[i].miCursor->colorRange[0][3] = 1;
	}

	for(i = 0; i < st->playerCount - 1; i++) {
		for(j = i + 1; j < st->playerCount; j++) {
			if(st->select[i] == st->select[j]) {
				st->cursor[i].miCursor->colorRange[0][3] = 0;
				st->cursor[j].miCursor->colorRange[0][3] = 0;
			}
		}
	}

	for(i = 0; i < ARRAY_SIZE(st->cursor); i++) {
		menuCursor_update(&st->cursor[i], &st->miLabel[st->select[i]]);

		if(st->depth[i] > 0) {
			item = st->cursor[i].miCursor;
			for(j = 0; j < ARRAY_SIZE(item->color); j++) {
				item->color[j] = item->colorRange[1][j];
			}
			st->cursor[i].flags.finger = 0;
			st->cursor[i].flags.blink = 0;
		}
		else {
			st->cursor[i].flags.finger = 1;
			st->cursor[i].flags.blink = 1;
		}
	}
}

// 描画
static void menuNameSelPanel_draw(SMenuNameSelPanel *st, Gfx **gpp)
{
	static const int _texEndOrGuest[] = { TEX_end_panel, TEX_guest_panel };

	Gfx *gp = *gpp;
	SMenuItem *item;
	STexInfo *texC, *texA;
	const char *name;
	void *list[st->playerCount];
	int i, j, cached;
	float bak[2];

	gSPDisplayList(gp++, fade_normal_texture_init_dl);

	// 下地
	texC = _getTexMain(st->global, TEX_menu_panel_5p);
	menuItem_drawTex(st->miBase, &gp, texC, false);

	// 名前パネル
	texC = _getTexMain(st->global, TEX_name_panel);
	for(i = cached = 0; i < ARRAY_SIZE(st->miLabel) - 1; i++) {
		cached += menuItem_drawTex(&st->miLabel[i], &gp, texC, cached);
	}

	// ゲスト or 終了パネル
	texC = _getTexMain(st->global, _texEndOrGuest[st->flags.guest]);
	menuItem_drawTex(&st->miLabel[i], &gp, texC, false);

	// 名前
	gSPDisplayList(gp++, fade_intensity_texture_init_dl);
	gDPSetTextureFilter(gp++, G_TF_BILERP);

	for(i = 0; i < ARRAY_SIZE(st->miLabel) - 1; i++) {
		item = &st->miLabel[i];

		if(menuItem_outOfScreen(item, 14, 14)) {
			continue;
		}

		if(evs_mem_data[i].mem_use_flg & DM_MEM_USE) {
			name = evs_mem_data[i].mem_name;
		}
		else {
			name = evs_mem_data[i].mem_name;
		}
                
		j = (int)(item->color[3] * 255);
		if(evs_mem_data[i].mem_use_flg & DM_MEM_USE) {
			gDPSetPrimColor(gp++, 0,0, 1,1,1, j);
		}
		else {
			gDPSetPrimColor(gp++, 0,0, 160,160,160, 255);
		}

		for(j = 0; j < MEM_NAME_SIZE; j++) {
			fontXX_drawID(&gp,
				item->trans[0] + 4 + j * 12,
				item->trans[1] + 4,
				item->scale[0] * FONT_WORD_W,
				item->scale[1] * FONT_WORD_H,
				(int) name[j] & 0xff); 
		}
	}

	// カーソル
	for(i = 0; i < st->playerCount; i++) {
		list[i] = &st->cursor[i];
	}
	menuCursor_draw((SMenuCursor **)list, st->playerCount, &gp);

	*gpp = gp;
}

//////////////////////////////////////
//## 名前変更or消去用パネル

typedef struct {
	void *global;
	int type;
	int select;
	char name[MEM_NAME_SIZE];
	SMenuItem miBase[1];
	SMenuItem miName[1];
	SMenuItem miPanel[2];
	SMenuCursor cursor[1];
} SMenuNameOpPanel;

// フレームインアウト
static void menuNameOpPanel_setFrame(SMenuNameOpPanel *st, int dir, float time)
{
	SMenuItem *item = st->miBase;
	item->transRange[0][0] = item->transRange[1][0] - SCREEN_WD;
	item->transStep = FRAME_MOVE_STEP;
	item->transTime = time;
	menuItem_setTransDir(item, dir);
}

// 初期化
static void menuNameOpPanel_init(SMenuNameOpPanel *st, void *global,
	int type, int select, const char *name, int x, int y)
{
	static const char _name[] = { 0, 0, 0, 0 };
	int i;

	st->global = global;
	st->type = type;
	st->select = select;

	// 名前をコピー
	if(!name) name = _name;
	for(i = 0; i < ARRAY_SIZE(st->name); i++) {
		st->name[i] = name[i];
	}

	// 下地
	menuItem_init(st->miBase, x, y);

	// 名前
	menuItem_init(st->miName, 35, 8);

	// パネル
	for(i = 0; i < ARRAY_SIZE(st->miPanel); i++) {
		menuItem_init(&st->miPanel[i], 11, 31 + i * 23);
	}

	// カーソル
	menuCursor_init(st->cursor, global, CURSOR_ITEM, 0, 0, 0, 105, 20);
	st->cursor->flags.finger = 1;
}

// 入力
static bool menuNameOpPanel_input(SMenuNameOpPanel *st, int playerNo)
{
	int rep = _getKeyRep(st->global, playerNo);
	int vec = st->select;

	if(st->miBase->transStep < 0 || st->miBase->transTime < 1) {
		return false;
	}

	if(rep & U_JPAD) vec--;
	if(rep & D_JPAD) vec++;
	vec = WrapI(0, ARRAY_SIZE(st->miPanel), vec);

	if(vec != st->select) {
		dm_snd_play(SE_mUpDown);
		st->select = vec;
	}

	return true;
}

// 更新
static void menuNameOpPanel_update(SMenuNameOpPanel *st, SMenuItem *parent)
{
	// 下地
	menuItem_update(st->miBase, parent);

	// 名前
	menuItem_update(st->miName, st->miBase);

	// パネル
	menuItem_updateN(st->miPanel, ARRAY_SIZE(st->miPanel), st->miBase);

	// カーソル
	menuCursor_update(st->cursor, &st->miPanel[st->select]);
}

// 描画
static void menuNameOpPanel_draw(SMenuNameOpPanel *st, Gfx **gpp)
{
	static const int _panel[][2] = {
		{ 24, 25 }, // 名前を変更, 名前を消す
		{ 28, 25 }, // やっぱり止める, 名前を消す
	};
	Gfx *gp = *gpp;
	SMenuItem *item;
	STexInfo *texC, *texA;
	void *list[1];
	int i;

	gSPDisplayList(gp++, fade_normal_texture_init_dl);

	// 下地
	texC = _getTexMain(st->global, TEX_menu_panel_3p);
	menuItem_drawTex(st->miBase, &gp, texC, false);

	// パネル
	texC = _getTexCommon(st->global, TEX_choice_a);
	for(i = 0; i < ARRAY_SIZE(st->miPanel); i++) {
		menuItem_drawItem(&st->miPanel[i], &gp, texC, false, CHOICE_A_SIZE, _panel[st->type][i]);
	}

	// 名前パネル
	texC = _getTexMain(st->global, TEX_name_panel);
	menuItem_drawTex(st->miName, &gp, texC, false);

	// 名前
	item = st->miName;
	gSPDisplayList(gp++, fade_intensity_texture_init_dl);
	gDPSetTextureFilter(gp++, G_TF_BILERP);
	gDPSetPrimColor(gp++, 0,0, 0,0,0,255);

	for(i = 0; i < ARRAY_SIZE(st->name); i++) {
		fontXX_drawID(&gp,
			item->trans[0] + 4 + i * 12,
			item->trans[1] + 4,
			item->scale[0] * 12,
			item->scale[1] * 12,
                        (int) st->name[i] & 0xff); 
	}

	// カーソル
	list[0] = st->cursor;
	menuCursor_draw((SMenuCursor **)list, 1, &gp);

	*gpp = gp;
}

//////////////////////////////////////
//## 名前変更or消去用パネル

typedef struct {
	void *global;
	int select;
	int bgmNo;
	int sndId;
	int sndNo;
	SMenuItem miBase[1];
	SMenuItem miPanel[4];
	SMenuNumber number[2];
	SMenuCursor cursor[1];
} SMenuSndSelPanel;

// フレームインアウト
static void menuSndSelPanel_setFrame(SMenuSndSelPanel *st, int dir, float time)
{
	SMenuItem *item = st->miBase;
	item->transRange[0][0] = item->transRange[1][0] - SCREEN_WD;
	item->transStep = FRAME_MOVE_STEP;
	item->transTime = time;
	menuItem_setTransDir(item, dir);
}

// 初期化
static void menuSndSelPanel_init(SMenuSndSelPanel *st, void *global,
	int select, int x, int y)
{
	static const int _pos[] = { 64, 64 };
	int i;

	st->global = global;
	st->select = select;
	st->bgmNo = 0;
	st->sndId = 0;
	st->sndNo = 0;

	// 下地
	menuItem_init(st->miBase, x, y);

	// パネル
	for(i = 0; i < ARRAY_SIZE(st->miPanel); i++) {
		static const int _y[] = { 22, 44, 98, 120 };
		menuItem_init(&st->miPanel[i], 14, _y[i]);
	}

	// 数値
	for(i = 0; i < ARRAY_SIZE(st->number); i++) {
		menuNumber_init(&st->number[i], global, 0, 2 + i, 0, _pos[i], 4);
	}

	// カーソル
	menuCursor_init(st->cursor, global, CURSOR_ITEM, 0, 0, 0, 105, 20);
	st->cursor->flags.finger = 1;
}

// 入力
static bool menuSndSelPanel_input(SMenuSndSelPanel *st, int playerNo)
{
	static const u8 _tbl[] = {
		  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
		 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
		 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
		 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
		 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
		 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
		 60, 61, 62, 63, 64, 65, 66, 67, 68,
		 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
		 80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
		 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
		100,101,102,103,
	};
	int rep = _getKeyRep(st->global, playerNo);
	int vec = st->select;
	int sound = -1;

	if(st->miBase->transStep < 0 || st->miBase->transTime < 1) {
		return false;
	}

	if(rep & U_JPAD) vec--;
	if(rep & D_JPAD) vec++;
	vec = WrapI(0, ARRAY_SIZE(st->miPanel), vec);

	if(vec != st->select) {
		sound = SE_mUpDown;
		st->select = vec;
	}

	vec = 0;
	if(rep & L_JPAD) vec--;
	if(rep & R_JPAD) vec++;
	if(rep & L_TRIG) vec -= 10;
	if(rep & R_TRIG) vec += 10;

	if(vec && st->select >= 2) {
		sound = SE_mLeftRight;

		switch(st->select) {
		case 2:
			st->bgmNo = WrapI(0, _SEQ_SUM, st->bgmNo + vec);
			break;

		case 3:
			st->sndId = WrapI(0, ARRAY_SIZE(_tbl), st->sndId + vec);
			st->sndNo = _tbl[st->sndId];
			break;
		}
	}

	if(sound >= 0) {
		dm_snd_play(sound);
	}

	return true;
}

// 更新
static void menuSndSelPanel_update(SMenuSndSelPanel *st, SMenuItem *parent)
{
	int i;

	st->number[0].number = st->bgmNo;
	st->number[1].number = st->sndId;

	// 下地
	menuItem_update(st->miBase, parent);

	// パネル
	menuItem_updateN(st->miPanel, ARRAY_SIZE(st->miPanel), st->miBase);

	// 数値
	for(i = 0; i < 2; i++) {
		menuNumber_update(&st->number[i], &st->miPanel[i + 2]);
	}

	// カーソル
	menuCursor_update(st->cursor, &st->miPanel[st->select]);
}

// 描画
static void menuSndSelPanel_draw(SMenuSndSelPanel *st, Gfx **gpp)
{
	static const int _panel[] = { 26, 27, 35, 36 };
	Gfx *gp = *gpp;
	SMenuItem *item;
	STexInfo *texC, *texA;
	void *list[4];
	float x, y;
	int i;

	gSPDisplayList(gp++, fade_normal_texture_init_dl);

	// 下地
	item = st->miBase;
	x = item->trans[0];
	y = item->trans[1];

	texC = _getTexMain(st->global, TEX_menu_panel_s);
	menuItem_drawTex(item, &gp, texC, false);
	item->trans[1] += 76;
	menuItem_drawTex(item, &gp, texC, false);

	item->trans[0] = x + 19;
	item->trans[1] = y + 2;
	texC = _getTexMain(st->global, TEX_menu_panel_s_2);
	menuItem_drawTex(item, &gp, texC, false);

	item->trans[0] = x;
	item->trans[1] = y;

	// パネル
	texC = _getTexCommon(st->global, TEX_choice_a);
	for(i = 0; i < ARRAY_SIZE(st->miPanel); i++) {
		menuItem_drawItem(&st->miPanel[i], &gp, texC, false, CHOICE_A_SIZE, _panel[i]);
	}

	// 数値
	for(i = 0; i < ARRAY_SIZE(st->number); i++) {
		list[i] = &st->number[i];
	}
	menuNumber_draw((SMenuNumber **)list, ARRAY_SIZE(st->number), &gp);

	// カーソル
	list[0] = st->cursor;
	menuCursor_draw((SMenuCursor **)list, 1, &gp);

	*gpp = gp;
}

//////////////////////////////////////
//## 二人で遊ぶ用のパネル

typedef struct {
	void *global;
	bool flash;
	bool timeAt;
	bool FL_TA;

	int size;
	int playerCount;
	int cpuCount;
	int playerNo;
	int charNo;
	int depth;
	int flow;
	void *animeAddr;

	struct {
		unsigned cpu : 1;
		unsigned loaded : 1;
	} flags;

	SMenuItem miBase[1];
	SMenuItem miPlayer[1];
	SMenuItem miPlayerNo[1];
	SMenuNumber lvNum[1];
	SMenuLvGauge lvGauge[1];
	SMenuSpeedAsk speedAsk[1];
	SMenuSpeedItem speedItem[1];
	SAnimeState animeState[1];
	SMenuItem miOk[1];
	SMenuCursor cursor[2];

	SMenuSpeedAsk glvAsk[1];
	SMenuSpeedItem glvItem[1];
} SMenuPlay2Panel;

// アニメーションを読み込む
static void _menuPlay2Panel_loadAnime(void *arg)
{
	SMenuPlay2Panel *st = (SMenuPlay2Panel *)arg;
	void *heap = st->animeAddr;

	animeState_load(st->animeState, &heap, st->charNo);
	st->flags.loaded = 1;
}

// 初期化
static void menuPlay2Panel_init(SMenuPlay2Panel *st, void *global, void **hpp, int size,
	int playerCount, int cpuCount, int playerNo, int cpuFlag, int charNo, int virusLevel, int gameSpeed, int x, int y)
{
	static const float _lvNumScale[][2] = { { 1, 1 }, { 14.0 / 16.0, 15.0 / 16.0 } };
	static const int _player[][2] = { { 4, 4 }, { 24, 60 } };
	static const int _lvNum[][2] = { { 114, 7 }, { 114, 7 } };
	static const int _lvGauge[][2] = { { 167, 12 }, { 169, 14 } };
	static const int _speedAsk[][2] = { { 94, 24 }, { 96, 41 } };
	static const int _speedItem[][2] = { { 142, 24 }, { 143, 36 } };
	static const int _glvAsk[][2] = { { 94, 4 }, { 96, 10 } };
	static const int _glvItem[][2] = { { 142, 4 }, { 143, 5 } };
	static const int _okY[] = { 19, 28 };
	static const int _cursor[][2][4] = {
		{ { 51, 0, 219, 23 }, { 51, 20, 219, 23 } },
		{ { 51, 0, 219, 34 }, { 51, 31, 219, 34 } },
	};

	mem_char *mc;
	game_config *cfg;

	SMenuItem *item;
	void *hp = *hpp;
	int i;

	st->global = global;
	switch(_getMode(global)) {
	case MODE_VSCOM:
	case MODE_VSMAN:
	case MODE_PLAY4_LV:
	case MODE_PLAY4_TB_LV:
		st->flash = false;
		st->timeAt = false;
		st->FL_TA = false;
		break;

	case MODE_VSCOM_FL:
	case MODE_VSMAN_FL:
	case MODE_PLAY4_FL_LV:
		st->flash = true;
		st->timeAt = false;
		st->FL_TA = true;
		break;

	case MODE_VSMAN_TA:
		st->flash = false;
		st->timeAt = true;
		st->FL_TA = true;
		break;
	}

	st->size = size;
	st->playerCount = playerCount;
	st->cpuCount = cpuCount;
	st->playerNo = playerNo;
	st->charNo = charNo;
	st->depth = 0;
	st->flow = cpuFlag ? -1 : 0;

	st->flags.cpu = cpuFlag = (cpuFlag ? 1 : 0);
	st->flags.loaded = 0;

	// 下地
	menuItem_init(st->miBase, x, y);

	// プレイヤー
	menuItem_init(st->miPlayer, _player[size][0], _player[size][1]);
	menuItem_init(st->miPlayerNo, 10, 4);

	// フラッシュ
	if(st->FL_TA) {
		menuSpeedAsk_init(st->glvAsk, global, 2, virusLevel, _glvAsk[size][0], _glvAsk[size][1]);
		menuSpeedItem_init(st->glvItem, global, size + 2, playerNo, virusLevel, _glvItem[size][0], _glvItem[size][1], 42);
	}
	// ウィルスレベル
	else {
		menuLvGauge_init(st->lvGauge, global, size, playerNo, virusLevel, _lvGauge[size][0], _lvGauge[size][1]);
		menuNumber_init(st->lvNum, global, size, 2, virusLevel, _lvNum[size][0], _lvNum[size][1]);
		item = st->lvNum->miBase;
		menuItem_setScaleLow(item, _lvNumScale[size][0], _lvNumScale[size][1]);
		menuItem_setScaleHi(item, _lvNumScale[size][0], _lvNumScale[size][1]);
	}

	// [LOW] [MED] [HI]
	menuSpeedAsk_init(st->speedAsk, global, 0, gameSpeed, _speedAsk[size][0], _speedAsk[size][1]);
	menuSpeedItem_init(st->speedItem, global, size, playerNo, gameSpeed, _speedItem[size][0], _speedItem[size][1], 42);

	// アニメーション
	switch(size) {
	case 0:
		break;

	case 1:
		st->animeAddr = (void *)ALIGN_16(hp);
		hp = (u8 *)st->animeAddr + animeState_getDataSize(charNo);
		cpuTask_sendTask(_menuPlay2Panel_loadAnime, st);
		break;
	}

	// OK
	menuItem_init(st->miOk, 8, _okY[size]);
	menuItem_setColor_fade(st->miOk);
	menuItem_setColorDir(st->miOk, -1);
	st->miOk->colorTime = 0;

	// カーソル
	for(i = 0; i < ARRAY_SIZE(st->cursor); i++) {
		menuCursor_init(&st->cursor[i], global, CURSOR_PANEL, playerNo,
			_cursor[size][i][0], _cursor[size][i][1],
			_cursor[size][i][2], _cursor[size][i][3]);
	}

	*hpp = hp;
}

// 設定をコピー
static void menuPlay2Panel_copyConfig(SMenuPlay2Panel *st, SMenuPlay2Panel *from)
{
	if(st == from) {
		return;
	}

	switch(from->depth) {
	case 0:
		if(st->FL_TA) {
			st->glvItem->select = from->glvItem->select;
		}
		else {
			st->lvGauge->level = from->lvGauge->level;
		}
		break;

	case 1:
		st->speedItem->select = from->speedItem->select;
		break;
	}
}

// カーソル設定をコピー
static void menuPlay2Panel_copyCursor(SMenuPlay2Panel *st, SMenuPlay2Panel *from)
{
	int i;

	if(st == from) {
		return;
	}

	for(i = 0; i < ARRAY_SIZE(st->cursor); i++) {
		st->cursor[i].flags.cursor = 0;
	}

	if(st->FL_TA) {
		st->glvItem->cursor->flags.cursor = 0;
		st->glvItem->cursor->flags.finger = 0;
	}
	else {
		st->lvGauge->cursor->flags.cursor = 0;
		st->lvGauge->cursor->flags.finger = 0;
	}

	st->speedItem->cursor->flags.cursor = 0;
	st->speedItem->cursor->flags.finger = 0;

	switch(from->depth) {
	case 0:
		if(st->FL_TA) st->glvItem->cursor->flags.finger = 1;
		else          st->lvGauge->cursor->flags.finger = 1;
		break;

	case 1:
		st->speedItem->cursor->flags.finger = 1;
		break;
	}
}

// 入力
static void menuPlay2Panel_input(SMenuPlay2Panel *st, int playerNo,
	int topFlag, int bottomFlag)
{
	int rep = _getKeyRep(st->global, playerNo);
	int trg = _getKeyTrg(st->global, playerNo);
	int vec = 0;
	int depth, sound = -1;

	switch(st->flow) {
	// キャンセル状態を解除
	case -1:
		st->flow++;

	// 設定中
	case 0:
		if((rep & U_JPAD) || (trg & CONT_CANCEL)) vec--;
		if((rep & D_JPAD) || (trg & CONT_OK)) vec++;
		depth = CLAMP(0, 1, st->depth + vec);

		// サウンドを再生
		if(depth != st->depth) {
			sound = SE_mUpDown;
			st->depth = depth;
		}
		// ウィルスレベル or スピード を選択
		else {
			switch(st->depth) {
			case 0:
				if(st->FL_TA) {
					menuSpeedItem_input(st->glvItem, playerNo);
				}
				else {
					menuLvGauge_input(st->lvGauge, playerNo);
				}
				break;
			case 1:
				menuSpeedItem_input(st->speedItem, playerNo);
				break;
			}
		}

		if(sound >= 0) {
			break;
		}

		switch(st->depth) {
		case 1:
			if((trg & CONT_OK) || ((rep & D_JPAD) && !bottomFlag)) {
				st->flow++;
				sound = SE_mDecide;
			}
			break;

		case 0:
			if((trg & CONT_CANCEL) || ((rep & U_JPAD) && !topFlag)) {
				st->flow--;
				sound = SE_mCANCEL;
			}
			break;
		}
		break;

	// 設定完了状態
	case 1:
		if((trg & CONT_CANCEL) || ((trg & U_JPAD) && !topFlag)) {
			st->flow--;
			sound = SE_mCANCEL;
		}
		break;
	}

	// サウンドを再生
	if(sound >= 0) {
		dm_snd_play(sound);
	}
}

// 更新
static void menuPlay2Panel_update(SMenuPlay2Panel *st, SMenuItem *parent)
{
	SMenuItem *item;
	int i;

	// フラッシュの難易度
	if(st->FL_TA) {
		st->glvAsk->select = st->glvItem->select;
	}
	// ウィルスレベル
	else {
		st->lvNum->number = st->lvGauge->level;
	}
	st->speedAsk->select = st->speedItem->select;

	// 下地
	menuItem_update(st->miBase, parent);

	// プレイヤー
	menuItem_update(st->miPlayer, st->miBase);
	menuItem_update(st->miPlayerNo, st->miBase);

	// 耐久 or タイムアタック 用ゲームレベル設定パネル
	if(st->FL_TA) {
		menuSpeedAsk_update(st->glvAsk, st->miBase);
		menuSpeedItem_update(st->glvItem, st->miBase);
	}
	else {
		menuNumber_update(st->lvNum, st->miBase);
		menuLvGauge_update(st->lvGauge, st->miBase);
	}

	// スピード
	menuSpeedAsk_update(st->speedAsk, st->miBase);
	menuSpeedItem_update(st->speedItem, st->miBase);

	// キャラアニメーションを更新
	switch(st->size) {
	case 0:
		break;

	case 1:
		if(!st->flags.loaded) break;
		item = st->miPlayer;
		st->animeState->color[0] = item->color[0] * 255;
		st->animeState->color[1] = item->color[1] * 255;
		st->animeState->color[2] = item->color[2] * 255;
		st->animeState->color[3] = item->color[3] * 255;
		animeState_update(st->animeState);
		break;
	}

	// OK
	menuItem_update(st->miOk, st->miBase);
	if(st->flow == 1) {
		item = st->miOk;
		for(i = 0; i < ARRAY_SIZE(item->color); i++) {
			item->color[i] = parent->color[i];
		}
	}

	// カーソル
	for(i = 0; i < ARRAY_SIZE(st->cursor); i++) {
		menuCursor_update(&st->cursor[i], st->miBase);
	}

	// パネルの色とか
	menuItem_setColorDir(st->miBase, (st->flow == 0 ? 1 : -1));

	i = (st->flow == 0 && st->depth == 0) ? 1 : 0;
	st->cursor[0].flags.cursor = i;
	if(st->FL_TA) {
		st->glvItem->cursor->flags.finger = i;
		st->glvItem->flags.blink = i;
	}
	else {
		st->lvGauge->cursor->flags.finger = i;
		st->lvGauge->cursor->flags.cursor = i;
		st->lvGauge->cursor->flags.blink = i;
	}

	i = (st->flow == 0 && st->depth == 1) ? 1 : 0;
	st->cursor[1].flags.cursor = i;
	st->speedItem->flags.blink = i;
	st->speedItem->cursor->flags.finger = i;
}

// 描画
static void menuPlay2Panel_draw(SMenuPlay2Panel **stAry, int count, Gfx **gpp)
{
	static const int _charTbl[] = {
		 0, // DMC_MARIO,
		 1, // DMC_WARIO,
		 2, // DMC_MAYU,
		 3, // DMC_YARI,
		 7, // DMC_RINGO,
		 5, // DMC_FUSEN,
		 8, // DMC_KAERU,
		 4, // DMC_KURAGE,
		 9, // DMC_IKA,
		 6, // DMC_KUMO,
		10, // DMC_ROBO,
		11, // DMC_MAD,
		12, // DMC_NAZO,
		14, // DMC_VWARIO,
		13, // DMC_MMARIO,
	};
	static const int _type[][4] = {
		{ 0, 4, 7, 9 },
		{ 0, 1, 5, 8 },
		{ 0, 1, 2, 6 },
		{ 0, 1, 2, 3 },
	};

	Gfx *gp = *gpp;
	SMenuPlay2Panel *st;
	SMenuItem *item;
	STexInfo *texC, *texA;
	void *list[8][count];
	int i, j, cached;

	for(i = 0; i < count; i++) {
		st = stAry[i];
		list[0][i] = st->lvNum;
		list[1][i] = st->lvGauge;
		list[2][i] = st->speedAsk;
		list[3][i] = st->speedItem;
		list[4][i] = st->glvAsk;
		list[5][i] = st->glvItem;
		list[6][i] = &st->cursor[0];
		list[7][i] = &st->cursor[1];
	}

	gSPDisplayList(gp++, fade_normal_texture_init_dl);

	// 下地
	for(i = 0; i < count; i++) {
		static const int _texPanelP4[2] = {
			TEX_p4_battle_panel, TEX_p4_newmode_panel,
		};
		static const int _texPanelP2[2] = {
			TEX_p2_battle_panel, TEX_p2_newmode_panel,
		};
		st = stAry[i];
		switch(st->size) {
		case 0:
			texC = _getTexP4(st->global, _texPanelP4[st->FL_TA ? 1 : 0]);
			break;
		case 1:
			texC = _getTexP2(st->global, _texPanelP2[st->FL_TA ? 1 : 0]);
			break;
		}
		menuItem_drawTex(st->miBase, &gp, texC, false);
	}

	// [LOW] [MED] [HI]
	menuSpeedAsk_draw((SMenuSpeedAsk **)list[2], count, &gp);
	menuSpeedItem_draw1((SMenuSpeedItem **)list[3], count, &gp);

	// 耐久 or タイムアタック 用ゲームレベル設定パネル
	if(st->FL_TA) {
		menuSpeedAsk_draw((SMenuSpeedAsk **)&list[4], count, &gp);
		menuSpeedItem_draw1((SMenuSpeedItem **)&list[5], count, &gp);
	}
	// ウィルスレベル
	else {
		menuNumber_draw((SMenuNumber **)&list[0], count, &gp);
		menuLvGauge_draw1((SMenuLvGauge **)&list[1], count, &gp);
	}

	// カーソル
	menuCursor_draw((SMenuCursor **)list[6], count, &gp);
	menuCursor_draw((SMenuCursor **)list[7], count, &gp);

	gSPDisplayList(gp++, fade_normal_texture_init_dl);

	// キャラ顔 or アニメーション を描画
	for(i = 0; i < count; i++) {
		st = stAry[i];
		item = st->miPlayer;

		switch(st->size) {
		case 0:
			texC = _getTexP4(st->global, TEX_character_face_b);
			menuItem_drawItem(item, &gp, texC, false, 15, _charTbl[st->charNo]);
			break;

		case 1:
			if(!st->flags.loaded) break;
			menuItem_setPrim(item, &gp);
			animeState_draw(st->animeState, &gp, item->trans[0], item->trans[1], -1, 1);
			break;
		}
	}

	// プレイヤー番号
	for(i = 0; i < count; i++) {
		st = stAry[i];
		texC = _getTexSetup(st->global, TEX_player_panel);

		if(st->playerNo == 1 && st->playerCount == 1 && st->cpuCount == 1) {
			j = 10;
		}
		else {
			j = _type[st->playerCount - 1][st->playerNo];
		}

		menuItem_drawItem(st->miPlayerNo, &gp, texC, false, 11, j);
	}

	gSPDisplayList(gp++, fade_alpha_texture_init_dl);

	// OK
	for(i = cached = 0; i < count; i++) {
		st = stAry[i];
		texC = _getTexSetup(st->global, TEX_ok);
		texA = _getTexSetup(st->global, TEX_ok_alpha);
		cached += menuItem_drawAlphaTex(st->miOk, &gp, texC, texA, cached);
	}

	// [LOW] [MED] [HI]
	menuSpeedItem_draw2((SMenuSpeedItem **)list[3], count, &gp);

	// 耐久 or タイムアタック 用ゲームレベル設定パネル
	if(st->FL_TA) {
		menuSpeedItem_draw2((SMenuSpeedItem **)&list[5], count, &gp);
	}
	// ウィルスレベル
	else {
		menuLvGauge_draw2((SMenuLvGauge **)&list[1], count, &gp);
	}

	*gpp = gp;
}

//////////////////////////////////////
//## 二人で遊ぶ用背景選択パネル

#define STAGE_BG_SIDE 16
#define STAGE_BG_WIDTH 72
#define STAGE_BG_HEIGHT 36

static const u8 _bgDataNo_to_stageNo[][2] = {
	{ 0, 1 }, { 0, 3 }, { 0, 0 }, { 1, 5 }, { 0, 5 },
	{ 0, 4 }, { 1, 4 }, { 0, 2 }, { 0, 6 }, { 0, 7 },
};

typedef struct {
	void *global;
	int depth;
	int flow;
	int topBgGap;

	SMenuItem miBase[1];
	SMenuMusicItem musicItem[1];
	SMenuNumber stageNum[1];
	SMenuItem miStageBg[1];
	SMenuItem miBgCursor[2];
	SMenuCursor cursor[2];
} SMenuPlay2PanelSub;

// フェードインアウト
static void menuPlay2PanelSub_setFrame(SMenuPlay2PanelSub *st, int dir, float time)
{
	SMenuItem *item = st->miBase;
	item->transRange[0][1] = item->transRange[1][1] - SCREEN_HT;
	item->transStep = FRAME_MOVE_STEP;
	item->transTime = time;
	menuItem_setTransDir(item, dir);
}

// 背景をスクロール
static void menuPlay2PanelSub_setStageBgFrame(SMenuPlay2PanelSub *st, int dir, float time, float vec)
{
	SMenuItem *item = st->miStageBg;
	item->transRange[0][0] = item->transRange[1][0] + vec;
	item->transStep = FRAME_MOVE_STEP;
	item->transTime = time;
	menuItem_setTransDir(item, dir);
}

// 初期化
static void menuPlay2PanelSub_init(SMenuPlay2PanelSub *st, void *global,
	void **hpp, int stageNo, int musicNo, int x, int y)
{
	static const int _bgCursor[][2] = { { 131, 54 }, { 236, 54 } };
	static const int _cursor[][4] = { { 0, 0, 270, 34 }, { 0, 31, 270, 60 } };
	SMenuItem *item;
	int i;

	st->global = global;
	st->depth = 0;
	st->flow = 0;
	st->topBgGap = -1;

	// 下地
	menuItem_init(st->miBase, x, y);

	// 音楽
	menuMusicItem_init(st->musicItem, global, musicNo, 70, 9);

	// ステージ番号
	menuNumber_init(st->stageNum, global, 1, 2, stageNo, 96, 40);
	item = st->stageNum->miBase;
	menuItem_setScaleLow(item, 14.0 / 16.0, 15.0 / 16.0);
	menuItem_setScaleHi(item, 14.0 / 16.0, 15.0 / 16.0);

	// ステージ背景
	menuItem_init(st->miStageBg, 156, 48);

	// 背景選択可能を示すカーソル
	for(i = 0; i < ARRAY_SIZE(st->miBgCursor); i++) {
		menuItem_init(&st->miBgCursor[i], _bgCursor[i][0], _bgCursor[i][1]);
		menuItem_setColor_cursor(&st->miBgCursor[i]);
	}

	// 右側のカーソルは左右反転
	item = &st->miBgCursor[1];
	item->scaleRange[0][0] = -item->scaleRange[0][0];
	item->scaleRange[1][0] = -item->scaleRange[1][0];

	// カーソル
	for(i = 0; i < ARRAY_SIZE(st->cursor); i++) {
		menuCursor_init(&st->cursor[i], global, CURSOR_ITEM, 0,
			_cursor[i][0], _cursor[i][1], _cursor[i][2], _cursor[i][3]);
	}

	// フレームアウト完了状態です
	menuPlay2PanelSub_setFrame(st, -1, 0);
}

// 入力
static bool menuPlay2PanelSub_input(SMenuPlay2PanelSub *st, int playerNo)
{
	int rep = _getKeyRep(st->global, playerNo);
	int trg = _getKeyTrg(st->global, playerNo);
	int vec, depth, sound = -1;

	if(st->miBase->transStep < 0 || st->miBase->transTime < 1) {
		return false;
	}

	switch(st->flow) {
	case -1:
		st->flow++;

	case 0:
		vec = 0;
		if((rep & U_JPAD) || (trg & CONT_CANCEL)) vec--;
		if((rep & D_JPAD) || (trg & CONT_OK)) vec++;
		depth = CLAMP(0, 1, st->depth + vec);

		// サウンドを再生
		if(depth != st->depth) {
			sound = SE_mUpDown;
			st->depth = depth;
		}
		// ステージ or 音楽 を設定
		else {
			switch(st->depth) {
			// 音楽を設定
			case 0:
				menuMusicItem_input(st->musicItem, playerNo);
				break;

			// 背景を設定
			case 1:
				vec = 0;
				if(rep & L_JPAD) vec--;
				if(rep & R_JPAD) vec++;
				if(vec) {
					st->stageNum->number = WrapI(1, 11, st->stageNum->number + vec);
					st->topBgGap = (vec < 0 ? -1 : -2);
					sound = SE_mLeftRight;
					menuPlay2PanelSub_setStageBgFrame(st, 1, 0, STAGE_BG_WIDTH * vec);
				}
				break;
			}
		}

		if(sound >= 0) {
			break;
		}

		if((trg & CONT_OK) && st->depth == 1) {
			sound = SE_mDecide;
			st->flow++;
		}
		else if((trg & CONT_CANCEL) && st->depth == 0) {
			sound = SE_mCANCEL;
			st->flow--;
		}

		break;

	case 1:
		if(trg & CONT_CANCEL) {
			sound = SE_mCANCEL;
			st->flow--;
		}
		break;
	}

	if(sound >= 0) {
		dm_snd_play(sound);
	}

	return true;
}

// 更新
static void menuPlay2PanelSub_update(SMenuPlay2PanelSub *st, SMenuItem *parent)
{
	SMenuItem *item;
//	int charTbl[4], stageTbl[4];
	int i, j;

	// 下地
	menuItem_update(st->miBase, parent);

	// 音楽 & ステージ
	if(st->depth != 0) st->musicItem->cursor->miBase->colorTime = 1;
	menuMusicItem_update(st->musicItem, st->miBase);
	menuNumber_update(st->stageNum, st->miBase);

	// ステージ背景を更新
	menuItem_update(st->miStageBg, st->miBase);

	// 背景選択可能を示すカーソル
	menuItem_updateN(st->miBgCursor, ARRAY_SIZE(st->miBgCursor), st->miBase);

	// カーソル
	for(i = 0; i < ARRAY_SIZE(st->cursor); i++) {
		menuCursor_update(&st->cursor[i], st->miBase);
	}

	// パネルの色とか
	i = (st->depth == 0);
	st->cursor[0].flags.cursor = i;
	st->musicItem->cursor->flags.finger = i;
	st->musicItem->flags.blink = i;

	i = (st->depth == 1);
	st->cursor[1].flags.cursor = i;

	if(st->depth != 1) {
		for(i = 0; i < ARRAY_SIZE(st->miBgCursor); i++) {
			item = &st->miBgCursor[i];
			for(j = 0; j < 3; j++) {
				item->color[j] = item->colorRange[1][j] * parent->color[j];
			}
		}
	}
}

// 描画
static void menuPlay2PanelSub_draw(SMenuPlay2PanelSub *st, Gfx **gpp)
{
	Gfx *gp = *gpp;
	SMenuItem *item;
	STexInfo *texC, *texA;
	void *list[8];
	int i, j, cached;
	float x;

	gSPDisplayList(gp++, fade_normal_texture_init_dl);

	// 下地
	texC = _getTexSetup(st->global, TEX_setup_sub_panel);
	menuItem_drawTex(st->miBase, &gp, texC, false);

	// カーソル
	for(i = 0; i < ARRAY_SIZE(st->cursor); i++) {
		list[i] = &st->cursor[i];
	}
	menuCursor_draw((SMenuCursor **)list, ARRAY_SIZE(st->cursor), &gp);

	// 音楽
	list[0] = st->musicItem;
	menuMusicItem_draw((SMenuMusicItem **)list, 1, &gp);

	// ステージ数
	list[0] = st->stageNum;
	menuNumber_draw((SMenuNumber **)list, 1, &gp);

	gSPDisplayList(gp++, fade_normal_texture_init_dl);

	// ステージ背景を描画
	item = st->miStageBg;

	menuItem_setPrim(item, &gp);
	gfxSetScissor(&gp, 2,
		st->miBase->trans[0] + item->transRange[1][0] - STAGE_BG_SIDE,
		st->miBase->trans[1] + item->transRange[1][1],
		STAGE_BG_SIDE * 2 + STAGE_BG_WIDTH, STAGE_BG_HEIGHT);

	texC = _getTexSetup(st->global, TEX_all_bg);
	x = item->trans[0];
	item->trans[0] += st->topBgGap * STAGE_BG_WIDTH;
	for(i = 0; i < 4; i++) {
		j = WrapI(0, 10, st->stageNum->number + st->topBgGap + i - 1);
		menuItem_drawItem(item, &gp, texC, false, 10, j);
		item->trans[0] += STAGE_BG_WIDTH;
	}
	gDPSetScissor(gp++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WD - 1, SCREEN_HT - 1);
	item->trans[0] = x;

	gSPDisplayList(gp++, fade_normal_texture_init_dl);

	// 背景選択可能を示すカーソル
	texC = _getTexSetup(st->global, TEX_bg_cursor);
	for(i = cached = 0; i < ARRAY_SIZE(st->miBgCursor); i++) {
		cached += menuItem_drawTex(&st->miBgCursor[i], &gp, texC, cached);
	}

	*gpp = gp;
}

//////////////////////////////////////////////////////////////////////////////
//{### ゲームに関わる定義

#include "main.h"
#include "dm_virus_init.h"

//////////////////////////////////////////////////////////////////////////////
//{### メインからコールされるサブメイン

//////////////////////////////////////
//## メインメニュー

MODE _menuMain_lastMode = MODE_MAIN;
int  _menuMain_lastDepth;
int  _menuMain_lastSelect[8];

typedef struct {
	void *global;

	MODE mode;
	int depth;
	int select[8];

	int screenNo;
	int screenDir;

	int playSeqNo;

	float kasaCount;

	SMenuMainPanel panel[2];
	SMenuNameSelPanel nameSel[1];
	SMenuNameOpPanel nameOp[1];
	SMenuSndSelPanel sndSel[1];
	int current;

	const char *msgStr;
	SMsgWnd msgWnd[1];
	SMenuItem miMsgWnd[1];

	SMenuItem miKaSaMaRu[1];
	SMenuItem miSmoke[1];
	SMenuItem miScrWnd[1];
	SMenuItem miScreen[1];

	SMenuCont cont[1];
	SMenuYN yesNo[1];
	SMenuMes message[1];
} SMenuMain;

// パネルの座標地
enum { MAIN_PANEL_X = 25, MAIN_PANEL_Y = 58 };

// メッセージを設定
static bool menuMain_setMsgStr(SMenuMain *st, MODE mode, int select)
{
	static const char *_tblMain[] = {
		_mesP1, _mes2P, _mes4P, _mesOpt,
	};
	static const char *_tblPlay1[] = {
		_mesLv, _mesSt, _mesVsCom, _mesVcFlash, _mesLvTaiQ, _mesLvTimeAt,
	};
	static const char *_tblPlay2[] = {
		_mesVsMan, _mesVmFlash, _mesVmTimeAt,
	};
	static const char *_tblPlay4[] = {
		_mesVsNum, _mesVsNum, _mesVsNum, _mesVsNum,
	};
	static const char *_tblBaTyp[] = {
		_mesVs4P, _mes4PTag, _mes4PFlash,
	};
	static const char *_tblOpt[] = {
		_mesRec, _mesTutol, _mesName, _mesBak, _mesMisc,
	};
	static const char *_tblTutol[] = {
		_mesRule1, _mesRule2, _mesRule3, _mesRule4, _mesKotu,
	};
	static const char *_tblSound[] = {
		_mesStereo, _mesMono, _mesSound, _mesSound,
	};
	static const char *_tblMisc[] = {
		_mesSnd, _mesVsCnt, _mesScore,
	};

	static const char *_loopTbl[] = {
		_mesSt, _mesLv, _mesVsCom, _mesVcFlash, _mesLvTaiQ, _mesLvTimeAt,
		_mesVsMan, _mesVmFlash, _mesVmTimeAt,
		_mesVs4P, _mes4PTag, _mes4PFlash,
		_mesSnd, _mesNs1, _mesNameErase, _mesDoErase,
	};

	const char **tbl, *mes;
	bool res = false;
	int i;

	tbl = NULL;
	mes = NULL;

	switch(mode) {
	case MODE_MAIN:
		tbl = _tblMain;
		break;

	case MODE_PLAY1:
		tbl = _tblPlay1;
		break;

	case MODE_PLAY2:
		tbl = _tblPlay2;
		break;

	case MODE_PLAY4:
		mes = _mesVsNum;
		break;

	case MODE_OPTION:
		tbl = _tblOpt;
		break;

	case MODE_MISC:
		tbl = _tblMisc;
		break;

	case MODE_SCORE:
		mes = _mesScoreOnOff;
		break;

	case MODE_PLAY4_TYPE1:
	case MODE_PLAY4_TYPE2:
	case MODE_PLAY4_TYPE3:
	case MODE_PLAY4_TYPE4:
		tbl = _tblBaTyp;
		break;

	case MODE_TUTORIAL:
		tbl = _tblTutol;
		break;

	case MODE_SOUND:
		mes = _tblSound[st->sndSel->select];
		break;

	case MODE_RECORD_MS:
		mes = _mesRecMs1;
		break;

	case MODE_RECORD_PLAY1:
	case MODE_RECORD_PLAY2:
		mes = _mesRecMs2;
		break;

	case MODE_NAME:
		mes = _mesNameSel;
		break;

	case MODE_NAME_DEL_YN:
		mes = _mesNameErase;
		break;

	case MODE_NAME_DEL_MES:
		mes = _mesNameErase2;
		break;

	case MODE_COUNT:
		mes = _mesVsCnt2;
		break;

	case MODE_BACKUP:
	case MODE_BACKUP_YN:
		mes = _mesDoErase;
		break;

	case MODE_BACKUP_MES:
		mes = _mesDoErase1;
		break;

	case MODE_STORY_NS:
	case MODE_LVSEL_NS:
	case MODE_LVSEL_TQ_NS:
	case MODE_LVSEL_TA_NS:

	case MODE_VSCOM_NS:
	case MODE_VSCOM_FL_NS:

	case MODE_VSMAN_NS:
	case MODE_VSMAN_FL_NS:
	case MODE_VSMAN_TA_NS:
		mes = _mesNs1;
		break;

	case MODE_NAME_NS:
		mes = _mesNs2;
		break;
	}

	if(tbl) {
		mes = tbl[select];
	}

	for(i = 0; i < ARRAY_SIZE(_loopTbl); i++) {
		if(mes == _loopTbl[i] && msgWnd_isEnd(st->msgWnd)) {
			st->msgStr = NULL;
		}
	}

	if(mes != st->msgStr) {
		msgWnd_clear(st->msgWnd);
		if(mes) {
			msgWnd_addStr(st->msgWnd, mes);
			res = true;
		}
		st->msgStr = mes;
	}

	return res;
}

// パネルを初期化
static void menuMain_initPanel(SMenuMain *st, MODE mode, int panelSw, int select)
{
	//一人で遊ぶ、二人で遊ぶ、みんなで遊ぶ、オプション
	static const int _root[] = { 0, 1, 2, 3 };
	// お話、エンドレス、ＶＳＣＯＭ、フラッシュ、耐久、タイムアタック
	static const int _play1[] = { 5, 4, 6, 30, 31, 32 };
	// ＶＳ２Ｐ、フラッシュ、タイムアタック
	static const int _play2[] = { 29, 30, 32 };
	// 一人、二人、三人、四人
	static const int _play4[] = { 9, 10, 11, 12 };
	// VS 4P, タッグバトル, フラッシュモード
	static const int _play4type[] = { 37, 38, 30 };

	// 記録、プレーの仕方、名前、サウンド、バックアップ
//	static const int _option[] = { 13, 14, 15, 39, 16, 17 };
	// 記録、プレーの仕方、名前、バックアップ、その他
	static const int _option[] = { 13, 14, 15, 17, 43 };
	// 一人用、二人用
	static const int _recMs[] = { 33, 34 };
	// おはなし、エンドレス、ＶＳＣＯＭ、フラッシュ耐久、スコアアタック
	static const int _recMs1[] = { 5, 4, 6, 30, 31, 32 };
	// ＶＳ２Ｐ、フラッシュ、スコアアタック
	static const int _recMs2[] = { 29, 30, 32 };
	// 操作の仕方, 基本ルール, 二人対戦のルール, 四人対戦のルール, 上達への道
	static const int _tutorial[] = { 19, 20, 21, 22, 23 };
	// １回、２回、３回
	static const int _count[] = { 40, 41, 42 };
//	static const int _count[] = { 42, 41, 40 };
	// やっぱり止める, バックアップを消す
	static const int _backup[] = { 28, 17 };

	// サウンド, 対戦回数, スコア表示
	static const int _misc[] = { 16, 39, 44 };
	// ON OFF
	static const int _score[] = { 45, 46 };

	const int *table;
	int count;

	switch(mode) {
	case MODE_MAIN:
		table = _root;
		count = ARRAY_SIZE(_root);
		break;

	case MODE_PLAY1:
		table = _play1;
		count = ARRAY_SIZE(_play1);
		break;

	case MODE_PLAY2:
		table = _play2;
		count = ARRAY_SIZE(_play2);
		break;

	case MODE_PLAY4:
		table = _play4;
		count = ARRAY_SIZE(_play4);
		break;

	case MODE_OPTION:
		table = _option;
		count = ARRAY_SIZE(_option);
		break;

	case MODE_PLAY4_TYPE1:
	case MODE_PLAY4_TYPE2:
	case MODE_PLAY4_TYPE3:
	case MODE_PLAY4_TYPE4:
		table = _play4type;
		count = ARRAY_SIZE(_play4type);
		break;

	case MODE_RECORD_MS:
		table = _recMs;
		count = ARRAY_SIZE(_recMs);
		break;

	case MODE_RECORD_PLAY1:
		table = _recMs1;
		count = ARRAY_SIZE(_recMs1);
		break;

	case MODE_RECORD_PLAY2:
		table = _recMs2;
		count = ARRAY_SIZE(_recMs2);
		break;

	case MODE_TUTORIAL:
		table = _tutorial;
		count = ARRAY_SIZE(_tutorial);
		break;

	case MODE_COUNT:
		table = _count;
		count = ARRAY_SIZE(_count);
		break;

	case MODE_BACKUP:
		table = _backup;
		count = ARRAY_SIZE(_backup);
		break;

	case MODE_MISC:
		table = _misc;
		count = ARRAY_SIZE(_misc);
		break;

	case MODE_SCORE:
		table = _score;
		count = ARRAY_SIZE(_score);
		break;

	default:
		table = _root;
		count = ARRAY_SIZE(_root);
		break;
	}

	// パネルを初期化
	menuMainPanel_init(&st->panel[panelSw], st->global,
		count, select, table, MAIN_PANEL_X, MAIN_PANEL_Y);

#if defined(USE_HALT)
	// ２Ｐ，４Ｐの項目を暗くする
	switch(mode) {
	case MODE_MAIN:
		menuItem_setColorDir(&st->panel[panelSw].miPanel[1], -1);
		st->panel[panelSw].miPanel[1].colorTime = 0;

		menuItem_setColorDir(&st->panel[panelSw].miPanel[2], -1);
		st->panel[panelSw].miPanel[2].colorTime = 0;
		break;

	case MODE_RECORD_MS:
		menuItem_setColorDir(&st->panel[panelSw].miPanel[1], -1);
		st->panel[panelSw].miPanel[1].colorTime = 0;
		break;
	}
#endif
}

// 初期化
static void menuMain_init(SMenuMain *st, void *global, void **hpp)
{
	SMenuItem *item;
	STexInfo *tex;
	int i;

	st->global = global;

	switch(_getModeOld(global)) {
	// 名前変更から戻ってきた場合
	case MODE_NAME_NE2:
		st->mode = _menuMain_lastMode = MODE_NAME_NS;
		break;

	default:
		st->mode = _menuMain_lastMode;
		break;
	}
	st->depth = _menuMain_lastDepth;

	for(i = 0; i < ARRAY_SIZE(st->select); i++) {
		st->select[i] = _menuMain_lastSelect[i];
	}

	if(_getModeOld(global) == MODE_MAIN) {
		st->select[0] = 0;
	}

	st->screenNo = 0;
	st->screenDir = -1;
	st->playSeqNo = SEQ_Menu;
	st->msgStr = NULL;
	st->current = 0;

	// 傘丸アニメーションカウンタ
	st->kasaCount = 0;

	// メインパネル
	for(i = 0; i < ARRAY_SIZE(st->panel); i++) {
		menuMain_initPanel(st, st->mode, i, st->select[st->depth]);
		menuMainPanel_setFrame(&st->panel[i], -1, 0);
	}

	// 名前選択パネル
	i = 1;
	switch(st->mode) {
	case MODE_VSMAN_NS:
	case MODE_VSMAN_FL_NS:
	case MODE_VSMAN_TA_NS:
		i = 2;
		break;
	}
	menuNameSelPanel_init(st->nameSel, global, st->mode != MODE_NAME_NS, i, MAIN_PANEL_X, MAIN_PANEL_Y);
	menuNameSelPanel_setFrame(st->nameSel, -1, 0);

	// 名前変更削除パネル
	menuNameOpPanel_init(st->nameOp, st->global, 0, 0, NULL, MAIN_PANEL_X, MAIN_PANEL_Y);
	menuNameOpPanel_setFrame(st->nameOp, -1, 0);

	// サウンドセレクトパネル
	menuSndSelPanel_init(st->sndSel, global, 0, MAIN_PANEL_X - 5, MAIN_PANEL_Y);
	menuSndSelPanel_setFrame(st->sndSel, -1, 0);

	// パネルをフレームイン
	switch(st->mode) {
	case MODE_STORY_NS:
	case MODE_LVSEL_NS:
	case MODE_LVSEL_TQ_NS:
	case MODE_LVSEL_TA_NS:

	case MODE_VSMAN_NS:
	case MODE_VSMAN_FL_NS:
	case MODE_VSMAN_TA_NS:

	case MODE_VSCOM_NS:
	case MODE_VSCOM_FL_NS:

	case MODE_NAME_NS:
		menuNameSelPanel_setFrame(st->nameSel, 1, 0);
		break;

	default:
		menuMainPanel_setFrame(&st->panel[0], 1, 0);
		break;
	}

	// メッセージ窓
	msgWnd_init(st->msgWnd, hpp, 12, 4, 156, 34);
	st->msgWnd->contFlags = 0;
	st->msgWnd->rowStep = 14;

	menuItem_init(st->miMsgWnd, 158, 47);
	item = st->miMsgWnd;
	item->transRange[0][0] += SCREEN_WD;
	item->transStep = FRAME_MOVE_STEP;
	item->transTime = 0;

	// 傘丸
	menuItem_init(st->miKaSaMaRu, 220, 99);
	item = st->miKaSaMaRu;
	item->transRange[0][0] += SCREEN_WD;
	item->transStep = FRAME_MOVE_STEP;
	item->transTime = 0;

	// ゲーム画面窓
	menuItem_init(st->miScrWnd, -68, 9+13);
	item = st->miScrWnd;
	item->transRange[0][0] = 32;
	item->transRange[0][1] = 54;
	item->transStep = FRAME_MOVE_STEP;
	item->transTime = 0;
	menuItem_setTransDir(item, -1);
	item->scaleStep = FRAME_MOVE_STEP;
	item->scaleTime = 0;
	menuItem_setScaleDir(item, -1);
	item->colorRange[1][0] = 255.0 / 255.0;
	item->colorRange[1][1] = 240.0 / 255.0;
	item->colorRange[1][2] = 248.0 / 255.0;

	// ゲームスクリーン
	menuItem_init(st->miScreen, 6, 5);
	st->miScreen->flags.localColor = 1;

	// 煙
	menuItem_init(st->miSmoke, 81, 30-13);

	// 操作の仕方
#if LOCAL==JAPAN
	menuCont_init(st->cont, global, 22, 87);
#elif LOCAL==AMERICA
	menuCont_init(st->cont, global, 14, 113);
#elif LOCAL==CHINA
	menuCont_init(st->cont, global, 14, 113);
#endif

	// はい, いいえ
	menuYN_init(st->yesNo, global, 120, 140);

	// メッセージ
#if LOCAL==JAPAN
	menuMes_init(st->message, global, hpp, 13, 2, 76, 166);
#elif LOCAL==AMERICA
//	menuMes_init(st->message, global, hpp, 17, 1, 64, 176);
	menuMes_init(st->message, global, hpp, 17, 2, 64, 166);
#elif LOCAL==CHINA
//	menuMes_init(st->message, global, hpp, 17, 1, 64, 176);
	menuMes_init(st->message, global, hpp, 17, 2, 64, 166);
#endif
}

// ゲーム開始直前のキャンセル
static void menuMain_waitCancel(SMenuMain *st)
{
	int trg = _getKeyTrg(st->global, 0);

	if((trg & CONT_CANCEL) && st->mode != MODE_MAIN) {
		_setFadeDir(st->global, -1);
		_setNextMain(st->global, MAIN_MENU);
		dm_snd_play(SE_mCANCEL);
	}
}

// EEPROMアクセス中の処理
static void _menuMain_updateCallback(void *args) {
	static void menuMain_update(SMenuMain *st);
	menuMain_update((SMenuMain *)args);
}

// 入力
static void menuMain_input(SMenuMain *st)
{
	SMenuMainPanel *panel = &st->panel[st->current];
	SMenuItem *item;
	int trg = _getKeyTrg(st->global, 0);
	int i, select, sound;
	int nameDir, panelDir, nameSelDir, wndDir, kasaDir, contDir, ynDir, mesDir, sndSelDir;

	// タイトルを設定
	switch(_getMode(st->global)) {
	case MODE_MAIN:
		_setTitle(st->global, st->mode);
		break;
	}

	i = false;

	switch(st->mode) {
	case MODE_BACKUP_MES:
	case MODE_NAME_DEL_MES:
		i = true;
		break;

	case MODE_BACKUP_YN:
	case MODE_NAME_DEL_YN:
		i = (i || menuYN_input(st->yesNo, 0));
		break;

	case MODE_SOUND:
		i = (i || menuSndSelPanel_input(st->sndSel, 0));
		break;

	default:
		i = (i || menuMainPanel_input(panel, 0));
		i = (i || menuNameSelPanel_input(st->nameSel));
		i = (i || menuCont_input(st->cont, 0));
		i = (i || menuNameOpPanel_input(st->nameOp, 0));
		break;
	}

	if(!i) {
		return;
	}

	panelDir = nameSelDir = wndDir = kasaDir = contDir = ynDir = mesDir = nameDir = sndSelDir = 0;
	sound = -1;

	switch(st->mode) {
	case MODE_STORY_NS:
	case MODE_LVSEL_NS:
	case MODE_LVSEL_TQ_NS:
	case MODE_LVSEL_TA_NS:

	case MODE_VSMAN_NS:
	case MODE_VSMAN_FL_NS:
	case MODE_VSMAN_TA_NS:

	case MODE_VSCOM_NS:
	case MODE_VSCOM_FL_NS:

	case MODE_NAME_NS:
		select = 0;
		break;

	case MODE_CONT:
		select = 0;
		break;

	case MODE_NAME_DEL_MES:
	case MODE_BACKUP_MES:
		select = 0;
		break;

	case MODE_NAME:
		select = st->nameOp->select;
		break;

	case MODE_SOUND:
		select = st->sndSel->select;
		break;

	case MODE_BACKUP_YN:
	case MODE_NAME_DEL_YN:
		select = st->yesNo->select;
		break;

	default:
		select = st->select[st->depth] = panel->select;
		break;
	}

	switch(st->mode) {
	// 0:一人で遊ぶ 1:二人で遊ぶ 2:みんなで遊ぶ 3:オプション を選択中
	case MODE_MAIN:
		if(trg & CONT_OK) {
			static const MODE tbl[] = {
				MODE_PLAY1, MODE_PLAY2, MODE_PLAY4, MODE_OPTION,
			};

			switch(tbl[select]) {
			case MODE_PLAY2:
				evs_playmax = joyResponseCheck();
				if(evs_playmax < 2) {
					menuMes_setStr(st->message, _mesNoCont2);
					mesDir++;
					sound = SE_mError;
				}
				else {
					st->mode = tbl[select];
					st->depth++;
					st->select[st->depth] = 0;
					panelDir++;
					sound = SE_mDecide;
				}
				break;

			default:
				st->mode = tbl[select];
				st->depth++;
				st->select[st->depth] = 0;
				panelDir++;
				sound = SE_mDecide;
				break;
			}

			if(mesDir == 0) {
				mesDir--;
			}
		}
		else if(trg & CONT_CANCEL) {
			if(st->message->miBase->colorStep < 0) {
				_setFadeDir(st->global, 1);
				_setNextMain(st->global, MAIN_TITLE);
				sound = SE_mCANCEL;
			}
			mesDir--;
		}
		else if(trg) {
			mesDir--;
		}
		break;

	// 0:エンドレス, 1:お話, 2:vsCOM, 3:フラッシュ, 4:耐久, 5:タイムアタック
	case MODE_PLAY1: {
		static const int _n[] = { 1, 0, 2, 3, 4, 5 };

		st->screenNo = _n[select];
		st->screenDir = 1;

		if(trg & CONT_OK) {
			static const MODE _mode[] = {
				MODE_LVSEL_NS, MODE_STORY_NS, MODE_VSCOM_NS,
				MODE_VSCOM_FL_NS, MODE_LVSEL_TQ_NS, MODE_LVSEL_TA_NS,
			};
			st->screenDir = -1;
			st->mode = _mode[select];

			switch(st->mode) {
			case MODE_VSCOM_FL_NS:
				evs_gamemode = GMD_FLASH;
				break;

			case MODE_LVSEL_TQ_NS:
				evs_gamemode = GMD_TaiQ;
				break;

			case MODE_LVSEL_TA_NS:
				evs_gamemode = GMD_TIME_ATTACK;
				break;

			default:
				evs_gamemode = GMD_NORMAL;
				break;
			}

			switch(st->mode) {
			case MODE_STORY_NS:
				evs_playcnt = 2;
				evs_story_flg = 1;
				evs_gamesel = GSL_VSCPU;
				game_state_data[0].player_type = PUF_PlayerMAN;
				game_state_data[1].player_type = PUF_PlayerCPU;
				nameSelDir++;
				break;

			case MODE_LVSEL_NS:
			case MODE_LVSEL_TQ_NS:
			case MODE_LVSEL_TA_NS:
				evs_playcnt = 1;
				evs_story_flg = 0;
				evs_gamesel = GSL_1PLAY;
				game_state_data[0].player_no = 0;
				game_state_data[0].player_type = PUF_PlayerMAN;
				game_state_data[0].charNo = 0;
				nameSelDir++;
				break;

			case MODE_VSCOM_NS:
			case MODE_VSCOM_FL_NS:
				evs_playcnt = 2;
				evs_story_flg = 0;
				evs_gamesel = GSL_VSCPU;
				game_state_data[0].player_type = PUF_PlayerMAN;
				game_state_data[1].player_type = PUF_PlayerCPU;
				nameSelDir++;
				break;
			}

			panelDir++;
			sound = SE_mDecide;
		}
		else if(trg & CONT_CANCEL) {
			st->screenDir = -1;
			st->mode = MODE_MAIN;
			st->depth--;
			panelDir++;
			sound = SE_mCANCEL;
		}
		} break;

	// 0:vs2p 1:フラッシュ 2:タイムアタック
	case MODE_PLAY2:
		st->screenNo = select + 6;
		st->screenDir = 1;

		if(trg & CONT_OK) {
			static const MODE mode[] = {
				MODE_VSMAN_NS, MODE_VSMAN_FL_NS, MODE_VSMAN_TA_NS,
			};
			st->screenDir = -1;
			st->mode = mode[select];
			evs_playcnt = 2;
			evs_story_flg = 0;
			evs_gamesel = GSL_2PLAY;
			game_state_data[0].player_type = PUF_PlayerMAN;
			game_state_data[1].player_type = PUF_PlayerMAN;
			nameSelDir++;
			panelDir++;
			sound = SE_mDecide;

			switch(st->mode) {
			case MODE_VSMAN_NS:
				evs_gamemode = GMD_NORMAL;
				break;

			case MODE_VSMAN_FL_NS:
				evs_gamemode = GMD_FLASH;
				break;

			case MODE_VSMAN_TA_NS:
				evs_gamemode = GMD_TIME_ATTACK;
				break;
			}
		}
		else if(trg & CONT_CANCEL) {
			st->screenDir = -1;
			st->mode = MODE_MAIN;
			st->depth--;
			panelDir++;
			sound = SE_mCANCEL;
		}
		break;

	// 0:一人 1:二人 2:三人 3:四人
	case MODE_PLAY4:
		st->screenNo = select + 12;
		st->screenDir = 1;

		if(trg & CONT_OK) {
			evs_playmax = joyResponseCheck();
			if(evs_playmax > select) {
				static const char _mode[] = {
					MODE_PLAY4_TYPE1, MODE_PLAY4_TYPE2,
					MODE_PLAY4_TYPE3, MODE_PLAY4_TYPE4,
				};
				st->screenDir = -1;
				st->mode = _mode[select];
				st->depth++;
				st->select[st->depth] = 0;

				evs_playcnt = 4;
				evs_story_flg = 0;
				evs_gamesel = GSL_4PLAY;
				evs_gamemode = GMD_NORMAL;
				for(i = 0; i < 4; i++) {
					game_state_data[i].player_type =
						(i <= select ? PUF_PlayerMAN : PUF_PlayerCPU);
				}

				panelDir++;
				mesDir--;
				sound = SE_mDecide;
			}
			else {
				static const char *mes[] = {
					_mesNoCont2, _mesNoCont3, _mesNoCont4,
				};
				menuMes_setStr(st->message, mes[select - 1]);
				mesDir++;
				sound = SE_mError;
			}
		}
		else if(trg & CONT_CANCEL) {
			if(st->message->miBase->colorStep < 0) {
				st->screenDir = -1;
				st->mode = MODE_MAIN;
				st->depth--;
				panelDir++;
				sound = SE_mCANCEL;
			}
			mesDir--;
		}
		else if(trg) {
			mesDir--;
		}
		break;

	// 名前選択
	case MODE_STORY_NS:
	case MODE_LVSEL_NS:
	case MODE_LVSEL_TQ_NS:
	case MODE_LVSEL_TA_NS:

	case MODE_VSMAN_NS:
	case MODE_VSMAN_FL_NS:
	case MODE_VSMAN_TA_NS:

	case MODE_VSCOM_NS:
	case MODE_VSCOM_FL_NS:

	case MODE_NAME_NS:
		if(st->nameSel->flow > 0) {
			static const MODE _tbl[][2] = {
				{ MODE_STORY,    MODE_OPTION  },
				{ MODE_STORY,    MODE_NAME    },
				{ MODE_STORY_NE, MODE_NAME_NE },
			};
			static const MODE _tblLS[][3] = {
				{ MODE_LVSEL,    MODE_LVSEL_TQ,    MODE_LVSEL_TA    },
				{ MODE_LVSEL,    MODE_LVSEL_TQ,    MODE_LVSEL_TA    },
				{ MODE_LVSEL_NE, MODE_LVSEL_TQ_NE, MODE_LVSEL_TA_NE },
			};
			static const MODE _tblVM[][3] = {
				{ MODE_VSMAN_CH, MODE_VSMAN_FL_CH, MODE_VSMAN_TA_CH },
				{ MODE_VSMAN_CH, MODE_VSMAN_FL_CH, MODE_VSMAN_TA_CH },
				{ MODE_VSMAN_NE, MODE_VSMAN_FL_NE, MODE_VSMAN_TA_NE },
			};
			static const MODE _tblVC[][2] = {
				{ MODE_VSCOM_CH, MODE_VSCOM_FL_CH },
				{ MODE_VSCOM_CH, MODE_VSCOM_FL_CH },
				{ MODE_VSCOM_NE, MODE_VSCOM_FL_NE },
			};
			MODE mode;
			int flow[2], ans;

			// 選択番号を設定
			for(i = 0; i < st->nameSel->playerCount; i++) {
				evs_select_name_no[i] = st->nameSel->select[i];
			}

			// 選択状況を取得
			for(i = 0; i < 2; i++) {
				flow[i] = 0;

				if(evs_select_name_no[i] == DM_MEM_GUEST) {
					continue;
				}
				flow[i]++;

				if(evs_mem_data[evs_select_name_no[i]].mem_use_flg & DM_MEM_USE) {
					continue;
				}
				flow[i]++;
			}
			ans = MAX(flow[0], flow[1]);

			switch(st->mode) {
			case MODE_STORY_NS:
				mode = _tbl[ans][0];
				break;

			case MODE_NAME_NS:
				mode = _tbl[ans][1];
				break;

			case MODE_LVSEL_NS:
				mode = _tblLS[ans][0];
				break;

			case MODE_LVSEL_TQ_NS:
				mode = _tblLS[ans][1];
				break;

			case MODE_LVSEL_TA_NS:
				mode = _tblLS[ans][2];
				break;

			case MODE_VSMAN_NS:
				mode = _tblVM[ans][0];
				break;

			case MODE_VSMAN_FL_NS:
				mode = _tblVM[ans][1];
				break;

			case MODE_VSMAN_TA_NS:
				mode = _tblVM[ans][2];
				break;

			case MODE_VSCOM_NS:
				mode = _tblVC[ans][0];
				break;

			case MODE_VSCOM_FL_NS:
				mode = _tblVC[ans][1];
				break;
			}

			switch(mode) {
			case MODE_OPTION:
				st->mode = mode;
				panelDir++;
				nameSelDir--;
				sound = SE_mCANCEL;
				break;

			case MODE_NAME:
				st->mode = mode;
				st->nameOp->select = 0;
				nameSelDir--;
				nameDir++;
				sound = SE_mDecide;
				break;

			default:
				_setMode(st->global, mode);
				panelDir++;
				nameSelDir--;
				wndDir--;
				kasaDir--;
				sound = SE_mDecide;
				break;
			}
		}
		else if(st->nameSel->flow < 0) {
			switch(st->mode) {
			case MODE_STORY_NS:
			case MODE_LVSEL_NS:
			case MODE_LVSEL_TQ_NS:
			case MODE_LVSEL_TA_NS:

			case MODE_VSCOM_NS:
			case MODE_VSCOM_FL_NS:
				st->mode = MODE_PLAY1;
				break;

			case MODE_VSMAN_NS:
			case MODE_VSMAN_FL_NS:
			case MODE_VSMAN_TA_NS:
				st->mode = MODE_PLAY2;
				break;

			case MODE_NAME_NS:
				st->mode = MODE_OPTION;
				break;
			}
			panelDir++;
			nameSelDir--;
			sound = SE_mCANCEL;
		}
		break;

	// 0:記録 1:チュートリアル 2:名前 3:対戦回数 4:サウンド 5:バックアップを消す
	case MODE_OPTION:
		if(trg & CONT_OK) {
			static const MODE tbl[] = {
				MODE_RECORD_MS, MODE_TUTORIAL, MODE_NAME_NS, MODE_BACKUP, MODE_MISC,
			};
			st->mode = tbl[select];

			switch(st->mode) {
			case MODE_NAME_NS:
				panelDir++;
				nameSelDir++;
				sound = SE_mDecide;
				break;

			case MODE_RECORD_MS:
			case MODE_TUTORIAL:
			case MODE_BACKUP:
			case MODE_MISC:
				st->depth++;
				st->select[st->depth] = 0;
				panelDir++;
				sound = SE_mDecide;
				break;
			}

			switch(st->mode) {
			case MODE_RECORD_MS:
				dm_seq_play_fade(SEQ_Staff, BGM_FADE_FRAME);
				break;
			case MODE_COUNT:
				st->select[st->depth] = 3 - evs_vs_count;
				break;
			}
		}
		else if(trg & CONT_CANCEL) {
			st->mode = MODE_MAIN;
			st->depth--;
			panelDir++;
			sound = SE_mCANCEL;
		}
		break;

	// 0:サウンド, 1:対戦回数, 2:スコア表示
	case MODE_MISC:
		if(trg & CONT_OK) {
			static const MODE tbl[] = {
				MODE_SOUND, MODE_COUNT, MODE_SCORE,
			};
			st->mode = tbl[select];

			switch(st->mode) {
			case MODE_SOUND:
				st->sndSel->select = (evs_stereo ? 0 : 1);
				panelDir++;
				sndSelDir++;
				sound = SE_mDecide;
				break;

			case MODE_COUNT:
			case MODE_SCORE:
				st->depth++;
				st->select[st->depth] = 0;
				panelDir++;
				sound = SE_mDecide;
				break;
			}

			switch(st->mode) {
			case MODE_COUNT:
				st->select[st->depth] = 3 - evs_vs_count;
				break;
			case MODE_SCORE:
				st->select[st->depth] = (evs_score_flag ? 0 : 1);
				break;
			}
		}
		else if(trg & CONT_CANCEL) {
			_eepWritePlayer(st->global);
			st->mode = MODE_OPTION;
			st->depth--;
			panelDir++;
			sound = SE_mCANCEL;
		}
		break;

	// 0:on 1:off
	case MODE_SCORE:
		if(trg & CONT_OK) {
			evs_score_flag = (select == 0 ? 1 : 0);
			st->mode = MODE_MISC;
			st->depth--;
			panelDir++;
			sound = SE_mDecide;
		}
		else if(trg & CONT_CANCEL) {
			st->mode = MODE_MISC;
			st->depth--;
			panelDir++;
			sound = SE_mCANCEL;
		}
		break;

	// 0:VS 4P 1:タッグバトル 2:フラッシュモード
	case MODE_PLAY4_TYPE1:
	case MODE_PLAY4_TYPE2:
	case MODE_PLAY4_TYPE3:
	case MODE_PLAY4_TYPE4:
		st->screenNo = select + 9;
		st->screenDir = 1;

		if(trg & CONT_OK) {
			static const char _team[][4] = {
				{ 0, 1, 2, 3 },
				{ 0, 0, 1, 1 },
				{ 0, 1, 2, 3 },
			};
			static const MODE _mode[] = {
				MODE_PLAY4_CH,
				MODE_PLAY4_TB_CH,
				MODE_PLAY4_FL_CH,
			};
			static const MODE _game[] = {
				GMD_NORMAL,
				GMD_NORMAL,
				GMD_FLASH,
			};

			for(i = 0; i < ARRAY_SIZE(_team[select]); i++) {
				game_state_data[i].team_no = _team[select][i];
			}

			evs_gamemode = _game[select];
			panelDir++;
			wndDir--;
			kasaDir--;
			sound = SE_mDecide;
			st->screenDir = -1;
			_setMode(st->global, _mode[select]);
		}
		else if(trg & CONT_CANCEL) {
			st->screenDir = -1;
			st->mode = MODE_PLAY4;
			st->depth--;
			panelDir++;
			sound = SE_mCANCEL;
		}
		break;

	// 0:一人用, 1:二人用
	case MODE_RECORD_MS:
		if(trg & CONT_OK) {
			static const MODE _mode[] = {
				MODE_RECORD_PLAY1, MODE_RECORD_PLAY2,
			};
			st->mode = _mode[select];
			st->depth++;
			st->select[st->depth] = 0;
			panelDir++;
			sound = SE_mDecide;
		}
		else if(trg & CONT_CANCEL) {
			st->mode = MODE_OPTION;
			st->depth--;
			panelDir++;
			sound = SE_mCANCEL;
			dm_seq_play_fade(SEQ_Menu, BGM_FADE_FRAME);
		}
		break;

	// 0:エンドレス, 1:お話, 2:vsCOM, 3:フラッシュ, 4:耐久, 5:スコアアタック
	// 0:vsMAN, 1:フラッシュ, 2:スコアアタック
	case MODE_RECORD_PLAY1:
	case MODE_RECORD_PLAY2:
		if(trg & CONT_OK) {
			static const MODE _mode1[] = {
				MODE_RECORD_LS, MODE_RECORD_ST, MODE_RECORD_VC,
				MODE_RECORD_VC_FL, MODE_RECORD_LS_TQ, MODE_RECORD_LS_TA,
			};
			static const MODE _mode2[] = {
				MODE_RECORD_VM, MODE_RECORD_VM_FL, MODE_RECORD_VM_TA,
			};
			MODE *modes = (st->mode == MODE_RECORD_PLAY1) ? _mode1 : _mode2;

			_setMode(st->global, modes[select]);
			panelDir++;
			wndDir--;
			kasaDir--;
			sound = SE_mDecide;
		}
		else if(trg & CONT_CANCEL) {
			st->mode = MODE_RECORD_MS;
			st->depth--;
			panelDir++;
			sound = SE_mCANCEL;
		}
		break;

	// 0:操作の仕方 1:基本ルール 2:２Ｐ対戦のルール 3:４Ｐ対戦のルール 4:上達への道
	case MODE_TUTORIAL:
		if(trg & CONT_OK) {
			switch(select) {
			case 0:
				st->mode = MODE_CONT;
				panelDir++;
				wndDir--;
				contDir++;
				sound = SE_mDecide;
				break;

			default:
				_setFadeDir(st->global, 1);
				_setNextMain(st->global, MAIN_MANUAL);
				evs_manual_no = select - 1;
				sound = SE_mDecide;
				break;
			}
		}
		else if(trg & CONT_CANCEL) {
			st->mode = MODE_OPTION;
			st->depth--;
			panelDir++;
			sound = SE_mCANCEL;
		}
		break;

	// 0:３回 1:２回 2:１回
	case MODE_COUNT:
		if(trg & CONT_OK) {
			evs_vs_count = 3 - select;
			st->mode = MODE_MISC;
			st->depth--;
			panelDir++;
			sndSelDir--;
			sound = SE_mDecide;
		}
		else if(trg & CONT_CANCEL) {
			st->mode = MODE_MISC;
			st->depth--;
			panelDir++;
			sound = SE_mCANCEL;
		}
		break;

	// 操作の仕方
	case MODE_CONT:
		menuCont_setFade(st->cont, 1, st->cont->miLabel->colorTime);

		if(trg & B_BUTTON) {
			st->mode = MODE_TUTORIAL;
			panelDir++;
			wndDir++;
			contDir--;
			sound = SE_mCANCEL;
		}
		break;

	// 0:変える 1:消す を選択中
	case MODE_NAME:
		if(trg & CONT_OK) {
			static const int tbl[] = { MODE_NAME_NE2, MODE_NAME_DEL_YN };
			MODE mode = tbl[select];

			switch(mode) {
			// 名前変更
			case MODE_NAME_NE2:
				_setMode(st->global, mode);
				panelDir++;
				wndDir--;
				kasaDir--;
				nameDir--;
				sound = SE_mDecide;
				break;

			// プレイヤー消去の確認
			case MODE_NAME_DEL_YN:
				st->mode = mode;
				st->nameOp->cursor->flags.finger = 0;
				st->yesNo->select = 0;
				ynDir++;
				sound = SE_mDecide;
				break;
			}
		}
		else if(trg & CONT_CANCEL) {
			st->mode = MODE_NAME_NS;
			nameSelDir++;
			nameDir--;
			sound = SE_mCANCEL;
		}
		break;

	// 0:ステレオ 1:モノラル を選択中
	case MODE_SOUND:
		if(trg & CONT_OK) {
			switch(select) {
			case 0: case 1:
				evs_stereo = (select == 0 ? 1 : 0);
				dm_audio_set_stereo(evs_stereo);
				st->mode = MODE_MISC;
				panelDir++;
				sndSelDir--;
				sound = SE_mDecide;
				if(st->playSeqNo != SEQ_Menu) {
					dm_seq_play_fade(SEQ_Menu, BGM_FADE_FRAME);
					st->playSeqNo = SEQ_Menu;
				}
				break;
			case 2:
				dm_seq_play_fade(st->sndSel->bgmNo, BGM_FADE_FRAME);
				st->playSeqNo = st->sndSel->bgmNo;
				break;
			case 3:
				dm_snd_play(st->sndSel->sndNo);
				break;
			}
		}
		else if(trg & CONT_CANCEL) {
			st->mode = MODE_MISC;
			panelDir++;
			sndSelDir--;
			sound = SE_mCANCEL;
			if(st->playSeqNo != SEQ_Menu) {
				dm_seq_play_fade(SEQ_Menu, BGM_FADE_FRAME);
				st->playSeqNo = SEQ_Menu;
			}
		}
		break;

	// 0:いいえ 1:はい を選択中
	case MODE_NAME_DEL_YN:
		st->yesNo->cursor->flags.finger = 1;

		if(trg & CONT_OK) {
			switch(select) {
			case 0:
				st->mode = MODE_NAME_NS;
				nameDir--;
				nameSelDir++;
				ynDir--;
				sound = SE_mCANCEL;
				break;
			case 1:
				dm_init_save_mem(&evs_mem_data[evs_select_name_no[0]]);
				_eepErasePlayer(st->global);
				st->mode = MODE_NAME_DEL_MES;
				st->yesNo->cursor->flags.finger = 0;
				sound = SE_mDataErase;
				break;
			}
		}
		else if(trg & CONT_CANCEL) {
			st->mode = MODE_NAME;
			st->nameOp->cursor->flags.finger = 1;
			ynDir--;
		}
		break;

	//
	case MODE_NAME_DEL_MES:
		if(trg & CONT_ANY) {
			st->mode = MODE_NAME_NS;
			nameDir--;
			nameSelDir++;
			ynDir--;
			sound = SE_mDecide;
		}
		break;

	// 0:やっぱり止める 1:バックアップを消す を選択中
	case MODE_BACKUP:
		panel->cursor->flags.finger = 1;

		// バックアップを削除
		if(trg & CONT_OK) {
			switch(select) {
			case 0:
				st->mode = MODE_OPTION;
				st->depth--;
				panelDir++;
				sound = SE_mCANCEL;
				break;
			case 1:
				st->mode = MODE_BACKUP_YN;
				st->yesNo->select = 0;
				panel->cursor->flags.finger = 0;
				ynDir++;
				sound = SE_mDecide;
				break;
			}
		}
		else if(trg & CONT_CANCEL) {
			st->mode = MODE_OPTION;
			st->depth--;
			panelDir++;
			sound = SE_mCANCEL;
		}
		break;

	// 0:いいえ 1:はい を選択中
	case MODE_BACKUP_YN:
		st->yesNo->cursor->flags.finger = 1;

		// バックアップを削除
		if(trg & CONT_OK) {
			switch(select) {
			case 0:
				st->mode = MODE_OPTION;
				st->depth--;
				ynDir--;
				panelDir++;
				sound = SE_mCANCEL;
				break;
			case 1:
				_eepEraseData(st->global);
				st->mode = MODE_BACKUP_MES;
				st->yesNo->cursor->flags.finger = 0;
				sound = SE_mDataErase;
				break;
			}
		}
		else if(trg & CONT_CANCEL) {
			st->mode = MODE_BACKUP;
			ynDir--;
			sound = SE_mCANCEL;
		}
		break;

	//
	case MODE_BACKUP_MES:
		if(trg & CONT_ANY) {
			st->mode = MODE_OPTION;
			st->depth--;
			ynDir--;
			panelDir++;
			sound = SE_mDecide;
		}
		break;
	}

	// メッセージウィンドウを移動
	if(wndDir) {
		menuItem_setTransDir(st->miMsgWnd, wndDir);
	}

	if(kasaDir) {
		menuItem_setTransDir(st->miKaSaMaRu, kasaDir);
	}

	// 各種パネルをフレームアウト
	if(ynDir) {
		menuYN_setFadeDir(st->yesNo, ynDir);
	}
	if(mesDir) {
		menuMes_setFadeDir(st->message, mesDir);
	}

	if(contDir < 0) {
		menuCont_setFade(st->cont, -1, 1);
		menuCont_setFrame(st->cont, -1, st->cont->miBase->transTime);
	}
	if(nameDir < 0) {
		menuNameOpPanel_setFrame(st->nameOp, -1, 1);
	}
	if(nameSelDir < 0) {
		menuNameSelPanel_setFrame(st->nameSel, -1, st->nameSel->miBase->transTime);
	}
	if(sndSelDir < 0) {
		menuSndSelPanel_setFrame(st->sndSel, -1, st->sndSel->miBase->transTime);
	}
	if(panelDir != 0) {
		menuMainPanel_setFrame(panel, -1, panel->miBase->transTime);
		st->current = st->current ^ 1;
		panel = &st->panel[st->current];
	}

	// 各種パネルをフレームイン
	if(st->mode != _menuMain_lastMode) {
		if(contDir > 0) {
			menuCont_setFrame(st->cont, 1, st->cont->miBase->transTime);
		}
		else if(nameDir > 0) {
			bcopy(evs_mem_data[evs_select_name_no[0]].mem_name, st->nameOp->name, MEM_NAME_SIZE);
			menuNameOpPanel_setFrame(st->nameOp, 1, 0);
		}
		else if(sndSelDir > 0) {
			menuSndSelPanel_setFrame(st->sndSel, 1, st->sndSel->miBase->transTime);
		}
		else if(nameSelDir > 0) {
			float time = st->nameSel->miBase->transTime;
			int playerCount = 1;

			switch(st->mode) {
			case MODE_VSMAN_NS:
			case MODE_VSMAN_FL_NS:
			case MODE_VSMAN_TA_NS:
				playerCount = 2;
				break;
			}

			menuNameSelPanel_clear(st->nameSel, st->mode != MODE_NAME_NS, playerCount);
			menuNameSelPanel_setFrame(st->nameSel, 1, time);
		}
		else if(panelDir != 0) {
			float time = panel->miBase->transTime;
			menuMain_initPanel(st, st->mode, st->current, st->select[st->depth]);
			menuMainPanel_setFrame(panel, 1, time);
		}
	}

//	DBG1(st->depth != _menuMain_lastDepth, "depth = %d\n", st->depth);

	_menuMain_lastMode = st->mode;
	_menuMain_lastDepth = st->depth;

	for(i = 0; i < ARRAY_SIZE(st->select); i++) {
		_menuMain_lastSelect[i] = st->select[i];
	}

	// サウンドを再生
	if(sound >= 0) {
		dm_snd_play(sound);
	}
}

// 更新
static void menuMain_update(SMenuMain *st)
{
	SMenuMainPanel *panel = &st->panel[st->current];
	SMenuItem *miRoot = _getRootItem(st->global);
	SMenuItem *item;
	int i;

	// モードセレクトパネル
	for(i = 0; i < ARRAY_SIZE(st->panel); i++) {
		menuMainPanel_update(&st->panel[i], miRoot);
	}

	// 名前選択パネル
	menuNameSelPanel_update(st->nameSel, miRoot);

	// 名前変更削除パネル
	menuNameOpPanel_update(st->nameOp, miRoot);

	// サウンドセレクトパネル
	menuSndSelPanel_update(st->sndSel, miRoot);

	// メッセージ
	item = st->miMsgWnd;
	menuItem_update(item, miRoot);
	st->msgWnd->posX = item->trans[0] + item->scale[0] * 6;
	st->msgWnd->posY = item->trans[1] + item->scale[1] * 6;
//	st->msgWnd->fontType = 1;

	if(!menuMain_setMsgStr(st, st->mode, st->select[st->depth])) {
		msgWnd_update(st->msgWnd);
	}

	// 傘丸
	menuItem_update(st->miKaSaMaRu, miRoot);

	// ゲーム画面窓
	menuItem_setTransDir(st->miScrWnd, st->screenDir);
	menuItem_setScaleDir(st->miScrWnd, st->screenDir);
	menuItem_update(st->miScrWnd, st->miKaSaMaRu);

	// 煙
	menuItem_update(st->miSmoke, st->miScrWnd);

	// ゲーム画面
	menuItem_update(st->miScreen, st->miScrWnd);
	for(i = 0; i < ARRAY_SIZE(st->miScreen->color); i++) {
		st->miScreen->color[i] = miRoot->color[i];
	}

	// 操作の仕方
	menuCont_update(st->cont, miRoot);

	// 傘丸アニメーション
	st->kasaCount = WrapF(0, 1, st->kasaCount + 1.0 / 128);

	// はい, いいえ
	menuYN_update(st->yesNo, miRoot);

	// メッセージ
	menuMes_update(st->message, miRoot);

	// カーソルを明るくする
	st->nameOp->cursor->flags.blink = 1;
	panel->cursor->flags.blink = 1;
	switch(st->mode) {
	case MODE_NAME_DEL_YN:
	case MODE_NAME_DEL_MES:
//		item = st->nameOp->cursor->miCursor;
//		for(i = 0; i < 3; i++) {
//			item->color[i] = item->colorRange[1][i];
//		}
		st->nameOp->cursor->flags.blink = 0;
		break;

	case MODE_BACKUP_YN:
	case MODE_BACKUP_MES:
//		item = panel->cursor->miCursor;
//		for(i = 0; i < 3; i++) {
//			item->color[i] = item->colorRange[1][i];
//		}
		panel->cursor->flags.blink = 0;
		break;
	}

	// はい、いいえ 選択カーソルを明るくする
	switch(st->mode) {
	case MODE_NAME_DEL_MES:
	case MODE_BACKUP_MES:
//		item = st->yesNo->cursor->miCursor;
//		for(i = 0; i < 3; i++) {
//			item->color[i] = item->colorRange[1][i];
//		}
		st->yesNo->cursor->flags.blink = 0;
		break;
	}
}

// 傘丸を描画
static void menuMain_drawKaSaMaRu(SMenuMain *st, Gfx **gpp)
{
	static const int _pat[] = { 0, 1, 2, 3, 2, 1 };
	Gfx *gp = *gpp;
	Mtx *mp = *_getMtxPtr(st->global);
	Vtx *vp = *_getVtxPtr(st->global);
	SMenuItem *item;
	STexInfo *texC, *texA;
	int i, width, height, size;
	float mfRot[4][4];
	float amp, angle[3];

	// 上下の揺れ具合
	amp = st->miScrWnd->scaleTime * 3;
	for(i = 0; i < ARRAY_SIZE(angle); i++) {
		angle[i] = WrapF(0, 1, st->kasaCount - i * (1.0 / 8)) * M_PI * 2;
	}

	// 下拵え
	guOrtho(mp, 0, SCREEN_WD, SCREEN_HT, 0, 1, 10, 1);
	gSPMatrix(gp++, mp, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
	mp++;

	guTranslate(mp, 0, 0, -5);
	gSPMatrix(gp++, mp, G_MTX_PROJECTION | G_MTX_MUL | G_MTX_NOPUSH);
	mp++;

	gSPClearGeometryMode(gp++, 0|G_ZBUFFER|G_SHADE|G_SHADING_SMOOTH|G_CULL_FRONT|G_CULL_BACK|G_LIGHTING|G_FOG|G_TEXTURE_GEN|G_TEXTURE_GEN_LINEAR|G_CLIPPING|G_LOD|G_TEXTURE_ENABLE);
	gDPSetTexturePersp(gp++, G_TP_NONE);

	guRotateRPYF(mfRot, 0, 0, sinf(angle[0]) * (1 - st->miScrWnd->scaleTime) * 4);

	// 傘丸
	i = WrapI(0, ARRAY_SIZE(_pat), st->kasaCount * ARRAY_SIZE(_pat) * 12);
	if(!msgWnd_isSpeaking(st->msgWnd)) i = 0;

	item = st->miKaSaMaRu;
	texC = _getTexKasa(st->global, TEX_kasamaru01 + _pat[i]);
	texA = _getTexKasa(st->global, TEX_kasamaru_alpha);
	width = MIN(texC->size[0], texA->size[0]);
	height = MIN(texC->size[1], texA->size[1]);

	if(!menuItem_outOfScreen(item, width, height)) {
		gSPDisplayList(gp++, fade_alpha_texture_init_dl);
		menuItem_setPrim(item, &gp);

		mfRot[3][0] = item->trans[0];
		mfRot[3][1] = item->trans[1] + sinf(angle[0]) * amp;
		guMtxF2L(mfRot, mp);
		gSPMatrix(gp++, mp, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
		mp++;

		RectAlphaTexTile(&gp, &vp, width, height,
			texC->addr[1], texC->size[0],
			texA->addr[1], texA->size[0],
			0, 0, width, height,
			0, 0, width, height);

//		// お口
//		if(msgWnd_isSpeaking(st->msgWnd) &&
//			((int)(st->kasaCount * 32) & 1))
//		{
//			gSPDisplayList(gp++, fade_normal_texture_init_dl);
//			menuItem_setPrim(item, &gp);
//
//			texC = _getTexKasa(st->global, TEX_kasamaru_mouth);
//			RectTexTile8(&gp, &vp,
//				texC->size[0], texC->size[1],
//				texC->addr[0], texC->addr[1],
//				0, 0, texC->size[0], texC->size[1],
//				24, 61, texC->size[0], texC->size[1]);
//		}
	}

	// ゲーム画面
	item = st->miScreen;
	if(st->screenNo < 12) {
		texC = _getTexMain(st->global, TEX_game_screen);
		i = st->screenNo;
		size = 12;
	}
	else {
#if LOCAL==CHINA
            goto skip_numberscreen;
#else
		texC = _getTexMain(st->global, TEX_number_screen);
		i = st->screenNo - 12;
		size = 4;
#endif
	}
	width = texC->size[0];
	height = texC->size[1] / size;

	if(!menuItem_outOfScreen(item, texC->size[0], texC->size[1])) {
		gSPDisplayList(gp++, fade_normal_texture_init_dl);
		menuItem_setPrim(item, &gp);

		mfRot[3][0] = item->trans[0];
		mfRot[3][1] = item->trans[1] + sinf(angle[2]) * amp;
		guMtxF2L(mfRot, mp);
		gSPMatrix(gp++, mp, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
		mp++;

		RectTexTile8(&gp, &vp, texC->size[0], texC->size[1], texC->addr[0],
			(u8 *)texC->addr[1] + width * height * i,
			0, 0, width, height,
			0, 0, width * item->scale[0], height * item->scale[1]);
	}

	gSPDisplayList(gp++, fade_intensity_texture_init_dl);

	// ゲーム画面窓, 煙
	for(i = 0; i < 2; i++) {
		int gapW, gapH;

		switch(i) {
		case 0:
			item = st->miSmoke;
			texC = _getTexMain(st->global, TEX_smoke);
			gapW = 0;
			gapH = 0;
			break;
		case 1:
			item = st->miScrWnd;
			texC = _getTexMain(st->global, TEX_game_window);
			gapW = 0;
			gapH = 0;
			break;
		}

		if(!menuItem_outOfScreen(item, texC->size[0], texC->size[1])) {
			menuItem_setPrim(item, &gp);

			mfRot[3][0] = item->trans[0];
			mfRot[3][1] = item->trans[1] + sinf(angle[i + 1]) * amp;
			guMtxF2L(mfRot, mp);
			gSPMatrix(gp++, mp, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
			mp++;

			RectTexTile4i(&gp, &vp,
				texC->size[0], texC->size[1], texC->addr[1],
				0, 0, texC->size[0], texC->size[1], 0, 0,
				(texC->size[0] + gapW) * item->scale[0],
				(texC->size[1] + gapH) * item->scale[1]);
		}
	}
skip_numberscreen:

	// 〆
	*_getVtxPtr(st->global) = vp;
	*_getMtxPtr(st->global) = mp;
	*gpp = gp;
}

// 描画
static void menuMain_draw(SMenuMain *st, Gfx **gpp)
{
	Gfx *gp = *gpp;
	STexInfo *texC, *texA;
	int i;

	gSPDisplayList(gp++, fade_normal_texture_init_dl);

	// メッセージウィンドウの下地
	texC = _getTexMain(st->global, TEX_message_window);
	menuItem_drawTex(st->miMsgWnd, &gp, texC, false);

	// メッセージ
	if(!menuItem_outOfScreen(st->miMsgWnd,
		st->msgWnd->colStep * st->msgWnd->colSize,
		st->msgWnd->rowStep * st->msgWnd->rowSize))
	{
		msgWnd_draw(st->msgWnd, &gp);
	}

	// 名前変更削除パネル
	menuNameOpPanel_draw(st->nameOp, &gp);

	// 名前選択パネル
	menuNameSelPanel_draw(st->nameSel, &gp);

	// サウンドセレクトパネル
	menuSndSelPanel_draw(st->sndSel, &gp);

	// モードセレクトパネル
	for(i = 0; i < ARRAY_SIZE(st->panel); i++) {
		menuMainPanel_draw(&st->panel[i], &gp);
	}

	// 傘丸
	menuMain_drawKaSaMaRu(st, &gp);

	// 操作の仕方
	menuCont_draw(st->cont, &gp);

	// はい, いいえ
	menuYN_draw(st->yesNo, &gp);

	// メッセージ
	menuMes_draw(st->message, &gp);

	*gpp = gp;
}

//////////////////////////////////////
//## ストーリー

typedef struct {
	void *global;

	int stageMax[4][2];
	int depth;
	int select[3];
	int loaded;
	void *animeAddr[2];

	SMenuItem miBase[1];
	SMenuItem miCharName[1];
	SMenuItem miChar[2];
	SAnimeState animeState[2];
	SMenuCursor charCursor[2];
	SMenuSpeedAsk speedAsk[1];
	SMenuSpeedItem speedItem[1];
	SMenuNumber stageNum[1];
	SMenuItem miStageBase[1];
	SMenuItem miStageBg[2];
	SMenuItem miBgCursor[2];
	SMenuItem miWalk[2];
	SMenuCursor cursor[3];
} SMenuStory;

// ちびキャラの位置を取得
static void menuStory_getCharPos(int charNo, int stageNo, int plane, int *x, int *y)
{
	static const int _pos[][9][2] = {
		{{ 32, 20 }, {  81,  7 }, { 130, 23 }, { 179, 17 },
		 { 32, 20 }, {  81, 20 }, { 130, 23 }, { 191, 14 }, { 191, 14 }, },
		{{ 29, 19 }, {  78,  6 }, { 127, 22 }, { 176, 16 },
		 { 29, 19 }, { 101,  2 }, { 138, 20 }, { 188, 13 }, { 188, 13 }, },
	};
	int i = 0;

	switch(plane) {
	case 0:
		i = CLAMP(1, 4, stageNo);
		break;

	case 1:
		i = CLAMP(5, 9, stageNo);
		break;
	}

	*x = _pos[charNo][i - 1][0];
	*y = _pos[charNo][i - 1][1];
}

// フェードインアウト
static void menuStory_setFrame(SMenuStory *st, int dir, float time)
{
	SMenuItem *item = st->miBase;
	item->transRange[0][1] = item->transRange[1][1] - SCREEN_HT;
	item->transStep = FRAME_MOVE_STEP;
	item->transTime = time;
	menuItem_setTransDir(item, dir);
}

// アニメーションを読み込む
static void _menuStory_loadAnime(void *arg)
{
	SMenuStory *st = (SMenuStory *)arg;
	void *heap;
	int i;

	for(i = 0; i < ARRAY_SIZE(st->animeState); i++) {
		heap = st->animeAddr[i];
		animeState_load(&st->animeState[i], &heap, i);
	}

	st->loaded = 1;
}

// 初期化
static void menuStory_init(SMenuStory *st, void *global, void **hpp)
{
	static const int _posChar[][2] = {
		{ 160, 46 }, { 230, 46 },
	};
	static const int _posBgCursor[][2] = {
		{ -8, 17 }, { 232, 17 },
	};
	static const int _cursor[][4] = {
		{ 0, 0, 270, 54 }, { 0, 51, 270, 36 }, { 0, 84, 270, 78 },
	};

	mem_char *mc = &evs_mem_data[evs_select_name_no[0]];
	game_config *cfg = &mc->config;
	void *hp = *hpp;
	SMenuItem *item;
	int i;

	st->global = global;
	for(i = 0; i < ARRAY_SIZE(st->stageMax); i++) {
		st->stageMax[i][0] = mc->clear_stage[i][0] + 1;
		st->stageMax[i][1] = mc->clear_stage[i][1] + 1;
	}
	st->depth = 0;
	st->select[0] = cfg->st_no;
	st->loaded = 0;

	// 下地
	menuItem_init(st->miBase, 25, 47);

	// マリオ、ワリオ
	st->select[0] = cfg->st_no;
	menuItem_init(st->miCharName, 69, 18);
	for(i = 0; i < ARRAY_SIZE(st->miChar); i++) {
		menuItem_init(&st->miChar[i], _posChar[i][0], _posChar[i][1]);
	}

	// アニメーション
	for(i = 0; i < ARRAY_SIZE(st->animeState); i++) {
		st->animeAddr[i] = (void *)ALIGN_16(hp);
		hp = (u8 *)st->animeAddr[i] + animeState_getDataSize(i);
	}
	cpuTask_sendTask(_menuStory_loadAnime, st);

	// キャラ選択カーソル
	for(i = 0; i < ARRAY_SIZE(st->charCursor); i++) {
		menuCursor_init(&st->charCursor[i], global, CURSOR_ITEM, 0,
			_posChar[i][0] - 24, _posChar[i][1] - 44, 48, 47);
	}

	// [EASY] [NORMAL] [HARD]
	st->select[1] = cfg->st_lv;
	menuSpeedAsk_init(st->speedAsk, global, 3, cfg->st_lv, 69, 61);
	menuSpeedItem_init(st->speedItem, global, 3, 0, cfg->st_lv, 125, 57, 52);
	st->speedItem->flags.special = cfg->st_sh ? 1 : 0;

	// ステージ数
	st->select[2] = cfg->st_st + 1;
	menuNumber_init(st->stageNum, global, 1, 1, cfg->st_st + 1, 101, 90);
	menuItem_init(st->miStageBase, 16, 110);
	for(i = 0; i < ARRAY_SIZE(st->miStageBg); i++) {
		item = &st->miStageBg[i];
		menuItem_init(item, 0, 0);
		menuItem_setColor_fade(item);
		item->colorStep = FRAME_MOVE_STEP;
	}

	// 背景選択可能を示すカーソル
	for(i = 0; i < ARRAY_SIZE(st->miBgCursor); i++) {
		menuItem_init(&st->miBgCursor[i], _posBgCursor[i][0], _posBgCursor[i][1]);
		menuItem_setColor_cursor(&st->miBgCursor[i]);
	}

	// ちびキャラ
	for(i = 0; i < ARRAY_SIZE(st->miWalk); i++) {
		int x, y;
		menuStory_getCharPos(st->select[0], st->select[2], i, &x, &y);
		menuItem_init(&st->miWalk[i], x, y);
	}

	// 左側のカーソルは左右反転
	item = &st->miBgCursor[0];
	item->scaleRange[0][0] = -item->scaleRange[0][0];
	item->scaleRange[1][0] = -item->scaleRange[1][0];

	// カーソル
	for(i = 0; i < ARRAY_SIZE(st->cursor); i++) {
		menuCursor_init(&st->cursor[i], global, CURSOR_PANEL, 0,
			_cursor[i][0], _cursor[i][1], _cursor[i][2], _cursor[i][3]);
	}

	// フレームイン
	menuStory_setFrame(st, 1, 0);

	*hpp = hp;
}

// ゲーム開始直前のキャンセル
static void menuStory_waitCancel(SMenuStory *st)
{
	int trg = _getKeyTrg(st->global, 0);

	if(trg & CONT_CANCEL) {
		_setFadeDir(st->global, -1);
		_setNextMain(st->global, MAIN_MENU);
		dm_snd_play(SE_mCANCEL);
	}
}

// 選択中のゲームレベルを取得
static int menuStory_getLevel(SMenuStory *st)
{
	int level = st->speedItem->select;

	if(level == 2) {
		level += st->speedItem->flags.special;
	}

	return level;
}

// 選択可能なステージ数を取得
static int menuStory_getStageMax(SMenuStory *st)
{
	return st->stageMax[ menuStory_getLevel(st) ][ st->select[0] ];
}

// 入力
static void menuStory_input(SMenuStory *st)
{
	game_config *cfg = &evs_mem_data[evs_select_name_no[0]].config;
	game_state *state = &game_state_data[0];
	SMenuItem *item;
	int rep = _getKeyRep(st->global, 0);
	int trg = _getKeyTrg(st->global, 0);
	int i, vec, select, sound = -1;
	bool special = false;

	if(st->miBase->transTime != 1.0) {
		return;
	}

	vec = 0;
	if((rep & U_JPAD) || (trg & CONT_CANCEL)) vec--;
	if((rep & D_JPAD) || (trg & CONT_OK)) vec++;
	select = CLAMP(0, ARRAY_SIZE(st->select) - 1, st->depth + vec);

	if(select != st->depth) {
		sound = SE_mUpDown;
		st->depth = select;
	}

	if((trg & Z_TRIG) && st->depth == 1 && st->speedItem->select == 2) {
		if(!st->speedItem->flags.special) {
			st->speedItem->flags.special = 1;
			special = true;
		}
		else {
			st->speedItem->flags.special = 0;
			sound = SE_mDecide;
		}
	}

	vec = 0;
	if(rep & L_JPAD) vec--;
	if(rep & R_JPAD) vec++;

	switch(st->depth) {
	// キャラクターを選択中
	case 0:
		select = CLAMP(0, 1, st->select[st->depth] + vec);

		if(select != st->select[st->depth]) {
			sound = SE_mLeftRight;
			st->select[st->depth] = select;
		}
		break;

	// ゲームレベルを選択中
	case 1:
		menuSpeedItem_input(st->speedItem, 0);
		st->select[st->depth] = st->speedItem->select;
		break;

	// ステージを選択中
	case 2:
		i = menuStory_getStageMax(st);
		select = CLAMP(1, i, st->select[st->depth] + vec);

		if(select != st->select[st->depth]) {
			sound = SE_mLeftRight;
			st->select[st->depth] = select;

			for(i = 0; i < ARRAY_SIZE(st->miWalk); i++) {
				item = &st->miWalk[i];
				item->transTime = 0;
				item->transRange[0][0] = item->transRange[1][0];
				item->transRange[0][1] = item->transRange[1][1];
			}
		}
		break;
	}

	i = menuStory_getStageMax(st);
	st->select[2] = CLAMP(1, i, st->select[2]);

	if(sound < 0) {
		if((trg & CONT_OK) && st->depth == 2) {
			_setFadeDir(st->global, 1);
			_setNextMain(st->global, MAIN_STORY);

			cfg->st_lv = st->speedItem->select;
			cfg->st_sh = st->speedItem->flags.special;
			cfg->st_st = st->stageNum->number - 1;
			cfg->st_no = st->select[0];

			evs_story_level = menuStory_getLevel(st);
			evs_story_no = st->stageNum->number;

			i = (evs_story_no > 1) ? 1 : 0;
			game_state_data[0].game_retry = i;
			evs_one_game_flg = i;

			story_proc_no = st->stageNum->number;

			if(st->select[0] == 1) {
				story_proc_no += STORY_W_OPEN;
			}

			evs_game_time = 0;
			evs_high_score = DEFAULT_HIGH_SCORE;
			if(evs_story_level < 3) {
				for(i = 0; i < MEM_CHAR_SIZE; i++) {
					evs_high_score = MAX(
						evs_high_score, evs_mem_data[i].story_data[evs_story_level].score);
				}
			}

			sound = SE_mDecide;
		}
		else if((trg & CONT_CANCEL) && st->depth == 0) {
			_setMode(st->global, MODE_MAIN);
			menuStory_setFrame(st, -1, 1);

			sound = SE_mCANCEL;
		}
	}

	// サウンドを再生
	if(special) {
		dm_snd_play_strange_sound();
	}
	else if(sound >= 0) {
		dm_snd_play(sound);
	}
}

// 更新
static void menuStory_update(SMenuStory *st)
{
	SMenuItem *miRoot = _getRootItem(st->global);
	SMenuItem *item;
	SMenuCursor *cursor;
	int i, j;

	st->speedAsk->select = menuStory_getLevel(st);
	st->stageNum->number = st->select[2];

	// 下地
	menuItem_update(st->miBase, miRoot);

	// キャラ
	for(i = 0; i < ARRAY_SIZE(st->miChar); i++) {
		menuItem_setColorDir(&st->miChar[i], i == st->select[0] ? 1 : -1);
	}
	menuItem_update(st->miCharName, st->miBase);
	menuItem_updateN(st->miChar, ARRAY_SIZE(st->miChar), st->miBase);

	// アニメーション
	for(i = 0; i < ARRAY_SIZE(st->animeState); i++) {
		if(!st->loaded) continue;

		animeState_update(&st->animeState[i]);

		for(j = 0; j < ARRAY_SIZE(st->animeState[i].color); j++) {
			st->animeState[i].color[j] = st->miChar->color[j] * 255;
		}
	}

	// キャラ選択カーソル
	for(i = 0; i < ARRAY_SIZE(st->charCursor); i++) {
		menuCursor_update(&st->charCursor[i], st->miBase);
	}

	// ゲームレベル
	if(st->depth != 1) st->speedItem->cursor->miBase->colorTime = 1;
	menuSpeedAsk_update(st->speedAsk, st->miBase);
	menuSpeedItem_update(st->speedItem, st->miBase);

	// ステージ
	menuNumber_update(st->stageNum, st->miBase);
	menuItem_update(st->miStageBase, st->miBase);
	i = (st->select[2] <= 4 ? 1 : -1);
	menuItem_setColorDir(&st->miStageBg[0],  i);
	menuItem_setColorDir(&st->miStageBg[1], -i);
	menuItem_updateN(st->miStageBg, ARRAY_SIZE(st->miStageBg), st->miStageBase);

	// ちびキャラ
	for(i = 0; i < ARRAY_SIZE(st->miWalk); i++) {
		int x, y;
		menuStory_getCharPos(st->select[0], st->select[2], i, &x, &y);

		item = &st->miWalk[i];
		menuItem_setTransHi(item, x, y);
		menuItem_update(item, &st->miStageBg[i]);
	}

	// 背景選択可能を示すカーソル
	menuItem_updateN(st->miBgCursor, ARRAY_SIZE(st->miBgCursor), st->miStageBase);
	item = st->miBgCursor;
	i = menuStory_getStageMax(st);
	if(st->select[2] > 4 || i < 5) {
		item[1].color[3] = 0;
	}
	if(st->select[2] < 5) {
		item[0].color[3] = 0;
	}

	// カーソル
	for(i = 0; i < ARRAY_SIZE(st->cursor); i++) {
		menuCursor_update(&st->cursor[i], st->miBase);
	}

	// パネルの色とか
	j = (st->depth == 0);
	for(i = 0; i < ARRAY_SIZE(st->charCursor); i++) {
		st->cursor[0].flags.cursor = j;
		st->charCursor[i].flags.blink = j;
		st->charCursor[st->select[0]].flags.finger = (j && i == st->select[0]);
	}

	j = (st->depth == 1);
	st->cursor[1].flags.cursor = j;
//	st->speedItem->cursor->flags.blink = j;
	st->speedItem->cursor->flags.finger = j;
	st->speedItem->flags.blink = j;

	j = (st->depth == 2);
	st->cursor[2].flags.cursor = j;

//	if(st->depth != 0) {
//		for(i = 0; i < ARRAY_SIZE(st->charCursor); i++) {
//			item = st->charCursor[i].miCursor;
//			for(j = 0; j < ARRAY_SIZE(item->color) - 1; j++) {
//				item->color[j] = item->colorRange[1][j] * miRoot->color[j];
//			}
//		}
//	}
//	if(st->depth != 1) {
//		item = st->speedItem->cursor->miCursor;
//		for(i = 0; i < ARRAY_SIZE(item->color) - 1; i++) {
//			item->color[i] = item->colorRange[1][i] * miRoot->color[i];
//		}
//	}
	if(st->depth != 2) {
		for(i = 0; i < ARRAY_SIZE(st->miBgCursor); i++) {
			item = st->miBgCursor;
			for(j = 0; j < ARRAY_SIZE(item->color) - 1; j++) {
				item->color[j] = item->colorRange[1][j] * miRoot->color[j];
			}
		}
	}

//	for(i = 0; i < ARRAY_SIZE(st->cursor); i++) {
//		cursor = &st->cursor[i];
//		cursor->flags.cursor = 0;
//	}
//	for(i = 0; i < ARRAY_SIZE(st->charCursor); i++) {
//		cursor = &st->charCursor[i];
//		cursor->flags.finger = 0;
//	}
//	cursor = st->speedItem->cursor;
//	cursor->flags.finger = 0;
//
//	switch(st->depth) {
//	case 0:
//		st->cursor[0].flags.cursor = 1;
//		st->charCursor[st->select[0]].flags.finger = 1;
//		break;
//
//	case 1:
//		st->cursor[1].flags.cursor = 1;
//		st->speedItem->cursor->flags.finger = 1;
//		break;
//
//	case 2:
//		st->cursor[2].flags.cursor = 1;
//		break;
//	}
}

// 描画
static void menuStory_draw(SMenuStory *st, Gfx **gpp)
{
	static const int _label[] = { 0, 6, 2 };
	static const int _map[][2] = {
		{ TEX_map_mario_a, TEX_map_mario_b },
		{ TEX_map_mario_a, TEX_map_wario_b },
	};
	static const int _cover[][2] = {
		{ TEX_map_mario_a_cover, TEX_map_mario_b_cover },
		{ TEX_map_mario_a_cover, TEX_map_wario_b_cover },
	};
	static const int _filter[][2][9] = {
		{{  48,  97, 146, 195,  999, 999, 999, 999, 999 },
		 {   0,   0,   0,   0,   48,  97, 146, 999, 999 }},
		{{  48,  97, 146, 195,  999, 999, 999, 999, 999 },
		 {   0,   0,   0,   0,   48, 120, 156, 999, 999 }},
	};
	static const int _wchar[] = {
		TEX_short_mario, TEX_short_wario,
	};
	Gfx *gp = *gpp;
	STexInfo *texC, *texA;
	SMenuItem *item;
	void *list[8];
	int i, j, cached;

	gSPDisplayList(gp++, fade_normal_texture_init_dl);

	// 下地
	texC = _getTexStory(st->global, TEX_ohanashi_panel);
	menuItem_drawTex(st->miBase, &gp, texC, false);

	// アニメーション
	for(i = 0; i < ARRAY_SIZE(st->animeState); i++) {
		float x, y;
		if(!st->loaded) continue;

		item = &st->miChar[i];
		x = item->trans[0];
		y = item->trans[1];
		item->trans[0] -= 26;
		item->trans[1] -= 48;

		if(!menuItem_outOfScreen(item, 64, 64)) {
			int alpha = item->colorTime * 255;

			gDPSetPrimColor(gp++, 0,0, 255,255,255, alpha);
			gSPDisplayList(gp++, fade_intensity_texture_init_dl);
			texC = _getTexStory(st->global, TEX_spot_chr);
			tiStretchTexTile(&gp, texC, false,
				0, 0, texC->size[0], texC->size[1],
				item->trans[0], item->trans[1],
				texC->size[0], texC->size[1]);

			item->trans[0] = x + 5;
			item->trans[1] = y + 2;
			gSPDisplayList(gp++, fade_shadow_texture_init_dl);
			gDPSetPrimColor(gp++, 0,0, 0,0,0, alpha >> 1);
			animeState_draw(&st->animeState[i], &gp, item->trans[0], item->trans[1], 1, 1);

			item->trans[0] = x;
			item->trans[1] = y;
			gSPDisplayList(gp++, fade_normal_texture_init_dl);
			menuItem_setPrim(item, &gp);
			animeState_draw(&st->animeState[i], &gp, item->trans[0], item->trans[1], 1, 1);
		}

		item->trans[0] = x;
		item->trans[1] = y;
	}

	gSPDisplayList(gp++, fade_normal_texture_init_dl);

	// ステージの背景
	gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
	for(i = 0; i < ARRAY_SIZE(st->miStageBg); i++) {
		texC = _getTexStory(st->global, _map[st->select[0]][i]);
		menuItem_drawTex(&st->miStageBg[i], &gp, texC, false);
	}

	gSPDisplayList(gp++, fade_intensity_texture_init_dl);

	// ステージの背景
	for(i = 0; i < ARRAY_SIZE(st->miStageBg); i++) {
		int x, w;

		item = &st->miStageBg[i];
		texC = _getTexStory(st->global, _cover[st->select[0]][i]);
		j = menuStory_getStageMax(st);
		x = _filter[st->select[0]][i][j - 1];
		x = MIN(x, texC->size[0] - 1);
		w = texC->size[0] - x;

		if(menuItem_outOfScreen(item, w, texC->size[1])) {
			continue;
		}

		gDPSetPrimColor(gp++, 0,0, 0,0,0, item->color[3] * 127);
		tiStretchTexTile(&gp, texC, false,
			x, 0, w, texC->size[1],
			item->trans[0] + x, item->trans[1], w, texC->size[1]);
	}

	gSPDisplayList(gp++, fade_normal_texture_init_dl);
	gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);

	// ちびキャラ
	for(i = 0; i < ARRAY_SIZE(st->miWalk); i++) {
		texC = _getTexStory(st->global, _wchar[st->select[0]]);
		menuItem_drawTex(&st->miWalk[i], &gp, texC, false);
	}

	gDPSetRenderMode(gp++, G_RM_OPA_SURF, G_RM_OPA_SURF2);

	// 背景選択可能を示すカーソル
	texC = _getTexStory(st->global, TEX_map_cursor);
	for(i = cached = 0; i < ARRAY_SIZE(st->miBgCursor); i++) {
		cached += menuItem_drawTex(&st->miBgCursor[i], &gp, texC, cached);
	}

	// ステージ
	list[0] = st->stageNum;
	menuNumber_draw((SMenuNumber **)list, 1, &gp);

	// ゲームレベル
	list[0] = st->speedAsk;
	menuSpeedAsk_draw((SMenuSpeedAsk **)&list[0], 1, &gp);
	list[0] = st->speedItem;
	menuSpeedItem_draw1((SMenuSpeedItem **)&list[0], 1, &gp);

	gSPDisplayList(gp++, fade_alpha_texture_init_dl);

	// キャラ名
	texC = _getTexStory(st->global, TEX_character_answer);
	texA = _getTexStory(st->global, TEX_character_answer_alpha);
	menuItem_drawAlphaItem(st->miCharName, &gp, texC, texA, false, 2, st->select[0]);

	// カーソル
	for(i = 0; i < ARRAY_SIZE(st->cursor); i++) {
		list[i] = &st->cursor[i];
	}
	menuCursor_draw((SMenuCursor **)list, ARRAY_SIZE(st->cursor), &gp);

	// ゲームレベル
	list[0] = st->speedItem;
	menuSpeedItem_draw2((SMenuSpeedItem **)&list[0], 1, &gp);

	*gpp = gp;
}

//////////////////////////////////////
//## レベルセレクト

typedef struct {
	void *global;
	MODE mode;

	SMenuItem miBase[1];
	SMenuItem miPanel[2];
	SMenuNumber lvNum[1];
	SMenuLvGauge lvGauge[1];
	SMenuSpeedAsk speedAsk[1];
	SMenuSpeedItem speedItem[1];
	SMenuMusicItem musicItem[1];
	SMenuBottle bottle[1];
	SMenuCursor cursor[3];
	SMenuItem miCapsule[1];
	SMenuItem miMusic[1];

	SMenuSpeedAsk glvAsk[1];
	SMenuSpeedItem glvItem[1];

	int depth;
	int levelMax;
} SMenuLvSel;

// フレームインアウト
static void menuLvSel_setFrame(SMenuLvSel *st, int dir, float time)
{
	SMenuItem *item = st->miBase;
	item->transRange[0][1] = item->transRange[1][1] - SCREEN_HT;
	item->transStep = FRAME_MOVE_STEP;
	item->transTime = time;
	menuItem_setTransDir(item, dir);
}

// 初期化
static void menuLvSel_init(SMenuLvSel *st, void *global, void **hpp)
{
	static const int _cursor[][4] = {
		{ 0, 0, 270, 34 }, { 0, 31, 270, 34 }, { 0, 62, 270, 30 },
	};

	mem_char *mc = &evs_mem_data[evs_select_name_no[0]];
	game_config *cfg = &mc->config;
	void *hp = *hpp;
	SMenuItem *item;
	int i;

	st->global = global;
	st->mode = _getMode(global);
	st->depth = 0;
	st->levelMax = 20;
	for(i = 0; i < ARRAY_SIZE(mc->level_data); i++) {
		st->levelMax = MAX(st->levelMax, mc->level_data[i].c_level);
	}
	st->levelMax = MIN(21, st->levelMax);

	// 下地
	menuItem_init(st->miBase, 25, 47);

	// パネル
	menuItem_init(&st->miPanel[0], 0, 0);
	menuItem_init(&st->miPanel[1], 0, 95);

	// 耐久 or タイムアタック 用ゲームレベル設定パネル
	switch(st->mode) {
	case MODE_LVSEL_TQ:
		menuSpeedAsk_init(st->glvAsk, global, 3, cfg->p1_tq_lv, 68, 9);
		menuSpeedItem_init(st->glvItem, global, 3, 0, cfg->p1_tq_lv, 130, 5, 46);
		break;

	case MODE_LVSEL_TA:
		menuSpeedAsk_init(st->glvAsk, global, 3, cfg->p1_ta_lv, 68, 9);
		menuSpeedItem_init(st->glvItem, global, 3, 0, cfg->p1_ta_lv, 130, 5, 46);
		break;

	// ウィルスレベル数
	case MODE_LVSEL:
		i = CLAMP(0, st->levelMax, (int)cfg->p1_lv);
		menuNumber_init(st->lvNum, global, 1, 2, i, 89, 5);
		menuLvGauge_init(st->lvGauge, global, 2, 0, i, 153, 15);
		menuItem_setColorLow(st->lvGauge->miBase, 1, 0.5);
		st->lvGauge->levelMax = st->levelMax;
		break;
	}

	// [LOW] [MED] [HI]
	menuSpeedAsk_init(st->speedAsk, global, 1, cfg->p1_sp, 68, 40);
	menuSpeedItem_init(st->speedItem, global, 1, 0, cfg->p1_sp, 130, 36, 46);
	menuItem_setColorLow(st->speedItem->miBase, 1, 0.5);

	// [FEVER] [CHILL] [DIZZY]
	menuMusicItem_init(st->musicItem, global, cfg->p1_m, 70, 70);

	// ボトル
	menuBottle_init(st->bottle, global, 25, 7);

	// カーソル
	for(i = 0; i < ARRAY_SIZE(st->cursor); i++) {
		menuCursor_init(&st->cursor[i], global, CURSOR_PANEL, 0,
			_cursor[i][0], _cursor[i][1], _cursor[i][2], _cursor[i][3]);
	}

	// カプセル
	menuItem_init(st->miCapsule, 86, 7);

	// ミュージック
	menuItem_init(st->miMusic, 153, 8);

	// フレームイン
	menuLvSel_setFrame(st, 1, 0);

	*hpp = hp;
}

// ゲーム開始直前のキャンセル
static void menuLvSel_waitCancel(SMenuLvSel *st)
{
	int trg = _getKeyTrg(st->global, 0);

	if(trg & CONT_CANCEL) {
		_setFadeDir(st->global, -1);
		_setNextMain(st->global, MAIN_MENU);
		dm_snd_play(SE_mCANCEL);
	}
}

// 入力
static void menuLvSel_input(SMenuLvSel *st)
{
	SMenuItem *item;
	game_config *cfg = &evs_mem_data[evs_select_name_no[0]].config;
	game_state *state = &game_state_data[0];
	int rep = _getKeyRep(st->global, 0);
	int trg = _getKeyTrg(st->global, 0);
	int vec, depth, sound = -1;

	if(st->miBase->transStep < 0 || st->miBase->transTime < 1) {
		return;
	}

	vec = 0;
	if((rep & U_JPAD) || (trg & CONT_CANCEL)) vec--;
	if((rep & D_JPAD) || (trg & CONT_OK)) vec++;
	depth = CLAMP(0, 2, st->depth + vec);

	if(depth != st->depth) {
		sound = SE_mUpDown;
		st->depth = depth;
	}

	switch(st->depth) {
	// ウィルスレベルを選択中
	case 0:
		switch(st->mode) {
		case MODE_LVSEL_TQ:
		case MODE_LVSEL_TA:
			menuSpeedItem_input(st->glvItem, 0);
			break;

		case MODE_LVSEL:
			menuLvGauge_input(st->lvGauge, 0);
			break;
		}
		break;

	// スピードを選択中
	case 1:
		menuSpeedItem_input(st->speedItem, 0);
		break;

	// ミュージックを選択中
	case 2:
		menuMusicItem_input(st->musicItem, 0);
		break;
	}

	if(sound < 0) {
		if((trg & CONT_OK) && st->depth == 2) {
			_setFadeDir(st->global, 1);
			_setNextMain(st->global, MAIN_12);

			switch(st->mode) {
			case MODE_LVSEL:
				state->virus_level = st->lvGauge->level;
				state->cap_def_speed = st->speedItem->select;

				cfg->p1_lv = st->lvGauge->level;
				break;

			case MODE_LVSEL_TQ:
				state->virus_level = 10;
				state->cap_def_speed = st->speedItem->select;
				state->game_level = st->glvItem->select;

				cfg->p1_tq_lv = st->glvItem->select;
				break;

			case MODE_LVSEL_TA: {
				state->virus_level = _timeAttack_levelTable[st->glvItem->select];
				state->cap_def_speed = st->speedItem->select;
				state->game_level = st->glvItem->select;

				cfg->p1_ta_lv = st->glvItem->select;
				} break;
			}

			cfg->p1_sp = st->speedItem->select;
			cfg->p1_m  = st->musicItem->select;

			evs_seqnumb = st->musicItem->select;
			evs_seqence = (evs_seqnumb != 4);
			evs_game_time = 0;

			sound = SE_mDecide;
		}
		else if((trg & CONT_CANCEL) && st->depth == 0) {
			_setMode(st->global, MODE_MAIN);
			menuLvSel_setFrame(st, -1, 1);
			if(st->musicItem->playNo >= 0) {
				dm_seq_play_fade(SEQ_Menu, BGM_FADE_FRAME);
			}
			sound = SE_mCANCEL;
		}
	}

	if(sound >= 0) {
		dm_snd_play(sound);
	}
}

// 更新
static void menuLvSel_update(SMenuLvSel *st)
{
	SMenuItem *miRoot = _getRootItem(st->global);
	SMenuItem *item;
	int i;

	// 耐久 or タイムアタック 用ゲームレベル設定パネル
	switch(st->mode) {
	case MODE_LVSEL_TQ:
		st->glvAsk->select = st->glvItem->select;
		st->bottle->level = 10;
		break;

	case MODE_LVSEL_TA:
		st->glvAsk->select = st->glvItem->select;
		st->bottle->level = _timeAttack_levelTable[st->glvItem->select];
		break;

	// ウィルスレベル
	case MODE_LVSEL:
		st->lvNum->number = st->lvGauge->level;
		st->bottle->level = st->lvGauge->level;
		break;
	}
	st->speedAsk->select = st->speedItem->select;

	// 下地
	menuItem_update(st->miBase, miRoot);

	// パネル
	menuItem_updateN(st->miPanel, ARRAY_SIZE(st->miPanel), st->miBase);

	// 耐久 or タイムアタック 用ゲームレベル設定パネル
	switch(st->mode) {
	case MODE_LVSEL_TQ:
	case MODE_LVSEL_TA:
		menuSpeedAsk_update(st->glvAsk, st->miBase);
		menuSpeedItem_update(st->glvItem, st->miBase);
		break;

	// ウィルスレベル
	case MODE_LVSEL:
		menuNumber_update(st->lvNum, st->miBase);
		menuLvGauge_update(st->lvGauge, st->miBase);
		break;
	}

	// スピード
	menuSpeedAsk_update(st->speedAsk, st->miBase);
	menuSpeedItem_update(st->speedItem, st->miBase);

	// 音楽
	menuMusicItem_update(st->musicItem, st->miBase);

	// ボトル
	menuBottle_update(st->bottle, &st->miPanel[1]);

	// カーソル
	for(i = 0; i < ARRAY_SIZE(st->cursor); i++) {
		menuCursor_update(&st->cursor[i], &st->miPanel[0]);
	}

	// カプセル
	menuItem_update(st->miCapsule, &st->miPanel[1]);

	// ミュージック
	menuItem_update(st->miMusic, &st->miPanel[1]);

	// ウィルスレベル or ゲームレベルのカーソル
	i = (st->depth == 0) ;
	switch(st->mode) {
	case MODE_LVSEL:
		st->lvGauge->cursor->flags.finger = i;
		st->lvGauge->cursor->flags.blink = i;
		break;

	case MODE_LVSEL_TQ:
	case MODE_LVSEL_TA:
		st->glvItem->cursor->flags.finger = i;
//		st->glvItem->cursor->flags.blink = i;
		st->glvItem->flags.blink = i;
		break;
	}
	st->cursor[0].flags.cursor = i;

	// スピードのカーソル
	i = (st->depth == 1);
	st->speedItem->cursor->flags.finger = i;
//	st->speedItem->cursor->flags.blink = i;
	st->speedItem->flags.blink = i;
	st->cursor[1].flags.cursor = i;

	// ミュージックのカーソル
	i = (st->depth == 2);
	st->musicItem->cursor->flags.finger = i;
//	st->musicItem->cursor->flags.blink = i;
	st->musicItem->flags.blink = i;
	st->cursor[2].flags.cursor = i;

//	// パネルの色とか
//	if(st->depth != 0) {
//		switch(st->mode) {
//		case MODE_LVSEL:
//			item = st->lvGauge->cursor->miCursor;
//			break;
//
//		case MODE_LVSEL_TQ:
//		case MODE_LVSEL_TA:
//			item = st->glvItem->cursor->miCursor;
//			break;
//		}
//
//		for(i = 0; i < ARRAY_SIZE(item->color) - 1; i++) {
//			item->color[i] = item->colorRange[1][i] * miRoot->color[i];
//		}
//	}
//	if(st->depth != 1) {
//		item = st->speedItem->cursor->miCursor;
//		for(i = 0; i < ARRAY_SIZE(item->color) - 1; i++) {
//			item->color[i] = item->colorRange[1][i] * miRoot->color[i];
//		}
//	}
//	if(st->depth != 2) {
//		item = st->musicItem->cursor->miCursor;
//		for(i = 0; i < ARRAY_SIZE(item->color) - 1; i++) {
//			item->color[i] = item->colorRange[1][i] * miRoot->color[i];
//		}
//	}
//
//	// ウィルスレベル or ゲームレベルのカーソル
//	i = (st->depth == 0) ? 1 : 0;
//	switch(st->mode) {
//	case MODE_LVSEL:
//		st->lvGauge->cursor->flags.finger = i;
//		break;
//
//	case MODE_LVSEL_TQ:
//	case MODE_LVSEL_TA:
//		st->glvItem->cursor->flags.finger = i;
//		break;
//	}
//	st->cursor[0].flags.cursor = i;
//
//	// スピードのカーソル
//	i = (st->depth == 1) ? 1 : 0;
//	st->speedItem->cursor->flags.finger = i;
//	st->cursor[1].flags.cursor = i;
//
//	// ミュージックのカーソル
//	i = (st->depth == 2) ? 1 : 0;
//	st->musicItem->cursor->flags.finger = i;
//	st->cursor[2].flags.cursor = i;
}

// 描画
static void menuLvSel_draw(SMenuLvSel *st, Gfx **gpp)
{
	static const int _label[] = { 3, 1, 4 };

	Gfx *gp = *gpp;
	STexInfo *texC, *texA;
	void *list[8];
	int i;

	gSPDisplayList(gp++, fade_normal_texture_init_dl);

	// パネル
	switch(st->mode) {
	case MODE_LVSEL:
		texC = _getTexLevel(st->global, TEX_endless_panel);
		break;

	case MODE_LVSEL_TQ:
	case MODE_LVSEL_TA:
		texC = _getTexLevel(st->global, TEX_newmode_panel);
		break;
	}
	menuItem_drawTex(&st->miPanel[0], &gp, texC, false);

	texC = _getTexLevel(st->global, TEX_endless_illust);
	menuItem_drawTex(&st->miPanel[1], &gp, texC, false);

	// カプセル
	if(st->speedItem->select > 0) {
		texC = _getTexLevel(st->global, TEX_endless_illust_speed);
		menuItem_drawItem(st->miCapsule, &gp, texC, false, 2, st->speedItem->select - 1);
	}

	// ミュージック
	if(st->musicItem->select < 4) {
		texC = _getTexLevel(st->global, TEX_endless_illust_music);
		menuItem_drawItem(st->miMusic, &gp, texC, false, 4, st->musicItem->select);
	}

	if(st->mode == MODE_LVSEL && st->levelMax > 20) {
		float x = st->miPanel->trans[0];
		st->miPanel->trans[0] += 222;
		texC = _getTexLevel(st->global, TEX_virus_gauge_lv21);
		menuItem_drawTex(st->miPanel, &gp, texC, false);
		st->miPanel->trans[0] = x;
	}

	// 音楽
	list[0] = st->musicItem;
	menuMusicItem_draw1((SMenuMusicItem **)&list[0], 1, &gp);

	// スピード
	list[1] = st->speedAsk;
	list[2] = st->speedItem;
	menuSpeedAsk_draw((SMenuSpeedAsk **)&list[1], 1, &gp);
	menuSpeedItem_draw1((SMenuSpeedItem **)&list[2], 1, &gp);

	// 耐久 or タイムアタック 用ゲームレベル設定パネル
	switch(st->mode) {
	case MODE_LVSEL_TQ:
	case MODE_LVSEL_TA:
		list[3] = st->glvAsk;
		list[4] = st->glvItem;
		menuSpeedAsk_draw((SMenuSpeedAsk **)&list[3], 1, &gp);
		menuSpeedItem_draw1((SMenuSpeedItem **)&list[4], 1, &gp);
		break;

	// ウィルスレベル
	case MODE_LVSEL:
		list[3] = st->lvNum;
		list[4] = st->lvGauge;
		menuNumber_draw((SMenuNumber **)&list[3], 1, &gp);
		menuLvGauge_draw1((SMenuLvGauge **)&list[4], 1, &gp);
		break;
	}

	// カーソル
	for(i = 0; i < ARRAY_SIZE(st->cursor); i++) {
		list[5 + i] = &st->cursor[i];
	}
	menuCursor_draw((SMenuCursor **)&list[5], ARRAY_SIZE(st->cursor), &gp);

	// 音楽
	menuMusicItem_draw2((SMenuMusicItem **)&list[0], 1, &gp);

	// スピード
	menuSpeedItem_draw2((SMenuSpeedItem **)&list[2], 1, &gp);

	// 耐久 or タイムアタック 用ゲームレベル設定パネル
	switch(st->mode) {
	case MODE_LVSEL_TQ:
	case MODE_LVSEL_TA:
		menuSpeedItem_draw2((SMenuSpeedItem **)&list[4], 1, &gp);
		break;

	// ウィルスレベル
	case MODE_LVSEL:
		menuLvGauge_draw2((SMenuLvGauge **)&list[4], 1, &gp);
		break;
	}

	// ボトル
	menuBottle_draw(st->bottle, &gp);

	*gpp = gp;
}

//////////////////////////////////////
//## キャラセレクト

typedef struct {
	void *global;

	int allCount;
	int select[4];
	int depth[4];
	int subDepth;
	int hardFlag;
	int hardMode;
	int comLvId;
	int comLvPn;

	int playerCount;
	int playerTable[4];

	int cpuFlag[4];
	int cpuCount;
	int cpuTable[4];

	SMenuItem miBase[1];
	SMenuItem miHard[1];
	SMenuItem miChar[15];
	SMenuItem miCharFilt[15];
	SMenuComLvPanel comLv[15];
	SMenuCursor cursor[4];
} SMenuChSel;

// フレームインアウト
static void menuChSel_setFrame(SMenuChSel *st, int dir, float time)
{
	SMenuItem *item = st->miBase;
	item->transRange[0][1] = item->transRange[1][1] - SCREEN_HT;
	item->transStep = FRAME_MOVE_STEP;
	item->transTime = time;
	menuItem_setTransDir(item, dir);
}

// 初期化
static void menuChSel_init(SMenuChSel *st, void *global, void **hpp)
{
	static const int _pos[][2] = {
//		{ 0,  0 }, { 28,  0 },
//		{ 0, 32 }, { 28, 32 },
		{ 0,  0 }, { 0,  0 },
		{ 0,  0 }, { 0,  0 },
	};

	game_config *cfg;
	SMenuItem *item;
	int i, charNo;
	MODE mode = _getMode(global);

	st->global = global;
	st->subDepth = -1;
	st->hardFlag = 0;
	st->hardMode = 0;
	st->comLvId = -1;
	st->comLvPn = -1;

	// プレイヤー数
	switch(mode) {
	case MODE_VSCOM_CH:
	case MODE_VSCOM_FL_CH:

	case MODE_VSMAN_CH:
	case MODE_VSMAN_FL_CH:
	case MODE_VSMAN_TA_CH:
		st->allCount = 2;
		break;

	case MODE_PLAY4_CH:
	case MODE_PLAY4_TB_CH:
	case MODE_PLAY4_FL_CH:
		st->allCount = 4;
		break;
	}

	st->playerCount = 0;
	st->cpuCount = 0;

	// HARD MODE
	menuItem_init(st->miHard, 249, 22);
	menuItem_setColor_fade(st->miHard);
	st->miHard->colorTime = 0;

	// CPUレベル
	for(i = 0; i < st->allCount; i++) {

		// プレイヤーを判別
		if(game_state_data[i].player_type == PUF_PlayerMAN) {
			st->playerTable[st->playerCount] = i;
			st->playerCount++;
			st->cpuFlag[i] = 0;
		}
		else {
			st->cpuTable[st->cpuCount] = i;
			st->cpuCount++;
			st->cpuFlag[i] = 1;
		}

		switch(mode) {
		case MODE_VSCOM_CH:
		case MODE_VSCOM_FL_CH:
			cfg = &evs_mem_data[evs_select_name_no[0]].config;
			charNo = cfg->vc_no[i];
			break;

		case MODE_VSMAN_CH:
		case MODE_VSMAN_FL_CH:
		case MODE_VSMAN_TA_CH:
			cfg = &evs_mem_data[evs_select_name_no[i]].config;
			charNo = cfg->vm_no;
			break;

		case MODE_PLAY4_CH:
		case MODE_PLAY4_TB_CH:
		case MODE_PLAY4_FL_CH:
			charNo = evs_cfg_4p.p4_no[i];
			break;
		}

		st->select[i] = charNo;
		st->depth[i] = -1;
	}

	// カーソル
	for(i = 0; i < st->allCount; i++) {
		menuCursor_init2(&st->cursor[i], global, CURSOR_ITEM,
			st->playerCount, st->cpuCount, i, 0, 0, 44, 44);
		item = st->cursor[i].miPlayer;
		item->transRange[0][0] = item->transRange[1][0] = _pos[i][0];
		item->transRange[0][1] = item->transRange[1][1] = _pos[i][1];
	}

	// 下地
	menuItem_init(st->miBase, 18, 47);

	// キャラ
	for(i = 0; i < ARRAY_SIZE(st->miChar); i++) {
		menuItem_init(&st->miChar[i], 12 + 54 * (i % 5), 12 + 53 * (i / 5));
		menuItem_init(&st->miCharFilt[i], 3, 3);
		menuItem_setColorHi(&st->miCharFilt[i], 0, 0);
		menuItem_setColorLow(&st->miCharFilt[i], 0, 0.5);
	}

	// com lv
	for(i = 0; i < ARRAY_SIZE(st->comLv); i++) {
		menuComLvPanel_init(&st->comLv[i], global, 0, -6, 30);
		menuComLvPanel_setFade(&st->comLv[i], -1, 0);
	}

	// フレームイン
	menuChSel_setFrame(st, 1, 0);
}

// 選択済みのキャラかどうか調べる
static bool menuChSel_checkSelected(SMenuChSel *st, int playerNo, int charNo, int depth)
{
	int i;

	for(i = 0; i < st->allCount; i++) {
		if(i != playerNo && st->select[i] == charNo && st->depth[i] >= depth) {
			return true;
		}
	}

	return false;
}

// 移動可能なカーソル位置かどうか調べる
static bool menuChSel_checkMoveable(SMenuChSel *st, int playerNo, int charNo)
{
	if(menuChSel_checkSelected(st, playerNo, charNo, 0)) {
		return true;
	}
	else if(charNo == 13 && !evs_secret_flg[1]) {
		return true;
	}
	else if(charNo == 14 && !evs_secret_flg[0]) {
		return true;
	}

	return false;
}

// 一人分の入力
static void menuChSel_input1(SMenuChSel *st, int playerNo, int contNo)
{
	int rep = _getKeyRep(st->global, contNo);
	int trg = _getKeyTrg(st->global, contNo);
	int lvl = _getKeyLvl(st->global, contNo);
	int sound;

	sound = -1;

	switch(st->depth[playerNo]) {
	// キャンセル状態を解除
	case -1:
		st->depth[playerNo]++;

	// キャラを選択中
	case 0:
		// カーソルが重ならないようにする
		while(menuChSel_checkMoveable(st, playerNo, st->select[playerNo])) {
			st->select[playerNo] = WrapI(0, ARRAY_SIZE(st->miChar), st->select[playerNo] + 1);
		}

		if(trg & CONT_OK) {
			if((lvl & L_TRIG) && st->cpuCount) {
				st->hardFlag = (st->hardFlag && st->hardMode == 1 ? 0 : 1);
				st->hardMode = 1;
				sound = SE_mDecide;
			}
			else {
				st->depth[playerNo]++;
				sound = SE_mDecide;
			}
		}
		else if(trg & CONT_CANCEL) {
			if((lvl & L_TRIG) && st->cpuCount) {
				st->hardFlag = (st->hardFlag && st->hardMode == 2 ? 0 : 1);
				st->hardMode = 2;
				sound = SE_mDecide;
			}
			else {
				st->depth[playerNo]--;
				sound = SE_mCANCEL;
			}
		}
		else {
			int sel, dx, dy;

			sel = st->select[playerNo];
			dx = dy = 0;
			if(rep & L_JPAD) dx--;
			if(rep & R_JPAD) dx++;
			if(rep & U_JPAD) dy--;
			if(rep & D_JPAD) dy++;

			// カーソルが重ならないようにする
			if(dx || dy) {
				int ix = sel % 5, iy = sel / 5;
				int ch;

				do {
					ix = WrapI(0, 5, ix + dx);
					iy = WrapI(0, 3, iy + dy);
					ch = ix + iy * 5;
				} while(menuChSel_checkMoveable(st, playerNo, ch));

				sel = ch;
			}

			if(sel != st->select[playerNo]) {
				st->select[playerNo] = sel;
				sound = SE_mUpDown;
			}
		}
		break;

	// 設定完了状態
	case 1:
		if(trg & CONT_CANCEL) {
			st->depth[playerNo]--;
			sound = SE_mCANCEL;
		}
		break;
	}

	// サウンドを再生
	if(sound >= 0) {
		dm_snd_play(sound);
	}
}

// 入力
static void menuChSel_inputMan(SMenuChSel *st)
{
	game_config *cfg;
	game_state *state;
	MODE mode;
	int i, ok, cancel;

	// プレイヤーのキー入力
	for(i = ok = cancel = 0; i < st->playerCount; i++) {

		menuChSel_input1(st, st->playerTable[i], st->playerTable[i]);

		switch(st->depth[st->playerTable[i]]) {
		case -1:
			cancel++;
			break;

		case 1:
			ok++;
			break;
		}
	}

	// メインメニューへ
	if(cancel) {
		_setMode(st->global, MODE_MAIN);
		menuChSel_setFrame(st, -1, 1);
		st->subDepth--;
	}

	if(ok == st->playerCount) {
		st->subDepth++;
	}
}

// 入力
static void menuChSel_inputCom(SMenuChSel *st)
{
	int i, pre, playerNo, select;

	// CPUのキー入力
	for(i = pre = 0; i < st->cpuCount; i++) {
		playerNo = st->cpuTable[i];
		select = st->select[playerNo];

		if(st->depth[playerNo] == 1) {
			pre = playerNo;
			continue;
		}

		// 1Pが代表で入力
		menuChSel_input1(st, playerNo, 0);

		switch(st->depth[playerNo]) {
		case -1:
			st->comLvId = -1;
			st->comLvPn = -1;
			st->depth[pre]--;
			st->subDepth--;
			break;

		case 0:
			st->comLvId = select;
			st->comLvPn = playerNo;
			break;
		}

		break;
	}

	// 全入力が完了しているか?
	if(i == st->cpuCount) {
		st->subDepth++;
	}
}

// 入力
static void menuChSel_input(SMenuChSel *st)
{
	static const int _charTbl[] = {
		DMC_MARIO, DMC_WARIO,  DMC_YARI, DMC_KUMO,   DMC_MAYU,
		DMC_RINGO, DMC_KURAGE, DMC_IKA,  DMC_FUSEN,  DMC_KAERU,
		DMC_ROBO,  DMC_MAD,    DMC_NAZO, DMC_MMARIO, DMC_VWARIO,
	};

	game_config *cfg;
	game_state *state;
	MODE mode;
	int i, dif;

	if(st->miBase->transStep < 0 || st->miBase->transTime < 1) {
		return;
	}

	// キー入力
	switch(st->subDepth) {
	case -1:
		st->subDepth++;

	case 0:
		menuChSel_inputMan(st);
		break;

	case 1:
		menuChSel_inputCom(st);
		break;
	}

	// 設定完了したか?
	if(st->subDepth < 2) {
		return;
	}

	// cpuの強さ
	dif = (st->hardFlag ? st->hardMode : 0);

	// 各設定画面へ
	switch(_getMode(st->global)) {
	case MODE_VSCOM_CH:
	case MODE_VSCOM_FL_CH:
		cfg = &evs_mem_data[evs_select_name_no[0]].config;
		for(i = 0; i < st->allCount; i++) {
			cfg->vc_no[i] = st->select[i];
		}
		break;

	case MODE_VSMAN_CH:
	case MODE_VSMAN_FL_CH:
	case MODE_VSMAN_TA_CH:
		if(evs_select_name_no[0] == evs_select_name_no[1]) {
			cfg = &evs_mem_data[0].config;
			cfg->vm_no = 0;
		}
		else {
			for(i = 0; i < st->allCount; i++) {
				cfg = &evs_mem_data[evs_select_name_no[i]].config;
				cfg->vm_no = st->select[i];
			}
		}
		break;

	case MODE_PLAY4_CH:
	case MODE_PLAY4_TB_CH:
	case MODE_PLAY4_FL_CH:
		for(i = 0; i < st->allCount; i++) {
			evs_cfg_4p.p4_no[i] = st->select[i];
		}
		break;
	}

	switch(_getMode(st->global)) {
	case MODE_VSCOM_CH:
		mode = MODE_VSCOM;
		break;
	case MODE_VSCOM_FL_CH:
		mode = MODE_VSCOM_FL;
		break;

	case MODE_VSMAN_CH:
		mode = MODE_VSMAN;
		break;
	case MODE_VSMAN_FL_CH:
		mode = MODE_VSMAN_FL;
		break;
	case MODE_VSMAN_TA_CH:
		mode = MODE_VSMAN_TA;
		break;

	case MODE_PLAY4_CH:
		mode = MODE_PLAY4_LV;
		break;
	case MODE_PLAY4_TB_CH:
		mode = MODE_PLAY4_TB_LV;
		break;
	case MODE_PLAY4_FL_CH:
		mode = MODE_PLAY4_FL_LV;
		break;
	}

	// ゲーム設定
	for(i = 0; i < st->allCount; i++) {
		state = &game_state_data[i];
		state->charNo = _charTbl[st->select[i]];
		state->think_level = dif;
	}

	_setMode(st->global, mode);
	menuChSel_setFrame(st, -1, 1);
}

// 更新
static void menuChSel_update(SMenuChSel *st)
{
	static const u8 _star[15] = {
		4, 4, 1, 1, 2,
		2, 2, 2, 3, 3,
		3, 4, 5, 5, 5,
	};
//	static const int _pos[][2] = {
//		{ -6, 30 }, { -6, 30 },
//		{ -6, 30 }, { -6, 30 },
//		{ 2, -10 }, { 2, -10 },
//	};
	SMenuItem *miRoot = _getRootItem(st->global);
	int i, j;

	// 下地
	menuItem_update(st->miBase, miRoot);

	// HARD MODE
	switch(st->subDepth) {
	case 0: case 1:
		menuItem_setColorDir(st->miHard, st->hardFlag ? 1 : -1);
		break;

	default:
		menuItem_setColorDir(st->miHard, -1);
		break;
	}
	menuItem_update(st->miHard, miRoot);

	// キャラ
	menuItem_updateN(st->miChar, ARRAY_SIZE(st->miChar), st->miBase);
	for(i = 0; i < ARRAY_SIZE(st->miCharFilt); i++) {

		if(menuChSel_checkSelected(st, -1, i, 1)) {
			menuItem_setColorDir(&st->miCharFilt[i], -1);
		}
		else {
			menuItem_setColorDir(&st->miCharFilt[i], 1);
		}

		menuItem_update(&st->miCharFilt[i], &st->miChar[i]);
	}

	// com lv
	for(i = 0; i < ARRAY_SIZE(st->comLv); i++) {
		st->comLv[i].level = _star[i] + (st->hardFlag ? st->hardMode : 0);
		menuComLvPanel_setFadeDir(&st->comLv[i], (i == st->comLvId) ? 1 : -1);
		menuComLvPanel_update(&st->comLv[i], &st->miChar[i]);
	}

	// カーソル
	for(i = 0; i < st->allCount; i++) {
		SMenuCursor *cur = &st->cursor[i];

		menuCursor_update(cur, &st->miChar[st->select[i]]);

		switch(st->depth[i]) {
		case -1:
			cur->flags.cursor = 0;
			cur->flags.player = 0;
			cur->flags.blink = 0;
			break;

		case 0:
			cur->flags.cursor = 1;
			cur->flags.player = 1;
			cur->flags.blink = 1;
			break;

		case 1:
//			for(j = 0; j < ARRAY_SIZE(cur->miCursor->color); j++) {
//				cur->miCursor->color[j] = cur->miCursor->colorRange[1][j];
//			}
			cur->flags.cursor = 1;
			cur->flags.player = 1;
			cur->flags.blink = 0;
			break;
		}
	}
}

// 描画
static void menuChSel_draw(SMenuChSel *st, Gfx **gpp)
{
	Gfx *gp = *gpp;
	SMenuItem *item;
	STexInfo *texC, *texA;
	void *list[15];
	int i, cached;

	gSPDisplayList(gp++, fade_normal_texture_init_dl);

	// 下地
	texC = _getTexChar(st->global, TEX_character_panel);
	menuItem_drawTex(st->miBase, &gp, texC, false);

	// はてな(?)マーク
	for(i = 0; i < 2; i++) {
		static const int _tex[] = { TEX_v_wario_face, TEX_m_mario_face };
#if LOCAL==JAPAN
		static const int _pos[][2] = { { -3, 3 }, { -2, 3 } };
#elif LOCAL==AMERICA
		static const int _pos[][2] = { { -4, 3 }, { -2, 3 } };
#elif LOCAL==CHINA
		static const int _pos[][2] = { { -4, 3 }, { -2, 3 } };
#endif
		float x, y;

		if(!evs_secret_flg[i]) continue;

		item = &st->miChar[14 - i];
		x = item->trans[0];
		y = item->trans[1];

		item->trans[0] += _pos[i][0];
		item->trans[1] += _pos[i][1];

		texC = _getTexChar(st->global, _tex[i]);
		menuItem_drawTex(item, &gp, texC, false);

		item->trans[0] = x;
		item->trans[1] = y;
	}

	gSPDisplayList(gp++, fade_fillrect_init_dl);

	// フィルター
	for(i = 0; i < ARRAY_SIZE(st->miCharFilt); i++) {
		int x, y, w, h;
		item = &st->miCharFilt[i];
		if(item->color[3] == 1) continue;

		x = item->trans[0];
		y = item->trans[1];
		w = h = 38;

		menuItem_setPrim(item, &gp);
		gDPScisFillRectangle(gp++, x, y, x+w, y+h);
	}

	// カーソル
	for(i = 0; i < st->allCount; i++) {
		list[i] = &st->cursor[i];
	}
	menuCursor_draw((SMenuCursor **)list, st->allCount, &gp);

	// com lv
	for(i = 0; i < ARRAY_SIZE(st->comLv); i++) {
		list[i] = &st->comLv[i];
	}
	menuComLvPanel_draw((SMenuComLvPanel **)list, ARRAY_SIZE(st->comLv), &gp);

	gSPDisplayList(gp++, fade_normal_texture_init_dl);
	gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);

	// HARD MODE
	if(st->hardMode > 0) {
		texC = _getTexChar(st->global, TEX_hard_comlv);
		menuItem_drawItem(st->miHard, &gp, texC, false, 2, st->hardMode - 1);
	}

	*gpp = gp;
}

//////////////////////////////////////
//## 二人で遊ぶ

typedef struct {
	void *global;
	int depth;

	int master;

	int playerCount;
	int playerTable[4];

	int cpuCount;
	int cpuTable[4];

	SMenuItem miONaJi[1];

	int panelCount;
	SMenuPlay2Panel panel[4];
	SMenuPlay2PanelSub panelSub[1];
} SMenuPlay2;

// フレームインアウト
static void menuPlay2_setFrame(SMenuPlay2 *st, int dir, float time)
{
	static const int _moveTbl[] = { SCREEN_WD, -SCREEN_WD };
	SMenuPlay2Panel *panel;
	SMenuItem *item;
	int i;

	for(i = 0; i < st->panelCount; i++) {
		panel = &st->panel[i];
		item = panel->miBase;
		item->transRange[0][0] = item->transRange[1][0] + _moveTbl[i & 1];
		item->transStep = FRAME_MOVE_STEP;
		item->transTime = time;
		menuItem_setTransDir(item, dir);
	}
}

// 初期化
static void menuPlay2_init(SMenuPlay2 *st, void *global, void **hpp)
{
	static const int _onaji[][2] = { { 216, 45 }, { 216, 35 } };
	static const int _panel2[][2] = { { 25, 58 }, { 25, 128 } };
	static const int _panel4[][2] = { { 25, 48 }, { 25, 93 }, { 25, 138 }, { 25, 183 } };
	game_config *cfg;
	game_state *state;
	int i, j, backNo, musicNo;

	st->global = global;
	st->master = -1;
	st->depth = -1;

	// パネル
	switch(_getMode(st->global)) {
	case MODE_VSMAN:
	case MODE_VSMAN_FL:
	case MODE_VSMAN_TA:
		cfg = &evs_mem_data[evs_select_name_no[0]].config;
		backNo = cfg->vm_st + 1;
		musicNo = cfg->vm_m;

		st->playerCount = 2;
		st->playerTable[0] = 0;
		st->playerTable[1] = 1;
		st->cpuCount = 0;
		st->panelCount = 2;

		for(i = 0; i < st->panelCount; i++) {
			cfg = &evs_mem_data[evs_select_name_no[i]].config;
			state = &game_state_data[i];

			switch(_getMode(st->global)) {
			case MODE_VSMAN:
				j = cfg->vm_lv;
				break;
			case MODE_VSMAN_FL:
				j = cfg->vm_fl_lv;
				break;
			case MODE_VSMAN_TA:
				j = cfg->vm_ta_lv;
				break;
			}

			menuPlay2Panel_init(&st->panel[i], global, hpp, 1,
				st->playerCount, st->cpuCount, i,
				state->player_type == PUF_PlayerCPU, state->charNo,
				j, cfg->vm_sp, _panel2[i][0], _panel2[i][1]);
		}
		break;

	case MODE_VSCOM:
	case MODE_VSCOM_FL:
		cfg = &evs_mem_data[evs_select_name_no[0]].config;
		backNo = cfg->vc_st + 1;
		musicNo = cfg->vc_m;

		st->playerCount = 1;
		st->playerTable[0] = 0;
		st->cpuCount = 1;
		st->cpuTable[0] = 1;
		st->panelCount = 2;

		for(i = 0; i < st->panelCount; i++) {
			state = &game_state_data[i];

			switch(_getMode(st->global)) {
			case MODE_VSCOM:
				j = cfg->vc_lv[i];
				break;
			case MODE_VSCOM_FL:
				j = cfg->vc_fl_lv[i];
				break;
			}

			menuPlay2Panel_init(&st->panel[i], global, hpp, 1,
				st->playerCount, st->cpuCount, i,
				state->player_type == PUF_PlayerCPU, state->charNo,
				j, cfg->vc_sp[i], _panel2[i][0], _panel2[i][1]);
		}
		break;

	case MODE_PLAY4_LV:
	case MODE_PLAY4_TB_LV:
	case MODE_PLAY4_FL_LV:
		backNo = evs_cfg_4p.p4_st + 1;
		musicNo = evs_cfg_4p.p4_m;

		st->playerCount = 0;
		st->cpuCount = 0;
		st->panelCount = 4;

		for(i = 0; i < st->panelCount; i++) {
			state = &game_state_data[i];

			if(state->player_type == PUF_PlayerCPU) {
				st->cpuTable[st->cpuCount] = i;
				st->cpuCount++;
			}
			else {
				st->playerTable[st->playerCount] = i;
				st->playerCount++;
			}
		}

		for(i = 0; i < st->panelCount; i++) {
			state = &game_state_data[i];

			switch(_getMode(st->global)) {
			case MODE_PLAY4_LV:
			case MODE_PLAY4_TB_LV:
				j = evs_cfg_4p.p4_lv[i];
				break;
			case MODE_PLAY4_FL_LV:
				j = evs_cfg_4p.p4_fl_lv[i];
				break;
			}

			menuPlay2Panel_init(&st->panel[i], global, hpp, 0,
				st->playerCount, st->cpuCount, i,
				state->player_type == PUF_PlayerCPU, state->charNo,
				j, evs_cfg_4p.p4_sp[i], _panel4[i][0], _panel4[i][1]);
		}
		break;
	}

	// みんな同じ
	i = st->panelCount / 2 - 1;
	menuItem_init(st->miONaJi, _onaji[i][0], _onaji[i][1]);
	menuItem_setColor_fade(st->miONaJi);
	st->miONaJi->colorTime = 0;

	// サブパネル
	menuPlay2PanelSub_init(st->panelSub, global, hpp, backNo, musicNo, 25, 75);

	// フレームイン
	menuPlay2_setFrame(st, 1, 0);
}

// ゲーム開始直前のキャンセル
static void menuPlay2_waitCancel(SMenuPlay2 *st)
{
	int trg = _getKeyTrg(st->global, 0);

	if(trg & CONT_CANCEL) {
		st->depth = 2;
		st->panelSub->flow = 0;
		_setFadeDir(st->global, -1);
		_setNextMain(st->global, MAIN_MENU);
		dm_snd_play(SE_mCANCEL);
	}
}

// 一括設定者かどうか判別
static bool menuPlay2_checkInputMaster(SMenuPlay2 *st, int playerNo, int contNo)
{
	SMenuPlay2Panel *panel = &st->panel[playerNo];
	int key = _getKeyLvl(st->global, contNo);

	return (key & R_TRIG) && (panel->flow == 0);
}

// 一括設定者の設定を全員に反映
static void menuPlay2_applyMasterConfig(SMenuPlay2 *st, int playerNo)
{
	SMenuPlay2Panel *master = &st->panel[playerNo];
	SMenuPlay2Panel *panel;
	int i;

	for(i = 0; i < st->panelCount; i++) {
		panel = &st->panel[i];
		if(panel->flow < 1) {
			menuPlay2Panel_copyConfig(panel, master);
		}
	}
}

// 一括設定者のカーソル設定を全員に反映
static void menuPlay2_applyMasterCursor(SMenuPlay2 *st, int playerNo)
{
	SMenuPlay2Panel *master = &st->panel[playerNo];
	SMenuPlay2Panel *panel;
	int i;

	for(i = 0; i < st->panelCount; i++) {
		panel = &st->panel[i];
		if(panel->flow < 1) {
			menuPlay2Panel_copyCursor(panel, master);
		}
	}
}

// プレイヤーのキー入力
static void menuPlay2_inputMan(SMenuPlay2 *st)
{
	SMenuPlay2Panel *panel;
	int i, ok, only1p, cancel, playerNo;

	// 一括設定者を前回から引き続くか?
	if(st->master >= 0) {
		if(!menuPlay2_checkInputMaster(st, st->master, st->master)) {
			st->master = -1;
		}
	}

	// 一括設定者を決定
	if(st->master < 0) {
		for(i = 0; i < st->playerCount; i++) {
			if(menuPlay2_checkInputMaster(st, st->playerTable[i], st->playerTable[i])) {
				st->master = st->playerTable[i];
				break;
			}
		}
	}

	// 1pのみが未決定状態か?
	for(i = 0; i < st->playerCount; i++) {
		playerNo = st->playerTable[i];
		panel = &st->panel[playerNo];

		if(playerNo == 0) {
			if(panel->flow == 1) break;
		}
		else {
			if(panel->flow != 1) break;
		}
	}
	only1p = (i == st->playerCount && st->cpuCount != 0);

	// プレイヤーのキー入力
	for(i = ok = cancel = 0; i < st->playerCount; i++) {
		playerNo = st->playerTable[i];
		panel = &st->panel[playerNo];

		if(st->master < 0 || st->master == playerNo) {
			menuPlay2Panel_input(panel, playerNo, true, !only1p || playerNo != 0);
		}

		switch(panel->flow) {
		case -1:
			cancel++;
			break;

		case 1:
			ok++;
			break;
		}
	}

	// メインメニューへ
	if(cancel) {
		MODE mode;

		switch(_getMode(st->global)) {
		case MODE_VSMAN:
			mode = MODE_VSMAN_CH;
			break;
		case MODE_VSMAN_FL:
			mode = MODE_VSMAN_FL_CH;
			break;
		case MODE_VSMAN_TA:
			mode = MODE_VSMAN_TA_CH;
			break;

		case MODE_VSCOM:
			mode = MODE_VSCOM_CH;
			break;
		case MODE_VSCOM_FL:
			mode = MODE_VSCOM_FL_CH;
			break;

		case MODE_PLAY4_LV:
			mode = MODE_PLAY4_CH;
			break;
		case MODE_PLAY4_TB_LV:
			mode = MODE_PLAY4_TB_CH;
			break;
		case MODE_PLAY4_FL_LV:
			mode = MODE_PLAY4_FL_CH;
			break;
		}

		st->depth--;
		_setMode(st->global, mode);
		menuPlay2_setFrame(st, -1, 1);
	}
	else if(ok == st->playerCount) {
		st->depth++;
	}
}

// CPUのキー入力
static void menuPlay2_inputCpu(SMenuPlay2 *st)
{
	SMenuPlay2Panel *panel;
	int i, pre, playerNo;

	// 一括設定の設定をOFF
	st->master = -1;

	// CPUのキー入力
	for(i = pre = 0; i < st->cpuCount; i++) {
		playerNo = st->cpuTable[i];
		panel = &st->panel[playerNo];

		if(panel->flow == 1) {
			pre = playerNo;
			continue;
		}

		// 一括設定の設定
		if(menuPlay2_checkInputMaster(st, playerNo, 0)) {
			st->master = playerNo;
		}

		// 1Pが代表で入力
		menuPlay2Panel_input(panel, 0, false, i == st->cpuCount - 1);

		if(panel->flow == -1) {
			st->panel[pre].flow = -1;

			// プレイヤーの入力へ戻る
			if(pre == 0) {
				st->depth--;
			}
		}

		break;
	}

	// 全入力が完了しているか?
	if(i == st->cpuCount) {
		st->depth++;
	}
}

// 入力
static void menuPlay2_input(SMenuPlay2 *st)
{
	game_config *cfg;
	game_state *state;
	SMenuPlay2Panel *panel;
	int i;

	if(st->panel[0].miBase->transStep < 0 || st->panel[0].miBase->transTime < 1) {
		return;
	}

	// [R-TRIG] + [OK] で 全決定
	if(st->master >= 0) {
		int trg = _getKeyTrg(st->global, st->master);
		int lvl = _getKeyLvl(st->global, st->master);

		if((lvl & R_TRIG) && (trg & CONT_OK)) {
			for(i = 0; i < st->panelCount; i++) {
				panel = &st->panel[i];
				panel->flow = 1;
			}
			dm_snd_play(SE_mDecide);
		}
	}

	switch(st->depth) {
	case -1:
		st->depth++;

	// プレイヤーの設定
	case 0:
		menuPlay2PanelSub_setFrame(st->panelSub, -1, st->panelSub->miBase->transTime);
		menuPlay2_inputMan(st);
		break;

	// CPUのキー入力
	case 1:
		menuPlay2PanelSub_setFrame(st->panelSub, -1, st->panelSub->miBase->transTime);
		menuPlay2_inputCpu(st);
		break;

	// 背景、音楽を選択
	case 2:
		// 一括設定をOFF
		st->master = -1;

		menuPlay2PanelSub_setFrame(st->panelSub, 1, st->panelSub->miBase->transTime);

		if(!menuPlay2PanelSub_input(st->panelSub, 0)) {
			break;
		}

		switch(st->panelSub->flow) {
		case -1:
			st->depth -= 2;
			for(i = 0; i < st->panelCount; i++) {
				st->panel[i].flow = -1;
				st->panel[i].depth = 0;
			}
			if(st->panelSub->musicItem->playNo >= 0) {
				dm_seq_play_fade(SEQ_Menu, BGM_FADE_FRAME);
				st->panelSub->musicItem->playNo = -1;
			}
			break;

		case 1:
			st->depth++;
			break;
		}
		break;
	}

	// 全部決定したか?
	if(st->depth < 3) {
		return;
	}

	// 各設定画面へ
	switch(_getMode(st->global)) {
	case MODE_VSMAN_FL:
	case MODE_VSMAN_TA:
	case MODE_VSMAN:
		for(i = 0; i < st->panelCount; i++) {
			panel = &st->panel[i];
			cfg = &evs_mem_data[evs_select_name_no[i]].config;

			if(panel->flash) {
				cfg->vm_fl_lv = panel->glvItem->select;
			}
			else if(panel->timeAt) {
				cfg->vm_ta_lv = panel->glvItem->select;
			}
			else {
				cfg->vm_lv = panel->lvGauge->level;
			}
			cfg->vm_sp = panel->speedItem->select;

			cfg->vm_st = st->panelSub->stageNum->number - 1;
			cfg->vm_m  = st->panelSub->musicItem->select;
		}
		break;

	case MODE_VSCOM_FL:
	case MODE_VSCOM:
		cfg = &evs_mem_data[evs_select_name_no[0]].config;
		for(i = 0; i < st->panelCount; i++) {
			panel = &st->panel[i];

			if(panel->flash) {
				cfg->vc_fl_lv[i] = panel->glvItem->select;
			}
			else {
				cfg->vc_lv[i] = panel->lvGauge->level;
			}
			cfg->vc_sp[i] = panel->speedItem->select;
		}
		cfg->vc_st = st->panelSub->stageNum->number - 1;
		cfg->vc_m  = st->panelSub->musicItem->select;
		break;

	case MODE_PLAY4_LV:
	case MODE_PLAY4_TB_LV:
	case MODE_PLAY4_FL_LV:
		for(i = 0; i < st->panelCount; i++) {
			panel = &st->panel[i];

			if(panel->flash) {
				evs_cfg_4p.p4_fl_lv[i] = panel->glvItem->select;
			}
			else {
				evs_cfg_4p.p4_lv[i] = panel->lvGauge->level;
			}
			evs_cfg_4p.p4_sp[i] = panel->speedItem->select;
		}
		evs_cfg_4p.p4_st = st->panelSub->stageNum->number - 1;
		evs_cfg_4p.p4_m  = st->panelSub->musicItem->select;
		break;
	}

	// ゲーム設定
	for(i = 0; i < st->panelCount; i++) {
		state = &game_state_data[i];
		panel = &st->panel[i];

		if(panel->flash) {
			state->virus_level = _timeAttack_levelTable[panel->glvItem->select];
			state->game_level = panel->glvItem->select;
		}
		else if(panel->timeAt) {
			state->virus_level = _timeAttack_levelTable[panel->glvItem->select];
			state->game_level = panel->glvItem->select;
		}
		else {
			state->virus_level = panel->lvGauge->level;
		}
		state->cap_def_speed = panel->speedItem->select;
	}

	// 背景を設定
	i = st->panelSub->stageNum->number - 1;
	story_proc_no  = _bgDataNo_to_stageNo[i][1] + 1;
	story_proc_no += _bgDataNo_to_stageNo[i][0] * STORY_W_OPEN;
	evs_story_no   = _bgDataNo_to_stageNo[i][1] + 1;

	// 音楽を設定
	evs_seqnumb = st->panelSub->musicItem->select;
	evs_seqence = (evs_seqnumb != 4);

	// タイムをクリア
	evs_game_time = 0;

	_setFadeDir(st->global, 1);
	_setNextMain(st->global, MAIN_12);
}

// 更新
static void menuPlay2_update(SMenuPlay2 *st)
{
	SMenuItem *miRoot = _getRootItem(st->global);
	int i;

	switch(st->depth) {
	case 0: case 1:
		menuItem_setColorDir(st->miONaJi, 1);
		break;

	default:
		menuItem_setColorDir(st->miONaJi, -1);
		break;
	}

	// マスターの設定に従う
	if(st->master >= 0) {
		menuPlay2_applyMasterConfig(st, st->master);
	}

	// パネル
	for(i = 0; i < st->panelCount; i++) {
		menuPlay2Panel_update(&st->panel[i], miRoot);
	}

	// マスターのカーソル設定に従う
	if(st->master >= 0) {
		menuPlay2_applyMasterCursor(st, st->master);
	}

	// サブパネル
	menuPlay2PanelSub_update(st->panelSub, miRoot);

	// みんな同じ
	menuItem_update(st->miONaJi, miRoot);
}

// 描画
static void menuPlay2_draw(SMenuPlay2 *st, Gfx **gpp)
{
	Gfx *gp = *gpp;
	SMenuItem *miRoot = _getRootItem(st->global);
	STexInfo *texC;
	void *list[st->panelCount];
	int i;

	// パネル
	for(i = 0; i < st->panelCount; i++) {
		list[i] = &st->panel[i];
	}
	menuPlay2Panel_draw((SMenuPlay2Panel **)list, st->panelCount, &gp);

	// サブパネル
	menuPlay2PanelSub_draw(st->panelSub, &gp);

	gSPDisplayList(gp++, fade_normal_texture_init_dl);
	gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);

	// みんな同じ
	texC = _getTexSetup(st->global, TEX_r_command);
	menuItem_drawTex(st->miONaJi, &gp, texC, false);

	*gpp = gp;
}

//////////////////////////////////////
//## 名前入力

typedef struct {
	void *global;
	int playerNo;
	int playerCount;

	int flow[2];
	int pageNo[2];
	int curPos[2][2];
	char name[2][MEM_NAME_SIZE];
	int namePos[2];

	SMenuItem miBase[1];
	SMenuItem miPanel[1];
	SMenuItem miPlayerNo[1];
	SMenuItem miTable[1];
	SMenuItem miCursor[1];
	SMenuCursor cursor[1];
	SMenuItem miName[1];
	SMenuItem miNamePos[1];
} SMenuNmEnt;

#define NAME_CH_TBL_W 15
#define NAME_CH_TBL_H 9
#define NAME_CH_STEP_X 15
#define NAME_CH_STEP_Y 13

static char *_nameEntry_charTable[] = {
#if LOCAL==JAPAN
	"あいうえお__らりるれろ__わをん"
	"かきくけこ__がぎぐげご____～！"
	"さしすせそ__ざじずぜぞ__・．？"
	"たちつてと__だぢづでど__000102"
	"なにぬねの__ばびぶべぼ__101112"
	"はひふへほ__ぱぴぷぺぽ__202122"
	"まみむめも__ぁぃぅぇぉ__303132"
	"や__ゆ__よ__ゃゅょっ____404142"
	"０１２３４__５６７８９__505152",

	"アイウエオ__ラリルレロ__ワヲン"
	"カキクケコ__ガギグゲゴ__ヴー＊"
	"サシスセソ__ザジズゼゾ__・．☆"
	"タチツテト__ダヂヅデド__000102"
	"ナニヌネノ__バビブベボ__101112"
	"ハヒフヘホ__パピプペポ__202122"
	"マミムメモ__ァィゥェォ__303132"
	"ヤ__ユ__ヨ__ャュョッ____404142"
	"０１２３４__５６７８９__505152",

	"ＡＢＣＤＥ__ａｂｃｄｅ__○×／"
	"ＦＧＨＩＪ__ｆｇｈｉｊ__・．："
	"ＫＬＭＮＯ__ｋｌｍｎｏ________"
	"ＰＱＲＳＴ__ｐｑｒｓｔ__000102"
	"ＵＶＷＸＹ__ｕｖｗｘｙ__101112"
	"Ｚ__________ｚ__________202122"
	"０１２３４__５６７８９__303132"
	"________________________404142"
	"________________________505152",
#elif LOCAL==AMERICA
	"ＡＢＣＤＥ__ａｂｃｄｅ__○×☆"
	"ＦＧＨＩＪ__ｆｇｈｉｊ__・．："
	"ＫＬＭＮＯ__ｋｌｍｎｏ__！？＆"
	"ＰＱＲＳＴ__ｐｑｒｓｔ__＊／％"
	"ＵＶＷＸＹ__ｕｖｗｘｙ__，’～"
	"Ｚ__________ｚ________________"
	"０１２３４__５６７８９__303132"
	"________________________404142"
	"________________________505152",
/*
	"ＡＢＣＤＥ__ａｂｃｄｅ__○×／"
	"ＦＧＨＩＪ__ｆｇｈｉｊ__・．："
	"ＫＬＭＮＯ__ｋｌｍｎｏ________"
	"ＰＱＲＳＴ__ｐｑｒｓｔ________"
	"ＵＶＷＸＹ__ｕｖｗｘｙ________"
	"Ｚ__________ｚ________________"
	"０１２３４__５６７８９__303132"
	"________________________404142"
	"________________________505152",
*/
#elif LOCAL==CHINA
	"ＡＢＣＤＥ__ａｂｃｄｅ__○×☆"
	"ＦＧＨＩＪ__ｆｇｈｉｊ__・．："
	"ＫＬＭＮＯ__ｋｌｍｎｏ__！？＆"
	"ＰＱＲＳＴ__ｐｑｒｓｔ__＊／％"
	"ＵＶＷＸＹ__ｕｖｗｘｙ__，’～"
	"Ｚ__________ｚ________________"
	"０１２３４__５６７８９__303132"
	"________________________404142"
	"________________________505152",
#endif
};

// フレームインアウト
static void menuNmEnt_setFrame(SMenuNmEnt *st, int dir, float time)
{
	SMenuItem *item = st->miBase;
	item->transRange[0][1] = item->transRange[1][1] - SCREEN_HT;
	item->transStep = FRAME_MOVE_STEP;
	item->transTime = time;
	menuItem_setTransDir(item, dir);
}

// 初期化
static void menuNmEnt_init(SMenuNmEnt *st, void *global, void **hpp)
{
	mem_char *mc;
	int i, j;

	st->global = global;

	switch(_getMode(global)) {
	case MODE_VSMAN_NE:
	case MODE_VSMAN_FL_NE:
	case MODE_VSMAN_TA_NE:
		st->playerCount = 2;
		break;
	default:
		st->playerCount = 1;
		break;
	}

	for(i = 0; i < st->playerCount; i++) {
		// ゲストが選択された?
		if(evs_select_name_no[i] == DM_MEM_GUEST) {
			continue;
		}

		mc = &evs_mem_data[evs_select_name_no[i]];
		if(_getMode(global) == MODE_NAME_NE2 || !(mc->mem_use_flg & DM_MEM_USE)) {
			break;
		}
	}
	st->playerNo = i;

	for(i = 0; i < ARRAY_SIZE(st->name); i++) {
		st->flow[i] = -1;
		st->pageNo[i] = 0;
		st->curPos[i][0] = 0;
		st->curPos[i][1] = 0;

		if(_getMode(global) == MODE_NAME_NE2 &&
			evs_select_name_no[i] != DM_MEM_GUEST)
		{
			bcopy(evs_mem_data[evs_select_name_no[i]].mem_name, st->name[i], MEM_NAME_SIZE);
		}
		else {
			for(j = 0; j < MEM_NAME_SIZE; j++) {
				st->name[i][j] = 0;
			}
		}
		st->namePos[i] = 0;
	}

	// 下地
	menuItem_init(st->miBase, 29, 98);

	// パネル
	menuItem_init(st->miPanel, 47, -47);

	// プレイヤー番号
#if LOCAL==JAPAN
	menuItem_init(st->miPlayerNo, 9, 5);
#elif LOCAL==AMERICA
	menuItem_init(st->miPlayerNo, 100, 5);
#elif LOCAL==CHINA
	menuItem_init(st->miPlayerNo, 80, 7);
#endif

	// テーブル
	menuItem_init(st->miTable, 20, 7);

	// カーソル
	menuItem_init(st->miCursor, 0, 0);
	menuCursor_init(st->cursor, global, CURSOR_ITEM, st->playerNo, -2, -2, 16, 16);
	st->cursor->flags.finger = 1;

	// 入力位置
	menuItem_init(st->miName, 59, 23);
	menuItem_init(st->miNamePos, 0, 16);

	// フレームイン
	menuNmEnt_setFrame(st, 1, 0);
}

// EEPROMアクセス中の処理
static void _menuNmEnt_updateCallback(void *args) {
	static void menuNmEnt_update(SMenuMain *st);
	menuNmEnt_update((SMenuMain *)args);
}

// 入力
static void menuNmEnt_input(SMenuNmEnt *st)
{
	int rep = _getKeyRep(st->global, st->playerNo);
	int trg = _getKeyTrg(st->global, st->playerNo);
	int i, vx, vy, sound = -1, exitDir = 0;
	int p = st->playerNo;

	if(st->miBase->transStep < 0 || st->miBase->transTime < 1) {
		return;
	}

	vx = vy = 0;
	if(rep & L_JPAD) vx--;
	if(rep & R_JPAD) vx++;
	if(rep & U_JPAD) vy--;
	if(rep & D_JPAD) vy++;

	// カーソル移動
	if(vx || vy) {
		int ix, iy, flag;
		char *c;

		flag = 0;
		ix = st->curPos[p][0];
		iy = st->curPos[p][1];

		do {
			c = &_nameEntry_charTable [st->pageNo[p]] [(ix + iy * NAME_CH_TBL_W) * 2];

#if 0
			if(isdigit(c[0]) && vx) {
				int v = (vx > 0 ? 3 : -1) - vx * (c[1] - '0');
				ix = WrapI(0, NAME_CH_TBL_W, ix + v);
			}
			else {
				ix = WrapI(0, NAME_CH_TBL_W, ix + vx);
			}
#else
                        ix = WrapI(0, NAME_CH_TBL_W, ix + vx);
#endif

			iy = WrapI(0, NAME_CH_TBL_H, iy + vy);

			c = &_nameEntry_charTable [st->pageNo[p]] [(ix + iy * NAME_CH_TBL_W) * 2];
		} while(c[0] == '_');

		if(c[0] & 0x80) {
			st->flow[p] = -1;
			flag++;
		}
		else if(isdigit(c[0])) {
			st->flow[p] = (c[0] - '0');
			ix -= (c[1] - '0');
			flag = (ix != st->curPos[p][0] || iy != st->curPos[p][1]);
		}

		if(flag) {
			st->curPos[p][0] = ix;
			st->curPos[p][1] = iy;
			sound = SE_mUpDown;
		}
	}

	// 全決定
	if((trg & START_BUTTON) && st->flow[p] != 5) {
		st->flow[p] = 5;
		st->curPos[p][0] = 12;
		st->curPos[p][1] = 8;
		sound = SE_mDecide;
	}
#if LOCAL==JAPAN
	// コードページ切り替え
	else if(trg & (L_TRIG | R_TRIG)) {
		vx = 0;
		if(trg & L_TRIG) vx--;
		if(trg & R_TRIG) vx++;
		if(vx) {
			st->pageNo[p] = WrapI(0, 3, st->pageNo[p] + vx);
			st->curPos[p][0] = 0;
			st->curPos[p][1] = 0;
			sound = SE_mDecide;
		}
	}
#endif
	// 決定
	else if(trg & CONT_OK) {
		char *c = &_nameEntry_charTable[st->pageNo[p]]
			[(st->curPos[p][0] + st->curPos[p][1] * NAME_CH_TBL_W) * 2];

		switch(st->flow[p]) {
		// 文字入力
		case -1:
			if(st->namePos[p] < MEM_NAME_SIZE) {
				st->name[p][st->namePos[p]] = font2index(c);
				st->namePos[p]++;
				sound = SE_mDecide;
			}
			if(st->namePos[p] == MEM_NAME_SIZE) {
				st->curPos[p][0] = NAME_CH_TBL_W - 3;
				st->curPos[p][1] = NAME_CH_TBL_H - 1;
				st->flow[p] = 5;
			}
			break;
#if LOCAL==JAPAN
		// 平仮名
		case 0:
			st->pageNo[p] = 0;
			sound = SE_mDecide;
			break;

		// 片仮名
		case 1:
			st->pageNo[p] = 1;
			sound = SE_mDecide;
			break;

		// 英
		case 2:
			st->pageNo[p] = 2;
			sound = SE_mDecide;
			break;
#endif
		// スペース
		case 3:
			if(st->namePos[p] < MEM_NAME_SIZE) {
				st->name[p][st->namePos[p]] = 0;
				st->namePos[p]++;
				sound = SE_mDecide;
			}
			if(st->namePos[p] == MEM_NAME_SIZE) {
				st->curPos[p][0] = NAME_CH_TBL_W - 3;
				st->curPos[p][1] = NAME_CH_TBL_H - 1;
				st->flow[p] = 5;
			}
			break;

		// 戻る
		case 4:
			if(st->namePos[p] > 0) {
				st->namePos[p]--;
				st->name[p][st->namePos[p]] = 0;
				sound = SE_mCANCEL;
			}
			else {
				sound = SE_mError;
			}
			break;

		// 終わり
		case 5:
			for(i = 0; i < MEM_NAME_SIZE; i++) {
				if(st->name[p][i] != 0) {
					break;
				}
			}

			if(i != MEM_NAME_SIZE) {
				exitDir++;
				sound = SE_mDecide;
			}
			else {
				sound = SE_mError;
			}
			break;
		}
	}
	// キャンセル
	else if(trg & CONT_CANCEL) {
		if(st->namePos[p] > 0) {
			st->namePos[p]--;
			st->name[p][st->namePos[p]] = 0;
		}
		else {
			exitDir--;
		}
		sound = SE_mCANCEL;
	}

	if(exitDir > 0) {
		mem_char *mc = &evs_mem_data[ evs_select_name_no[p] ];

		// 名前変更ではない場合
		if(_getMode(st->global) != MODE_NAME_NE2) {
			// プレイヤー情報を初期化
			dm_init_save_mem(mc);
		}

		// 使用済みフラグを設定
		mc->mem_use_flg |= DM_MEM_USE;

		// 名前をセーブ用構造体へ代入
		for(i = 0; i < MEM_NAME_SIZE; i++) {
			mc->mem_name[i] = st->name[p][i];
		}

		for(i = p + 1; i < st->playerCount; i++) {
			mem_char *mc;

			if(evs_select_name_no[i] == DM_MEM_GUEST) {
				continue;
			}

			mc = &evs_mem_data[evs_select_name_no[i]];
			if(_getMode(st->global) == MODE_NAME_NE2 || !(mc->mem_use_flg & DM_MEM_USE)) {
				break;
			}
		}

		if(i >= st->playerCount) {
			MODE mode;

			// プレイヤー情報を書き込む
			_eepWritePlayer(st->global);

			switch(_getMode(st->global)) {
			case MODE_STORY_NE:
				mode = MODE_STORY;
				break;
			case MODE_LVSEL_NE:
				mode = MODE_LVSEL;
				break;
			case MODE_LVSEL_TQ_NE:
				mode = MODE_LVSEL_TQ;
				break;
			case MODE_LVSEL_TA_NE:
				mode = MODE_LVSEL_TA;
				break;

			case MODE_VSCOM_NE:
				mode = MODE_VSCOM_CH;
				break;
			case MODE_VSCOM_FL_NE:
				mode = MODE_VSCOM_FL_CH;
				break;

			case MODE_VSMAN_NE:
				mode = MODE_VSMAN_CH;
				break;
			case MODE_VSMAN_FL_NE:
				mode = MODE_VSMAN_FL_CH;
				break;
			case MODE_VSMAN_TA_NE:
				mode = MODE_VSMAN_TA_CH;
				break;

			case MODE_NAME_NE:
				mode = MODE_MAIN;
				break;
			case MODE_NAME_NE2:
				mode = MODE_MAIN;
				break;
			}

			_setMode(st->global, mode);
			menuNmEnt_setFrame(st, -1, 1);
		}
		else {
			st->playerNo = i;
		}
	}
	else if(exitDir < 0) {
		for(i = p - 1; i >= 0; i--) {
			mem_char *mc;

			if(evs_select_name_no[i] == DM_MEM_GUEST) {
				continue;
			}

			mc = &evs_mem_data[evs_select_name_no[i]];
			if(_getMode(st->global) == MODE_NAME_NE2 || !(mc->mem_use_flg & DM_MEM_USE)) {
				break;
			}
		}

		if(i < 0) {
			switch(_getMode(st->global)) {
			case MODE_NAME_NE2:
				_setMode(st->global, MODE_MAIN);
				break;

			default:
				_setMode(st->global, MODE_MAIN);
				break;
			}
			menuNmEnt_setFrame(st, -1, 1);
		}
		else {
			st->playerNo = i;
		}
		sound = SE_mCANCEL;
	}

	if(sound >= 0) {
		dm_snd_play(sound);
	}
}

// 更新
static void menuNmEnt_update(SMenuNmEnt *st)
{
	SMenuItem *miRoot = _getRootItem(st->global);
	float x, y;

	// 下地
	menuItem_update(st->miBase, miRoot);

	// パネル
	menuItem_update(st->miPanel, st->miBase);

	// プレイヤー番号
	menuItem_update(st->miPlayerNo, st->miPanel);

	// テーブル
	menuItem_update(st->miTable, st->miBase);

	// カーソル
	st->cursor->size[0] = (st->flow[st->playerNo] == -1 ? 16 : 50);
	st->cursor->playerNo = st->playerNo;
	x = st->curPos[st->playerNo][0] * NAME_CH_STEP_X;
	y = st->curPos[st->playerNo][1] * NAME_CH_STEP_Y;
	menuItem_setTransHi(st->miCursor, x, y);
	menuItem_update(st->miCursor, st->miTable);
	menuCursor_update(st->cursor, st->miCursor);

	// 名前入力位置
	menuItem_update(st->miName, st->miPanel);
	st->miNamePos->transRange[1][0] = st->namePos[st->playerNo] * 13;
	menuItem_update(st->miNamePos, st->miName);
}

// 描画
static void menuNmEnt_draw(SMenuNmEnt *st, Gfx **gpp)
{
	Gfx *gp = *gpp;
	STexInfo *texC, *texA;
	void *list[1];
	char *tbl;
	int i, j, tx, ty;

	gSPDisplayList(gp++, fade_normal_texture_init_dl);

	// パネル
	texC = _getTexName(st->global, TEX_name_frame);
	menuItem_drawTex(st->miPanel, &gp, texC, false);

	// プレイヤー番号
#if LOCAL==JAPAN
	gSPDisplayList(gp++, fade_alpha_texture_init_dl);
	texC = _getTexName(st->global, TEX_name_frame_no);
	texA = _getTexName(st->global, TEX_name_frame_no_alpha);
	menuItem_drawAlphaItem(st->miPlayerNo, &gp, texC, texA, false, 2, st->playerNo);
#elif LOCAL==AMERICA
	if(st->playerNo == 1) {
		texC = _getTexName(st->global, TEX_name_frame_2);
		menuItem_drawTex(st->miPlayerNo, &gp, texC, false);
	}
#elif LOCAL==CHINA
	if(st->playerNo == 1) {
		texC = _getTexName(st->global, TEX_name_frame_2);
		menuItem_drawTex(st->miPlayerNo, &gp, texC, false);
	}
#endif

	gSPDisplayList(gp++, fade_normal_texture_init_dl);

	// 下地
	texC = _getTexName(st->global, TEX_letter_panel);
	if(!menuItem_drawTex(st->miBase, &gp, texC, false)) {
		return;
	}

	// 文字
	gSPDisplayList(gp++, fade_intensity_texture_init_dl);
	gDPSetTextureFilter(gp++, G_TF_BILERP);
	gDPSetPrimColor(gp++, 0,0, 0,0,0,255);

	tbl = _nameEntry_charTable[st->pageNo[st->playerNo]];
	ty = st->miTable->trans[1];
	for(i = 0; i < NAME_CH_TBL_H; i++) {
		tx = st->miTable->trans[0];
		for(j = 0; j < NAME_CH_TBL_W; j++) {
			if(tbl[0] & 0x80) {
				fontXX_draw(&gp, tx, ty, 12, 12, tbl);
			}
			tx += NAME_CH_STEP_X;
			tbl += 2;
		}
		ty += NAME_CH_STEP_Y;
	}

	// 名前
	tx = st->miName->trans[0];
	ty = st->miName->trans[1];
	for(i = 0; i < MEM_NAME_SIZE; i++) {
		fontXX_drawID(&gp, tx, ty, 12, 12, (int)st->name[st->playerNo][i] & 0xff);
		tx += 13;
	}

	gSPDisplayList(gp++, fade_alpha_texture_init_dl);

	// 入力位置
	texC = _getTexName(st->global, TEX_position_cursor);
	texA = _getTexName(st->global, TEX_position_cursor_alpha);
	menuItem_drawAlphaTex(st->miNamePos, &gp, texC, texA, false);

	// カーソル
	list[0] = st->cursor;
	menuCursor_draw((SMenuCursor **)list, 1, &gp);

	*gpp = gp;
}

//////////////////////////////////////////////////////////////////////////////
//{### 記録

//////////////////////////////////////
//## 下地

typedef struct {
	void *global;
	SMenuItem miBase[1];
} SMenuRankBase;

// 初期化
static void menuRankBase_init(SMenuRankBase *st, void *global, int x, int y)
{
	st->global = global;
	menuItem_init(st->miBase, x, y);
}

// 更新
static void menuRankBase_update(SMenuRankBase *st, SMenuItem *parent)
{
	menuItem_update(st->miBase, parent);
}

// 描画
static void menuRankBase_draw(SMenuRankBase **stAry, int count, Gfx **gpp)
{
	Gfx *gp = *gpp;
	SMenuRankBase *st;
	SMenuItem *item;
	STexInfo *texC;
	int i, j, cached;
	float y;

	gSPDisplayList(gp++, fade_normal_texture_init_dl);

	for(j = 0; j < 2; j++) {
		cached = 0;
		for(i = 0; i < count; i++) {
			st = stAry[i];
			item = st->miBase;
			y = item->trans[1];
			item->trans[1] += j * 8;
			texC = _getTexRank(st->global, TEX_rank_panel);
			cached += menuItem_drawItem(item, &gp, texC, cached, 2, j);
			item->trans[1] = y;
		}
	}

	*gpp = gp;
}

//////////////////////////////////////
//## 順位

typedef struct {
	void *global;
	int number;
	SMenuItem miBase[1];
} SMenuRankNum;

// 初期化
static void menuRankNum_init(SMenuRankNum *st, void *global, int number, int x, int y)
{
	st->global = global;
	st->number = number;
	menuItem_init(st->miBase, x, y);
}

// 更新
static void menuRankNum_update(SMenuRankNum *st, SMenuItem *parent)
{
	menuItem_update(st->miBase, parent);
}

// 描画
static void menuRankNum_draw(SMenuRankNum **stAry, int count, Gfx **gpp)
{
	Gfx *gp = *gpp;
	SMenuRankNum *st;
	SMenuItem *item;
	STexInfo *texC, *texA;
	int i, j, cached, init;
	float x, y;

	for(i = -1; cached = init = 0, i < 10; i++) {
	for(j = 0; j < count; j++) {
		st = stAry[j];
		item = st->miBase;
		if(i != st->number) continue;

		switch(i) {
		case -1:
			if(!init) {
				gSPDisplayList(gp++, fade_normal_texture_init_dl);
				init++;
			}
			texC = _getTexRank(st->global, TEX_vs);
			cached += menuItem_drawTex(item, &gp, texC, cached);
			break;

		default:
			if(!init) {
				gSPDisplayList(gp++, fade_alpha_texture_init_dl);
				init++;
			}
			texC = _getTexGameP1(st->global, TEX_P1_VS_TIME_NUMBER);
			texA = _getTexGameP1(st->global, TEX_P1_VS_TIME_NUMBER_ALPHA);
			x = item->trans[0]; y = item->trans[1];
			item->trans[0] += 2; item->trans[1] += 2;
			cached += menuItem_drawAlphaItem(item, &gp, texC, texA, cached, 12, st->number + 2);
			item->trans[0] = x; item->trans[1] = y;
			break;
		}
	}}

	*gpp = gp;
}

//////////////////////////////////////
//## 数字

typedef enum {
	RANK_FIG_NUMBER = 7,
	RANK_FIG_TIME,
	RANK_FIG_RATIO,
	RANK_FIG_LEVEL,
	RANK_FIG_TIME2,
	RANK_FIG_CLEAR,
	RANK_FIG_ALL,
	RANK_FIG_SUM,
} RANK_FIG;

typedef struct {
	void *global;
	RANK_FIG type;
	int column;
	int columns[10];
	int number;
	SMenuItem miBase[1];
} SMenuRankFig;

#define RANK_NUM_Y  -1
#define RANK_NAME_Y  2
#define RANK_FIG_Y   2

// 初期化
static void menuRankFig_init(SMenuRankFig *st, void *global,
	RANK_FIG column, int number, int color, int x, int y)
{
	static const u8 _color[][3] = {
		{ 255, 255, 255 }, // 白
		{ 242,  96,  96 }, // 赤
		{   0,   0, 255 }, // 青
		{ 255, 255,   0 }, // 黄
		{ 106, 243,  79 }, // 緑
		{ 255, 154,  83 }, // オレンジ
		{ 255, 152, 195 }, // ピンク
		{  74, 226, 227 }, // 水色
	};
	SMenuItem *item;
	const u8 *colorP;
	int i;

	st->global = global;

	// 通常の数値
	if(column <= RANK_FIG_NUMBER) {
		st->type = RANK_FIG_NUMBER;
		st->column = column;
	}
	// タイム
	else if(column == RANK_FIG_TIME) {
		st->type = column;
		st->column = 5;
	}
	// 勝率
	else if(column == RANK_FIG_RATIO) {
		st->type = column;
		st->column = 6;
	}
	// レベル
	else if(column == RANK_FIG_LEVEL) {
		st->type = column;
#if LOCAL==JAPAN
		st->column = 5;
#elif LOCAL==AMERICA
		st->column = 7;
#elif LOCAL==CHINA
		st->column = 7;
#endif
	}
	// タイム2
	else if(column == RANK_FIG_TIME2) {
		st->type = column;
		st->column = 6;
	}
	// ＣＬＥＡＲ
	else if(column == RANK_FIG_CLEAR) {
		st->type = column;
		st->column = 5;
	}
	// ＡＬＬ
	else if(column == RANK_FIG_ALL) {
		st->type = column;
		st->column = 3;
	}

	st->number = number;
	menuItem_init(st->miBase, x, y);

	// 色を設定
	item = st->miBase;
	colorP = _color[color];
	for(i = 0; i < 3; i++) {
		item->colorRange[0][i] = item->colorRange[1][i] = colorP[i] * (1.0 / 255.0);
	}
}

// 更新
static void menuRankFig_update(SMenuRankFig *st, SMenuItem *parent)
{
	int i, n = st->number;

	switch(st->type) {
	// 数値
	case RANK_FIG_NUMBER:
		for(i = st->column - 1; i >= 0; i--) {
			st->columns[i] = n % 10;
			n /= 10;
		}
		break;

	// タイム
	case RANK_FIG_TIME:
		st->columns[4] = n % 10; n /= 10;
		st->columns[3] = n %  6; n /=  6;
		st->columns[2] = 15; // '
		st->columns[1] = n % 10; n /= 10;
		st->columns[0] = n % 10;
		break;

	// 勝率
	case RANK_FIG_RATIO:
		st->columns[5] = 16; // %
		st->columns[4] = n % 10; n /= 10;
		st->columns[3] = 17; // .
		st->columns[2] = n % 10; n /= 10;
		st->columns[1] = n % 10; n /= 10;
		st->columns[0] = n % 10;
		break;

	// レベル
	case RANK_FIG_LEVEL:
#if LOCAL==JAPAN
		st->columns[4] = n % 10; n /= 10;
		st->columns[3] = n % 10;
		st->columns[2] = 20; // れ
		st->columns[1] = 19; // べ
		st->columns[0] = 18; // る
#elif LOCAL==AMERICA
		st->columns[6] = n % 10; n /= 10;
		st->columns[5] = n % 10;
		st->columns[4] = 11; // Ｌ
		st->columns[3] = 12; // Ｅ
		st->columns[2] = 18; // Ｖ
		st->columns[1] = 12; // Ｅ
		st->columns[0] = 11; // Ｌ
#elif LOCAL==CHINA
		st->columns[6] = n % 10; n /= 10;
		st->columns[5] = n % 10;
		st->columns[4] = 11; // Ｌ
		st->columns[3] = 12; // Ｅ
		st->columns[2] = 18; // Ｖ
		st->columns[1] = 12; // Ｅ
		st->columns[0] = 11; // Ｌ
#endif
		break;

	// タイム2
	case RANK_FIG_TIME2:
		st->columns[5] = n % 10; n /= 10;
		st->columns[4] = 15; // .
		st->columns[3] = n % 10; n /= 10;
		st->columns[2] = n %  6; n /=  6;
		st->columns[1] = 15; // '
		st->columns[0] = n % 10;
		break;

	// ＣＬＥＡＲ
	case RANK_FIG_CLEAR:
		st->columns[0] = 10; // Ｃ
		st->columns[1] = 11; // Ｌ
		st->columns[2] = 12; // Ｅ
		st->columns[3] = 13; // Ａ
		st->columns[4] = 14; // Ｒ
		break;

	// ＡＬＬ
	case RANK_FIG_ALL:
		st->columns[0] = 13; // Ａ
		st->columns[1] = 11; // Ｌ
		st->columns[2] = 11; // Ｌ
		break;
	}

	menuItem_update(st->miBase, parent);
}

// 描画
static void menuRankFig_draw(SMenuRankFig **stAry, int count, Gfx **gpp)
{
	static char _code[] = {
		0x82,0x4f, 0x82,0x50, 0x82,0x51, 0x82,0x52, 0x82,0x53,
		0x82,0x54, 0x82,0x55, 0x82,0x56, 0x82,0x57, 0x82,0x58,
		0x82,0x62, 0x82,0x6b, 0x82,0x64, 0x82,0x60, 0x82,0x71, // ＣＬＥＡＲ
		0x81,0x46, 0x81,0x93, 0x81,0x44, // ：％．
#if LOCAL==JAPAN
		0x83,0x8c, 0x83,0x78, 0x83,0x8b, // レベル
#elif LOCAL==AMERICA
		0x82,0x75, // "LEVEL" の Ｖ
#elif LOCAL==CHINA
		0x82,0x75, // "LEVEL" の Ｖ
#endif
	};
	static const int _posX_0[] = { 0,9,18,27,36,45,54,63,72,81 };
	static const int _posX_1[] = { 0,9,16,23,32 };
	static const int _posX_2[] = { 0,9,18,27,31,40 };
	static const int _posX_4[] = { 0,7,14,23,30,37 };
	static const int *_posX_tbl[] = { _posX_0, _posX_1, _posX_2, _posX_0, _posX_4, _posX_0, _posX_0 };
	Gfx *gp = *gpp;
	SMenuRankFig *st;
	SMenuItem *item;
	STexInfo *texC, *texA;
	int i, j, k, code, cached;
	float step = 9;

//	gSPDisplayList(gp++, fade_intensity_texture_init_dl);
//	gDPSetTextureFilter(gp++, G_TF_BILERP);
	font16_initDL2(&gp);
	gDPSetTextureFilter(gp++, G_TF_BILERP);

	for(i = 0; cached = 0, i < ARRAY_SIZE(_code) / 2; i++) {
		code = font2index(&_code[i * 2]);

		for(j = 0; j < count; j++) {
			st = stAry[j];
			item = st->miBase;

			for(k = 0; k < st->column; k++) {
				if(i != st->columns[k] || menuItem_outOfScreen(item, step, 12)) {
					continue;
				}

                                menuItem_setPrim(item, &gp);
                              
#if (LOCAL == CHINA)
                                if (st->column > 5 && st->columns[0] == 11 
                                    && st->columns[1] == 12 &&  st->columns[2] == 18 && 
                                    st->columns[3] == 12 && st->columns[4] == 11) {
                                    if (k < 5) continue;
				    fontXX_drawID2(&gp,
					item->trans[0] + _posX_tbl[st->type - RANK_FIG_NUMBER][k-2]*item->scale[0],
					item->trans[1],
					item->scale[0] * step,
					item->scale[1] * 12, code);
				} else 
                                    fontXX_drawID2(&gp,
					item->trans[0] + _posX_tbl[st->type - RANK_FIG_NUMBER][k] * item->scale[0],
					item->trans[1],
					item->scale[0] * step,
					item->scale[1] * 12, code);
                                code = -1;
#else
				if(fontXX_drawID2(&gp,
					item->trans[0] + _posX_tbl[st->type - RANK_FIG_NUMBER][k] * item->scale[0],
					item->trans[1],
					item->scale[0] * step,
					item->scale[1] * 12, code))
				{
					code = -1;
				}
#endif
			}
		}
	}

	*gpp = gp;
}

//////////////////////////////////////
//## 名前

typedef struct {
	void *global;
	char name[MEM_NAME_SIZE];
	SMenuItem miBase[1];
} SMenuRankName;

// 初期化
static void menuRankName_init(SMenuRankName *st, void *global, const char *name, int x, int y)
{
	static const u8 _color[] = { 128, 128, 128 };
	SMenuItem *item;
	int i;

	st->global = global;

	for(i = 0; i < ARRAY_SIZE(st->name); i++) {
		st->name[i] = name[i];
	}

	// 下地
	item = st->miBase;
	menuItem_init(item, x, y);
	for(i = 0; i < 3; i++) {
		item->colorRange[0][i] = item->colorRange[1][i] = _color[i] * (1.0 / 255.0);
	}
}

// 更新
static void menuRankName_update(SMenuRankName *st, SMenuItem *parent)
{
	// 下地
	menuItem_update(st->miBase, parent);
}

// 描画
static void menuRankName_draw(SMenuRankName **stAry, int count, Gfx **gpp)
{
	Gfx *gp = *gpp;
	SMenuRankName *st;
	SMenuItem *item;
	float x, step = 13;
	int i, j;

//	gSPDisplayList(gp++, fade_intensity_texture_init_dl);
//	gDPSetTextureFilter(gp++, G_TF_BILERP);
	font16_initDL2(&gp);
	gDPSetTextureFilter(gp++, G_TF_BILERP);

	// 名前
	for(i = 0; i < count; i++) {
		st = stAry[i];
		item = st->miBase;

		if(menuItem_outOfScreen(item, step * MEM_NAME_SIZE, 12)) {
			continue;
		}

		x = item->trans[0];
		menuItem_setPrim(item, &gp);

		for(j = 0; j < MEM_NAME_SIZE; j++) {
			fontXX_drawID2(&gp, x, item->trans[1],
				12, 12 * item->scale[1], (int)st->name[j] & 0xff);
			x += step;
		}
	}

	*gpp = gp;
}

//////////////////////////////////////
//## ラベル

typedef struct {
	void *global;
	int type;
	int index;
	SMenuItem miBase[1];
} SMenuRankLabel;

// 初期化
static void menuRankLabel_init(SMenuRankLabel *st, void *global, int type, int index, int x, int y)
{
	st->global = global;
	st->type = type;
	st->index = index;
	menuItem_init(st->miBase, x, y);
}

// 更新
static void menuRankLabel_update(SMenuRankLabel *st, SMenuItem *parent)
{
	menuItem_update(st->miBase, parent);
}

// 描画
static void menuRankLabel_draw(SMenuRankLabel **stAry, int count, Gfx **gpp)
{
#if LOCAL==JAPAN
	static const int _rows[] = { 10, 3, 6, 7 };
#elif LOCAL==AMERICA
	static const int _rows[] = { 9, 2, 6, 7 };
#elif LOCAL==CHINA
	static const int _rows[] = { 9, 2, 6, 7 };
#endif
	static const int _tex[] = { 0, TEX_rank_head_b, TEX_rank_lv };
	static const int _choice[] = { 4, 5, 6, 29, 30, 31, 32 };

	Gfx *gp = *gpp;
	SMenuRankLabel *st;
	SMenuItem *item;
	STexInfo *texC;
	int i, j, k, rows, cached;

	gSPDisplayList(gp++, fade_normal_texture_init_dl);

	for(i = 0; rows = _rows[i], i < 4; i++) {
	for(j = 0; cached = 0, j < rows; j++) {
	for(k = 0; k < count; k++) {
		st = stAry[k];
		item = st->miBase;

		if(i != st->type || j != st->index) {
			continue;
		}

		switch(i) {
		// なまえ、スコア、タイム、など
		case 0:
			texC = _getTexRank(st->global, TEX_rank_head_a);
			cached += menuItem_drawItem(item, &gp, texC, cached, rows, j);
			break;

		// ゲームレベル、スピード、プレイヤー
		// easy, normal, hard
		// low, med, hi
		case 1:
		case 2:
			texC = _getTexRank(st->global, _tex[i]);
			cached += menuItem_drawItem(item, &gp, texC, cached, rows, j);
			break;

		// おはなし、エンドレス、ＶＳＣＯＭ、ＶＳ２Ｐ、フラッシュ、たいきゅう、スコアアタック
		case 3:
			texC = _getTexCommon(st->global, TEX_choice_a);
			cached += menuItem_drawItem(item, &gp, texC, cached, CHOICE_A_SIZE, _choice[j]);
			break;
		}
	}}}

	*gpp = gp;
}

//////////////////////////////////////
//## ヘッダー

typedef struct {
	void *global;
	int labelCount;
	SMenuItem miBase[1];
	SMenuRankLabel label[5];
} SMenuRankHeader;

// 初期化
static void menuRankHeader_init(SMenuRankHeader *st, void *global,
	const int *labelType, const int *labelIndex,
	const int *labelX, const int *labelY,
	int labelCount, int x, int y)
{
	int i;

	st->global = global;
	st->labelCount = labelCount;

	// 下地
	menuItem_init(st->miBase, x, y);

	// ラベル
	for(i = 0; i < labelCount; i++) {
		menuRankLabel_init(&st->label[i], global,
			labelType[i], labelIndex[i], labelX[i], labelY[i]);
	}
}

// 更新
static void menuRankHeader_update(SMenuRankHeader *st, SMenuItem *parent)
{
	int i;

	// 下地
	menuItem_update(st->miBase, parent);

	// ラベル
	for(i = 0; i < st->labelCount; i++) {
		menuRankLabel_update(&st->label[i], st->miBase);
	}
}

// 描画
static void menuRankHeader_draw(SMenuRankHeader **stAry, int count, Gfx **gpp)
{
	Gfx *gp = *gpp;
	SMenuRankHeader *st;
	void *list[ARRAY_SIZE(st->label) * count];
	int i, j, k;

	// ラベル
	for(i = j = 0; i < count; i++) {
		st = stAry[i];
		for(k = 0; k < st->labelCount; k++) {
			list[j++] = &st->label[k];
		}
	}
	menuRankLabel_draw((SMenuRankLabel **)list, j, &gp);

	*gpp = gp;
}

//////////////////////////////////////
//## パネル

typedef struct {
	void *global;
	int type;

	SMenuRankBase base[1];
	SMenuRankNum rank[1];
	SMenuRankName name[1];

	union {
		struct {
			SMenuRankFig score[1];
			SMenuRankFig time[1];
			SMenuRankFig stage[1];
		} oha;

		struct {
			SMenuRankFig level[1];
			SMenuRankFig score[1];
		} end;

		struct {
			SMenuRankFig time[1];
			SMenuRankFig score[1];
		} taiQ;

		struct {
			SMenuRankFig score[1];
			SMenuRankFig time[1];
			SMenuRankFig erase[1];
		} timeAt;

		struct {
			SMenuRankFig ratio[1];
			SMenuRankFig win[1];
			SMenuRankFig lose[1];
		} vs1;
	} misc;
} SMenuRankPanel;

// 初期化(お話)
static void menuRankPanel_initSt(SMenuRankPanel *st, void *global,
	int rank, const char *name, int score, int time, int stage, int x, int y)
{
	int col, pos;

	st->global = global;
	st->type = 0;

	// 下地
	menuRankBase_init(st->base, global, x, y);

	// 順位
	menuRankNum_init(st->rank, global, rank, 4, RANK_NUM_Y);

	// 名前
	menuRankName_init(st->name, global, name, 26, RANK_NAME_Y);

	// お話
	menuRankFig_init(st->misc.oha.score, global, 7,             score, 1, 131, RANK_FIG_Y);
	menuRankFig_init(st->misc.oha.time,  global, RANK_FIG_TIME, time,  2, 205, RANK_FIG_Y);

	pos = 90;
	if(stage < 10) {
		col = 1;
		pos += 9;
	}
	else {
		col = RANK_FIG_ALL;
	}
	menuRankFig_init(st->misc.oha.stage, global, col, stage, 3, pos, RANK_FIG_Y);
}

// 初期化(エンドレス)
static void menuRankPanel_initLs(SMenuRankPanel *st, void *global,
	int rank, const char *name, int level, int score, int x, int y)
{
	st->global = global;
	st->type = 1;

	// 下地
	menuRankBase_init(st->base, global, x, y);

	// 順位
	menuRankNum_init(st->rank, global, rank, 4, RANK_NUM_Y);

	// 名前
	menuRankName_init(st->name, global, name, 41, RANK_NAME_Y);

	// エンドレス
#if LOCAL==JAPAN
	menuRankFig_init(st->misc.end.level, global, RANK_FIG_LEVEL, level, 4, 115, RANK_FIG_Y);
#elif LOCAL==AMERICA
	menuRankFig_init(st->misc.end.level, global, RANK_FIG_LEVEL, level, 4, 106, RANK_FIG_Y);
#elif LOCAL==CHINA
	menuRankFig_init(st->misc.end.level, global, RANK_FIG_LEVEL, level, 4, 106, RANK_FIG_Y);
#endif
	menuRankFig_init(st->misc.end.score, global, 7,              score, 1, 183, RANK_FIG_Y);
}

// 初期化(耐久)
static void menuRankPanel_initLsTq(SMenuRankPanel *st, void *global,
	int rank, const char *name, int time, int score, int x, int y)
{
	st->global = global;
	st->type = 2;

	// 下地
	menuRankBase_init(st->base, global, x, y);

	// 順位
	menuRankNum_init(st->rank, global, rank, 4, RANK_NUM_Y);

	// 名前
	menuRankName_init(st->name, global, name, 41, RANK_NAME_Y);

	// 耐久
	menuRankFig_init(st->misc.taiQ.time, global, RANK_FIG_TIME, time,  2, 117, RANK_FIG_Y);
	menuRankFig_init(st->misc.taiQ.score, global, 7,            score, 1, 183, RANK_FIG_Y);
}

// 初期化(タイムアタック)
static void menuRankPanel_initLsTa(SMenuRankPanel *st, void *global,
	int rank, const char *name, int score, int time, int erase, int level, int x, int y)
{
	int col, pos;

	st->global = global;
	st->type = 3;

	// 下地
	menuRankBase_init(st->base, global, x, y);

	// 順位
	menuRankNum_init(st->rank, global, rank, 4, RANK_NUM_Y);

	// 名前
	menuRankName_init(st->name, global, name, 26, RANK_NAME_Y);

	// タイムアタック
	menuRankFig_init(st->misc.timeAt.score, global, 7,              score, 1,  83, RANK_FIG_Y);
	menuRankFig_init(st->misc.timeAt.time,  global, RANK_FIG_TIME2, time,  2, 153, RANK_FIG_Y);

	if(erase < dm_get_first_virus_count_in_new_mode(level)) {
		col = 2;
		pos = 217;
	}
	else {
		col = RANK_FIG_ALL;
		pos = 213;
	}
	menuRankFig_init(st->misc.timeAt.erase, global, col, erase, 1, pos, RANK_FIG_Y);
}

// 初期化(対戦)
static void menuRankPanel_initVs1(SMenuRankPanel *st, void *global,
	int rank, const char *name, int ratio, int win, int lose, int x, int y)
{
	st->global = global;
	st->type = 4;

	// 下地
	menuRankBase_init(st->base, global, x, y);

	// 順位
	menuRankNum_init(st->rank, global, rank, 4, RANK_NUM_Y);

	// 名前
	menuRankName_init(st->name, global, name, 26, RANK_NAME_Y);

	// 対戦
	menuRankFig_init(st->misc.vs1.ratio, global, RANK_FIG_RATIO, ratio, 5,  90, RANK_FIG_Y);
	menuRankFig_init(st->misc.vs1.win,   global, 2,              win,   6, 167, RANK_FIG_Y);
	menuRankFig_init(st->misc.vs1.lose,  global, 2,              lose,  7, 217, RANK_FIG_Y);
}

// 更新
static void menuRankPanel_update(SMenuRankPanel *st, SMenuItem *parent)
{
	// 下地
	menuRankBase_update(st->base, parent);

	// 順位
	menuRankNum_update(st->rank, st->base->miBase);

	// 名前
	menuRankName_update(st->name, st->base->miBase);

	switch(st->type) {
	// お話
	case 0:
		menuRankFig_update(st->misc.oha.score, st->base->miBase);
		menuRankFig_update(st->misc.oha.time, st->base->miBase);
		menuRankFig_update(st->misc.oha.stage, st->base->miBase);
		break;

	// エンドレス
	case 1:
		menuRankFig_update(st->misc.end.level, st->base->miBase);
		menuRankFig_update(st->misc.end.score, st->base->miBase);
		break;

	// 耐久
	case 2:
		menuRankFig_update(st->misc.taiQ.time, st->base->miBase);
		menuRankFig_update(st->misc.taiQ.score, st->base->miBase);
		break;

	// タイムアタック
	case 3:
		menuRankFig_update(st->misc.timeAt.score, st->base->miBase);
		menuRankFig_update(st->misc.timeAt.time, st->base->miBase);
		menuRankFig_update(st->misc.timeAt.erase, st->base->miBase);
		break;

	// 対戦
	case 4:
		menuRankFig_update(st->misc.vs1.ratio, st->base->miBase);
		menuRankFig_update(st->misc.vs1.win, st->base->miBase);
		menuRankFig_update(st->misc.vs1.lose, st->base->miBase);
		break;
	}
}

// 描画
static void menuRankPanel_draw(SMenuRankPanel **stAry, int count, Gfx **gpp)
{
	Gfx *gp = *gpp;
	SMenuRankPanel *st;
	SMenuItem *item;
	STexInfo *texC, *texA;
	void *list[count * 4];
	int i, j;

	// 下地
	for(i = j = 0; i < count; i++) {
		st = stAry[i];
		list[j++] = st->base;
	}
	menuRankBase_draw((SMenuRankBase **)list, j, &gp);

	// ランク
	for(i = j = 0; i < count; i++) {
		st = stAry[i];
		list[j++] = st->rank;
	}
	menuRankNum_draw((SMenuRankNum **)list, j, &gp);

	// 名前
	for(i = j = 0; i < count; i++) {
		st = stAry[i];
		list[j++] = st->name;
	}
	menuRankName_draw((SMenuRankName **)list, j, &gp);

	// スコア, タイム, ステージ数, 勝数, 負け数
	for(i = j = 0; i < count; i++) {
		st = stAry[i];
		switch(st->type) {
		// お話
		case 0:
			list[j++] = st->misc.oha.score;
			list[j++] = st->misc.oha.time;
			list[j++] = st->misc.oha.stage;
			break;

		// エンドレス
		case 1:
			list[j++] = st->misc.end.level;
			list[j++] = st->misc.end.score;
			break;

		// 耐久
		case 2:
			list[j++] = st->misc.taiQ.score;
			list[j++] = st->misc.taiQ.time;
			break;

		// タイムアタック
		case 3:
			list[j++] = st->misc.timeAt.score;
			list[j++] = st->misc.timeAt.time;
			list[j++] = st->misc.timeAt.erase;
			break;

		// 対戦
		case 4:
			list[j++] = st->misc.vs1.ratio;
			list[j++] = st->misc.vs1.win;
			list[j++] = st->misc.vs1.lose;
			break;
		}
	}
	menuRankFig_draw((SMenuRankFig **)list, j, &gp);

	*gpp = gp;
}

//////////////////////////////////////
//## メイン

#define RANK_TITLE_X 35
#define RANK_TITLE_Y 48

#define RANK_HEADER_X 0
#define RANK_HEADER_Y -16

#define RANK_LINE_STEP 17

typedef struct {
	int panelCount;
	SMenuRankHeader header[1];
	SMenuRankPanel panel[8];
} SMenuRankContext;

typedef struct {
	void *global;
	MODE mode;
	bool vsFlag;
	int level;
	int levelOld;
	int curBuf;
	int oldBuf;
	SRankSortInfo sortInfo[1];
	SMenuItem miBase[1];
	SMenuItem miLR[2];
	SMenuItem miNameBase[1];
	SMenuRankLabel title[1];
	SMenuRankLabel nameLabel[2];
	SMenuRankContext ct[2];
} SMenuRank;

//
static void menuRank_setNameBaseScale(SMenuRank *st, int dir, float time)
{
	SMenuItem *item, *list[3];
	int i;

	i = 0;
	list[i++] = st->miNameBase;
	list[i++] = &st->miLR[0];
	list[i++] = &st->miLR[1];

	for(i = 0; i < ARRAY_SIZE(list); i++) {
		item = list[i];
		item->transRange[0][0] = item->transRange[1][0];
		item->transRange[0][1] = item->transRange[1][1] + 8;
		item->scaleRange[0][0] = item->scaleRange[1][0];
		item->scaleRange[0][1] = 0;
		item->transStep = FRAME_MOVE_STEP * 2;
		item->scaleStep = FRAME_MOVE_STEP * 2;
		item->transTime = time;
		item->scaleTime = time;
		menuItem_setTransDir(item, dir);
		menuItem_setScaleDir(item, dir);
	}
}

// スライド
static void menuRank_setSlide(SMenuRank *st, int buf, int dir, float time, float vec)
{
	SMenuRankContext *ct = &st->ct[buf];
	SMenuItem *item;
	int i, j;

	item = ct->header->miBase;
	item->transRange[0][0] = item->transRange[1][0] + vec;
	item->transRange[0][1] = item->transRange[1][1];
	item->transStep = FRAME_MOVE_STEP;
	item->transTime = time;
	menuItem_setTransDir(item, dir);

	for(i = 0; i < ct->panelCount; i++) {
		item = ct->panel[i].base->miBase;
		item->transRange[0][0] = item->transRange[1][0] + vec;
		item->transRange[0][1] = item->transRange[1][1];
		item->transStep = FRAME_MOVE_STEP;
		item->transTime = time;
		menuItem_setTransDir(item, dir);
	}

	dir = (dir < 0 ? -1 : 1);

	for(i = 0; i < ct->panelCount; i++) {
		item = ct->panel[i].base->miBase;
		item->transStep -= FRAME_MOVE_STEP * dir * i / (ct->panelCount * 2);
		menuItem_setTransDir(item, dir);
	}
}

// フレームインアウト
static void menuRank_setFrame(SMenuRank *st, int buf, int dir, float time)
{
	static const int _pos[] = { -SCREEN_WD, SCREEN_WD };
	SMenuRankContext *ct = &st->ct[buf];
	SMenuItem *item;
	int i;

	// タイトル
	item = st->title->miBase;
	item->transRange[0][0] = item->transRange[1][0];
	item->transRange[0][1] = item->transRange[1][1] - SCREEN_HT / 2;
	item->transStep = FRAME_MOVE_STEP;
	item->transTime = time;
	menuItem_setTransDir(item, dir);

	// ＲＬ操作ガイド
	for(i = 0; i < ARRAY_SIZE(st->miLR); i++) {
		item = &st->miLR[i];
		item->transRange[0][0] = item->transRange[1][0];
		item->transRange[0][1] = item->transRange[1][1] - SCREEN_HT / 2;
		item->transStep = FRAME_MOVE_STEP;
		item->transTime = time;
		menuItem_setTransDir(item, dir);
	}

	// ヘッダー
	item = ct->header->miBase;
	item->transRange[0][0] = item->transRange[1][0];
	item->transRange[0][1] = item->transRange[1][1] - SCREEN_HT / 2;
	item->transStep = FRAME_MOVE_STEP;
	item->transTime = time;
	menuItem_setTransDir(item, dir);

	// サブヘッダー
	item = st->miNameBase;
	item->transRange[0][0] = item->transRange[1][0];
	item->transRange[0][1] = item->transRange[1][1] - SCREEN_HT / 2;
	item->transStep = FRAME_MOVE_STEP;
	item->transTime = time;
	menuItem_setTransDir(item, dir);

	// パネル
	for(i = 0; i < ct->panelCount; i++) {
		item = ct->panel[i].base->miBase;
		item->transRange[0][0] = item->transRange[1][0] + _pos[i & 1];
		item->transRange[0][1] = item->transRange[1][1];
		item->transStep = FRAME_MOVE_STEP;
		item->transTime = time;
		menuItem_setTransDir(item, dir);
	}
}

// 共通の初期化
static void menuRank_initCommon(SMenuRank *st, int buf)
{
	SMenuRankContext *ct = &st->ct[buf];
	SMenuItem *item;
	int i;

	// サブヘッダー
	menuItem_init(st->miNameBase, 164, 48);

	// 名前ラベル
	menuRankLabel_init(&st->nameLabel[0], st->global, 1, 1, 2, 2);
	menuRankLabel_init(&st->nameLabel[1], st->global, 2, 1, 62, 2);
}

// パネルの設定(お話)
static void menuRank_setPanel(SMenuRank *st, int buf, MODE mode, int level)
{
	static const int _hedAllType[] = { 0, 0, 0, 0 };
//	// 名前, スコア, タイム, ステージ
//	static const int _hedStoryId[] = { 0, 1, 2, 3 };
//	static const int _hedStoryX[] = { 32, 95, 156, 206 };
//	static const int _hedStoryY[] = { 0, 0, 0, 0 };
	// 名前, ステージ, スコア, タイム
	static const int _hedStoryId[] = { 0, 3, 1, 2 };
	static const int _hedStoryX[] = { 32, 85, 143, 206 };
	static const int _hedStoryY[] = { 0, 0, 0, 0 };
	// 名前, レベル, スコア
	static const int _hedLevelId[] = { 0, 4, 1 };
#if LOCAL==JAPAN
	static const int _hedLevelX[] = { 47, 118, 195 };
#elif LOCAL==AMERICA
	static const int _hedLevelX[] = { 47, 110, 195 };
#elif LOCAL==CHINA
	static const int _hedLevelX[] = { 47, 110, 195 };
#endif
	static const int _hedLevelY[] = { 0, 0, 0 };
	// 名前, タイム, スコア
	static const int _hed1PTaiQId[] = { 0, 2, 1 };
	static const int _hed1PTaiQX[] = { 47, 118, 195 };
	static const int _hed1PTaiQY[] = { 0, 0, 0 };
	// 名前, スコア、タイム、消したウィルス
	static const int _hed1PTimeAtId[] = { 0, 1, 2, 8 };
	static const int _hed1PTimeAtX[] = { 32, 95, 156, 206 };
	static const int _hed1PTimeAtY[] = { 0, 0, 0, 0 };
	// 名前, 勝率, 勝数, 負け数
	static const int _hedVsId[] = { 0, 5, 6, 7 };
	static const int _hedVsX[] = { 32, 95, 156, 206 };
	static const int _hedVsY[] = { 0, 0, 0, 0 };

	SMenuRankContext *ct = &st->ct[buf];
	SRankSortInfo *si = st->sortInfo;
	const int *headerId, *headerX, *headerY;
	int i, titleNo, headerSize;

	ct->panelCount = 8;

	// タイトル番号を取得
	switch(mode) {
	case MODE_RECORD_ST:
		titleNo  = 0;
		break;
	case MODE_RECORD_LS:
		titleNo  = 1;
		break;
	case MODE_RECORD_VC:
		titleNo  = 2;
		break;
	case MODE_RECORD_VM:
		titleNo  = 3;
		break;
	case MODE_RECORD_VC_FL:
	case MODE_RECORD_VM_FL:
		titleNo  = 4;
		break;
	case MODE_RECORD_LS_TQ:
		titleNo  = 5;
		break;
	case MODE_RECORD_LS_TA:
	case MODE_RECORD_VM_TA:
		titleNo  = 6;
		break;
	}

	// ヘッダー
	switch(mode) {
	case MODE_RECORD_ST:
		headerSize = ARRAY_SIZE(_hedStoryId);
		headerId   = _hedStoryId;
		headerX    = _hedStoryX;
		headerY    = _hedStoryY;
		break;
	case MODE_RECORD_LS:
		headerSize = ARRAY_SIZE(_hedLevelId);
		headerId   = _hedLevelId;
		headerX    = _hedLevelX;
		headerY    = _hedLevelY;
		break;
	case MODE_RECORD_LS_TQ:
		headerSize = ARRAY_SIZE(_hed1PTaiQId);
		headerId   = _hed1PTaiQId;
		headerX    = _hed1PTaiQX;
		headerY    = _hed1PTaiQY;
		break;
	case MODE_RECORD_LS_TA:
		headerSize = ARRAY_SIZE(_hed1PTimeAtId);
		headerId   = _hed1PTimeAtId;
		headerX    = _hed1PTimeAtX;
		headerY    = _hed1PTimeAtY;
		break;
	case MODE_RECORD_VC:
	case MODE_RECORD_VC_FL:
	case MODE_RECORD_VM:
	case MODE_RECORD_VM_FL:
	case MODE_RECORD_VM_TA:
		headerSize = ARRAY_SIZE(_hedVsId);
		headerId   = _hedVsId;
		headerX    = _hedVsX;
		headerY    = _hedVsY;
		break;
	}

	// タイトル
	menuRankLabel_init(st->title, st->global, 3, titleNo, RANK_TITLE_X, RANK_TITLE_Y);

	// ヘッダー
	menuRankHeader_init(ct->header, st->global,
		_hedAllType, headerId, headerX, headerY, headerSize,
		RANK_HEADER_X, RANK_HEADER_Y);

	// パネル
	for(i = 0; i < ct->panelCount; i++) {
		mem_char *mc;
		mem_story *ms;
		mem_level *ml;
		mem_taiQ *mq;
		mem_timeAt *ma;
		int sortId;
		u16 *vsRes, *vsAve;
		u8  *vsRnk;

		switch(mode) {
		case MODE_RECORD_ST:
			sortId = si->story_sort[level][i];
			mc = &evs_mem_data[sortId];
			ms = &mc->story_data[level];
			menuRankPanel_initSt(&ct->panel[i], st->global,
				si->story_rank[level][i], mc->mem_name,
				ms->score, ms->time, ms->c_stage + 1,
				0, i * RANK_LINE_STEP);
			break;
		case MODE_RECORD_LS:
			sortId = si->level_sort[level][i];
			mc = &evs_mem_data[sortId];
			ml = &mc->level_data[level];
			menuRankPanel_initLs(&ct->panel[i], st->global,
				si->level_rank[level][i], mc->mem_name,
				ml->c_level, ml->score,
				0, i * RANK_LINE_STEP);
			break;
		case MODE_RECORD_LS_TQ:
			sortId = si->taiQ_sort[level][i];
			mc = &evs_mem_data[sortId];
			mq = &mc->taiQ_data[level];
			menuRankPanel_initLsTq(&ct->panel[i], st->global,
				si->taiQ_rank[level][i], mc->mem_name,
				mq->time, mq->score,
				0, i * RANK_LINE_STEP);
			break;
		case MODE_RECORD_LS_TA:
			sortId = si->timeAt_sort[level][i];
			mc = &evs_mem_data[sortId];
			ma = &mc->timeAt_data[level];
			menuRankPanel_initLsTa(&ct->panel[i], st->global,
				si->timeAt_rank[level][i], mc->mem_name,
				ma->score, ma->time, ma->erase, level,
				0, i * RANK_LINE_STEP);
			break;
		case MODE_RECORD_VC:
			sortId = si->vscom_sort[i];
			mc = &evs_mem_data[sortId];
			vsRes = mc->vscom_data;
			vsAve = si->vscom_ave;
			vsRnk = si->vscom_rank;
			break;
		case MODE_RECORD_VC_FL:
			sortId = si->vc_fl_sort[i];
			mc = &evs_mem_data[sortId];
			vsRes = mc->vc_fl_data;
			vsAve = si->vc_fl_ave;
			vsRnk = si->vc_fl_rank;
			break;
		case MODE_RECORD_VM:
			sortId = si->vsman_sort[i];
			mc = &evs_mem_data[sortId];
			vsRes = mc->vsman_data;
			vsAve = si->vsman_ave;
			vsRnk = si->vsman_rank;
			break;
		case MODE_RECORD_VM_FL:
			sortId = si->vm_fl_sort[i];
			mc = &evs_mem_data[sortId];
			vsRes = mc->vm_fl_data;
			vsAve = si->vm_fl_ave;
			vsRnk = si->vm_fl_rank;
			break;
		case MODE_RECORD_VM_TA:
			sortId = si->vm_ta_sort[i];
			mc = &evs_mem_data[sortId];
			vsRes = mc->vm_ta_data;
			vsAve = si->vm_ta_ave;
			vsRnk = si->vm_ta_rank;
			break;
		}

		// 仕切りなおし
		switch(mode) {
		case MODE_RECORD_VC:
		case MODE_RECORD_VC_FL:
		case MODE_RECORD_VM:
		case MODE_RECORD_VM_FL:
		case MODE_RECORD_VM_TA:
			mc = &evs_mem_data[sortId];
			menuRankPanel_initVs1(&ct->panel[i], st->global,
				vsRnk[i], mc->mem_name, vsAve[sortId], vsRes[0], vsRes[1],
				0, i * RANK_LINE_STEP);
			break;
		}
	}

	// 共通の初期化
	menuRank_initCommon(st, buf);

	// サブヘッダー
	st->nameLabel[1].index = level;
}

// 初期化
static void menuRank_init(SMenuRank *st, void *global, void **hpp)
{
	static const int _lr[][2] = { { 35, 30 }, { 221, 30 } };
	int i;

	st->global = global;
	st->mode = _getMode(global);

	switch(st->mode) {
	case MODE_RECORD_VC:
	case MODE_RECORD_VC_FL:
	case MODE_RECORD_VM:
	case MODE_RECORD_VM_FL:
	case MODE_RECORD_VM_TA:
		st->vsFlag = true;
		break;
	default:
		st->vsFlag = false;
		break;
	}

	st->level = 1;
	st->levelOld = 1;
	st->curBuf = 0;
	st->oldBuf = -1;

	menuItem_init(st->miBase, 35, 86);

	// ＬＲボタンガイド
	for(i = 0; i < ARRAY_SIZE(st->miLR); i++) {
		menuItem_init(&st->miLR[i], _lr[i][0], _lr[i][1]);
	}

	switch(st->mode) {
	case MODE_RECORD_ST:
		dm_data_mode_story_sort(st->sortInfo);
		break;

	case MODE_RECORD_LS:
		dm_data_mode_level_sort(st->sortInfo);
		break;

	case MODE_RECORD_LS_TQ:
		dm_data_mode_taiQ_sort(st->sortInfo);
		break;

	case MODE_RECORD_LS_TA:
		dm_data_mode_timeAt_sort(st->sortInfo);
		break;

	case MODE_RECORD_VC:
		dm_data_vscom_sort(st->sortInfo);
		break;

	case MODE_RECORD_VC_FL:
		dm_data_vc_fl_sort(st->sortInfo);
		break;

	case MODE_RECORD_VM:
		dm_data_vsman_sort(st->sortInfo);
		break;

	case MODE_RECORD_VM_FL:
		dm_data_vm_fl_sort(st->sortInfo);
		break;

	case MODE_RECORD_VM_TA:
		dm_data_vm_ta_sort(st->sortInfo);
		break;
	}

	menuRank_setPanel(st, st->curBuf, st->mode, st->level);

	// フレームイン
	menuRank_setFrame(st, st->curBuf, 1, 0);
}

// 入力
static void menuRank_input(SMenuRank *st)
{
	SMenuRankContext *ct = &st->ct[st->curBuf];
	int trg = _getKeyTrg(st->global, 0);
	int rep = _getKeyRep(st->global, 0);
	int vx = 0, vy = 0, sound = -1;
	SMenuItem *item;
	float x;

	item = st->miNameBase;
	if(item->scaleTime == 0 && item->scaleStep < 0) {
		menuRank_setNameBaseScale(st, 1, 0);
	}

	item = ct->panel[0].base->miBase;
	if(item->transTime < 1 || item->transStep < 0) {
		return;
	}

	item = ct->panel[0].base->miBase;
	if(item->scaleTime < 1 || item->scaleStep < 0) {
		return;
	}

	if(!st->vsFlag) {
		if(trg & (L_TRIG | L_JPAD)) vx--;
		if(trg & (R_TRIG | R_JPAD)) vx++;
	}
	if(rep & U_JPAD) vy--;
	if(rep & D_JPAD) vy++;

	if(trg & CONT_CANCEL) {
		menuRank_setFrame(st, st->curBuf, -1, 1);
		_setMode(st->global, MODE_MAIN);
		sound = SE_mCANCEL;
	}
	else if(vx) {
		st->levelOld = st->level;
		st->level = WrapI(0, 3, st->level + vx);

		sound = SE_mLeftRight;

		menuRank_setSlide(st, st->curBuf, -1, 1, -SCREEN_WD * vx);
		st->oldBuf = st->curBuf;
		st->curBuf = (st->curBuf + 1) & 1;
		menuRank_setPanel(st, st->curBuf, st->mode, st->level);

		menuRank_setNameBaseScale(st, -1, 1);
		menuRank_setSlide(st, st->curBuf, 1, 0, SCREEN_WD * vx);
	}

	// サウンドを再生
	if(sound >= 0) {
		dm_snd_play(sound);
	}
}

// 更新
static void menuRank_update(SMenuRank *st)
{
	static const int _dir[] = { 1, -1 };
	SMenuItem *miRoot = _getRootItem(st->global);
	int i, j;

	// 下地
	menuItem_update(st->miBase, miRoot);

	// ＬＲボタンガイド
	menuItem_updateN(&st->miLR[i], ARRAY_SIZE(st->miLR), miRoot);

	// サブヘッダー
	menuItem_update(st->miNameBase, miRoot);

	// タイトル
	menuRankLabel_update(st->title, miRoot);

	// 名前ラベル
	menuRankLabel_update(&st->nameLabel[0], st->miNameBase);
	menuRankLabel_update(&st->nameLabel[1], st->miNameBase);

	for(i = 0; i < 2; i++) {
		SMenuRankContext *ct;
		int buf;

		switch(i) {
			case 0: buf = st->oldBuf; break;
			case 1: buf = st->curBuf; break;
		}

		if(buf < 0) continue;
		ct = &st->ct[buf];

		// ヘッダー
		menuRankHeader_update(ct->header, st->miBase);

		// パネル
		for(j = 0; j < ct->panelCount; j++) {
			menuRankPanel_update(&ct->panel[j], st->miBase);
		}
	}
}

// 描画
static void menuRank_draw(SMenuRank *st, Gfx **gpp)
{
	Gfx *gp = *gpp;
	SMenuItem *item;
	STexInfo *texC;
	void *list[16];
	int i, j;
	float x, y;

	gSPDisplayList(gp++, fade_normal_texture_init_dl);

	if(!st->vsFlag) {
		// サブヘッダー
		texC = _getTexRank(st->global, TEX_mode_panel_sub);
		menuItem_drawTex(st->miNameBase, &gp, texC, false);

		list[0] = &st->nameLabel[0];
		list[1] = &st->nameLabel[1];

		if(st->miNameBase->scaleStep < 0) {
			st->nameLabel[1].index = st->levelOld;
		}
		else {
			st->nameLabel[1].index = st->level;
		}

		if(st->mode == MODE_RECORD_LS) {
			st->nameLabel[0].index = 1;
			st->nameLabel[1].index += 3;
		}
		else {
			st->nameLabel[0].index = 0;
		}

		menuRankLabel_draw((SMenuRankLabel **)list, 2, &gp);

		// ＬＲボタンガイド
		for(i = 0; i < ARRAY_SIZE(st->miLR); i++) {
			static const int _dir[] = { -1, 1 };
			int level;

			item = &st->miLR[i];
			x = item->trans[0]; y = item->trans[1];

			if(item->scaleStep < 0) {
				level = st->levelOld;
			}
			else {
				level = st->level;
			}

			j = WrapI(0, 3, level + _dir[i]);
			if(st->mode == MODE_RECORD_LS) j += 3;

			texC = _getTexRank(st->global, TEX_rank_operation_a);
			menuItem_drawItem(item, &gp, texC, false, 2, i);

			item->trans[0] += 12; item->trans[1] += 2;
			texC = _getTexRank(st->global, TEX_rank_operation_b);
			menuItem_drawItem(&st->miLR[i], &gp, texC, false, 6, j);

			item->trans[0] = x; item->trans[1] = y;
		}
	}

	// タイトル
	list[0] = st->title;
	menuRankLabel_draw((SMenuRankLabel **)list, 1, &gp);

	// パネルなど
	for(i = 0; i < 2; i++) {
		SMenuRankContext *ct;
		int buf;

		switch(i) {
			case 0: buf = st->oldBuf; break;
			case 1: buf = st->curBuf; break;
		}

		if(buf < 0) continue;
		ct = &st->ct[buf];

		// ヘッダー
		list[0] = ct->header;
		menuRankHeader_draw((SMenuRankHeader **)list, 1, &gp);

		// パネル
		for(j = 0; j < ct->panelCount; j++) {
			list[j] = &ct->panel[j];
		}
		menuRankPanel_draw((SMenuRankPanel **)list, ct->panelCount, &gp);
	}

	*gpp = gp;
}

//////////////////////////////////////////////////////////////////////////////
//{### メイン

#include "graphic.h"
#include "message.h"
#include "segment.h"
#include "joy.h"
#include "boot_data.h"

MODE _menuAll_lastMode = MODE_MAIN;

typedef struct SMenuAll SMenuAll;

struct SMenuAll {
	NNSched *sched;
	NNScClient client;
	OSMesgQueue msgQ;
	OSMesg msgbuf[MAX_MESGS];

	Mtx mtxBuf[GFX_GTASK_NUM][16];
	Vtx vtxBuf[GFX_GTASK_NUM][128];

	Mtx *mtxPtr[GFX_GTASK_NUM];
	Vtx *vtxPtr[GFX_GTASK_NUM];

	void *heapAll;
	int heapSize;
	void *heapTop[2];
	void *heapEnd[2];

	STexInfo *texChar;
	STexInfo *texCommon;
	STexInfo *texCredit;
	STexInfo *texLevel;
	STexInfo *texMain;
	STexInfo *texName;
	STexInfo *texP2;
	STexInfo *texP4;
	STexInfo *texRank;
	STexInfo *texSetup;
	STexInfo *texStory;
	STexInfo *texGameAl;
	STexInfo *texGameP1;
	STexInfo *texCont;
	STexInfo *texTutol;
	STexInfo *texKasa;

	SMenuItem miRoot[1];
	SMenuTitle title[1];

	union {
		SMenuMain  main[1];
		SMenuStory story[1];
		SMenuLvSel lvSel[1];
		SMenuPlay2 play2[1];
		SMenuChSel chSel[1];
		SMenuNmEnt nmEnt[1];
		SMenuRank  rank[1];
	} context[2];
	int nowContext;
	int oldContext;

	MODE oldMode;
	MODE nowMode;
	MODE newMode;
	int nextMain;
#if defined(NN_SC_PERF)
	bool perfMeter;
#endif

	int eepRom;

	float fadeRatio;
	float fadeStep;

	bool mainFirst;
	bool grapFirst;

	u32 mainCount;
	u32 grapCount;

	u32 graphicThreadPri;

	// EEPROM書き込み中の警告メッセージ
	RecWritingMsg writingMsg;

#if defined(DEBUG)
	u32 gfxMax;
	u32 mtxMax;
	u32 vtxMax;
#endif
};

// セーブ中の警告メッセージ表示(cpuTask スレッド)
static void _eep_writingCallback(void *arg)
{
	SMenuAll *st = (SMenuAll *)arg;

	// EEPROM書き込み中の警告メッセージ
	RecWritingMsg_setPos(&st->writingMsg,
		(SCREEN_WD - msgWnd_getWidth(&st->writingMsg.msgWnd)) / 2,
		(SCREEN_HT - msgWnd_getHeight(&st->writingMsg.msgWnd)) - 32);
	RecWritingMsg_start(&st->writingMsg);

	// スリープタイマー
	setSleepTimer(500);
}

// セーブデータを行う(cpuTask スレッド)
static void _eep_writePlayer(void *arg)
{
	SMenuAll *st = (SMenuAll *)arg;
	EepRom_WriteAll(_eep_writingCallback, st);
	st->eepRom--;
}

// セーブデータを削除(cpuTask スレッド)
static void _eep_eraseData(void *arg)
{
	SMenuAll *st = (SMenuAll *)arg;
	EepRom_InitFirst(_eep_writingCallback, st);
	st->eepRom--;
}

// セーブ中のメインスレッド・リトレース処理
static void _eep_retrace(void *global)
{
	SMenuAll *st = (SMenuAll *)global;

	while(st->eepRom > 0 || !RecWritingMsg_isEnd(&st->writingMsg)) {
		// EEPROM書き込み中の警告メッセージ
		RecWritingMsg_calc(&st->writingMsg);
		_waitRetrace(st);
	}
}

// セーブデータを書き込む
static void _eepWritePlayer(void *global)
{
	SMenuAll *st = (SMenuAll *)global;

	// EEPROM書き込み中の警告メッセージ
	RecWritingMsg_setStr(&st->writingMsg, _mesWriting);

	st->eepRom++;
	cpuTask_sendTask(_eep_writePlayer, st);

	// セーブが終わるまで待つ
	_eep_retrace(st);
}

// プレイヤーを削除
static void _eepErasePlayer(void *global)
{
	SMenuAll *st = (SMenuAll *)global;

	// EEPROM書き込み中の警告メッセージ
	RecWritingMsg_setStr(&st->writingMsg, _mesDeleting);

	st->eepRom++;
	cpuTask_sendTask(_eep_writePlayer, st);

	// セーブが終わるまで待つ
	_eep_retrace(st);
}

// セーブデータを削除
static void _eepEraseData(void *global)
{
	SMenuAll *st = (SMenuAll *)global;

	// EEPROM書き込み中の警告メッセージ
	RecWritingMsg_setStr(&st->writingMsg, _mesDeleting);

	st->eepRom++;
	cpuTask_sendTask(_eep_eraseData, st);

	// セーブが終わるまで待つ
	_eep_retrace(st);
}

static void _waitRetrace(void *global)
{
	SMenuAll *st = (SMenuAll *)global;
	osRecvMesg(&st->msgQ, NULL, OS_MESG_BLOCK);
}

static Mtx **_getMtxPtr(void *global)
{
	SMenuAll *st = (SMenuAll *)global;
	return &st->mtxPtr[gfx_gtask_no];
}

static Vtx **_getVtxPtr(void *global)
{
	SMenuAll *st = (SMenuAll *)global;
	return &st->vtxPtr[gfx_gtask_no];
}

static STexInfo *_getTexChar(void *global, int index)
{
	SMenuAll *st = (SMenuAll *)global;
	return &st->texChar[index];
}

static STexInfo *_getTexCommon(void *global, int index)
{
	SMenuAll *st = (SMenuAll *)global;
	return &st->texCommon[index];
}

static STexInfo *_getTexCredit(void *global, int index)
{
	SMenuAll *st = (SMenuAll *)global;
	return &st->texCredit[index];
}

static STexInfo *_getTexLevel(void *global, int index)
{
	SMenuAll *st = (SMenuAll *)global;
	return &st->texLevel[index];
}

static STexInfo *_getTexMain(void *global, int index)
{
	SMenuAll *st = (SMenuAll *)global;
	return &st->texMain[index];
}

static STexInfo *_getTexName(void *global, int index)
{
	SMenuAll *st = (SMenuAll *)global;
	return &st->texName[index];
}

static STexInfo *_getTexP2(void *global, int index)
{
	SMenuAll *st = (SMenuAll *)global;
	return &st->texP2[index];
}

static STexInfo *_getTexP4(void *global, int index)
{
	SMenuAll *st = (SMenuAll *)global;
	return &st->texP4[index];
}

static STexInfo *_getTexRank(void *global, int index)
{
	SMenuAll *st = (SMenuAll *)global;
	return &st->texRank[index];
}

static STexInfo *_getTexSetup(void *global, int index)
{
	SMenuAll *st = (SMenuAll *)global;
	return &st->texSetup[index];
}

static STexInfo *_getTexStory(void *global, int index)
{
	SMenuAll *st = (SMenuAll *)global;
	return &st->texStory[index];
}

static STexInfo *_getTexGameAl(void *global, int index)
{
	SMenuAll *st = (SMenuAll *)global;
	return &st->texGameAl[index];
}

static STexInfo *_getTexGameP1(void *global, int index)
{
	SMenuAll *st = (SMenuAll *)global;
	return &st->texGameP1[index];
}

static STexInfo *_getTexCont(void *global, int index)
{
	SMenuAll *st = (SMenuAll *)global;
	return &st->texCont[index];
}

static STexInfo *_getTexTutol(void *global, int index)
{
	SMenuAll *st = (SMenuAll *)global;
	return &st->texTutol[index];
}

static STexInfo *_getTexKasa(void *global, int index)
{
	SMenuAll *st = (SMenuAll *)global;
	return &st->texKasa[index];
}

static SMenuItem *_getRootItem(void *global)
{
	SMenuAll *st = (SMenuAll *)global;
	return st->miRoot;
}

static int _getKeyLvl(void *global, int contNo)
{
	SMenuAll *st = (SMenuAll *)global;
	return joynew[main_joy[contNo]];
}

static int _getKeyTrg(void *global, int contNo)
{
	SMenuAll *st = (SMenuAll *)global;
	return joyupd[main_joy[contNo]];
}

static int _getKeyRep(void *global, int contNo)
{
	SMenuAll *st = (SMenuAll *)global;
	return joycur[main_joy[contNo]];
}

static MODE _getMode(void *global)
{
	SMenuAll *st = (SMenuAll *)global;
	return st->nowMode;
}

static MODE _getModeOld(void *global)
{
	SMenuAll *st = (SMenuAll *)global;
	return st->oldMode;
}

static void _setMode(void *global, MODE mode)
{
	SMenuAll *st = (SMenuAll *)global;
	st->newMode = mode;
}

static void _setNextMain(void *global, int nextMain)
{
	SMenuAll *st = (SMenuAll *)global;
	st->nextMain = nextMain;
}

static void _setTitle(void *global, MODE mode)
{
	SMenuAll *st = (SMenuAll *)global;
	menuTitle_setTitle(st->title, mode);
}

static void _setFadeDir(void *global, int dir)
{
	SMenuAll *st = (SMenuAll *)global;

	if(dir < 0) {
		if(st->fadeStep > 0) st->fadeStep = -st->fadeStep;
	}
	else {
		if(st->fadeStep < 0) st->fadeStep = -st->fadeStep;
	}
}

static void menuAll_init(SMenuAll *st, void **hpp, NNSched *sched)
{
	void *hp = *hpp;
	void *(*tbl)[2] = _romDataTbl;
	int i;

	st->heapAll = hp;

	// スケジューラに登録
	st->sched = sched;
	osCreateMesgQueue(&st->msgQ, st->msgbuf, MAX_MESGS);
	nnScAddClient(sched, &st->client, &st->msgQ);

	// mtx & vtx buffer pointer
	for(i = 0; i < GFX_GTASK_NUM; i++) {
		st->mtxPtr[i] = st->mtxBuf[i];
		st->vtxPtr[i] = st->vtxBuf[i];
	}

	// 背景
	hp = (void *)init_menu_bg((u32)hp, evs_level_21 ? 1 : 0);

	// テクスチャを読む
	st->texChar   = tiLoadTexData(&hp, tbl[_menu_char][0],     tbl[_menu_char][1]);
	st->texCommon = tiLoadTexData(&hp, tbl[_menu_common][0],   tbl[_menu_common][1]);
	st->texLevel  = tiLoadTexData(&hp, tbl[_menu_level][0],    tbl[_menu_level][1]);
	st->texMain   = tiLoadTexData(&hp, tbl[_menu_main][0],     tbl[_menu_main][1]);
	st->texName   = tiLoadTexData(&hp, tbl[_menu_name][0],     tbl[_menu_name][1]);
	st->texP2     = tiLoadTexData(&hp, tbl[_menu_p2][0],       tbl[_menu_p2][1]);
	st->texP4     = tiLoadTexData(&hp, tbl[_menu_p4][0],       tbl[_menu_p4][1]);
	st->texRank   = tiLoadTexData(&hp, tbl[_menu_rank][0],     tbl[_menu_rank][1]);
	st->texSetup  = tiLoadTexData(&hp, tbl[_menu_setup][0],    tbl[_menu_setup][1]);
	st->texStory  = tiLoadTexData(&hp, tbl[_menu_story][0],    tbl[_menu_story][1]);
	st->texGameAl = tiLoadTexData(&hp, tbl[_game_al][0],       tbl[_game_al][1]);
	st->texGameP1 = tiLoadTexData(&hp, tbl[_game_p1][0],       tbl[_game_p1][1]);
	st->texCont   = tiLoadTexData(&hp, tbl[_menu_cont][0],     tbl[_menu_cont][1]);
	st->texTutol  = tiLoadTexData(&hp, tbl[_tutorial_data][0], tbl[_tutorial_data][1]);
	st->texKasa   = tiLoadTexData(&hp, tbl[_menu_kasa][0],     tbl[_menu_kasa][1]);

	// ルートアイテムを初期化
	menuItem_init(st->miRoot, 0, 0);

	// タイトル
	menuTitle_init(st->title, st, 112, 18);

	//
	st->nowContext = 0;
	st->oldContext = -1;

	// モード
	st->oldMode = st->nowMode = st->newMode = _menuAll_lastMode;

	// 次のメイン
	st->nextMain = MAIN_MENU;

	// パフォーマンスメータのON/OFF
#if defined(NN_SC_PERF)
	st->perfMeter = false;
#endif

	// EEPROM処理中フラグ
	st->eepRom = 0;

	// フェード値
	st->fadeRatio = 1;
	st->fadeStep = -1 / 30.0;

	// 初回フレームフラグ
	st->mainFirst = true;
	st->grapFirst = true;

	// フレームカウント
	st->mainCount = 0;
	st->grapCount = 0;

	//
	st->graphicThreadPri = OS_PRIORITY_APPMAX;

	// EEPROM書き込み中の警告メッセージ
	RecWritingMsg_init(&st->writingMsg, &hp);

#if defined(DEBUG)
	st->gfxMax = 0;
	st->mtxMax = 0;
	st->vtxMax = 0;
#endif

	st->heapSize = 0x00060000;
	for(i = 0; i < ARRAY_SIZE(st->heapTop); i++) {
		st->heapTop[i] = st->heapEnd[i] = (void *)ALIGN_16(hp);
		hp = (u8 *)st->heapTop[i] + st->heapSize;
	}

	// 得点, リトライ回数 を初期化
	for(i = 0; i < 4; i++) {
		game_state_data[i].game_score = 0;
		game_state_data[i].game_retry = 0;
	}

	// ストーリ、１ゲームフラグをクリア
	evs_one_game_flg = 0;

	*hpp = hp;
}

static void menuAll_exit(SMenuAll *st)
{
	nnScRemoveClient(st->sched, &st->client);
	_menuAll_lastMode = st->nowMode;
}

static void menuAll_waitCancel(SMenuAll *st)
{
	int cur = st->nowContext;

	switch(st->nowMode) {
	case MODE_MAIN:
		menuMain_waitCancel(st->context[cur].main);
		break;

	case MODE_STORY:
		menuStory_waitCancel(st->context[cur].story);
		break;

	case MODE_LVSEL:
	case MODE_LVSEL_TQ:
	case MODE_LVSEL_TA:
		menuLvSel_waitCancel(st->context[cur].lvSel);
		break;

	case MODE_VSMAN:
	case MODE_VSMAN_FL:
	case MODE_VSMAN_TA:

	case MODE_VSCOM:
	case MODE_VSCOM_FL:

	case MODE_PLAY4_LV:
	case MODE_PLAY4_TB_LV:
	case MODE_PLAY4_FL_LV:
		menuPlay2_waitCancel(st->context[cur].play2);
		break;
	}
}

static void menuAll_changeMenu(SMenuAll *st)
{
	int cur = st->nowContext;

	st->heapEnd[cur] = st->heapTop[cur];
	bzero(st->heapEnd[cur], st->heapSize);

	st->mainFirst = true;
	st->grapFirst = true;
	st->mainCount = 0;
	st->grapCount = 0;

	switch(st->nowMode) {
	case MODE_MAIN:
		break;

	default:
		_setTitle(st, st->nowMode);
		break;
	}

	switch(st->nowMode) {
	case MODE_MAIN:
		menuMain_init(st->context[cur].main, st, &st->heapEnd[cur]);
		break;

	case MODE_STORY:
		menuStory_init(st->context[cur].story, st, &st->heapEnd[cur]);
		break;

	case MODE_LVSEL:
	case MODE_LVSEL_TQ:
	case MODE_LVSEL_TA:
		menuLvSel_init(st->context[cur].lvSel, st, &st->heapEnd[cur]);
		break;

	case MODE_VSMAN:
	case MODE_VSMAN_FL:
	case MODE_VSMAN_TA:

	case MODE_VSCOM:
	case MODE_VSCOM_FL:

	case MODE_PLAY4_LV:
	case MODE_PLAY4_TB_LV:
	case MODE_PLAY4_FL_LV:
		menuPlay2_init(st->context[cur].play2, st, &st->heapEnd[cur]);
		break;

	case MODE_VSCOM_CH:
	case MODE_VSCOM_FL_CH:

	case MODE_VSMAN_CH:
	case MODE_VSMAN_FL_CH:
	case MODE_VSMAN_TA_CH:

	case MODE_PLAY4_CH:
	case MODE_PLAY4_TB_CH:
	case MODE_PLAY4_FL_CH:
		menuChSel_init(st->context[cur].chSel, st, &st->heapEnd[cur]);
		break;

	case MODE_STORY_NE:
	case MODE_LVSEL_NE:
	case MODE_LVSEL_TQ_NE:
	case MODE_LVSEL_TA_NE:

	case MODE_VSCOM_NE:
	case MODE_VSCOM_FL_NE:

	case MODE_VSMAN_NE:
	case MODE_VSMAN_FL_NE:
	case MODE_VSMAN_TA_NE:

	case MODE_NAME_NE:
	case MODE_NAME_NE2:
		menuNmEnt_init(st->context[cur].nmEnt, st, &st->heapEnd[cur]);
		break;

	case MODE_RECORD_ST:
	case MODE_RECORD_LS:
	case MODE_RECORD_LS_TQ:
	case MODE_RECORD_LS_TA:
	case MODE_RECORD_VC:
	case MODE_RECORD_VC_FL:
	case MODE_RECORD_VM:
	case MODE_RECORD_VM_FL:
	case MODE_RECORD_VM_TA:
		menuRank_init(st->context[cur].rank, st, &st->heapEnd[cur]);
		break;
	}
}

static void menuAll_input(SMenuAll *st)
{
	int cur = st->nowContext;

	if(st->fadeStep > 0 && st->fadeRatio > 0) {
		menuAll_waitCancel(st);
		return;
	}

	switch(st->nowMode) {
	case MODE_MAIN:
		menuMain_input(st->context[cur].main);
		break;

	case MODE_STORY:
		menuStory_input(st->context[cur].story);
		break;

	case MODE_LVSEL:
	case MODE_LVSEL_TQ:
	case MODE_LVSEL_TA:
		menuLvSel_input(st->context[cur].lvSel);
		break;

	case MODE_VSMAN:
	case MODE_VSMAN_FL:
	case MODE_VSMAN_TA:

	case MODE_VSCOM:
	case MODE_VSCOM_FL:

	case MODE_PLAY4_LV:
	case MODE_PLAY4_TB_LV:
	case MODE_PLAY4_FL_LV:
		menuPlay2_input(st->context[cur].play2);
		break;

	case MODE_VSCOM_CH:
	case MODE_VSCOM_FL_CH:

	case MODE_VSMAN_CH:
	case MODE_VSMAN_FL_CH:
	case MODE_VSMAN_TA_CH:

	case MODE_PLAY4_CH:
	case MODE_PLAY4_TB_CH:
	case MODE_PLAY4_FL_CH:
		menuChSel_input(st->context[cur].chSel);
		break;

	case MODE_STORY_NE:
	case MODE_LVSEL_NE:
	case MODE_LVSEL_TQ_NE:
	case MODE_LVSEL_TA_NE:

	case MODE_VSCOM_NE:
	case MODE_VSCOM_FL_NE:

	case MODE_VSMAN_NE:
	case MODE_VSMAN_FL_NE:
	case MODE_VSMAN_TA_NE:

	case MODE_NAME_NE:
	case MODE_NAME_NE2:
		menuNmEnt_input(st->context[cur].nmEnt);
		break;

	case MODE_RECORD_ST:
	case MODE_RECORD_LS:
	case MODE_RECORD_LS_TQ:
	case MODE_RECORD_LS_TA:
	case MODE_RECORD_VC:
	case MODE_RECORD_VC_FL:
	case MODE_RECORD_VM:
	case MODE_RECORD_VM_FL:
	case MODE_RECORD_VM_TA:
		menuRank_input(st->context[cur].rank);
		break;
	}
}

static void menuAll_update(SMenuAll *st)
{
	SMenuItem *item = st->miRoot;
	MODE mode;
	int i, j;
	float fade;

	st->fadeRatio = CLAMP(0, 1, st->fadeRatio + st->fadeStep);
	fade = (st->fadeRatio - 0.5) * 1.2 + 0.5;
	fade = 1.0 - CLAMP(0, 1, fade);
	for(i = 0; i < 3; i++) {
		item->colorRange[0][i] = item->colorRange[1][i] = fade;
	}

#if defined(NN_SC_PERF)
	if(_getKeyTrg(st, 0) & Z_TRIG) {
		st->perfMeter = !st->perfMeter;
	}

	if(0) {
		SMenuItem *item = st->miRoot;
		int pos[2];

		if(st->perfMeter) {
			pos[0] =  joypad[0].stick_x * 4;
			pos[1] = -joypad[0].stick_y * 3;
		}
		else {
			pos[0] = pos[1] = 0;
		}

		item->transTime = 0;
		for(i = 0; i < ARRAY_SIZE(item->trans); i++) {
			item->transRange[0][i] = item->trans[i];
			item->transRange[1][i] = pos[i];
		}
//		st->fadeRatio = joypad[0].stick_x * (1.0 / 80.0);
	}
#endif

	menuItem_updateN(st->miRoot, ARRAY_SIZE(st->miRoot), NULL);
	menuTitle_update(st->title, st->miRoot);

	for(i = 0; i < 2; i++) {
		j = (i ? st->nowContext : st->oldContext);
		if(j < 0) continue;

		mode = (i ? st->nowMode : st->oldMode);

		switch(mode) {
		case MODE_MAIN:
			menuMain_update(st->context[j].main);
			break;

		case MODE_STORY:
			menuStory_update(st->context[j].story);
			break;

		case MODE_LVSEL:
		case MODE_LVSEL_TQ:
		case MODE_LVSEL_TA:
			menuLvSel_update(st->context[j].lvSel);
			break;

		case MODE_VSMAN:
		case MODE_VSMAN_FL:
		case MODE_VSMAN_TA:

		case MODE_VSCOM:
		case MODE_VSCOM_FL:

		case MODE_PLAY4_LV:
		case MODE_PLAY4_TB_LV:
		case MODE_PLAY4_FL_LV:
			menuPlay2_update(st->context[j].play2);
			break;

		case MODE_VSCOM_CH:
		case MODE_VSCOM_FL_CH:

		case MODE_VSMAN_CH:
		case MODE_VSMAN_FL_CH:
		case MODE_VSMAN_TA_CH:

		case MODE_PLAY4_CH:
		case MODE_PLAY4_TB_CH:
		case MODE_PLAY4_FL_CH:
			menuChSel_update(st->context[j].chSel);
			break;

		case MODE_STORY_NE:
		case MODE_LVSEL_NE:

		case MODE_VSCOM_NE:
		case MODE_VSCOM_FL_NE:
		case MODE_LVSEL_TQ_NE:
		case MODE_LVSEL_TA_NE:

		case MODE_VSMAN_NE:
		case MODE_VSMAN_FL_NE:
		case MODE_VSMAN_TA_NE:

		case MODE_NAME_NE:
		case MODE_NAME_NE2:
			menuNmEnt_update(st->context[j].nmEnt);
			break;

		case MODE_RECORD_ST:
		case MODE_RECORD_LS:
		case MODE_RECORD_LS_TQ:
		case MODE_RECORD_LS_TA:
		case MODE_RECORD_VC:
		case MODE_RECORD_VC_FL:
		case MODE_RECORD_VM:
		case MODE_RECORD_VM_FL:
		case MODE_RECORD_VM_TA:
			menuRank_update(st->context[j].rank);
			break;
		}
	}

	st->mainFirst = false;
	st->mainCount++;
}

// 背景を描画
static void menuAll_drawBg(SMenuAll *st, Gfx **gpp)
{
	Gfx *gp = *gpp;
	SMenuItem *item = st->miRoot;
	STexInfo *tex;
	int i, cached;

	// 雲
	menuItem_setPrim(item, &gp);
	draw_menu_bg(&gp, item->trans[0] + 0, -item->trans[1] - SCREEN_HT / 2);

	gSPDisplayList(gp++, fade_normal_texture_init_dl);

	// 帯
	menuItem_setPrim(item, &gp);

	tex = _getTexCommon(st, TEX_obi);
	for(i = cached = 0; i < 5; i++) {
		cached += menuItem_drawTex2(item, &gp, tex, cached, i * 64, 0, 1, 1);
	}
	for(; i < 10; i++) {
		cached += menuItem_drawTex2(item, &gp, tex, cached, (i - 5) * 64, 200, 1, -1);
	}

//	tex = _getTexCommon(st, TEX_obi_a);
//	tiStretchTexBlock(&gp, tex, false,
//		item->trans[0], item->trans[1],
//		tex->size[0], tex->size[1]);

//	tex = _getTexCommon(st, TEX_obi_b);
//	tiStretchTexBlock(&gp, tex, false,
//		item->trans[0], item->trans[1] + SCREEN_HT - tex->size[1],
//		tex->size[0], tex->size[1]);

	*gpp = gp;
}

// 描画
static void menuAll_draw(SMenuAll *st, Gfx **gpp)
{
	MODE mode;
	int i, j, x, y, fade;

	// mtx & vtx buffer pointer
	st->mtxPtr[gfx_gtask_no] = st->mtxBuf[gfx_gtask_no];
	st->vtxPtr[gfx_gtask_no] = st->vtxBuf[gfx_gtask_no];

	fade = ((st->fadeRatio - 0.5) * 1.2 + 0.5) * 255.0;
	fade = CLAMP(0, 255, fade);
	gDPSetEnvColor(gp++, fade,fade,fade, 255);

	// 背景を描画
	menuAll_drawBg(st, gpp);

	// タイトルを描画
	menuTitle_draw(st->title, &gp);

	for(i = 0; i < 2; i++) {
		j = (i ? st->nowContext : st->oldContext);
		if(j < 0) continue;

		mode = (i ? st->nowMode : st->oldMode);

		gDPSetEnvColor(gp++, fade,fade,fade, 255);

		switch(mode) {
		case MODE_MAIN:
			menuMain_draw(st->context[j].main, gpp);
			break;

		case MODE_STORY:
			menuStory_draw(st->context[j].story, gpp);
			break;

		case MODE_LVSEL:
		case MODE_LVSEL_TQ:
		case MODE_LVSEL_TA:
			menuLvSel_draw(st->context[j].lvSel, gpp);
			break;

		case MODE_VSMAN:
		case MODE_VSMAN_FL:
		case MODE_VSMAN_TA:

		case MODE_VSCOM:
		case MODE_VSCOM_FL:

		case MODE_PLAY4_LV:
		case MODE_PLAY4_TB_LV:
		case MODE_PLAY4_FL_LV:
			menuPlay2_draw(st->context[j].play2, gpp);
			break;

		case MODE_VSCOM_CH:
		case MODE_VSCOM_FL_CH:

		case MODE_VSMAN_CH:
		case MODE_VSMAN_FL_CH:
		case MODE_VSMAN_TA_CH:

		case MODE_PLAY4_CH:
		case MODE_PLAY4_TB_CH:
		case MODE_PLAY4_FL_CH:
			menuChSel_draw(st->context[j].chSel, gpp);
			break;

		case MODE_STORY_NE:
		case MODE_LVSEL_NE:
		case MODE_LVSEL_TQ_NE:
		case MODE_LVSEL_TA_NE:

		case MODE_VSCOM_NE:
		case MODE_VSCOM_FL_NE:

		case MODE_VSMAN_NE:
		case MODE_VSMAN_FL_NE:
		case MODE_VSMAN_TA_NE:

		case MODE_NAME_NE:
		case MODE_NAME_NE2:
			menuNmEnt_draw(st->context[j].nmEnt, gpp);
			break;

		case MODE_RECORD_ST:
		case MODE_RECORD_LS:
		case MODE_RECORD_LS_TQ:
		case MODE_RECORD_LS_TA:
		case MODE_RECORD_VC:
		case MODE_RECORD_VC_FL:
		case MODE_RECORD_VM:
		case MODE_RECORD_VM_FL:
		case MODE_RECORD_VM_TA:
			menuRank_draw(st->context[j].rank, gpp);
			break;
		}
	}

	// EEPROM書き込み中の警告メッセージ
	RecWritingMsg_draw(&st->writingMsg, &gp);

	st->grapFirst = false;
	st->grapCount++;
}

//////////////////////////////////////////////////////////////////////////////
//{### スケジューラから呼ばれる関数

#include "hardcopy.h"

#define HEAP_START _codeSegmentEnd

SMenuAll *watchMenu;

//////////////////////////////////////

int main_menu(NNSched *sched)
{
	void *heap = HEAP_START;
	SMenuAll *st = (SMenuAll *)ALIGN_16(heap);
	int i;

	if(main_old == MAIN_TITLE) {
		_menuAll_lastMode = MODE_MAIN;
		_menuMain_lastMode = MODE_MAIN;
		_menuMain_lastDepth = 0;
		_menuMain_lastSelect[0] = 0;
	}

	cpuTask_createThread();

	heap = st + 1;
	bzero(st, sizeof(SMenuAll));
	watchMenu = st;

	menuAll_init(st, &heap, sched);

	DBG3(1, "use heap %08x ( %08x - %08x )\n", (u32)heap - (u32)HEAP_START, HEAP_START, heap);

	// コントローラ接続状況確認
	evs_playmax = joyResponseCheck();

	// キーリピート対応キーの設定
	for(i = 0;i < MAXCONTROLLERS;i ++){
		joyflg[i] = U_JPAD | D_JPAD | L_JPAD | R_JPAD | L_TRIG | R_TRIG;
	}
	joycur1 = 24;
	joycur2 = 6;

	gp = &gfx_glist[gfx_gtask_no][0];

	// シーケンスを再生
	dm_seq_play(SEQ_Menu);

	while(st->nextMain == MAIN_MENU || st->fadeRatio < 1) {

		if(graphic_no == GFX_NULL) {
			while(pendingGFX || cpuTask_getTaskCount()) {
				_waitRetrace(st);
			}
			menuAll_changeMenu(st);
		}

		// EEPROMアクセス中はコントローラを読まない
		if(st->eepRom > 0) {
			for(i = 0; i < MAXCONTROLLERS; i++) {
				joynew[i] = joyold[i] = joyupd[i] = joycur[i] = 0;
			}
		}
		else {
			joyProcCore();
			while ( GameHalt ) {
				_waitRetrace(st);
				joyProcCore();
				graphic_no = GFX_NULL;
				dm_audio_update();
			}
		}
		_waitRetrace(st);

#ifdef NN_SC_PERF
	nnScPushPerfMeter(NN_SC_PERF_MAIN);
#endif

		menuAll_input(st);
		menuAll_update(st);
		dm_audio_update();

		if(st->nowMode != st->newMode) {
			st->oldMode = st->nowMode;
			st->nowMode = st->newMode;
			st->oldContext = st->nowContext;
			st->nowContext ^= 1;
			graphic_no = GFX_NULL;
		}
		else {
			graphic_no = GFX_MENU;
		}

#ifdef NN_SC_PERF
	nnScPopPerfMeter(); // NN_SC_PERF_MAIN
#endif
	}

	// グラフィックスレッドのプライオリティが元に戻るまで待つ
	graphic_no = GFX_MENU;
	while(st->graphicThreadPri != GRAPHIC_THREAD_PRI) {
		osRecvMesg(&st->msgQ, NULL, OS_MESG_BLOCK);
		dm_audio_update();
	}
	graphic_no = GFX_NULL;

	// 音楽停止
	dm_seq_stop();

	while(pendingGFX || !dm_seq_is_stopped() || cpuTask_getTaskCount()) {
		osRecvMesg(&st->msgQ, NULL, OS_MESG_BLOCK);
		dm_audio_update();
	}

	menuAll_exit(st);

	cpuTask_destroyThread();

	return st->nextMain;
}

//////////////////////////////////////

void graphic_menu()
{
	SMenuAll *st = watchMenu;
	int i;

	if(st->nextMain != MAIN_MENU && st->fadeRatio == 1) {
		osSetThreadPri(NULL, GRAPHIC_THREAD_PRI);
		st->graphicThreadPri = GRAPHIC_THREAD_PRI;
		return;
	}

	if(gp != gfx_glist[gfx_gtask_no]) {
		gDPFullSync(gp++);
		gSPEndDisplayList(gp++);
		osWritebackDCacheAll();

#if defined(DEBUG)
		st->gfxMax = MAX(st->gfxMax, gp - gfx_glist[gfx_gtask_no]);
		st->mtxMax = MAX(st->mtxMax, st->mtxPtr[gfx_gtask_no] - st->mtxBuf[gfx_gtask_no]);
		st->vtxMax = MAX(st->vtxMax, st->vtxPtr[gfx_gtask_no] - st->vtxBuf[gfx_gtask_no]);
#endif

		gfxTaskStart(&gfx_task[gfx_gtask_no], gfx_glist[gfx_gtask_no],
			(s32)(gp - gfx_glist[gfx_gtask_no]) * sizeof(Gfx),
			GFX_GSPCODE_F3DEX, NN_SC_SWAPBUFFER);

		gp = gfx_glist[gfx_gtask_no];
	}

	//
	osSetThreadPri(NULL, GRAPHIC_THREAD_PRI);

	F3RCPinitRtn();
	F3ClearFZRtn(0);

	menuAll_draw(st, &gp);

//	HCEffect_Grap(&gp);

#if defined(NN_SC_PERF)
	if(st->perfMeter) {
		nnScWritePerfMeter(&gp, 32, 180);
	}
#endif

//	{
//		extern Acmd *alEnvmixerPull(void *filter, s16 *outp, s32 outCount, s32 sampleOffset, Acmd *p);
//		u32 *addr = (u32 *)((u8 *)alEnvmixerPull + 0x10C);
//		DbgCon_Open(&gp, NULL, 32, 32);
//		DbgCon_Format("%08x: %08x", addr, *addr);
//		DbgCon_Close();
//	}

	// オーディオより高いプライオリティーに
	osSetThreadPri(NULL, OS_PRIORITY_APPMAX);
}

//////////////////////////////////////////////////////////////////////////////
//{### EOF
