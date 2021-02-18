
#include <ultra64.h>
#include <PR/ramrom.h>
#include <PR/gs2dex.h>
#include <assert.h>

#include "def.h"
#include "segment.h"
#include "message.h"
#include "nnsched.h"
#include "graphic.h"

//////////////////////////////////////////////////////////////////////////////
// s2dex 初期化ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄ
Gfx S2RDPinit_dl[] = {
	gsDPPipeSync(),
	gsDPPipelineMode(G_PM_1PRIMITIVE),
	gsDPSetTextureLOD(G_TL_TILE),
	gsDPSetTextureLUT(G_TT_NONE),
	gsDPSetTextureDetail(G_TD_CLAMP),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetTextureFilter(G_TF_BILERP),
	gsDPSetTextureConvert(G_TC_FILT),
	gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
	gsDPSetCombineKey(G_CK_NONE),
	gsDPSetAlphaCompare(G_AC_NONE),
	gsDPSetColorDither(G_CD_DISABLE),
//	gsDPSetScissor(G_SC_NON_INTERLACE, 0, 0, SCREEN_WD-1, SCREEN_HT-1),
	gsSPEndDisplayList(),
};

Gfx S2ClearCFB_dl[] = {
	gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_FILL),
	gsDPSetRenderMode(G_RM_OPA_SURF, G_RM_OPA_SURF2),
	gsDPSetFillColor(GPACK_RGBA5551(16,16,16,1) << 16 | GPACK_RGBA5551(16,16,16,1)),		// black
//	gsDPFillRectangle( 0, 0, SCREEN_WD-1, SCREEN_HT-1),
	gsSPEndDisplayList(),
};

Gfx S2Spriteinit_dl[] = {
	gsDPPipeSync(),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetTextureLOD(G_TL_TILE),
	gsDPSetTextureLUT(G_TT_NONE),
	gsDPSetTextureConvert(G_TC_FILT),
	gsDPSetAlphaCompare(G_AC_THRESHOLD),
	gsDPSetBlendColor(0, 0, 0, 0x01),
	gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
	gsSPEndDisplayList(),
};

//////////////////////////////////////////////////////////////////////////////
// f3dex 初期化ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄ

// Remember, viewport structures have 2 bits of fraction in them.
/*
static Vp vp = {
	SCREEN_WD*2, SCREEN_HT*2, G_MAXZ/2, 0,	// scale
	SCREEN_WD*2, SCREEN_HT*2, G_MAXZ/2, 0,	// translate
};
Lights1 sun_light = gdSPDefLights1(
	 80,  80,  80,	// ｱﾝﾋﾞｴﾝﾄ･ﾗｲﾄｶﾗｰ
	200, 200, 200,	// ﾃﾞｨﾌｭｰｽﾞ･ﾗｲﾄｶﾗｰ
	  0, 127, 127	// X,Y,Z 座標
);
*/

Gfx F3RDPinit_dl[] = {

	// set all of the attribute registers to zero
	gsDPSetEnvColor(0,0,0,0),
	gsDPSetPrimColor(0,0,0,0,0,0),
	gsDPSetBlendColor(0,0,0,0),
	gsDPSetFogColor(0,0,0,0),
	gsDPSetFillColor(0),
	gsDPSetPrimDepth(0,0),
	gsDPSetConvert(0,0,0,0,0,0),
	gsDPSetKeyR(0,0,0),
	gsDPSetKeyGB(0,0,0,0,0,0),

	// set combine mode
	gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),

	// initialize the scissor box
//	gsDPSetScissor(G_SC_NON_INTERLACE, 0, 0, SCREEN_WD-1, SCREEN_HT-1),

	// initialize all the texture tile descriptors to zero
	gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
	gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0),
	gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0),
	gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0),
	gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0),
	gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0),
	gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0),
	gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0),

	gsDPSetTileSize(0, 0, 0, (1 - 1) << G_TEXTURE_IMAGE_FRAC, (1 - 1) << G_TEXTURE_IMAGE_FRAC),
	gsDPSetTileSize(1, 0, 0, (1 - 1) << G_TEXTURE_IMAGE_FRAC, (1 - 1) << G_TEXTURE_IMAGE_FRAC),
	gsDPSetTileSize(2, 0, 0, (1 - 1) << G_TEXTURE_IMAGE_FRAC, (1 - 1) << G_TEXTURE_IMAGE_FRAC),
	gsDPSetTileSize(3, 0, 0, (1 - 1) << G_TEXTURE_IMAGE_FRAC, (1 - 1) << G_TEXTURE_IMAGE_FRAC),
	gsDPSetTileSize(4, 0, 0, (1 - 1) << G_TEXTURE_IMAGE_FRAC, (1 - 1) << G_TEXTURE_IMAGE_FRAC),
	gsDPSetTileSize(5, 0, 0, (1 - 1) << G_TEXTURE_IMAGE_FRAC, (1 - 1) << G_TEXTURE_IMAGE_FRAC),
	gsDPSetTileSize(6, 0, 0, (1 - 1) << G_TEXTURE_IMAGE_FRAC, (1 - 1) << G_TEXTURE_IMAGE_FRAC),
	gsDPSetTileSize(7, 0, 0, (1 - 1) << G_TEXTURE_IMAGE_FRAC, (1 - 1) << G_TEXTURE_IMAGE_FRAC),

	gsDPPipeSync(),
	gsSPEndDisplayList(),
};

Gfx F3SetupRDP_dl[] = {
	gsDPSetRenderMode(G_RM_OPA_SURF, G_RM_OPA_SURF2),
//	gsDPSetColorDither(G_CD_BAYER),
	gsDPSetColorDither(G_CD_DISABLE),
//	gsDPSetFogColor(255, 0, 0, 255),
//	gsDPSetScissor(G_SC_NON_INTERLACE, 0, 0, SCREEN_WD-1, SCREEN_HT-1),
	gsDPPipeSync(),
	gsSPEndDisplayList(),
};

// intialize the RSP state:
Gfx F3SetupRSP_dl[] = {
//	gsSPViewport(&vp),
//	gsSPClipRatio(FRUSTRATIO_3),
	gsSPClipRatio(FRUSTRATIO_6),
	gsSPClearGeometryMode((G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH | G_CULL_BOTH | G_FOG | G_LIGHTING)),
	gsSPTexture(0, 0, 0, 0, G_OFF),
//	gsSPFogPosition(996, 1000),
	gsSPEndDisplayList(),
};

//////////////////////////////////////////////////////////////////////////////
// s2dex 描画設定ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄ
// a: anti
// b: bilinier
#if 0 // 削除
/*
Gfx prev_normal_ab_dl[] = {
	gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_COPY),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetRenderMode(G_RM_SPRITE, G_RM_SPRITE2),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetCombineMode(G_CC_DECALRGBA,G_CC_DECALRGBA),
	gsDPSetPrimColor(0,0,255,255,255,255),
	gsDPSetTextureFilter(G_TF_POINT),
	gsSPEndDisplayList(),
};
Gfx prev_normal_aB_dl[] = {
	gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_1CYCLE),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetRenderMode(G_RM_SPRITE, G_RM_SPRITE2),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetCombineMode(G_CC_DECALRGBA,G_CC_DECALRGBA),
	gsDPSetPrimColor(0,0,255,255,255,255),
	gsDPSetTextureFilter(G_TF_BILERP),
	gsSPEndDisplayList(),
};
Gfx prev_normal_Ab_dl[] = {
	gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_1CYCLE),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetRenderMode(G_RM_AA_SPRITE, G_RM_AA_SPRITE2),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetCombineMode(G_CC_DECALRGBA,G_CC_DECALRGBA),
	gsDPSetPrimColor(0,0,255,255,255,255),
	gsDPSetTextureFilter(G_TF_POINT),
	gsSPEndDisplayList(),
};
Gfx prev_normal_AB_dl[] = {
	gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_1CYCLE),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetRenderMode(G_RM_AA_SPRITE, G_RM_AA_SPRITE2),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetCombineMode(G_CC_DECALRGBA,G_CC_DECALRGBA),
	gsDPSetPrimColor(0,0,255,255,255,255),
	gsDPSetTextureFilter(G_TF_BILERP),
	gsSPEndDisplayList(),
};
Gfx prev_halftrans_ab_dl[] = {
	gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_1CYCLE),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetRenderMode(G_RM_XLU_SPRITE, G_RM_XLU_SPRITE2),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetCombineMode(G_CC_MODULATEIA_PRIM,G_CC_MODULATEIA_PRIM),
	gsDPSetPrimColor(0,0,255,255,255,255),
	gsDPSetTextureFilter(G_TF_POINT),
	gsSPEndDisplayList(),
};
Gfx prev_halftrans_aB_dl[] = {
	gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_1CYCLE),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetRenderMode(G_RM_XLU_SPRITE, G_RM_XLU_SPRITE2),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetCombineMode(G_CC_MODULATEIA_PRIM,G_CC_MODULATEIA_PRIM),
	gsDPSetPrimColor(0,0,255,255,255,255),
	gsDPSetTextureFilter(G_TF_BILERP),
	gsSPEndDisplayList(),
};
Gfx prev_halftrans_Ab_dl[] = {
	gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_1CYCLE),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetRenderMode(G_RM_AA_XLU_SPRITE, G_RM_AA_XLU_SPRITE2),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetCombineMode(G_CC_MODULATEIA_PRIM,G_CC_MODULATEIA_PRIM),
	gsDPSetPrimColor(0,0,255,255,255,255),
	gsDPSetTextureFilter(G_TF_POINT),
	gsSPEndDisplayList(),
};
Gfx prev_halftrans_AB_dl[] = {
	gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_1CYCLE),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetRenderMode(G_RM_AA_XLU_SPRITE, G_RM_AA_XLU_SPRITE2),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetCombineMode(G_CC_MODULATEIA_PRIM,G_CC_MODULATEIA_PRIM),
	gsDPSetPrimColor(0,0,255,255,255,255),
	gsDPSetTextureFilter(G_TF_BILERP),
	gsSPEndDisplayList(),
};
Gfx prev_halftrans_AB_no_prim_dl[] = {
	gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_1CYCLE),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetRenderMode(G_RM_AA_XLU_SPRITE, G_RM_AA_XLU_SPRITE2),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetCombineMode(G_CC_MODULATEIA_PRIM,G_CC_MODULATEIA_PRIM),
	gsDPSetTextureFilter(G_TF_BILERP),
	gsSPEndDisplayList(),
};

Gfx prev_mario_dl[] = {
	gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_1CYCLE),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetRenderMode(G_RM_SPRITE, G_RM_SPRITE2),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetCombineMode(G_CC_DECALRGBA,G_CC_DECALRGBA),
	gsDPSetPrimColor(0,0,255,255,255,255),
	gsDPSetTextureFilter(G_TF_POINT),
	gsSPEndDisplayList(),
};
*/
#endif
/*--------------------------------------
	create		:	1999/03/15
	modify		:	1999/10/18

	created		:	Hiroyuki Watanabe
--------------------------------------*/
/*
Gfx	ClearCfb[] = {
	gsDPPipeSync(),
	gsDPSetCycleType( G_CYC_FILL),
	gsDPSetFillColor((GPACK_RGBA5551(0, 0, 0, 1)<<16)|(GPACK_RGBA5551(0, 0, 0, 1))	),
	gsDPFillRectangle(0, 0, SCREEN_WD-1, SCREEN_HT-1),
	gsDPSetCycleType( G_CYC_1CYCLE),
	gsSPEndDisplayList()
};

Gfx	ClearZbuff[] = {
	gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_FILL),
	gsDPSetColorImage(G_IM_FMT_RGBA, G_IM_SIZ_16b,SCREEN_WD,OS_K0_TO_PHYSICAL(zbuffer)),
	gsDPSetFillColor((GPACK_RGBA5551(0, 0, 0, 1)<<16)|(GPACK_RGBA5551(0, 0, 0, 1))	),
	gsDPFillRectangle( 0, 0, SCREEN_WD-1, SCREEN_HT-1),
	gsDPSetCycleType( G_CYC_1CYCLE),
	gsDPSetDepthImage(OS_K0_TO_PHYSICAL(zbuffer)),
	gsSPEndDisplayList()
};
*/

//	描画設定用（半透明・Ｚなし）
Gfx Texture_NZ_dl[] = {
	gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_1CYCLE),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetRenderMode(G_RM_XLU_SPRITE, G_RM_XLU_SPRITE2),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetCombineMode(G_CC_MODULATEIA_PRIM,G_CC_MODULATEIA_PRIM),
	gsDPSetPrimColor(0,0,255,255,255,255),
	gsDPSetTextureFilter(G_TF_POINT),
	gsSPEndDisplayList(),
};
//	描画設定(半透明不可)
Gfx Texture_TE_dl[] = {
	gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_1CYCLE),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetRenderMode(G_RM_TEX_EDGE, G_RM_TEX_EDGE2),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetCombineMode(G_CC_MODULATEIA_PRIM,G_CC_MODULATEIA_PRIM),
	gsDPSetPrimColor(0,0,255,255,255,255),
	gsDPSetTextureFilter(G_TF_POINT),
	gsSPEndDisplayList(),
};

// 四角描画
void disp_rect(u8 r, u8 g, u8 b, u8 a,
	int x, int y, int width, int height)
{
	int x0, y0, x1, y1;

	gDPPipeSync(gp++);
	gDPSetPrimColor(gp++,  0, 0, r, g, b, a);
	gDPSetCombineLERP(gp++,
		0, 0, 0, PRIMITIVE, 0, 0, 0, PRIMITIVE,
		0, 0, 0, PRIMITIVE, 0, 0, 0, PRIMITIVE);

	x0 = x<<2;
	y0 = y<<2;
	x1 = (x + width)<<2;
	y1 = (y + height)<<2;

	gSPScisTextureRectangle ( gp++,
		x0, y0,				/* ｽｸﾘｰﾝ座標右上の座標 */
		x1, y1,				/* ｽｸﾘｰﾝ座標左下の座標 */
		G_TX_RENDERTILE,
		0, 0,				/* ﾃｸｽﾁｬUV座標 */
		1<<10, 1<<10);
}

// パレット読込み
void load_TexPal(void *pal)
{
	gDPLoadTLUT_pal256(gp++, pal);
}

// パレット読込み(16色16エントリ)
void load_TexPal_16(void *pal, int pal_no)
{
	gDPLoadTLUT_pal16(gp++, pal_no, pal);
}

// テクスチャ読込み16色
void load_TexBlock_4b(void *grp, int size_w, int size_h)
{
	gDPLoadTextureBlock_4b(gp++, grp,
		G_IM_FMT_CI, size_w, size_h, 0,
		G_TX_CLAMP,G_TX_CLAMP, G_TX_NOMASK,G_TX_NOMASK, G_TX_NOLOD,G_TX_NOLOD);
}

// テクスチャ読込みインテンシティ
void load_TexBlock_i(void *grp, int size_w, int size_h)
{
	gDPLoadTextureBlock_4b(gp++, grp,
		G_IM_FMT_I, size_w, size_h, 0,
		G_TX_CLAMP,G_TX_CLAMP, G_TX_NOMASK,G_TX_NOMASK, G_TX_NOLOD,G_TX_NOLOD);
}

// テクスチャ読込み16色
void load_TexTile_4b(void *grp, int size_w, int size_h, int start_w, int start_h, int end_w, int end_h)
{
	gDPLoadTextureTile_4b(gp++, grp,
		G_IM_FMT_CI,size_w,size_h,start_w,start_h,end_w,end_h,0,
		G_TX_CLAMP,G_TX_CLAMP, G_TX_NOMASK,G_TX_NOMASK, G_TX_NOLOD,G_TX_NOLOD);
}

// テクスチャ読込み256色
void load_TexBlock_8b(void *grp, int size_w, int size_h)
{
	gDPLoadTextureBlock(gp++, grp,
		G_IM_FMT_CI,G_IM_SIZ_8b,size_w,size_h,0,
		G_TX_CLAMP,G_TX_CLAMP, G_TX_NOMASK,G_TX_NOMASK, G_TX_NOLOD,G_TX_NOLOD);
}

// テクスチャ読込み256色
void load_TexTile_8b(void *grp, int size_w, int size_h, int start_w, int start_h, int end_w, int end_h)
{
	gDPLoadTextureTile(gp++, grp,
		G_IM_FMT_CI,G_IM_SIZ_8b,size_w,size_h,start_w,start_h,end_w,end_h,0,
		G_TX_CLAMP,G_TX_CLAMP, G_TX_NOMASK,G_TX_NOMASK, G_TX_NOLOD,G_TX_NOLOD);
}

// テクスチャ描画
void draw_Tex(int x_pos, int y_pos, int draw_w, int draw_h, int start_w, int start_h)
{
	gSPScisTextureRectangle(gp++,
		x_pos << 2,y_pos << 2,x_pos + draw_w << 2,y_pos + draw_h << 2,
		G_TX_RENDERTILE, start_w << 5, start_h << 5,1 << 10,1 << 10);
}

// テクスチャ描画( G_CYC_COPY用 )
void draw_Tex_Copy(int x_pos, int y_pos, int draw_w, int draw_h, int start_w, int start_h)
{
	gSPScisTextureRectangle(gp++,
		x_pos << 2,y_pos << 2,x_pos + draw_w << 2,y_pos + draw_h << 2,
		G_TX_RENDERTILE, start_w << 5, start_h << 5,4 << 10,4 << 10);
}

// テクスチャ描画(拡大・縮小あり)
void draw_ScaleTex(int x_pos, int y_pos, int size_w, int size_h, int draw_w, int draw_h, int start_w, int start_h)
{
	gSPScisTextureRectangle(gp++,
		x_pos << 2,y_pos << 2,x_pos + draw_w << 2,y_pos + draw_h << 2,
		G_TX_RENDERTILE, start_w << 5, start_h << 5, (size_w << 10) / draw_w,(size_h << 10) / draw_h);
}

// テクスチャ描画(反転あり)
void draw_TexFlip(int x_pos, int y_pos, int draw_w, int draw_h, int start_w, int start_h, int x_flip, int y_flip)
{
	int f_x, f_y;

	// 横反転
	f_x = f_y = 1 << 10;
	if( x_flip ){
		f_x = ((-1)<<10)&0xffff;
		start_w = draw_w - 1;
	}
	// 縦反転
	if( y_flip ){
		f_y = ((-1)<<10)&0xffff;
		start_h = draw_h - 1;
	}

	gSPScisTextureRectangle(gp++,
		x_pos << 2,y_pos << 2,x_pos + draw_w << 2,y_pos + draw_h << 2,
		G_TX_RENDERTILE, start_w << 5, start_h << 5,f_x,f_y);
}

// テクスチャ描画(拡縮・反転あり)
void draw_TexScaleFlip(int x_pos, int y_pos, int size_w, int size_h, int draw_w, int draw_h, int start_w, int start_h, int end_w, int end_h, int x_flip, int y_flip)
{
	int f_x, f_y;

	// 横反転
	f_x = (size_w << 10) / draw_w;
	f_y = (size_h << 10) / draw_h;
	if( x_flip ){
		f_x = ((f_x * -1) & 0xffff);
		start_w = end_w - 1;
	}
	// 縦反転
	if( y_flip ){
		f_y = ((f_y * -1) & 0xffff);
		start_h = end_h - 1;
	}

	gSPScisTextureRectangle(gp++,
		x_pos << 2,y_pos << 2,x_pos + draw_w << 2,y_pos + draw_h << 2,
		G_TX_RENDERTILE, start_w << 5, start_h << 5,f_x,f_y);
}

#if 0 // 削除
/*
// 背景を S2DEX で描画する場合の BG 設定関数
void S2d_ObjBg_InitTile(uObjBg *bg, void *txtr, int fmt, int siz, int w_size, int h_sise, int draw_w, int draw_h, int x_p, int y_p, int image_x, int imagw_y)
{
	bg -> b.imageX		= image_x << 5;
	bg -> b.imageW		= w_size << 2;
	bg -> b.frameX		= x_p << 2;
	bg -> b.frameW		= 320 << 2;
	bg -> b.imageY		= imagw_y << 5;
	bg -> b.imageH		= h_sise << 2;
	bg -> b.frameY		= y_p << 2;
	bg -> b.frameH		= 240 << 2;
	bg -> b.imagePtr	= (u64 *)txtr;
	bg -> b.imageLoad	= G_BGLT_LOADTILE;
	bg -> b.imageFmt	= fmt;
	bg -> b.imageSiz	= siz;
	bg -> b.imagePal	= 0;
	bg -> b.imageFlip	= 0;
	guS2DInitBg(bg);
	bg -> s.scaleW	 	= ( w_size << 10 ) / draw_w;
	bg -> s.scaleH	 	= ( h_sise << 10 ) / draw_h;
	bg -> s.imageYorig 	= imagw_y << 5;
}
*/
#endif

