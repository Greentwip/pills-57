
#include <ultra64.h>
#include "def.h"
#include "nnsched.h"
#include "graphic.h"

#include "tex_func.h"
#include "util.h"
#include "debug.h"

//////////////////////////////////////////////////////////////////////////////
// 初期化ディスプレイリスト

Gfx copy_texture_init_dl[] = {
	gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_COPY),
	gsSPTexture(0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON),
	gsDPSetRenderMode(G_RM_NOOP, G_RM_NOOP2),
	gsDPSetCombineLERP(0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0),
	gsDPSetAlphaCompare(G_AC_THRESHOLD),
	gsDPSetBlendColor(0, 0, 0, 1),
	gsDPSetTextureFilter(G_TF_POINT),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetTextureLOD(G_TL_TILE),
	gsDPSetTextureDetail(G_TD_CLAMP),
	gsDPSetTextureConvert(G_TC_FILT),
	gsSPEndDisplayList(),
};

Gfx alpha_texture_init_dl[] = {
	gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_2CYCLE),
	gsSPTexture(0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON),
	gsDPSetRenderMode(G_RM_PASS, G_RM_XLU_SURF2),
	gsDPSetCombineLERP(0,0,0,TEXEL0, 0,0,0,TEXEL0, 0,0,0,COMBINED, 0,0,0,TEXEL0),
	gsDPSetAlphaCompare(G_AC_NONE),
//	gsDPSetAlphaCompare(G_AC_THRESHOLD),
//	gsDPSetBlendColor(0, 0, 0, 1),
	gsDPSetTextureFilter(G_TF_POINT),
	gsDPSetTextureLUT(G_TT_NONE),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetTextureLOD(G_TL_TILE),
	gsDPSetTextureDetail(G_TD_CLAMP),
	gsDPSetTextureConvert(G_TC_FILT),
	gsSPEndDisplayList(),
};

Gfx normal_texture_init_dl[] = {
	gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_1CYCLE),
	gsSPTexture(0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON),
	gsDPSetRenderMode(G_RM_OPA_SURF, G_RM_OPA_SURF2),
	gsDPSetCombineLERP(0,0,0,TEXEL0, 0,0,0,TEXEL0, 0,0,0,TEXEL0, 0,0,0,TEXEL0),
	gsDPSetAlphaCompare(G_AC_THRESHOLD),
	gsDPSetBlendColor(0, 0, 0, 1),
	gsDPSetTextureFilter(G_TF_POINT),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetTextureLOD(G_TL_TILE),
	gsDPSetTextureDetail(G_TD_CLAMP),
	gsDPSetTextureConvert(G_TC_FILT),
	gsSPEndDisplayList(),
};

//////////////////////////////////////////////////////////////////////////////
// シザー

void gfxSetScissor(Gfx **gpp, int flags, int x, int y, int w, int h)
{
	static const int _modes[] = {
		G_SC_ODD_INTERLACE,
		G_SC_EVEN_INTERLACE,
		G_SC_NON_INTERLACE,
		G_SC_NON_INTERLACE,
	};
	Gfx *gp = *gpp;
	int scis[] = {
		CLAMP(0, SCREEN_WD - 1, x),
		CLAMP(0, SCREEN_HT - 1, y),
		CLAMP(0, SCREEN_WD - 1, x + w - 1),
		CLAMP(0, SCREEN_HT - 1, y + h - 1),
	};
	int mode = _modes[ flags & 3 ];

	gDPSetScissor(gp++, mode, scis[0], scis[1], scis[2], scis[3]);

	*gpp = gp;
}

//////////////////////////////////////////////////////////////////////////////
// 塗りつぶし

// 不透明塗りつぶし
void FillRectRGB(Gfx **gpp, int x, int y, int w, int h, int r, int g, int b)
{
	static Gfx init_dl[] = {
		gsDPPipeSync(),
		gsDPSetCycleType(G_CYC_FILL),
		gsDPSetRenderMode(G_RM_NOOP, G_RM_NOOP2),
		gsSPEndDisplayList(),
	};
	Gfx *gp = *gpp;
	int color = GPACK_RGBA5551(r, g, b, 1);
	color = (color << 16) | color;

	gSPDisplayList(gp++, init_dl);
	gDPSetFillColor(gp++, color);
	gDPScisFillRectangle(gp++, x, y, x+w-1, y+h-1);

	*gpp = gp;
}

// 半透明塗りつぶし
void FillRectRGBA(Gfx **gpp, int x, int y, int w, int h, int r, int g, int b, int a)
{
	static Gfx init_dl[] = {
		gsDPPipeSync(),
		gsDPSetCycleType(G_CYC_1CYCLE),
		gsDPSetRenderMode(G_RM_XLU_SURF, G_RM_XLU_SURF2),
		gsDPSetCombineMode(G_CC_PRIMITIVE, G_CC_PRIMITIVE),
		gsSPEndDisplayList(),
	};
	Gfx *gp = *gpp;

	gSPDisplayList(gp++, init_dl);
	gDPSetPrimColor(gp++, 0,0, r,g,b,a);
	gDPScisFillRectangle(gp++, x, y, x+w, y+h);

	*gpp = gp;
}

//////////////////////////////////////////////////////////////////////////////
// テクスチャをコピーモードで描画(コア部)

typedef struct SCopyTexBlock SCopyTexBlock;

struct SCopyTexBlock {
	// 引数
	Gfx **gpp;
	void *tex;
	int x, y, w, h;

	// 色数別引数
	void (*func)(SCopyTexBlock *st);
	int step;

	// ワーク
	int top, bottom;
};

static void CopyTexBlock(SCopyTexBlock *st)
{
	int x = (st->x < 0 ? st->x : st->x << 2);

	for(st->top = 0; st->top < st->h; st->top += st->step) {
		st->bottom = MIN(st->top + st->step, st->h);

		if(st->tex) {
			st->func(st);
		}

		gSPScisTextureRectangle((*st->gpp)++,
			x,
			(st->y + st->top) << 2,
			(st->x + st->w - 1) << 2,
			(st->y + st->bottom - 1) << 2,
			G_TX_RENDERTILE,
			0 << 5, 0 << 5,
			4 << 10, 1 << 10);
	}
}

//////////////////////////////////////////////////////////////////////////////
// テクスチャをコピーモードで描画

// コピーモードでテクスチャを描画(4bit)
static void CopyTexBlock4_LoadTex(SCopyTexBlock *st)
{
	gDPLoadTextureBlock_4b((*st->gpp)++,
		(u8 *)st->tex + st->w * st->top / 2,
		G_IM_FMT_CI,
		st->w, st->bottom - st->top, 0,
		G_TX_CLAMP, G_TX_CLAMP,
		G_TX_NOMASK, G_TX_NOMASK,
		G_TX_NOLOD, G_TX_NOLOD);
}
void CopyTexBlock4(Gfx **gpp, void *lut, void *tex, int x, int y, int w, int h)
{
	SCopyTexBlock st[1];

	st->gpp = gpp;
	st->tex = tex;
	st->x = x;
	st->y = y;
	st->w = w;
	st->h = h;

	st->func = CopyTexBlock4_LoadTex;
	st->step = 4096 / w;

	gSPDisplayList((*gpp)++, copy_texture_init_dl);

	if(lut) {
		gDPLoadTLUT_pal16((*gpp)++, 0, lut);
	}

	CopyTexBlock(st);
}

// コピーモードでテクスチャを描画(8bit)
static void CopyTexBlock8_LoadTex(SCopyTexBlock *st)
{
	gDPLoadTextureBlock((*st->gpp)++,
		(u8 *)st->tex + st->w * st->top,
		G_IM_FMT_CI, G_IM_SIZ_8b,
		st->w, st->bottom - st->top, 0,
		G_TX_CLAMP, G_TX_CLAMP,
		G_TX_NOMASK, G_TX_NOMASK,
		G_TX_NOLOD, G_TX_NOLOD);
}
void CopyTexBlock8(Gfx **gpp, void *lut, void *tex, int x, int y, int w, int h)
{
	SCopyTexBlock st[1];

	st->gpp = gpp;
	st->tex = tex;
	st->x = x;
	st->y = y;
	st->w = w;
	st->h = h;

	st->func = CopyTexBlock8_LoadTex;
	st->step = 2048 / w;

	gSPDisplayList((*gpp)++, copy_texture_init_dl);

	if(lut) {
		gDPLoadTLUT_pal256((*gpp)++, lut);
	}

	CopyTexBlock(st);
}

// コピーモードでテクスチャを描画(16bit)
static void CopyTexBlock16_LoadTex(SCopyTexBlock *st)
{
	gDPLoadTextureBlock((*st->gpp)++,
		(u8 *)st->tex + st->w * st->top * 2,
		G_IM_FMT_RGBA, G_IM_SIZ_16b,
		st->w, st->bottom - st->top, 0,
		G_TX_CLAMP, G_TX_CLAMP,
		G_TX_NOMASK, G_TX_NOMASK,
		G_TX_NOLOD, G_TX_NOLOD);
}
void CopyTexBlock16(Gfx **gpp, void *tex, int x, int y, int w, int h)
{
	SCopyTexBlock st[1];

	st->gpp = gpp;
	st->tex = tex;
	st->x = x;
	st->y = y;
	st->w = w;
	st->h = h;

	st->func = CopyTexBlock16_LoadTex;
	st->step = 2048 / w;

	gSPDisplayList((*gpp)++, copy_texture_init_dl);
	gDPSetTextureLUT((*gpp)++, G_TT_NONE);

	CopyTexBlock(st);
}

//////////////////////////////////////////////////////////////////////////////
// テクスチャを描画(コア部)

typedef struct SStretchTexBlock SStretchTexBlock;

struct SStretchTexBlock {
	// 引数
	Gfx **gpp;
	Vtx **vpp;
	int   texW, texH;
	void *colorTex;
	int   colorTexW;
	void *alphaTex;
	int   alphaTexW;
	int   x, y, w, h;

	// 色数別引数
	void (*colorFunc)(SStretchTexBlock *st);
	void (*alphaFunc)(SStretchTexBlock *st);
	int step;

	// 描画関数
	void (*rectFunc)(SStretchTexBlock *st);

	// ワーク
	int top, bottom, loadH, rect[8];
};

static void StretchTexBlock(SStretchTexBlock *st)
{
	int w, h, *t, sign[2], biMask, biStep;

	     if(st->w < 0) { w = -st->w; sign[0] = -1; }
	else if(st->w > 0) { w =  st->w; sign[0] =  1; }
	else { return; }

	     if(st->h < 0) { h = -st->h; sign[1] = -1; }
	else if(st->h > 0) { h =  st->h; sign[1] =  1; }
	else { return; }

	if(st->vpp == NULL && w == (st->texW << 2) && h == (st->texH << 2)) {
		gDPSetTextureFilter((*st->gpp)++, G_TF_POINT);
		biMask = 0;
		biStep = st->step;
	}
	else {
		gDPSetTextureFilter((*st->gpp)++, G_TF_BILERP);
		biMask = -1;
		biStep = MAX(1, st->step - 1);
	}

	t = st->rect;

	t[0] = st->x;
	t[2] = st->x + w;
	t[4] = (sign[0] < 0 ? st->texW - 1 : 0) << 5;
	t[6] = (sign[0] << 12) * st->texW / w;
//	t[4] -= (t[6] / (1 << 6)) & biMask;

	for(st->top = 0; st->top < st->texH; st->top += biStep) {
		st->bottom = MIN(st->top + st->step, st->texH);
		st->loadH = st->bottom - st->top;

		if(st->colorTex) st->colorFunc(st);
		if(st->alphaTex) st->alphaFunc(st);

		if(sign[1] < 0) {
			t[1] = st->texH - st->bottom;
			t[3] = st->texH - st->top;
			t[5] = st->loadH - 1;
		}
		else {
			t[1] = st->top;
			t[3] = st->bottom;
			t[5] = 0;
		}

		t[1] = st->y + (t[1] << 2) * h / (st->texH << 2);
		t[3] = st->y + (t[3] << 2) * h / (st->texH << 2);
		t[5] = t[5] << 5;
		t[7] = (sign[1] << 12) * st->texH / h;
		t[5] -= (t[7] / (1 << 6)) & biMask;

		st->rectFunc(st);
	}
}

//////////////////////////////////////////////////////////////////////////////
// テクスチャを描画

// テクスチャを描画
static void StretchTexBlock_ScisRect(SStretchTexBlock *st)
{
	int *t = st->rect;
	gSPScisTextureRectangle((*st->gpp)++,
		t[0], t[1], t[2], t[3], G_TX_RENDERTILE,
		t[4], t[5], t[6], t[7]);
}

// アルファチャンネル付きテクスチャを描画
static void StretchAlphaTexBlock_LoadColorTex(SStretchTexBlock *st)
{
	gDPLoadMultiBlock((*st->gpp)++,
		(u16 *)st->colorTex + st->colorTexW * st->top, 0,
		G_TX_RENDERTILE + 0,
		G_IM_FMT_RGBA, G_IM_SIZ_16b,
		st->colorTexW, st->loadH, 0,
		G_TX_CLAMP, G_TX_CLAMP,
		G_TX_NOMASK, G_TX_NOMASK,
		G_TX_NOLOD, G_TX_NOLOD);
}
static void StretchAlphaTexBlock_LoadAlphaTex(SStretchTexBlock *st)
{
	gDPLoadMultiBlock_4b((*st->gpp)++,
		(u8 *)st->alphaTex + st->alphaTexW * st->top / 2, 410,
		G_TX_RENDERTILE + 1,
		G_IM_FMT_I,
		st->alphaTexW, st->loadH, 0,
		G_TX_CLAMP, G_TX_CLAMP,
		G_TX_NOMASK, G_TX_NOMASK,
		G_TX_NOLOD, G_TX_NOLOD);
}
void StretchAlphaTexBlock(Gfx **gpp,
	int texW, int texH,            // アライン部分を除いたテクスチャのサイズ
	void *colorTex, int colorTexW, // カラーテクスチャのアドレスと幅
	void *alphaTex, int alphaTexW, // アルファテクスチャのアドレスと幅
	float x, float y,              // 描画座標
	float w, float h               // 描画範囲, 負の値を指定すると反転描画
	)
{
	SStretchTexBlock st[1];

	st->gpp = gpp;
	st->vpp = NULL;
	st->texW = texW;
	st->texH = texH;
	st->colorTex = colorTex;
	st->colorTexW = colorTexW;
	st->alphaTex = alphaTex;
	st->alphaTexW = alphaTexW;
	st->x = (int)(x * 4.0);
	st->y = (int)(y * 4.0);
	st->w = (int)(w * 4.0);
	st->h = (int)(h * 4.0);

	st->colorFunc = StretchAlphaTexBlock_LoadColorTex;
	st->alphaFunc = StretchAlphaTexBlock_LoadAlphaTex;
	st->step = 1632 / MAX(colorTexW, alphaTexW);

	st->rectFunc = StretchTexBlock_ScisRect;

	StretchTexBlock(st);
}

// テクスチャを描画(4bit)
static void StretchTexBlock4_LoadTex(SStretchTexBlock *st)
{
	gDPLoadTextureBlock_4b((*st->gpp)++,
		(u8 *)st->colorTex + st->colorTexW * st->top / 2,
		G_IM_FMT_CI,
		st->colorTexW, st->loadH, 0,
		G_TX_CLAMP, G_TX_CLAMP,
		G_TX_NOMASK, G_TX_NOMASK,
		G_TX_NOLOD, G_TX_NOLOD);
}
void StretchTexBlock4(Gfx **gpp,
	int texW, int texH, void *lut, void *tex,
	float x, float y, float w, float h)
{
	SStretchTexBlock st[1];

	st->gpp = gpp;
	st->vpp = NULL;
	st->texW = texW;
	st->texH = texH;
	st->colorTex = tex;
	st->colorTexW = texW;
	st->alphaTex = NULL;
	st->alphaTexW = 0;
	st->x = (int)(x * 4.0);
	st->y = (int)(y * 4.0);
	st->w = (int)(w * 4.0);
	st->h = (int)(h * 4.0);

	st->colorFunc = StretchTexBlock4_LoadTex;
	st->alphaFunc = NULL;
	st->step = 4096 / texW;

	st->rectFunc = StretchTexBlock_ScisRect;

	if(lut) {
		gDPLoadTLUT_pal16((*gpp)++, 0, lut);
	}

	StretchTexBlock(st);
}

// テクスチャを描画(8bit)
static void StretchTexBlock8_LoadTex(SStretchTexBlock *st)
{
	gDPLoadTextureBlock((*st->gpp)++,
		(u8 *)st->colorTex + st->colorTexW * st->top,
		G_IM_FMT_CI, G_IM_SIZ_8b,
		st->colorTexW, st->loadH, 0,
		G_TX_CLAMP, G_TX_CLAMP,
		G_TX_NOMASK, G_TX_NOMASK,
		G_TX_NOLOD, G_TX_NOLOD);
}
void StretchTexBlock8(Gfx **gpp,
	int texW, int texH, void *lut, void *tex,
	float x, float y, float w, float h)
{
	SStretchTexBlock st[1];

	st->gpp = gpp;
	st->vpp = NULL;
	st->texW = texW;
	st->texH = texH;
	st->colorTex = tex;
	st->colorTexW = texW;
	st->alphaTex = NULL;
	st->alphaTexW = 0;
	st->x = (int)(x * 4.0);
	st->y = (int)(y * 4.0);
	st->w = (int)(w * 4.0);
	st->h = (int)(h * 4.0);

	st->colorFunc = StretchTexBlock8_LoadTex;
	st->alphaFunc = NULL;
	st->step = 2048 / texW;

	st->rectFunc = StretchTexBlock_ScisRect;

	if(lut) {
		gDPLoadTLUT_pal256((*gpp)++, lut);
	}

	StretchTexBlock(st);
}

// テクスチャを描画(16bit)
static void StretchTexBlock16_LoadTex(SStretchTexBlock *st)
{
	gDPLoadTextureBlock((*st->gpp)++,
		(u8 *)st->colorTex + st->colorTexW * st->top * 2,
		G_IM_FMT_RGBA, G_IM_SIZ_16b,
		st->colorTexW, st->loadH, 0,
		G_TX_CLAMP, G_TX_CLAMP,
		G_TX_NOMASK, G_TX_NOMASK,
		G_TX_NOLOD, G_TX_NOLOD);
}
void StretchTexBlock16(Gfx **gpp,
	int texW, int texH, void *tex,
	float x, float y, float w, float h)
{
	SStretchTexBlock st[1];

	st->gpp = gpp;
	st->vpp = NULL;
	st->texW = texW;
	st->texH = texH;
	st->colorTex = tex;
	st->colorTexW = texW;
	st->alphaTex = NULL;
	st->alphaTexW = 0;
	st->x = (int)(x * 4.0);
	st->y = (int)(y * 4.0);
	st->w = (int)(w * 4.0);
	st->h = (int)(h * 4.0);

	st->colorFunc = StretchTexBlock16_LoadTex;
	st->alphaFunc = NULL;
	st->step = 2048 / texW;

	st->rectFunc = StretchTexBlock_ScisRect;

	StretchTexBlock(st);
}

// テクスチャを描画(4bit ｲﾝﾃﾝｼﾃｨ)
static void StretchTexBlock4i_LoadTex(SStretchTexBlock *st)
{
	gDPLoadTextureBlock_4b((*st->gpp)++,
		(u8 *)st->colorTex + st->colorTexW * st->top / 2,
		G_IM_FMT_I,
		st->colorTexW, st->loadH, 0,
		G_TX_CLAMP, G_TX_CLAMP,
		G_TX_NOMASK, G_TX_NOMASK,
		G_TX_NOLOD, G_TX_NOLOD);
}
void StretchTexBlock4i(Gfx **gpp,
	int texW, int texH, void *tex,
	float x, float y, float w, float h)
{
	SStretchTexBlock st[1];

	st->gpp = gpp;
	st->vpp = NULL;
	st->texW = texW;
	st->texH = texH;
	st->colorTex = tex;
	st->colorTexW = texW;
	st->alphaTex = NULL;
	st->alphaTexW = 0;
	st->x = (int)(x * 4.0);
	st->y = (int)(y * 4.0);
	st->w = (int)(w * 4.0);
	st->h = (int)(h * 4.0);

	st->colorFunc = StretchTexBlock4i_LoadTex;
	st->alphaFunc = NULL;
	st->step = 8192 / texW;

	st->rectFunc = StretchTexBlock_ScisRect;

	StretchTexBlock(st);
}

// テクスチャを描画(8bit ｲﾝﾃﾝｼﾃｨ)
static void StretchTexBlock8i_LoadTex(SStretchTexBlock *st)
{
	gDPLoadTextureBlock((*st->gpp)++,
		(u8 *)st->colorTex + st->colorTexW * st->top,
		G_IM_FMT_I, G_IM_SIZ_8b,
		st->colorTexW, st->loadH, 0,
		G_TX_CLAMP, G_TX_CLAMP,
		G_TX_NOMASK, G_TX_NOMASK,
		G_TX_NOLOD, G_TX_NOLOD);
}
void StretchTexBlock8i(Gfx **gpp,
	int texW, int texH, void *tex,
	float x, float y, float w, float h)
{
	SStretchTexBlock st[1];

	st->gpp = gpp;
	st->vpp = NULL;
	st->texW = texW;
	st->texH = texH;
	st->colorTex = tex;
	st->colorTexW = texW;
	st->alphaTex = NULL;
	st->alphaTexW = 0;
	st->x = (int)(x * 4.0);
	st->y = (int)(y * 4.0);
	st->w = (int)(w * 4.0);
	st->h = (int)(h * 4.0);

	st->colorFunc = StretchTexBlock8i_LoadTex;
	st->alphaFunc = NULL;
	st->step = 4096 / texW;

	st->rectFunc = StretchTexBlock_ScisRect;

	StretchTexBlock(st);
}

//////////////////////////////////////////////////////////////////////////////
// テクスチャタイルを描画(コア部)

typedef struct SStretchTexTile SStretchTexTile;

struct SStretchTexTile {
	// 引数
	Gfx **gpp;
	Vtx **vpp;
	int   texW, texH;
	void *colorTex;
	int   colorTexW;
	void *alphaTex;
	int   alphaTexW;
	int   lx,ly,lw,lh;
	int    x, y, w, h;

	// 色数別引数
	void (*colorFunc)(SStretchTexTile *st);
	void (*alphaFunc)(SStretchTexTile *st);
	int step;

	// 描画関数
	void (*rectFunc)(SStretchTexTile *st);

	// ワーク
	int top, bottom, rect[8];
};

static void StretchTexTile(SStretchTexTile *st)
{
	int w, h, endX, endY, *t, sign[2], biMask, biStep;

	     if(st->w < 0) { w = -st->w; sign[0] = -1; }
	else if(st->w > 0) { w =  st->w; sign[0] =  1; }
	else { return; }

	     if(st->h < 0) { h = -st->h; sign[1] = -1; }
	else if(st->h > 0) { h =  st->h; sign[1] =  1; }
	else { return; }

	endX = st->lx + st->lw;
	endY = st->ly + st->lh;

	if(st->vpp == NULL && w == (st->lw << 2) && h == (st->lh << 2)) {
		gDPSetTextureFilter((*st->gpp)++, G_TF_POINT);
		biMask = 0;
		biStep = st->step;
	}
	else {
		gDPSetTextureFilter((*st->gpp)++, G_TF_BILERP);
		biMask = -1;
		biStep = MAX(1, st->step - 1);
	}

	t = st->rect;

	t[0] = st->x;
	t[2] = st->x + w;
	t[4] = (sign[0] < 0 ? endX - 1 : st->lx) << 5;
	t[6] = (sign[0] << 12) * st->lw / w;
//	t[4] -= (t[6] / (1 << 6)) & biMask;

	for(st->top = st->ly; st->top < endY; st->top += biStep) {
		st->bottom = MIN(st->top + st->step, endY);

		if(st->colorTex) st->colorFunc(st);
		if(st->alphaTex) st->alphaFunc(st);

		if(sign[1] < 0) {
			t[1] = endY - st->bottom;
			t[3] = endY - st->top;
			t[5] = st->bottom - 1;
		}
		else {
			t[1] = st->top;
			t[3] = st->bottom;
			t[5] = st->top;
		}

		t[1] = st->y + ((t[1] - st->ly) << 2) * h / (st->lh << 2);
		t[3] = st->y + ((t[3] - st->ly) << 2) * h / (st->lh << 2);
		t[5] = t[5] << 5;
		t[7] = (sign[1] << 12) * st->lh / h;
		t[5] -= (t[7] / (1 << 6)) & biMask;

		st->rectFunc(st);
	}
}

//////////////////////////////////////////////////////////////////////////////
// テクスチャタイルを描画

// テクスチャを描画
static void StretchTexTile_ScisRect(SStretchTexTile *st)
{
	int *t = st->rect;
	gSPScisTextureRectangle((*st->gpp)++,
		t[0], t[1], t[2], t[3], G_TX_RENDERTILE,
		t[4], t[5], t[6], t[7]);
}
static void RectTexTile_ScisRect(SStretchTexTile *st)
{
	Gfx *gp = *st->gpp;
	Vtx *vp = *st->vpp;
	int *t = st->rect;

	gSPVertex(gp++, vp, 4, 0);
	gSP2Triangles(gp++, 0,1,2,0, 3,2,1,0);

	// 0-2
	// |/|
	// 1-3

	vp[0].v.ob[0] = vp[1].v.ob[0] = t[0] >> 2;
	vp[0].v.ob[1] = vp[2].v.ob[1] = t[1] >> 2;
	vp[2].v.ob[0] = vp[3].v.ob[0] = t[2] >> 2;
	vp[1].v.ob[1] = vp[3].v.ob[1] = t[3] >> 2;

	vp[0].v.tc[0] = vp[1].v.tc[0] = (t[4] << 2);
	vp[0].v.tc[1] = vp[2].v.tc[1] = (t[5] << 2);
	vp[2].v.tc[0] = vp[3].v.tc[0] = (((t[2] - t[0]) * t[6] + (t[4] << 7)) >> 5);
	vp[1].v.tc[1] = vp[3].v.tc[1] = (((t[3] - t[1]) * t[7] + (t[5] << 7)) >> 5);

	*(u32 *)&vp[0].v.ob[2] = *(u32 *)&vp[1].v.ob[2] =
	*(u32 *)&vp[2].v.ob[2] = *(u32 *)&vp[3].v.ob[2] = 0x00000000;

	*(u32 *)&vp[0].v.cn[0] = *(u32 *)&vp[1].v.cn[0] =
	*(u32 *)&vp[2].v.cn[0] = *(u32 *)&vp[3].v.cn[0] = 0xffffffff;

	*st->vpp = vp + 4;
	*st->gpp = gp;
}

// アルファチャンネル付きテクスチャタイルを描画
static void StretchAlphaTexTile_LoadColorTex(SStretchTexTile *st)
{
	gDPLoadMultiTile((*st->gpp)++,
		st->colorTex, 0, G_TX_RENDERTILE + 0, G_IM_FMT_RGBA, G_IM_SIZ_16b,
		st->colorTexW, st->texH,
		st->lx, st->top,
		st->lx + st->lw - 1, st->bottom - 1,
		0,
		G_TX_CLAMP, G_TX_CLAMP,
		G_TX_NOMASK, G_TX_NOMASK,
		G_TX_NOLOD, G_TX_NOLOD);
}
static void StretchAlphaTexTile_LoadAlphaTex(SStretchTexTile *st)
{
	gDPLoadMultiTile_4b((*st->gpp)++,
		st->alphaTex, 410, G_TX_RENDERTILE + 1, G_IM_FMT_I,
		st->alphaTexW, st->texH,
		st->lx, st->top,
		st->lx + st->lw - 1, st->bottom - 1,
		0,
		G_TX_CLAMP, G_TX_CLAMP,
		G_TX_NOMASK, G_TX_NOMASK,
		G_TX_NOLOD, G_TX_NOLOD);
}
void StretchAlphaTexTile(Gfx **gpp,
	int texW, int texH,            // アライン部分を除いたテクスチャのサイズ
	void *colorTex, int colorTexW, // カラーテクスチャのアドレスと幅
	void *alphaTex, int alphaTexW, // アルファテクスチャのアドレスと幅
	int lx, int ly,                // 
	int lw, int lh,                // 
	float x, float y,              // 描画座標
	float w, float h               // 描画範囲, 負の値を指定すると反転描画
	)
{
	SStretchTexTile st[1];

	st->gpp = gpp;
	st->vpp = NULL;
	st->texW = texW;
	st->texH = texH;
	st->colorTex = colorTex;
	st->colorTexW = colorTexW;
	st->alphaTex = alphaTex;
	st->alphaTexW = alphaTexW;
	st->lx = lx;
	st->ly = ly;
	st->lw = lw;
	st->lh = lh;
	st->x = (int)(x * 4.0);
	st->y = (int)(y * 4.0);
	st->w = (int)(w * 4.0);
	st->h = (int)(h * 4.0);

	st->colorFunc = StretchAlphaTexTile_LoadColorTex;
	st->alphaFunc = StretchAlphaTexTile_LoadAlphaTex;
	st->step = 1632 / MAX(((colorTexW + 3) & ~3), ((alphaTexW + 15) & ~15));

	st->rectFunc = StretchTexTile_ScisRect;

	StretchTexTile(st);
}
void RectAlphaTexTile(Gfx **gpp, Vtx **vpp,
	int texW, int texH,
	void *colorTex, int colorTexW,
	void *alphaTex, int alphaTexW,
	int lx, int ly,
	int lw, int lh,
	float x, float y,
	float w, float h)
{
	SStretchTexTile st[1];

	st->gpp = gpp;
	st->vpp = vpp;
	st->texW = texW;
	st->texH = texH;
	st->colorTex = colorTex;
	st->colorTexW = colorTexW;
	st->alphaTex = alphaTex;
	st->alphaTexW = alphaTexW;
	st->lx = lx;
	st->ly = ly;
	st->lw = lw;
	st->lh = lh;
	st->x = (int)(x * 4.0);
	st->y = (int)(y * 4.0);
	st->w = (int)(w * 4.0);
	st->h = (int)(h * 4.0);

	st->colorFunc = StretchAlphaTexTile_LoadColorTex;
	st->alphaFunc = StretchAlphaTexTile_LoadAlphaTex;
	st->step = 1632 / MAX(((colorTexW + 3) & ~3), ((alphaTexW + 15) & ~15));

	st->rectFunc = RectTexTile_ScisRect;

	StretchTexTile(st);
}

// テクスチャタイルを描画(4bit)
static void StretchTexTile4_LoadTex(SStretchTexTile *st)
{
	gDPLoadTextureTile_4b((*st->gpp)++,
		st->colorTex, G_IM_FMT_CI,
		st->colorTexW, st->texH,
		st->lx, st->top,
		st->lx + st->lw - 1, st->bottom - 1,
		0,
		G_TX_CLAMP, G_TX_CLAMP,
		G_TX_NOMASK, G_TX_NOMASK,
		G_TX_NOLOD, G_TX_NOLOD);
}
void StretchTexTile4(Gfx **gpp,
	int texW, int texH, void *lut, void *tex,
	int lx, int ly, int lw, int lh,
	float x, float y, float w, float h)
{
	SStretchTexTile st[1];

	st->gpp = gpp;
	st->vpp = NULL;
	st->texW = texW;
	st->texH = texH;
	st->colorTex = tex;
	st->colorTexW = texW;
	st->alphaTex = NULL;
	st->alphaTexW = 0;
	st->lx = lx;
	st->ly = ly;
	st->lw = lw;
	st->lh = lh;
	st->x = (int)(x * 4.0);
	st->y = (int)(y * 4.0);
	st->w = (int)(w * 4.0);
	st->h = (int)(h * 4.0);

	st->colorFunc = StretchTexTile4_LoadTex;
	st->alphaFunc = NULL;
	st->step = 4096 / ((lw + 15) & ~15);

	st->rectFunc = StretchTexTile_ScisRect;

	if(lut) {
		gDPLoadTLUT_pal16((*gpp)++, 0, lut);
	}

	StretchTexTile(st);
}

// テクスチャタイルを描画(8bit)
static void StretchTexTile8_LoadTex(SStretchTexTile *st)
{
	gDPLoadTextureTile((*st->gpp)++,
		st->colorTex, G_IM_FMT_CI, G_IM_SIZ_8b,
		st->colorTexW, st->texH,
		st->lx, st->top,
		st->lx + st->lw - 1, st->bottom - 1,
		0,
		G_TX_CLAMP, G_TX_CLAMP,
		G_TX_NOMASK, G_TX_NOMASK,
		G_TX_NOLOD, G_TX_NOLOD);
}
void StretchTexTile8(Gfx **gpp,
	int texW, int texH, void *lut, void *tex,
	int lx, int ly, int lw, int lh,
	float x, float y, float w, float h)
{
	SStretchTexTile st[1];

	st->gpp = gpp;
	st->vpp = NULL;
	st->texW = texW;
	st->texH = texH;
	st->colorTex = tex;
	st->colorTexW = texW;
	st->alphaTex = NULL;
	st->alphaTexW = 0;
	st->lx = lx;
	st->ly = ly;
	st->lw = lw;
	st->lh = lh;
	st->x = (int)(x * 4.0);
	st->y = (int)(y * 4.0);
	st->w = (int)(w * 4.0);
	st->h = (int)(h * 4.0);

	st->colorFunc = StretchTexTile8_LoadTex;
	st->alphaFunc = NULL;
	st->step = 2048 / ((lw + 7) & ~7);

	st->rectFunc = StretchTexTile_ScisRect;

	if(lut) {
		gDPLoadTLUT_pal256((*gpp)++, lut);
	}

	StretchTexTile(st);
}
void RectTexTile8(Gfx **gpp, Vtx **vpp,
	int texW, int texH, void *lut, void *tex,
	int lx, int ly, int lw, int lh,
	float x, float y, float w, float h)
{
	SStretchTexTile st[1];

	st->gpp = gpp;
	st->vpp = vpp;
	st->texW = texW;
	st->texH = texH;
	st->colorTex = tex;
	st->colorTexW = texW;
	st->alphaTex = NULL;
	st->alphaTexW = 0;
	st->lx = lx;
	st->ly = ly;
	st->lw = lw;
	st->lh = lh;
	st->x = (int)(x * 4.0);
	st->y = (int)(y * 4.0);
	st->w = (int)(w * 4.0);
	st->h = (int)(h * 4.0);

	st->colorFunc = StretchTexTile8_LoadTex;
	st->alphaFunc = NULL;
	st->step = 2048 / ((lw + 7) & ~7);

	st->rectFunc = RectTexTile_ScisRect;

	if(lut) {
		gDPLoadTLUT_pal256((*gpp)++, lut);
	}

	StretchTexTile(st);
}

// テクスチャタイルを描画(16bit)
static void StretchTexTile16_LoadTex(SStretchTexTile *st)
{
	gDPLoadTextureTile((*st->gpp)++,
		st->colorTex, G_IM_FMT_RGBA, G_IM_SIZ_16b,
		st->colorTexW, st->texH,
		st->lx, st->top,
		st->lx + st->lw - 1, st->bottom - 1,
		0,
		G_TX_CLAMP, G_TX_CLAMP,
		G_TX_NOMASK, G_TX_NOMASK,
		G_TX_NOLOD, G_TX_NOLOD);
}
void StretchTexTile16(Gfx **gpp,
	int texW, int texH, void *tex,
	int lx, int ly, int lw, int lh,
	float x, float y, float w, float h)
{
	SStretchTexTile st[1];

	st->gpp = gpp;
	st->vpp = NULL;
	st->texW = texW;
	st->texH = texH;
	st->colorTex = tex;
	st->colorTexW = texW;
	st->alphaTex = NULL;
	st->alphaTexW = 0;
	st->lx = lx;
	st->ly = ly;
	st->lw = lw;
	st->lh = lh;
	st->x = (int)(x * 4.0);
	st->y = (int)(y * 4.0);
	st->w = (int)(w * 4.0);
	st->h = (int)(h * 4.0);

	st->colorFunc = StretchTexTile16_LoadTex;
	st->alphaFunc = NULL;
	st->step = 2048 / ((lw + 3) & ~3);

	st->rectFunc = StretchTexTile_ScisRect;

	StretchTexTile(st);
}

// テクスチャを描画(4bit ｲﾝﾃﾝｼﾃｨ)
static void StretchTexTile4i_LoadTex(SStretchTexTile *st)
{
	gDPLoadTextureTile_4b((*st->gpp)++,
		st->colorTex, G_IM_FMT_I,
		st->colorTexW, st->texH,
		st->lx, st->top,
		st->lx + st->lw - 1, st->bottom - 1,
		0,
		G_TX_CLAMP, G_TX_CLAMP,
		G_TX_NOMASK, G_TX_NOMASK,
		G_TX_NOLOD, G_TX_NOLOD);
}
void StretchTexTile4i(Gfx **gpp,
	int texW, int texH, void *tex,
	int lx, int ly, int lw, int lh,
	float x, float y, float w, float h)
{
	SStretchTexTile st[1];

	st->gpp = gpp;
	st->vpp = NULL;
	st->texW = texW;
	st->texH = texH;
	st->colorTex = tex;
	st->colorTexW = texW;
	st->alphaTex = NULL;
	st->alphaTexW = 0;
	st->lx = lx;
	st->ly = ly;
	st->lw = lw;
	st->lh = lh;
	st->x = (int)(x * 4.0);
	st->y = (int)(y * 4.0);
	st->w = (int)(w * 4.0);
	st->h = (int)(h * 4.0);

	st->colorFunc = StretchTexTile4i_LoadTex;
	st->alphaFunc = NULL;
	st->step = 8192 / ((lw + 15) & ~15);

	st->rectFunc = StretchTexTile_ScisRect;

	StretchTexTile(st);
}
void RectTexTile4i(Gfx **gpp, Vtx **vpp,
	int texW, int texH, void *tex,
	int lx, int ly, int lw, int lh,
	float x, float y, float w, float h)
{
	SStretchTexTile st[1];

	st->gpp = gpp;
	st->vpp = vpp;
	st->texW = texW;
	st->texH = texH;
	st->colorTex = tex;
	st->colorTexW = texW;
	st->alphaTex = NULL;
	st->alphaTexW = 0;
	st->lx = lx;
	st->ly = ly;
	st->lw = lw;
	st->lh = lh;
	st->x = (int)(x * 4.0);
	st->y = (int)(y * 4.0);
	st->w = (int)(w * 4.0);
	st->h = (int)(h * 4.0);

	st->colorFunc = StretchTexTile4i_LoadTex;
	st->alphaFunc = NULL;
	st->step = 8192 / ((lw + 15) & ~15);

	st->rectFunc = RectTexTile_ScisRect;

	StretchTexTile(st);
}

//////////////////////////////////////////////////////////////////////////////

// テクスチャデータ内のアドレスをメモリ上にマッピング
void tiMappingAddr(STexInfo *tiArray, int count, u32 segGap)
{
	int i;

	for(i = 0; i < count; i++) {
		STexInfo *ti = &tiArray[i];

		if(ti->addr) {
			(u8 *)ti->addr += segGap;

			if(ti->addr[0]) {
				(u8 *)ti->addr[0] += segGap;
			}

			if(ti->addr[1]) {
				(u8 *)ti->addr[1] += segGap;
			}
		}

		if(ti->size) {
			(u8 *)ti->size += segGap;
		}
	}
}

// テクスチャをＲＯＭから読み込む
STexInfo *tiLoadTexData(void **hpp, void *romStart, void *romEnd)
{
	void *hp = (void *)ALIGN_16(*hpp);
	STexInfo *texInfo;
	int texSize;

//	*hpp = (void *)ExpandRomData(romStart, hp, (u32)romEnd - (u32)romStart);
	*hpp = (void *)ExpandGZip(romStart, hp, (u32)romEnd - (u32)romStart);

	texInfo =  (STexInfo *)( ((u32 *)hp)[0] += (u32)hp );
	texSize = *(int *)     ( ((u32 *)hp)[1] += (u32)hp );

	tiMappingAddr(texInfo, texSize, (u32)hp);

	return texInfo;
}

//////////////////////////////////////////////////////////////////////////////

// ディスプレイリストをＲＯＭから読み込む
Gfx **loadGfxData(void **hpp, void **segAddr, void *baseAddr, void *romStart, void *romEnd)
{
	Gfx **gfx;
	u32 gap, size;
	int i;

	*segAddr = (void *)ALIGN_16(*hpp);
	*hpp = (void *)ExpandGZip(romStart, *segAddr, (u32)romEnd - (u32)romStart);

	gap  = (u32)*segAddr - (u32)baseAddr;
	gfx  =  (Gfx **)( ((u32 *)*segAddr)[0] += gap );
	size = *(int *) ( ((u32 *)*segAddr)[1] += gap );

	for(i = 0; i < size; i++) {
		(u8 *)gfx[i] += gap;
	}

	return gfx;
}

//////////////////////////////////////////////////////////////////////////////

void tiCopyTexBlock(Gfx **gpp, STexInfo *ti, int cached, int x, int y)
{
	void *lut, *tex;

	if(cached) {
		lut = NULL;
		tex = NULL;
	}
	else {
		lut = ti->addr[0];
		tex = ti->addr[1];
	}

	switch(ti->size[2]) {
	case 4:
		CopyTexBlock4(gpp, lut, tex, x, y, ti->size[0], ti->size[1]);
		break;
	case 8:
		CopyTexBlock8(gpp, lut, tex, x, y, ti->size[0], ti->size[1]);
		break;
	case 16:
		CopyTexBlock16(gpp, tex, x, y, ti->size[0], ti->size[1]);
		break;
	}
}

void tiStretchTexBlock(Gfx **gpp, STexInfo *ti, int cached,
	float x, float y, float w, float h)
{
	void *lut, *tex;

	if(cached) {
		lut = NULL;
		tex = NULL;
	}
	else {
		lut = ti->addr[0];
		tex = ti->addr[1];
	}

	if(ti->addr[0]) {
		switch(ti->size[2]) {
		case 4:
			StretchTexBlock4(gpp, ti->size[0], ti->size[1], lut, tex, x, y, w, h);
			break;
		case 8:
			StretchTexBlock8(gpp, ti->size[0], ti->size[1], lut, tex, x, y, w, h);
			break;
		}
	}
	else {
		switch(ti->size[2]) {
		case 4:
			StretchTexBlock4i(gpp, ti->size[0], ti->size[1], tex, x, y, w, h);
			break;
		case 16:
			StretchTexBlock16(gpp, ti->size[0], ti->size[1], tex, x, y, w, h);
			break;
		}
	}
}

void tiStretchTexTile(Gfx **gpp, STexInfo *ti, int cached,
	int lx, int ly, int lw, int lh,
	float x, float y, float w, float h)
{
	void *lut, *tex;

	if(cached) {
		lut = NULL;
		tex = NULL;
	}
	else {
		lut = ti->addr[0];
		tex = ti->addr[1];
	}

	if(ti->addr[0]) {
		switch(ti->size[2]) {
		case 4:
			StretchTexTile4(gpp, ti->size[0], ti->size[1], lut, tex,
				lx, ly, lw, lh, x, y, w, h);
			break;
		case 8:
			StretchTexTile8(gpp, ti->size[0], ti->size[1], lut, tex,
				lx, ly, lw, lh, x, y, w, h);
			break;
		}
	}
	else {
		switch(ti->size[2]) {
		case 4:
			StretchTexTile4i(gpp, ti->size[0], ti->size[1], tex,
				lx, ly, lw, lh, x, y, w, h);
			break;
		case 16:
			StretchTexTile16(gpp, ti->size[0], ti->size[1], tex,
				lx, ly, lw, lh, x, y, w, h);
			break;
		}
	}
}

void tiStretchTexItem(Gfx **gpp, STexInfo *ti, int cached,
	int count, int index, float x, float y, float w, float h)
{
	int height, offset;
	void *lut, *tex;

	height = ti->size[1] / count;
	offset = ti->size[0] * height * index;

	if(cached) {
		lut = NULL;
		tex = NULL;
	}
	else {
		lut = ti->addr[0];
		tex = ti->addr[1];

		switch(ti->size[2]) {
		case 4:
			(u8 *)tex += offset / 2;
			break;

		case 8:
			(u8 *)tex += offset;
			break;

		case 16:
			(u8 *)tex += offset * 2;
			break;
		}
	}

	if(ti->size[3] & 1) {
		switch(ti->size[2]) {
		case 4:
			StretchTexBlock4(gpp, ti->size[0], height, lut, tex, x, y, w, h);
			break;
		case 8:
			StretchTexBlock8(gpp, ti->size[0], height, lut, tex, x, y, w, h);
			break;
		}
	}
	else {
		switch(ti->size[2]) {
		case 4:
			StretchTexTile4(gpp, ti->size[0], height, lut, tex,
				0, 0, ti->size[0], height, x, y, w, h);
			break;
		case 8:
			StretchTexTile8(gpp, ti->size[0], height, lut, tex,
				0, 0, ti->size[0], height, x, y, w, h);
			break;
		}
	}
}

void tiStretchAlphaTexItem(Gfx **gpp, STexInfo *tiC, STexInfo *tiA,
	int cached, int count, int index, float x, float y, float w, float h)
{
	int width, height;
	void *tex[2];

	width = MIN(tiC->size[0], tiA->size[0]);
	height = tiC->size[1] / count;
	height = MIN(height, tiA->size[1]);

	if(cached) {
		tex[0] = NULL;
		tex[1] = NULL;
	}
	else {
		tex[0] = (u8 *)tiC->addr[1] + tiC->size[0] * height * index * 2;
		tex[1] = (u8 *)tiA->addr[1] + tiA->size[0] * height * index / 2;
	}

	if((tiC->size[3] & 1) && (tiA->size[3] & 1)) {
		StretchAlphaTexBlock(gpp, width, height,
			tex[0], tiC->size[0], tex[1], tiA->size[0], x, y, w, h);
	}
	else {
		StretchAlphaTexTile(gpp, width, height,
			tex[0], tiC->size[0], tex[1], tiA->size[0],
			0, 0, width, height, x, y, w, h);
	}
}

void tiStretchAlphaTexItem2(Gfx **gpp, STexInfo *tiC, STexInfo *tiA,
	int cached, int count, int index, float x, float y, float w, float h)
{
	int width, height;
	void *tex[2];

	width = MIN(tiC->size[0], tiA->size[0]);
	height = MIN(tiC->size[1] / count, tiA->size[1]);

	if(cached) {
		tex[0] = NULL;
		tex[1] = NULL;
	}
	else {
		tex[0] = (u8 *)tiC->addr[1] + tiC->size[0] * height * index * 2;
		tex[1] = (u8 *)tiA->addr[1];
	}

	if((tiC->size[3] & 1) && (tiA->size[3] & 1)) {
		StretchAlphaTexBlock(gpp, width, height,
			tex[0], tiC->size[0], tex[1], tiA->size[0], x, y, w, h);
	}
	else {
		StretchAlphaTexTile(gpp, width, height,
			tex[0], tiC->size[0], tex[1], tiA->size[0],
			0, 0, width, height, x, y, w, h);
	}
}

//////////////////////////////////////////////////////////////////////////////

// 
void drawCursorPattern(Gfx **gpp,
	int texW, int texH, int frmW, int frmH,
	int posX, int posY, int posW, int posH)
{
	static const u8 _pnts[][8] = {
		{ 0,0,1,1,0,0,2,2 }, { 1,0,2,1,2,0,1,2 }, { 2,0,3,1,1,0,0,2 },
		{ 0,1,1,2,0,2,2,1 }, { 1,1,2,2,2,2,1,1 }, { 2,1,3,2,1,2,0,1 },
		{ 0,2,1,3,0,1,2,0 }, { 1,2,2,3,2,1,1,0 }, { 2,2,3,3,1,1,0,0 },
	};

	Gfx *gp = *gpp;
	int px[4], py[4], tx[3], ty[3], dx[3], size, i;

	size = posW + frmW;
	px[0] = (posX << 2) - (frmW << 1);

	px[3] = px[0] + (size << 2);
	px[1] = px[0] + (frmW << 2);
	px[2] = px[3] - (frmW << 2);

	tx[0] = 32 * 0;
	tx[1] = 32 * (frmW - 1);
	tx[2] = 32 * frmW;

	size = posH + frmH;
	py[0] = (posY << 2) - (frmH << 1);

	py[3] = py[0] + (size << 2);
	py[1] = py[0] + (frmH << 2);
	py[2] = py[3] - (frmH << 2);

	ty[0] = 32 * 0;
	ty[1] = 32 * (frmH - 1);
	ty[2] = 32 * frmH;

	dx[0] = -1 * 1024;
	dx[1] =  0 * 1024;
	dx[2] =  1 * 1024;

	for(i = 0; i < ARRAY_SIZE(_pnts); i++) {
		u8 *pnts = _pnts[i];
		gSPScisTextureRectangle(gp++,
			px[pnts[0]], py[pnts[1]],
			px[pnts[2]], py[pnts[3]], G_TX_RENDERTILE,
			tx[pnts[4]], ty[pnts[5]],
			dx[pnts[6]], dx[pnts[7]]);
	}

	*gpp = gp;
}

