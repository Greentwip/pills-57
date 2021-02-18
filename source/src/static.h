#ifndef _STATIC_H_
#define _STATIC_H_

extern Gfx S2RDPinit_dl[];		// 各種初期化 DL
extern Gfx S2ClearCFB_dl[];
extern Gfx S2Spriteinit_dl[];
extern Gfx F3SetupRSP_dl[];
extern Gfx F3RDPinit_dl[];
extern Gfx F3SetupRDP_dl[];

#if 0 // 削除
/*
extern Gfx prev_normal_ab_dl[];
extern Gfx prev_normal_aB_dl[];
extern Gfx prev_normal_Ab_dl[];
extern Gfx prev_normal_AB_dl[];
extern Gfx prev_halftrans_ab_dl[];
extern Gfx prev_halftrans_aB_dl[];
extern Gfx prev_halftrans_Ab_dl[];
extern Gfx prev_halftrans_AB_dl[];
extern Gfx prev_halftrans_AB_no_prim_dl[];
extern Gfx prev_mario_dl[];
*/
#endif

/*--------------------------------------
	create		:	1999/03/15
	modify		:	1999/10/29

	created		:	Hiroyuki Watanabe
--------------------------------------*/
#define PAI         3.141592654
#define DEGREE(a)   ((a) * PAI / 180)
#define flip_on     1
#define flip_off    0

extern Gfx ClearCfb[];	//	画面のクリア
extern Gfx ClearZbuff[];
extern Gfx Texture_NZ_dl[];
extern Gfx Texture_TE_dl[];	//	不透明
extern Gfx Intensity_XNZ_Texture_dl[];	//	インテンシティ使用の前準備

extern void disp_rect(u8,u8,u8,u8,int,int,int,int);	//	四角描画
extern void gfxRCPInit(void);
extern void gfxClearCfb(void);

extern void load_TexPal(void *);													//	パレット読込み
extern void load_TexPal_16(void *,int);											//	パレット読込み(16色16エントリ)
extern void load_TexBlock_4b(void *,int,int);										//	テクスチャ読込み16色
extern void load_TexTile_4b(void *,int,int,int,int,int,int);						//	テクスチャ読込み16色
extern void load_TexBlock_i(void *,int,int);										//	テクスチャ読込みインテンシティ
extern void load_TexBlock_8b(void *,int,int);										//	テクスチャ読込み256色
extern void load_TexTile_8b(void *,int,int,int,int,int,int);						//	テクスチャ読込み256色
extern void draw_Tex(int,int,int,int,int,int);									//	テクスチャ描画
extern void draw_Tex_Copy(int,int,int,int,int,int);								//	テクスチャ描画(G_CYC_COPY用)
extern void draw_ScaleTex(int,int,int,int,int,int,int,int);						//	テクスチャ描画(拡大・縮小あり)
extern void draw_TexFlip(int,int,int,int,int,int,int,int);						//	テクスチャ描画(反転あり)
extern void draw_TexScaleFlip(int,int,int,int,int,int,int,int,int,int,int,int);	//	テクスチャ描画(拡大・縮小・反転あり)

#if 0 // 削除
/*
extern void S2d_ObjBg_InitTile(uObjBg *, void *,int,int,int,int,int,int,int,int,int,int);			//	ObjBg型を設定する
*/
#endif

#endif
