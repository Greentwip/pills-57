/*--------------------------------------
	filename	:	dm_logo_action_main.c

	create		:	1999/07/05
	modify		:	1999/12/13

	created		:	Hiroyuki Watanabe
--------------------------------------*/
#define F2DEX_GBI
#include <ultra64.h>
#include <PR/ramrom.h>
#include <PR/gs2dex.h>
#include <assert.h>

#include "def.h"
#include "vram.h"
#include "segment.h"
#include "message.h"
#include "nnsched.h"
#include "main.h"
#include "graphic.h"
#include "joy.h"
#include "evsworks.h"
#include "static.h"
#include "sprite.h"

#include	"dm_nab_include.h"

/*--------------------------------------
	変数データ
--------------------------------------*/

//	2D描画設定DL
static	Gfx	init_2d_dl[] = {
	gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_1CYCLE),
	gsDPSetColorDither( G_CD_BAYER ),
	gsDPSetCombineKey( G_CK_NONE ),
	gsDPSetAlphaDither( G_AD_NOTPATTERN ),
	gsDPSetAlphaCompare( G_AC_NONE ),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetTextureLOD( G_TL_TILE ),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetTextureFilter(G_TF_BILERP),
	gsDPSetTextureConvert( G_TC_FILTCONV ),
	gsDPSetRenderMode(  G_RM_TEX_EDGE, G_RM_TEX_EDGE2),
	gsDPSetCombineMode(G_CC_MODULATEIA_PRIM,G_CC_MODULATEIA_PRIM),
	gsDPSetPrimColor(0,0,255,255,255,255),
	gsDPSetEnvColor( 255,255,255,255 ),
	gsSPObjRenderMode( 0x08 ),
	gsSPEndDisplayList(),
};

//	2D描画設定DL(インテンシティ)
static	Gfx	init_ia_2d_dl[] = {
	gsDPPipeSync(),
	gsDPSetTextureLUT ( G_TT_NONE ),
	gsDPSetTextureConvert(G_TC_FILT),
	gsDPSetRenderMode(G_RM_XLU_SPRITE, G_RM_XLU_SPRITE2),
	gsDPSetCombineLERP(  PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0,0,ENVIRONMENT,0
						,PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0,0,ENVIRONMENT,0),
	gsDPSetPrimColor(0,0,255,255,255,255),
	gsDPSetEnvColor(255,255,255,255),
	gsSPEndDisplayList(),

};


/*---------- GAME OVER ----------*/
#include	"game_over_action.dat"									//	GAME OVER の処理データ配列
static	s8	game_over_action_count_flg;								//	GAME OVER の内部制御変数
static	s8	game_over_action_count[2];								//	GAME OVER の内部制御変数
static	s8	game_over_action_flg;									//	GAME OVER の内部制御変数
static	s8	game_over_size[2][2];									//	GAME OVER の縦横のサイズ
static	s8	game_over_mul[2];										//	GAME OVER の拡縮の大きさ用変数
static	s16	game_over_position[2];									//	GAME OVER の座標
static	s16	game_over_sin;											//	GAME OVER の拡縮計算用変数
/*---------- GAME OVER ----------*/

/*---------- TRY NEXT STAGE ----------*/
static	s8	try_out_count;											//	TRY NEXT STAGE の内部制御用変数
static	s8	try_action_flg;											//	TRY NEXT STAGE の内部制御用変数
static	u8	try_no[] = {7,8,0,3,2,1,4,2,0,6,8,6,10,5,2,9,8 };		//	TRY NEXT STAGE の描画するグラフィックの番号
static	s16	try_position[17][4];									//	TRY NEXT STAGE の描画するグラフィックの座標
static	s16	init_try_position[17][3] = {							//	TRY NEXT STAGE の描画するグラフィックの初期化座標
	{36,-30,86},
	{51,-40,88},
	{60,-26,88},
	{70,-35,87},
	{83,-21,87},
	{35,-30,108},
	{49,-40,109},
	{59,-26,108},
	{71,-35,109},
	{83,-21,108},
	{47,-30,148},
	{60,-40,147},
	{73,-26,147},
	{39,-35,169},
	{54,-21,168},
	{67,-30,168},
	{83,-40,169}
};

static	u8	*try_bm[] = {											//	TRY NEXT STAGE の描画するグラフィックデータ
	test_a_bm0_0,
	test_c_bm0_0,
	test_e_bm0_0,
	test_g_bm0_0,
	test_l_bm0_0,
	test_n_bm0_0,
	test_r_bm0_0,
	test_s_bm0_0,
	test_t_bm0_0,
	test_x_bm0_0,
	test_y_bm0_0
};

static	u16	*try_bm_tlut[] = {										//	TRY NEXT STAGE の描画するパレットデータ
	test_a_bm0_0tlut,
	test_c_bm0_0tlut,
	test_e_bm0_0tlut,
	test_g_bm0_0tlut,
	test_l_bm0_0tlut,
	test_n_bm0_0tlut,
	test_r_bm0_0tlut,
	test_s_bm0_0tlut,
	test_t_bm0_0tlut,
	test_x_bm0_0tlut,
	test_y_bm0_0tlut
};
/*---------- TRY NEXT STAGE ----------*/

/*---------- WIN ----------*/
#include	"win2_action.dat"										//	WIN の処理データ配列
static	uObjMtx			s2d_win_mtx[4];								//	WIN	用マトリクス変数
static	uObjSprite		s2d_win_sp;									//	WIN 用スプライト設定変数
static	uObjTxtr		s2d_win_tex;								//	WIN	用テクスチャロード変数
static	uObjTxtr		s2d_win_pal;								//	WIN	用パレットロード変数
static	uObjSprite		s2d_win_ia_sp;								//	WIN	用スプライト設定変数(インテンシティ)
static	uObjTxtr		s2d_win_ia_tex;								//	WIN	用テクスチャロード変数(インテンシティ)
static	u8				win_mode[4];									//	WIN 用制御変数
static	u8				win_count[4];								//	WIN 用制御変数
static	s8				win_flg[4];									//	WIN 用制御変数
/*---------- WIN ----------*/

/*---------- LOSE ----------*/
#include	"lose2_action.dat"										//	LOSE の処理データ配列
static	uObjMtx			s2d_lose_mtx[4];							//	LOSE 用マトリクス変数
static	uObjSprite		s2d_lose_sp;								//	LOSE 用スプライト設定変数
static	uObjTxtr		s2d_lose_tex;								//	LOSE 用テクスチャロード変数
static	uObjTxtr		s2d_lose_pal;								//	LOSE 用パレットロード変数
static	uObjSprite		s2d_lose_ia_sp;								//	LOSE 用スプライト設定変数(インテンシティ)
static	uObjTxtr		s2d_lose_ia_tex;							//	LOSE 用テクスチャロード変数(インテンシティ)
static	u8				lose_count[4];								//	LOSE 用制御変数
static	s8				lose_flg[4];								//	LOSE 用制御変数
/*---------- LOSE ----------*/

/*---------- DRAW ----------*/
#include	"draw3_action.dat"										//	DRAW の処理データ配列
static	uObjMtx			s2d_draw_mtx[4];							//	DRAW 用マトリクス変数
static	uObjSprite		s2d_draw_sp;								//	DRAW 用スプライト設定変数
static	uObjTxtr		s2d_draw_tex;								//	DRAW 用テクスチャロード変数
static	uObjTxtr		s2d_draw_pal;								//	DRAW 用パレットロード変数
static	uObjSprite		s2d_draw_ia_sp;								//	DRAW 用スプライト設定変数(インテンシティ)
static	uObjTxtr		s2d_draw_ia_tex;							//	DRAW 用テクスチャロード変数(インテンシティ)
static	u8				draw_count[4];								//	DRAW 用制御変数
static	s8				draw_flg[4];								//	DRAW 用制御変数
/*---------- DRAW ----------*/

/*---------- 連鎖 ----------*/
static	uObjSprite		s2d_chaine_sp[4];							//	CHAINED 用スプライト設定変数
static	uObjTxtr		s2d_chaine_tex;								//	CHAINED 用テクスチャロード変数
static	uObjTxtr		s2d_chaine_pal;								//	CHAINED 用パレットロード変数

static	uObjSprite		s2d_chaine_num_sp;							//	連鎖数 用スプライト設定変数
static	uObjTxtr		s2d_chaine_num_pal;							//	連鎖数 用パレットロード変数
static	uObjTxtr		s2d_chaine_num_tex[4][4];					//	連鎖数 用テクスチャロード変数
static	uObjSubMtx		s2d_chaine_num_mtx[4][4];					//	連鎖数 用マトリクス変数

static	uObjSprite		s2d_chaine_mes_sp[4];						//	連鎖メッセージ	用スプライト設定変数
static	uObjTxtr		s2d_chaine_mes_tex[4];						//	連鎖メッセージ	用テクスチャロード変数
static	uObjTxtr		s2d_chaine_mes_pal[4];						//	連鎖メッセージ	用パレットロード変数

static	uObjSprite		s2d_chaine_cap_sp;							//	連鎖カプセル	用スプライト設定変数
static	uObjTxtr		s2d_chaine_cap_tex[3];						//	連鎖カプセル	用テクスチャロード変数
static	uObjTxtr		s2d_chaine_cap_pal[3];						//	連鎖カプセル	用パレットロード変数
static	uObjMtx			s2d_chaine_cap_mtx[4][3];					//	連鎖カプセル	用マトリクス変数

static	uObjSprite		s2d_chaine_player_sp;						//	連鎖プレイヤー(1P.2P.3P,4P)	用スプライト設定変数
static	uObjTxtr		s2d_chaine_player_tex[4][3];				//	連鎖プレイヤー(1P.2P.3P,4P)	用テクスチャロード変数
static	uObjTxtr		s2d_chaine_player_pal[4][3];				//	連鎖プレイヤー(1P.2P.3P,4P)	用パレットロード変数
static	uObjSubMtx		s2d_chaine_player_mtx[4][3];				//	連鎖プレイヤー(1P.2P.3P,4P)	用マトリクス変数

static	u8	test_chaine_mode[4];									//	連鎖数表示内部制御変数
static	u8	test_chaine_num[4];										//	現在の連鎖数保持変数
static	s8	test_chaine_max[4][4];									//	攻撃相手数と相手番号変数
static	u8	test_chaine_save[4];									//	前の連鎖数保持変数
static	u8	test_chaine_scis_w[4];									//	描画範囲(横幅)設定用変数
static	s8	test_chaine_num_w_size[4];								//	連鎖数用	横幅
static	s8	test_chaine_num_h_size[4];								//	連鎖数用	縦幅
static	s16	test_chaine_alpha[4];									//	連鎖数用	透明度
static	s16	test_chaine_center[4];									//	描画中心座標
static	s16	test_chaine_scis_x[4];									//	描画範囲指定の開始座標設定用変数
static	s16	test_chaine_cap_r[4][3];								//	カプセル回転用角度保持変数

/*---------- 連鎖 ----------*/

/*---------- PAUSE ----------*/

#include	"pause_action.dat"										//	PAUSE の処理データ配列
static	uObjSprite		s2d_pause_sp;								//	PAUSE 用スプライト設定変数
static	uObjTxtr		s2d_pause_tex;								//	PAUSE 用テクスチャロード変数
static	uObjTxtr		s2d_pause_pal;								//	PAUSE 用パレットロード変数
static	uObjSubMtx		s2d_pause_mtx[4];							//	PAUSE 用マトリクス変数
static	u8				pause_count[4];								//	PAUSE 用制御変数
static	u8				pause_anime_count[4];						//	PAUSE 用制御変数
static	s8				pause_mode[4];								//	PAUSE 用制御変数

/*---------- PAUSE ----------*/

/*---------- 4PLAY SHOCK ----------*/
static	s8	shock_mode[4];											//	4PLAY ショック用内部制御変数
static	s8	shock_size[4][2];										//	4PLAY ショック用表示する大きさ
static	s16	shock_position[4][2];									//	4PLAY ショック用座標
static	s16	shock_alpha[4];											//	4PLAY ショック用透明度
/*---------- 4PLAY SHOCK ----------*/

/*---------- 4PLAY HUMMING ----------*/
static	s8	humming_mode[4];										//	4PLAY ハミング用内部制御変数
static	s8	humming_size[4][2];										//	4PLAY ハミング用表示する大きさ
static	s16	humming_sin[4];											//	4PLAY ハミング用角度保持変数(揺れを表現するため)
static	s16	humming_position[4][3];									//	4PLAY ハミング用座標
static	s16	humming_alpha[4];										//	4PLAY ハミング用透明度
/*---------- 4PLAY HUMMING ----------*/


/*---------- 連鎖その２ ----------*/
	effect_bubble	dm_bubble[4];									//	連鎖後の処理

static	u16	*bubble_pal[] = {										//	バブル用パレットデータ
	dm_bubble_red_bm0_0tlut,										//	バブル赤
	dm_bubble_blue_bm0_0tlut,										//	バブル青
	dm_bubble_yellow_bm0_0tlut,										//	バブル黄
	dm_bubble_green_bm0_0tlut										//	バブル緑
};

static	u8	*bubble_graphic[] = {									//	バブル用グラフィックデータ
	dm_bubble_b_bm0_0,												//	バブル大
	dm_bubble_m_bm0_0,												//	バブル中
	dm_bubble_s_bm0_0,												//	バブル小
	dm_bubble_ss_bm0_0												//	バブル粒
};

/*---------- 連鎖その２ ----------*/

/*---------- PUSH ANY KEY ----------*/
static	s16	push_sin;												//	上下の揺れをつけるための変数
static	s8	push_pos_y;												//	Ｙ座標
/*---------- PUSH ANY KEY ----------*/

/*--------------------------------------
	S2DEX 用スプライト設定関数
--------------------------------------*/
void	set_sprite(uObjSprite *sp,s16 x_p,s16 y_p,s16 size_w,s16 size_h,u8 im_fmt,u8 im_siz,s16 wide )
{
	sp->s.objX = x_p << 2;
	sp->s.objY = y_p << 2;
	sp->s.imageW = size_w << 5;
	sp->s.imageH = size_h << 5;
	sp->s.scaleW = sp->s.scaleH = 1<<10;
	sp->s.paddingX = sp->s.paddingY = sp->s.imageFlags = 0;
	sp->s.imageStride = GS_PIX2TMEM(wide,im_siz);
	sp->s.imageAdrs = GS_PIX2TMEM(0,im_siz);
	sp->s.imageFmt = im_fmt;
	sp->s.imageSiz = im_siz;
	sp->s.imagePal = 0;
}

/*--------------------------------------
	S2DEX 用テクスチャロード設定関数
--------------------------------------*/
void	load_set_sprite(uObjTxtr *tex,u8 type,u8 *image,u8 im_fmt,u8 im_siz,s16	image_w,s16 image_h)
{
	tex->block.image	=	(u64 *)image;
	tex->block.tmem		=	GS_PIX2TMEM(0,im_siz);
	tex->block.sid		=	0;
	tex->block.mask		=	-1;
   	tex->block.flag		=	(u32)image;

	switch(type)
	{
	case	0:	//	block
		tex->block.type		=	G_OBJLT_TXTRBLOCK;
		tex->block.tsize	=	GS_TB_TSIZE(image_w * image_h,im_siz);
    	tex->block.tline	=	GS_TB_TLINE(image_w,im_siz);
		break;
	case	1:	//	tile
		tex->tile.type		=	G_OBJLT_TXTRTILE;
		tex->tile.twidth	=	GS_TT_TWIDTH(image_w,im_siz);
    	tex->tile.theight	=	GS_TT_THEIGHT(image_h,im_siz);
		break;
	}
}

/*--------------------------------------
	S2DEX 用パレットロード設定関数
--------------------------------------*/
void	load_set_pal(uObjTxtr *tex,u16 *image,s16 col)
{
	tex->tlut.type		=	G_OBJLT_TLUT;
	tex->tlut.image		=	(u64 *)image;
	tex->tlut.phead		=	GS_PAL_HEAD(0);
	tex->tlut.pnum		=	GS_PAL_NUM(col);
	tex->tlut.zero		=	tex->tlut.sid	=	0;
	tex->tlut.mask		=	-1;
	tex->tlut.flag		=	(u32)image;
}

/*--------------------------------------
	S2DEX 用回転可能マトリクス設定関数
--------------------------------------*/
void	set_sp_mtx(uObjMtx *mtx,s32 a,s32 b,s32 c,s32 d,s16 x,s16 y,s16 sc_w,s16 sc_h,s16 size_w,s16 size_h)
{

	mtx->m.A = ((a << 1) * sc_w ) / size_w;
	mtx->m.B = ((b << 1) * sc_h ) / size_h;
	mtx->m.C = ((c << 1) * sc_w ) / size_w;
	mtx->m.D = ((d << 1) * sc_h ) / size_h;

	mtx->m.X = x << 2;
	mtx->m.Y = y << 2;
	mtx->m.BaseScaleX = 1 << 10;
	mtx->m.BaseScaleY = 1 << 10;
}

/*--------------------------------------
	S2DEX 用回転不能マトリクス設定関数
--------------------------------------*/
void	set_sp_sub_mtx(uObjSubMtx *mtx,s16 x,s16 y,s16 sc_w,s16 sc_h,s16 size_w,s16 size_h)
{
	mtx->m.X = x << 2;
	mtx->m.Y = y << 2;
	mtx->m.BaseScaleX = (size_w << 10) / sc_w;
	mtx->m.BaseScaleY = (size_h << 10) / sc_h;
}



/*--------------------------------------
	TRY NEXT STAGE 初期化関数
--------------------------------------*/
void	init_try_next_stage(void)
{
	s8	i,j;

	try_out_count = 0;
	for( i = 0;i < 17;i++ ){
		try_position[i][3] = 0;
		for( j = 0;j < 3;j++ ){
			try_position[i][j] = init_try_position[i][j];	//	TRY NEXT STAGE の座標の初期化
		}
	}
}
/*--------------------------------------
	TRY NEXT STAGE 処理メイン関数
--------------------------------------*/
s8	try_next_stage_main(void)
{
	s8	i,j;

	try_out_count ++;
	for( i = j = 0;i < 17;i++ ){
		if( try_position[i][3] ){
			//	上下にゆれる処理
			j++;
			try_position[i][1] = try_position[i][2] + (5 * sinf(DEGREE(try_position[i][3])));
			try_position[i][3] += 6;
			if( try_position[i][3] >= 360 )
				try_position[i][3] = 1;
		}else{
			//	指定座標まで移動
			try_position[i][1] += 4;
			if( try_position[i][1] >= try_position[i][2] ){
				try_position[i][1] = try_position[i][2];
				try_position[i][3] = 1;
			}
		}
	}
	if( try_out_count >= dm_demo_count ){
		try_out_count = dm_demo_count;
		return	1;	//	キー受付開始
	}else	{
		return	-1;	//	まだ処理中
	}
}
/*--------------------------------------
	TRY NEXT STAGE 描画メイン関数
--------------------------------------*/
void	try_next_stage_draw(s8	flg)
{
	s16	i;
	s16	puls;
	s16	x_pos;

	if( flg == 0 ){	//	1P位置(現在未使用)
		puls = 0;
	}else	if( flg == 1 ){	//	2P位置(現在未使用)
		puls = 184;
	}else	if( flg == 2 ){	//	中心位置
		puls = 90;
	}

	//	描画( グラフィックのサイズは共通 )
	for( i = 16;i >= 0;i-- ){
		gDPLoadTLUT_pal256(gp++,try_bm_tlut[try_no[i]]);
		gDPLoadTextureTile_4b(gp++,try_bm[try_no[i]], G_IM_FMT_CI,24,22,0,0,23,21,0, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK,G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
		x_pos = try_position[i][0] + puls;
		gSPScisTextureRectangle(gp++,x_pos << 2,try_position[i][1] << 2,x_pos + 24 << 2,
										try_position[i][1] + 22 << 2,G_TX_RENDERTILE, 0, 0,1<<10,1 << 10);

	}

}

/*--------------------------------------
	GAME OVER 初期化関数
--------------------------------------*/
void	init_game_over(void)
{
	game_over_action_count[0] = game_over_action_count[1] = 0;				//	内部制御フラグの初期化
	game_over_action_flg = 0;												//	内部制御フラグの初期化

	game_over_size[0][0] = game_w_scale_table[0];							//	gameの初期横幅
	game_over_size[0][1] = game_h_scale_table[0];							//	gameの初期縦幅
	game_over_size[1][0] = over_w_scale_table[0];							//	overの初期横幅
	game_over_size[1][1] = over_h_scale_table[0];							//	overの初期横幅
	game_over_position[0] = game_position_table[0];							//	gameの初期Ｙ座標
	game_over_position[1] = over_position_table[0];							//	overの初期Ｙ座標

}

/*--------------------------------------
	GAME OVER 処理メイン関数
--------------------------------------*/
s8	game_over_main(void)
{
	switch(game_over_action_flg)
	{
	case	0:	//	移動

		game_over_position[0] = game_position_table[game_over_action_count[0]];	//	GAME のＹ座標
		game_over_position[1] = over_position_table[game_over_action_count[1]];	//	OVER のＹ座標

		game_over_size[0][0] = game_w_scale_table[game_over_action_count[0]];	//	GAME の横幅
		game_over_size[0][1] = game_h_scale_table[game_over_action_count[0]];	//	GAME の縦幅
		game_over_size[1][0] = over_w_scale_table[game_over_action_count[1]];	//	OVER の横幅
		game_over_size[1][1] = over_h_scale_table[game_over_action_count[1]];	//	OVER の縦幅
		if( game_over_action_count_flg >= 1 ){
			game_over_action_count_flg = 0;
			game_over_action_count[0] ++;
			game_over_action_count[1] ++;
		}else{
			game_over_action_count_flg ++;
		}

		if( game_over_action_count[0] >= 40 ){
			game_over_action_flg = 1;
		}
		if( game_over_action_count[0] >= dm_demo_count ){
			return	1;	//	キー受付開始
		}else	{
			return	-1;	//	まだ処理中
		}
	case	1:	//	拡大
		game_over_position[0] = game_position_table[game_over_action_count[0]];	//	GAME のＹ座標
		game_over_position[1] = over_position_table[game_over_action_count[1]];	//	OVER のＹ座標

		game_over_size[0][0] = game_w_scale_table[game_over_action_count[0]];	//	GAME の横幅
		game_over_size[0][1] = game_h_scale_table[game_over_action_count[0]];	//	GAME の縦幅
		game_over_size[1][0] = over_w_scale_table[game_over_action_count[1]];	//	OVER の横幅
		game_over_size[1][1] = over_h_scale_table[game_over_action_count[1]];	//	OVER の縦幅

		if( game_over_action_count_flg >= 1 ){
			game_over_action_count_flg = 0;
			game_over_action_count[0] ++;
			game_over_action_count[1] ++;

			if( game_over_action_count[0] > 52 ){
				game_over_action_count[0] = 40;
			}

			if( game_over_action_count[1] > 52 ){
				game_over_action_count[1] = 40;
			}
		}else{
			game_over_action_count_flg ++;
		}

		return	1;
	}
}
/*--------------------------------------
	GAME OVER 描画メイン関数
--------------------------------------*/
void	game_over_draw(s16 x_pos)
{
	s8	i;

	//	game描画
	gDPLoadTLUT_pal256(gp++,test_game_bm0_0tlut);
	gDPLoadTextureTile_4b(gp++,&test_game_bm0_0[0], G_IM_FMT_CI,72,23,0,0,71,22,0, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK,G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
	gSPScisTextureRectangle(gp++,x_pos - (game_over_size[0][0] >> 1)<< 2,game_over_position[0] << 2,x_pos + (game_over_size[0][0] >> 1) << 2,
									game_over_position[0] + game_over_size[0][1] << 2,G_TX_RENDERTILE, 0, 0,  (72 << 10) / game_over_size[0][0], (23 << 10) / game_over_size[0][1]);

	//	over描画
	gDPLoadTLUT_pal256(gp++,test_over_bm0_0tlut);
	gDPLoadTextureTile_4b(gp++,&test_over_bm0_0[0], G_IM_FMT_CI,72,23,0,0,71,22,0, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK,G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
	gSPScisTextureRectangle(gp++,x_pos - (game_over_size[1][0] >> 1)<< 2,game_over_position[1] << 2,x_pos + (game_over_size[1][0] >> 1) << 2,
									game_over_position[1] + game_over_size[1][1] << 2,G_TX_RENDERTILE, 0, 0,  (72 << 10) / game_over_size[1][0], (23 << 10) / game_over_size[1][1]);

}
/*--------------------------------------
	WIN 初期化関数
--------------------------------------*/
void	init_win(void)
{
	s8	i;

	for( i = 0;i < 4;i++ ){
		win_mode[i] = 0;
		win_count[i] = 0;
		win_flg[i] = -1;
		set_sp_mtx(&s2d_win_mtx[i],1,0,0,1,win2_position_x_table[win_count[i]],win2_position_y_table[win_count[i]] + 110,48,32,48,32);	//	初期マトリクスの設定
	}
	set_sprite(&s2d_win_sp,-32,-16,64,32,G_IM_FMT_CI,G_IM_SIZ_4b,64);						//	WIN 用スプライトの設定
	load_set_sprite(&s2d_win_tex,0,test_win_bm0_0,G_IM_FMT_CI,G_IM_SIZ_4b,64,32);			//	WIN	用テクスチャロードの設定
	load_set_pal(&s2d_win_pal,test_win_bm0_0tlut,16);										//	WIN	用パレットロードの設定
	set_sprite(&s2d_win_ia_sp,-24,-16,48,32,G_IM_FMT_I,G_IM_SIZ_4b,48);						//	WIN 用スプライトの設定(インテンシティ)
	load_set_sprite(&s2d_win_ia_tex,0,test_win_i_bm0_0,G_IM_FMT_I,G_IM_SIZ_4b,48,32);		//	WIN	用テクスチャロードの設定(インテンシティ)
}

/*--------------------------------------
	WIN 処理メイン関数
--------------------------------------*/
s8	win_main(s16 x_pos,s8 mtx_no)
{
	s32	Cos,Sin;

	//	マトリクス計算
	Cos = (s32)coss( (win2_rotate_z_table[win_count[mtx_no]] * 0xffff / 360) );
	Sin = (s32)sins( (win2_rotate_z_table[win_count[mtx_no]] * 0xffff / 360) );
	set_sp_mtx(&s2d_win_mtx[mtx_no],Cos,Sin,-Sin,Cos,win2_position_x_table[win_count[mtx_no]] + x_pos,win2_position_y_table[win_count[mtx_no]] + 110,
					win2_w_scale_table[win_count[mtx_no]] * 48,win2_h_scale_table[win_count[mtx_no]] * 32,48,32);

	win_count[mtx_no] ++;

	if( win_count[mtx_no] >= dm_demo_count ){
		win_flg[mtx_no] = 1;	//	キー受付開始
	}

	if(win_count[mtx_no] > 74)
	{
		win_count[mtx_no] = 26;
	}
	return	win_flg[mtx_no];
}

/*--------------------------------------
	WIN 描画メイン関数
--------------------------------------*/
void	win_draw(s8 mtx_no)
{
	gSPDisplayList( gp++, init_2d_dl );			//	2D描画設定
	gSPObjMatrix( gp++, &s2d_win_mtx[mtx_no] );	//	マトリクスロード

	//	テクスチャ描画
	gSPObjLoadTxtr( gp++,&s2d_win_pal );	//	パレットロード
	gSPObjLoadTxtr( gp++,&s2d_win_tex );	//	テクスチャロード
	gSPObjSprite( gp++,&s2d_win_sp );		//	描画
}

/*--------------------------------------
	LOSE 初期化関数
--------------------------------------*/
void	init_lose(void)
{
	s8	i;

	set_sprite(&s2d_lose_sp,-32,-16,64,32,G_IM_FMT_CI,G_IM_SIZ_4b,64);					//	LOSE 用スプライトの設定
	load_set_sprite(&s2d_lose_tex,0,test_lose_bm0_0,G_IM_FMT_CI,G_IM_SIZ_4b,64,32);		//	LSOE 用テクスチャロードの設定
	load_set_pal(&s2d_lose_pal,test_lose_bm0_0tlut,16);									//	LSOE 用パレットロードの設定

	set_sprite(&s2d_lose_ia_sp,-24,-15,48,30,G_IM_FMT_I,G_IM_SIZ_4b,48);				//	LOSE 用スプライトの設定(インテンシティ)
	load_set_sprite(&s2d_lose_ia_tex,0,test_lose_i_bm0_0,G_IM_FMT_I,G_IM_SIZ_4b,48,30);	//	LSOE 用テクスチャロードの設定(インテンシティ)


	for(i = 0;i < 4;i++){
		lose_count[i] = 0;
		lose_flg[i] = -1;
		set_sp_mtx(&s2d_lose_mtx[i],1,0,0,1,lose2_x_position_table[lose_count[i]],lose2_y_position_table[lose_count[i]] + 110,48,30,48,30);	// 初期マトリクスの作成
	}
}

/*--------------------------------------
	LOSE 処理メイン関数
--------------------------------------*/
s8	lose_main(s8 mtx_no,s16 x_pos)
{
	s32	Cos,Sin;

	Cos = (s32)coss( (lose2_rotate_z_table[lose_count[mtx_no]] * 0xffff / 360) );
	Sin = (s32)sins( (lose2_rotate_z_table[lose_count[mtx_no]] * 0xffff / 360) );
	set_sp_mtx(&s2d_lose_mtx[mtx_no],Cos,Sin,-Sin,Cos,lose2_x_position_table[lose_count[mtx_no]] + x_pos,(-1 * lose2_y_position_table[lose_count[mtx_no]]) + 110,
					lose2_w_scale_table[lose_count[mtx_no]] * 48,lose2_h_scale_table[lose_count[mtx_no]] * 30,48,30);

	lose_count[mtx_no] ++;
	if(lose_count[mtx_no] >= dm_demo_count){
		lose_flg[mtx_no] = 1;
	}

	if(lose_count[mtx_no] > 169)
	{
		lose_count[mtx_no] = 26;
	}

	return	lose_flg[mtx_no];
}

/*--------------------------------------
	LOSE 描画メイン関数
--------------------------------------*/
void	lose_draw(s8 mtx_no)
{
	gSPDisplayList( gp++, init_2d_dl );				//	2D描画設定
	gSPObjMatrix( gp++,	&s2d_lose_mtx[mtx_no] );	//	マトリクスロード

	//	テクスチャ描画
	gSPObjLoadTxtr( gp++,&s2d_lose_pal );			//	パレットロード
	gSPObjLoadTxtr( gp++,&s2d_lose_tex );			//	テクスチャロード
	gSPObjSprite( gp++,&s2d_lose_sp );				//	描画
}

/*--------------------------------------
	DRAW 初期化関数
--------------------------------------*/
void	init_draw(void)
{
	s8	i;

	set_sprite(&s2d_draw_sp,-32,-16,64,32,G_IM_FMT_CI,G_IM_SIZ_4b,64);					//	DRAW 用スプライトの設定
	load_set_sprite(&s2d_draw_tex,0,test_draw_bm0_0,G_IM_FMT_CI,G_IM_SIZ_4b,64,32);		//	DRAW 用テクスチャロードの設定

	load_set_pal(&s2d_draw_pal,test_draw_bm0_0tlut,16);									//	DRAW 用パレットロードの設定

	set_sprite(&s2d_draw_ia_sp,-24,-14,48,28,G_IM_FMT_I,G_IM_SIZ_4b,48);				//	DRAW 用スプライトの設定(インテンシティ)
	load_set_sprite(&s2d_draw_ia_tex,0,test_draw_i_bm0_0,G_IM_FMT_I,G_IM_SIZ_4b,48,30);	//	DRAW 用テクスチャロードの設定(インテンシティ)


	for(i = 0;i < 4;i++){
		draw_count[i] = 0;
		draw_flg[i] = -1;
		set_sp_mtx(&s2d_draw_mtx[i],1,0,0,1,draw3_x_position_table[draw_count[i]],0,48,28,48,28);	// 初期マトリクスの作成
	}
}
/*--------------------------------------
	DRAW 処理メイン関数
--------------------------------------*/
s8	draw_main(s8 mtx_no,s16 x_pos)
{
	s32	Cos,Sin;

	Cos = (s32)coss( (draw3_rotate_z_table[draw_count[mtx_no]] * 0xffff / 360) );
	Sin = (s32)sins( (draw3_rotate_z_table[draw_count[mtx_no]] * 0xffff / 360) );
	set_sp_mtx(&s2d_draw_mtx[mtx_no],Cos,Sin,-Sin,Cos,draw3_x_position_table[draw_count[mtx_no]] + x_pos,110,
					draw3_w_scale_table[draw_count[mtx_no]] * 48,draw3_h_scale_table[draw_count[mtx_no]] * 28,48,28);

	draw_count[mtx_no] ++;
	if(draw_count[mtx_no] >= dm_demo_count){
		draw_flg[mtx_no] = 1;
	}
	if(draw_count[mtx_no] > 68)
	{
		draw_count[mtx_no] = 28;
	}

	return	draw_flg[mtx_no];
}

/*--------------------------------------
	DRAW 描画メイン関数
--------------------------------------*/
void	draw_draw(s8 mtx_no)
{
	gSPDisplayList( gp++, init_2d_dl );				//	2D描画設定
	gSPObjMatrix( gp++,	&s2d_draw_mtx[mtx_no] );	//	マトリクスロード


	//	テクスチャ描画
	gSPObjLoadTxtr( gp++,&s2d_draw_pal );			//	パレットロード
	gSPObjLoadTxtr( gp++,&s2d_draw_tex );			//	テクスチャロード
	gSPObjSprite( gp++,&s2d_draw_sp );				//	描画
}


/*--------------------------------------
	連鎖 初期化関数
--------------------------------------*/
void	init_chaine(void)
{
	s8	i,j;

	u16	*cap_pal[] = {
		test_capsel_blue_bm0_0tlut,
		test_capsel_yellow_bm0_0tlut,
		test_capsel_red_bm0_0tlut,
	};

	u16	*p_icon_pal[] = {
		dm_4p_game_target_1p_bm0_0tlut,
		dm_4p_game_target_2p_bm0_0tlut,
		dm_4p_game_target_3p_bm0_0tlut,
		dm_4p_game_target_4p_bm0_0tlut
	};

	u8	*p_icon_bm[] = {
		dm_4p_game_target_1p_bm0_0,
		dm_4p_game_target_2p_bm0_0,
		dm_4p_game_target_3p_bm0_0,
		dm_4p_game_target_4p_bm0_0
	};

	u8	*cap_bm[] = {
		test_capsel_blue_bm0_0,
		test_capsel_yellow_bm0_0,
		test_capsel_red_bm0_0,
	};

	u8	set_table[4][3] = {
		{1,2,3},{2,3,0},{3,0,1},{0,1,2}
	};

	//	連鎖表示関係の初期化
	load_set_sprite(&s2d_chaine_tex,0,test_chained_bm0_0,G_IM_FMT_CI,G_IM_SIZ_4b,160,20);
	load_set_pal(&s2d_chaine_pal,test_chained_bm0_0tlut,16);

	//	数字
	set_sprite(&s2d_chaine_num_sp,-16,-16,32,32,G_IM_FMT_CI,G_IM_SIZ_4b,32);														//	スプライト設定
	load_set_pal(&s2d_chaine_num_pal,test_chained_num_bm0_0tlut,16);																//	読込みパレット設定

	//	カプセル
	set_sprite(&s2d_chaine_cap_sp,-8,-8,16,16,G_IM_FMT_CI,G_IM_SIZ_4b,16);															//	スプライト設定
	for(i = 0;i < 3;i++){
		load_set_sprite(&s2d_chaine_cap_tex[i],0,cap_bm[i],G_IM_FMT_CI,G_IM_SIZ_4b,16,16);													//	読込みテクスチャ設定
		load_set_pal(&s2d_chaine_cap_pal[i],cap_pal[i],16);																			//	読込みパレット設定
	}

	//	プレイヤーマーク
	set_sprite(&s2d_chaine_player_sp,0,0,32,16,G_IM_FMT_CI,G_IM_SIZ_4b,32);															//	スプライト設定
	for(i = 0;i < 4;i++){
		test_chaine_mode[i] = 0;
		for(j = 0;j < 3;j++){
			load_set_sprite(&s2d_chaine_player_tex[i][j],0,p_icon_bm[set_table[i][j]],G_IM_FMT_CI,G_IM_SIZ_4b,32,16);				//	読込みテクスチャ設定
			load_set_pal(&s2d_chaine_player_pal[i][j],p_icon_pal[set_table[i][j]],16);												//	読込みパレット設定
		}
	}
}
/*--------------------------------------
	連鎖 強制表示停止関数
--------------------------------------*/
void	stop_chain_main(u8 no){
	test_chaine_mode[no] = 0;
}

/*--------------------------------------
	連鎖 処理メイン関数
--------------------------------------*/
void	chain_main(game_state *state,u8 player_no,u8 chaine,u8 mode,s16 center)
{
	s8	size_w[] = {80,64};							//	描画範囲の横幅
	s8	chaine_w[] = {70,60};						//	縮小率（スケール値）
	s8	num_max_w[][2] = {{48,36},{42,36}};			//	数字拡大時の最大の大きさ
	s8	num_center[][3] = {{24,18,30},{20,16,24}};	//	数字の拡大中心座標を求める数値
	s16	i,j,x,x2,y;
	s32	Cos,Sin;

	switch( test_chaine_mode[player_no] )
	{
	case	0:	//	初期設定
		if( chaine > 1 ){


			test_chaine_mode[player_no] = 1;								//	次の処理に
			test_chaine_num[player_no]	= chaine;							//	連鎖数のセット
			test_chaine_alpha[player_no] = 0xff;							//	透明度
			test_chaine_center[player_no] = center;						//	瓶の中心位置の保存
			test_chaine_scis_x[player_no] = center - (size_w[mode]	>> 1);	//	描画範囲開始Ｘ座標の設定
			test_chaine_scis_w[player_no] = size_w[mode];					//	描画範囲横幅の設定

			//	CHAINED
			set_sprite(&s2d_chaine_sp[player_no],test_chaine_center[player_no] - 10,104,160,20,G_IM_FMT_CI,G_IM_SIZ_4b,160);	//	スプライト設定
			s2d_chaine_sp[player_no].s.scaleW = (160 << 10) / chaine_w[mode];
			s2d_chaine_sp[player_no].s.scaleH = 2048;

			//	連鎖数
			load_set_sprite(&s2d_chaine_num_tex[player_no][0],0,&test_chained_num_bm0_0[512 * test_chaine_num[player_no]],G_IM_FMT_CI,G_IM_SIZ_4b,32,32);	//	読込みテクスチャ設定
			set_sp_sub_mtx(&s2d_chaine_num_mtx[player_no][0],test_chaine_center[player_no] - num_center[mode][0],108,14,10,32,32);		//	マトリクス設定

			for( i = 0;i < 4;i++ ){
				test_chaine_max[player_no][i] = 0;
			}

			for( i = 2,x = j = 0 ;i >= 0;i-- ){
				if( state -> chain_color[3] & ( 1 << i ) ){
					test_chaine_max[player_no][x] = 1;	//	表示するフラグ
					j ++;
				}
				x++;
			}
			test_chaine_max[player_no][3] = j; //	攻撃相手の数
		}
		break;
	case	1:	//	連鎖数比較
		if( chaine == 0 ){
			//	連鎖終了
			test_chaine_mode[player_no] = 99;	//	確定処理(NICE.FINE.BEOUTIFUL表示)に

			//	スプライトの設定
			set_sprite(&s2d_chaine_mes_sp[player_no],test_chaine_center[player_no] + (size_w[mode]	>> 1),120,160,20,G_IM_FMT_CI,G_IM_SIZ_4b,160);				//	スプライト設定
			s2d_chaine_mes_sp[player_no].s.scaleW = (160 << 10) / chaine_w[mode];
			s2d_chaine_mes_sp[player_no].s.scaleH = 2048;

			//	連鎖数を元に表示する文字を選ぶ
			switch( test_chaine_num[player_no] )
			{
			case	2:	//	Fine
				load_set_sprite(&s2d_chaine_mes_tex[player_no],0,&test_fine_bm0_0[0],G_IM_FMT_CI,G_IM_SIZ_4b,160,20);						//	読込みテクスチャ設定
				load_set_pal(&s2d_chaine_mes_pal[player_no],test_fine_bm0_0tlut,16);														//	読込みパレット設定
				break;
			case	3:	//	Nice
				load_set_sprite(&s2d_chaine_mes_tex[player_no],0 ,&test_nice_bm0_0[0],G_IM_FMT_CI,G_IM_SIZ_4b,160,20);						//	読込みテクスチャ設定
				load_set_pal(&s2d_chaine_mes_pal[player_no],test_nice_bm0_0tlut,16);														//	読込みパレット設定
				break;
			default:	//	Beoutiful
				load_set_sprite(&s2d_chaine_mes_tex[player_no],0,&test_beoutiful_bm0_0[0],G_IM_FMT_CI,G_IM_SIZ_4b,160,20);					//	読込みテクスチャ設定
				load_set_pal(&s2d_chaine_mes_pal[player_no],test_beoutiful_bm0_0tlut,16);													//	読込みパレット設定
				break;
			}
			if( mode ){
				//	4Pモードの場合
				Cos = (s32)coss( (180 * 0xffff / 360)  );
				Sin = (s32)sins( (180 * 0xffff / 360)  );
				x = test_chaine_center[player_no] + 40;
				y = 140;
				//	飛んでくる 1P.2P.3P.4Pとカプセルの設定
				for( i = j = 0;i < 3;i++ ){
					test_chaine_cap_r[player_no][i] = 180;
					set_sp_sub_mtx(&s2d_chaine_player_mtx[player_no][i],x + 10,y - 8,20,12,32,16);		//	移動だけのマトリクス設定
					set_sp_mtx(&s2d_chaine_cap_mtx[player_no][i],Cos,Sin,-Sin,Cos,x,y,12,14,16,16);		//	回転のマトリクス設定
					y += 14;
					x += 10;
				}
			}
		}else{
			if( test_chaine_num[player_no] != chaine ){	//	連鎖が増えている
				//	連鎖数(半透明拡大用)

				test_chaine_save[player_no] = chaine;	//	拡大前の連鎖数保存
				test_chaine_mode[player_no] = 10;		//	拡大処理に

				test_chaine_num_w_size[player_no] = 14;	//	初期連鎖数横幅の設定
				test_chaine_num_h_size[player_no] = 10;	//	初期連鎖数縦幅の設定

				load_set_sprite(&s2d_chaine_num_tex[player_no][3],0,&test_chained_num_bm0_0[512 * (test_chaine_num[player_no] % 10)],G_IM_FMT_CI,G_IM_SIZ_4b,32,32);	//	読込みテクスチャ設定

				if(test_chaine_num[player_no] < 10){
					//	連鎖数が１桁
					set_sp_sub_mtx(&s2d_chaine_num_mtx[player_no][3],test_chaine_center[player_no] - 24,108,14,10,32,32);				//	マトリクス設定

				}else	if(test_chaine_num[player_no] > 9){
					//	連鎖数が２桁
					load_set_sprite(&s2d_chaine_num_tex[player_no][2],0,&test_chained_num_bm0_0[512 * (test_chaine_num[player_no] / 10)],G_IM_FMT_CI,G_IM_SIZ_4b,32,32);	//	読込みテクスチャ設定
					set_sp_sub_mtx(&s2d_chaine_num_mtx[player_no][3],test_chaine_center[player_no] - 18,108,14,10,32,32);				//	マトリクス設定
					set_sp_sub_mtx(&s2d_chaine_num_mtx[player_no][2],test_chaine_center[player_no] - 30,108,14,10,32,32);				//	マトリクス設定

				}
			}
		}
		break;
	case	10:	//	数字拡大処理

		i = 2;	//	拡大速度
		if( test_chaine_save[player_no] > 4 ){
			j = num_max_w[mode][0];	//	拡大の最大の大きさ
		}else{
			j = num_max_w[mode][1];	//	拡大の最大の大きさ
		}

		test_chaine_num_w_size[player_no] += i;	//	横幅増加
		test_chaine_num_h_size[player_no] += i;	//	縦幅増加

		//	縦幅が最大値に達した場合
		if( test_chaine_num_h_size[player_no] >= j )
			test_chaine_num_h_size[player_no] = j;	//	縦幅を最大値にする

		//	横幅が最大値に達した場合
		if(test_chaine_num_w_size[player_no] >= j)
		{
			test_chaine_mode[player_no] = 1;		//	連鎖待ちに
			test_chaine_num[player_no] = test_chaine_save[player_no];	//	連鎖数の増加
			load_set_sprite(&s2d_chaine_num_tex[player_no][0],0,&test_chained_num_bm0_0[512 * (test_chaine_num[player_no] % 10)],G_IM_FMT_CI,G_IM_SIZ_4b,32,32);	//	読込みテクスチャ設定
			if( test_chaine_num[player_no] < 4){
				//	連鎖数が１桁で３以下
				set_sp_sub_mtx(&s2d_chaine_num_mtx[player_no][0],test_chaine_center[player_no] - num_center[mode][0],108,14,10,32,32);					//	マトリクス設定
			}else{
				if( test_chaine_num[player_no] < 10){
					//	連鎖数が１桁で４以上
					set_sp_sub_mtx(&s2d_chaine_num_mtx[player_no][0],test_chaine_center[player_no] - num_center[mode][0],108,16,16,32,32);				//	マトリクス設定
				}else	if( test_chaine_num[player_no] > 9){
					//	連鎖数が２桁
					load_set_sprite(&s2d_chaine_num_tex[player_no][1],0,&test_chained_num_bm0_0[512 * (test_chaine_num[player_no] / 10)],G_IM_FMT_CI,G_IM_SIZ_4b,32,32);	//	読込みテクスチャ設定
					set_sp_sub_mtx(&s2d_chaine_num_mtx[player_no][0],test_chaine_center[player_no] - num_center[mode][1],108,16,16,32,32);				//	マトリクス設定
					set_sp_sub_mtx(&s2d_chaine_num_mtx[player_no][1],test_chaine_center[player_no] - num_center[mode][2],108,16,16,32,32);				//	マトリクス設定
				}
			}
		}else{
			if( test_chaine_num[player_no] < 10){
				set_sp_sub_mtx(&s2d_chaine_num_mtx[player_no][3],test_chaine_center[player_no] - num_center[mode][0],108,test_chaine_num_w_size[player_no],test_chaine_num_h_size[player_no],32,32);				//	マトリクス設定
			}else	if( test_chaine_num[player_no] > 9){
				set_sp_sub_mtx(&s2d_chaine_num_mtx[player_no][3],test_chaine_center[player_no] - num_center[mode][1],108,test_chaine_num_w_size[player_no],test_chaine_num_h_size[player_no],32,32);				//	マトリクス設定
				set_sp_sub_mtx(&s2d_chaine_num_mtx[player_no][2],test_chaine_center[player_no] - num_center[mode][2],108,test_chaine_num_w_size[player_no],test_chaine_num_h_size[player_no],32,32);				//	マトリクス設定
			}
		}
		break;
	case	99:	//	FINE NICE BEOUTIFUL の横移動
		x = s2d_chaine_mes_sp[player_no].s.objX >> 2;
		x -= 4;
		//	文字によって最終位置が違う
		if( test_chaine_num[player_no] < 4 ){
			j = test_chaine_center[player_no] - 8;		//	FINE
		}else{
			j = test_chaine_center[player_no] - 32 + (mode << 2);	//	BEOUTIFUL
		}

		//	最終位置に達した場合
		if( x <= j ){
			x = j ;
			if(mode){
				test_chaine_mode[player_no] = 100;		//	カプセルスクロールイン処理に
			}else{
				test_chaine_mode[player_no] = 101;		//	フェードアウト処理に
			}
		}
		s2d_chaine_mes_sp[player_no].s.objX = x << 2;
		break;
	case	100:	//	カプセルスクロールイン
		j = test_chaine_center[player_no] - 4;	//	カプセルの最終移動地点
		for(i = 0;i < test_chaine_max[player_no][3];i++){
			test_chaine_cap_r[player_no][i] += 30;				//	回転角度を30度足す
			x = s2d_chaine_cap_mtx[player_no][i].m.X >> 2;		//	現在のＸ座標
			y = s2d_chaine_cap_mtx[player_no][i].m.Y >> 2;		//	現在のＹ座標

			x -= 2;	//	Ｘ座標の移動
			if( x <= j ){
				x = j;	//	最終移動地点に到達
				test_chaine_cap_r[player_no][i] = 0;				//	回転角度を０に戻す

				if( i == test_chaine_max[player_no][3] - 1){
					//	カプセルが最終移動地点に到達した場合
					test_chaine_mode[player_no] = 101;		//	フェードアウト処理に
				}
			}
			Cos = (s32)coss( (test_chaine_cap_r[player_no][i] * 0xffff / 360)  );
			Sin = (s32)sins( (test_chaine_cap_r[player_no][i] * 0xffff / 360)  );
			set_sp_mtx(&s2d_chaine_cap_mtx[player_no][i],Cos,Sin,-Sin,Cos,x,y,12,14,16,16);	//	カプセルマトリクス設定

			y = s2d_chaine_player_mtx[player_no][i].m.Y >> 2;
			set_sp_sub_mtx(&s2d_chaine_player_mtx[player_no][i],x + 10,y,20,12,32,16);			//	1P.2P.3P.4Pのマトリクス設定
		}
		break;
	case	101:	//	フェードアウト処理
		test_chaine_alpha[player_no] -= 8;
		if( test_chaine_alpha[player_no] <= 0x00 )
		{
			test_chaine_alpha[player_no] = 0;
			test_chaine_mode[player_no] = 0;		//	連鎖待ち
		}
		break;
	}
}

/*--------------------------------------
	連鎖 描画メイン関数
--------------------------------------*/
void	draw_chaine(s8 player_no,s8 mode)
{
	s8	i,j;

	gSPDisplayList( gp++, init_2d_dl );							//	2D描画設定
	gDPSetRenderMode( gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2 );	//	半透明指定

	if( test_chaine_mode[player_no] > 0){
		gDPSetScissor( gp++, G_SC_NON_INTERLACE,test_chaine_scis_x[player_no],0, test_chaine_scis_x[player_no] + test_chaine_scis_w[player_no], SCREEN_HT );	//	描画範囲指定
		gDPSetPrimColor(gp++,0,0,255,255,255,test_chaine_alpha[player_no]);	//	透明度設定
		gSPObjLoadTxtr( gp++,&s2d_chaine_tex );							//	CHAINE	テクスチャロード
		gSPObjLoadTxtr( gp++,&s2d_chaine_pal );							//	CHAINE	パレットロード
		gSPObjRectangle( gp++,&s2d_chaine_sp[player_no]);						//	CHAINE	描画

		//	描画順の計算
		if( test_chaine_num[player_no] > 9){
			j = 1;	//	2桁の場合、十の位から描画する
		}else{
			j = 0;	//	1桁の場合はそのまま
		}

		//	現在の連鎖数
		gSPObjLoadTxtr( gp++,&s2d_chaine_num_pal );				//	連鎖数	パレットのロード
		for( i = j;i >= 0;i-- ){
			gSPObjLoadTxtr( gp++,&s2d_chaine_num_tex[player_no][i] );	//	連鎖数	テクスチャのロード

			gSPObjSubMatrix(gp++, &s2d_chaine_num_mtx[player_no][i] );	//	連鎖数	マトリクスのロード(移動数値)
			gSPObjRectangleR( gp++,&s2d_chaine_num_sp );			//	連鎖数	描画
		}


		//	過去の拡大連鎖数
		if( test_chaine_mode[player_no] > 1 && test_chaine_mode[player_no] < 99 ){
			gDPSetPrimColor(gp++,0,0,255,255,255,128);
			if( test_chaine_num[player_no] > 9)
				j = 2;
			else
				j = 3;

			for(i = 3;i >= j;i--){
				gSPObjLoadTxtr( gp++,&s2d_chaine_num_tex[player_no][i] );
				gSPObjSubMatrix(gp++, &s2d_chaine_num_mtx[player_no][i] );
				gSPObjRectangleR( gp++,&s2d_chaine_num_sp );
			}
			gDPSetPrimColor(gp++,0,0,255,255,255,test_chaine_alpha[player_no]);	//	アルファを戻す
		}

		if( test_chaine_mode[player_no] > 98 ){
			//	メッセージの描画
			gSPObjLoadTxtr( gp++,&s2d_chaine_mes_tex[player_no] );
			gSPObjLoadTxtr( gp++,&s2d_chaine_mes_pal[player_no] );
			gSPObjRectangle( gp++,&s2d_chaine_mes_sp[player_no] );

			if( mode ){
				//	4Pモードの場合
				//	カプセルの描画
				for(i = j = 0;i < 3;i++){
					if( test_chaine_max[player_no][i] ){
						gSPObjLoadTxtr( gp++,&s2d_chaine_cap_tex[i] );					//	テクスチャロード
						gSPObjLoadTxtr( gp++,&s2d_chaine_cap_pal[i] );					//	パレットロード
						gSPObjMatrix(gp++, &s2d_chaine_cap_mtx[player_no][j] );			//	マトリクスロード
						gSPObjSprite( gp++,&s2d_chaine_cap_sp );						//	描画開始

						gSPObjLoadTxtr( gp++,&s2d_chaine_player_tex[player_no][i] );	//	テクスチャロード
						gSPObjLoadTxtr( gp++,&s2d_chaine_player_pal[player_no][i] );	//	パレットロード
						gSPObjSubMatrix(gp++, &s2d_chaine_player_mtx[player_no][j] );	//	マトリクスロード
						gSPObjRectangleR( gp++,&s2d_chaine_player_sp );					//	描画開始
						j ++;
					}
				}
			}
		}
	}
}

/*--------------------------------------
	PAUSE 初期化関数
--------------------------------------*/
void	init_pause(void)
{
	s8	i;

	set_sprite(&s2d_pause_sp,-32,-12,64,20,G_IM_FMT_CI,G_IM_SIZ_4b,64);					//	PAUSE 用スプライトの設定
	load_set_sprite(&s2d_pause_tex,0,test_pause_bm0_0,G_IM_FMT_CI,G_IM_SIZ_4b,64,20);	//	PAUSE 用テクスチャロードの設定
	load_set_pal(&s2d_pause_pal,test_pause_bm0_0tlut,16);								//	PAUSE 用パレットロードの設定

	for( i = 0;i < 4;i++ ){
		pause_count[i] = pause_anime_count[i] = pause_mode[i] = 0;								//	PAUSE 用制御変数
	}
}

/*--------------------------------------
	PAUSE 処理メイン関数
--------------------------------------*/
s8	pause_main( u8 player_no,s16 center )
{
	switch( pause_mode[player_no] )
	{
	case	0:	//	初期化
		pause_count[player_no] = 1;
		pause_anime_count[player_no] = 0;
		set_sp_sub_mtx(&s2d_pause_mtx[player_no],center,pause_position_table[pause_anime_count[player_no]] + 20,
				pause_w_scale_table[pause_anime_count[player_no]],pause_h_scale_table[pause_anime_count[player_no]],64,20);		//	マトリクス設定

		pause_mode[player_no] = 1;
		return	-1;
	case	1:	//	アニメーション

		pause_count[player_no] ++;
		if( pause_anime_count[player_no] < 18 ){
			if( pause_count[player_no] > 1 ){
				pause_count[player_no] = 0;
				pause_anime_count[player_no] ++;	//	コマを進める
			}
		}else{
			if( pause_count[player_no] > 6 ){
				pause_count[player_no] = 0;
				pause_anime_count[player_no] ++;	//	コマを進める
			}
		}

		if( pause_anime_count[player_no] > 22 ){
			pause_anime_count[player_no] = 18;
		}

		set_sp_sub_mtx(&s2d_pause_mtx[player_no],center,pause_position_table[pause_anime_count[player_no]] + 20,
				pause_w_scale_table[pause_anime_count[player_no]],pause_h_scale_table[pause_anime_count[player_no]],64,20);		//	マトリクス設定

		if( pause_anime_count[player_no] >= 5 ){
			return	1;
		}else{
			return	-1;
		}
	}
}
/*--------------------------------------
	PAUSE 描画メイン関数
--------------------------------------*/
void	pause_draw( u8 player_no )
{
	if( pause_mode[player_no] > 0 ){
		gSPObjLoadTxtr( gp++,&s2d_pause_pal );				//	PAUSE パレットロード
		gSPObjLoadTxtr( gp++,&s2d_pause_tex );				//	PAUSE テクスチャロード
		gSPObjSubMatrix(gp++, &s2d_pause_mtx[player_no] );	//	PAUSE マトリクスロード
		gSPObjRectangleR( gp++,&s2d_pause_sp );				//	PAUSE 描画
	}
}

/*--------------------------------------
	SHOCK 初期化関数
--------------------------------------*/
void	init_shock( void )
{
	s8	i;

	for( i = 0;i < 4;i++ ){
		shock_mode[i] = 0;
		shock_size[i][0] = shock_size[i][1] = 1;
		shock_position[i][0] = shock_position[i][1] = 1;
		shock_alpha[i] = 0xff;
	}
}
/*--------------------------------------
	SHOCK 初期設定関数
--------------------------------------*/
void	set_shock( u8 player_no, s16 x_pos,s16 y_pos )
{
	shock_mode[player_no] = 1;												//	処理開始
	shock_alpha[player_no] = 0xff;											//	透明度(不透明)
	shock_size[player_no][0] = shock_size[player_no][1] = 12;				//	初期表示サイズ
	shock_position[player_no][0] = x_pos - (shock_size[player_no][0] >> 1);	//	初期Ｘ座標
	shock_position[player_no][1] = y_pos - (shock_size[player_no][1] >> 1);	//	初期Ｙ座標
}
/*--------------------------------------
	SHOCK 処理メイン関数
--------------------------------------*/
void	shock_main( u8 player_no, s16 x_pos,s16 y_pos )
{
	s8	i;

	switch( shock_mode[player_no] )
	{
	case	1:	//	拡大処理
		shock_size[player_no][0] += 1;
		shock_size[player_no][1] += 2;
		shock_position[player_no][0] = x_pos - (shock_size[player_no][0] >> 1);	//	座標設定
		shock_position[player_no][1] = y_pos - (shock_size[player_no][1] >> 1);	//	座標設定

		if( shock_size[player_no][0] >= 30 ){
			shock_mode[player_no] = 2;
		}
		break;
	case	2:	//	フェードアウト
		shock_alpha[player_no] -= 0x08;
		if( shock_alpha[player_no] <= 0 ){
			shock_mode[player_no] = 0;
		}
		break;
	}
}
/*--------------------------------------
	SHOCK 描画メイン関数
--------------------------------------*/
void	shock_draw( u8 player_no )
{
	if( shock_mode[player_no] ){
		gDPSetPrimColor(gp++,0,0,255,255,255,shock_alpha[player_no]);		//	透明度設定
		load_TexPal( face_4p_game_shock_bm0_0tlut );						//	パレットロード
		load_TexTile_4b( face_4p_game_shock_bm0_0,24,24,0,0,23,23 );		//	テクスチャロード
		draw_ScaleTex( shock_position[player_no][0],shock_position[player_no][1],
						24,24,shock_size[player_no][0],shock_size[player_no][1],0,0 );	//	描画
	}
}

/*--------------------------------------
	HUMMING 初期化関数
--------------------------------------*/
void	init_humming( void )
{
	s8	i;

	for( i = 0;i < 4;i++ ){
		humming_mode[i] = 0;
		humming_sin[i] = 0;
		humming_size[i][0] = humming_size[i][1] = 1;
		humming_position[i][0] = humming_position[i][1] = humming_position[i][1] = 1;
		humming_alpha[i] = 0xff;
	}
}
/*--------------------------------------
	HUMMING 初期設定関数
--------------------------------------*/
void	set_humming( u8 player_no, s16 x_pos,s16 y_pos )
{
	humming_mode[player_no] = 1;												//	処理開始
	humming_alpha[player_no] = 0xff;											//	透明度(不透明)
	humming_sin[player_no] = 0x00;												//	揺れの指定
	humming_size[player_no][0] = humming_size[player_no][1] = 12;				//	初期表示サイズ
	humming_position[player_no][0] = x_pos - (humming_size[player_no][0] >> 1);	//	初期Ｘ座標
	humming_position[player_no][1] = y_pos - (humming_size[player_no][1] >> 1);	//	初期Ｙ座標
	humming_position[player_no][2] = y_pos;
}
/*--------------------------------------
	HUMMING 処理メイン関数
--------------------------------------*/
void	humming_main( u8 player_no, s16 x_pos )
{
	s8	i;

	switch( humming_mode[player_no] )
	{
	case	1:	//	拡大処理
		for( i = 0;i < 2;i++ ){
			humming_size[player_no][i] += 1;
		}
		humming_sin[player_no] += 30;
		if( humming_sin[player_no] >= 360 ){
			humming_sin[player_no] = 0;
		}
		x_pos = x_pos + (s16)(sinf( DEGREE (humming_sin[player_no])) * 10);
		humming_position[player_no][2] --;
		humming_position[player_no][0] = x_pos - (humming_size[player_no][0] >> 1);							//	座標設定
		humming_position[player_no][1] = humming_position[player_no][2] - (humming_size[player_no][1] >> 1);	//	座標設定

		if( humming_position[player_no][2] < 190 ){
			humming_mode[player_no] = 2;
		}
		break;
	case	2:	//	フェードアウト
		humming_alpha[player_no] -= 0x08;
		if( humming_alpha[player_no] <= 0 ){
			humming_mode[player_no] = 0;
		}
		break;
	}
}
/*--------------------------------------
	HUMMING 描画メイン関数
--------------------------------------*/
void	humming_draw( u8 player_no )
{
	if( humming_mode[player_no] ){
		gDPSetPrimColor(gp++,0,0,255,255,255,humming_alpha[player_no]);		//	透明度設定
		load_TexPal( face_4p_game_onpu_bm0_0tlut );						//	パレットロード
		load_TexTile_4b( face_4p_game_onpu_bm0_0,24,24,0,0,23,23 );		//	テクスチャロード
		draw_ScaleTex( humming_position[player_no][0],humming_position[player_no][1],
						24,24,humming_size[player_no][0],humming_size[player_no][1],0,0 );	//	描画
	}
}

/*--------------------------------------
	バブル 初期化関数
--------------------------------------*/
void	init_bubble( void )
{
	s8	i,j,k;

	//	各フラグの初期化
	for( i = 0;i < 4;i++ ){
		dm_bubble[i].bubble_disp_flg = 0;
		for( j = 0;j < 3;j ++ ){
			for( k = 0;k < 2;k ++ ){
				dm_bubble[i].bubble[j].bubble_flg[k] = 0;
				dm_bubble[i].bubble[j].bubble_pos[k] = 0;
			}
			for( k = 0;k < 4;k ++ ){
				dm_bubble[i].bubble[j].bubble_size[k] = 0;
			}
		}
	}
}
/*--------------------------------------
	バブル 設定関数
--------------------------------------*/
void	set_bubble( game_state *state,s8 player_no ,s8 flg ,s8	set_flg,s8 col_r,s8 col_y,s8 col_b )
{
	s8	i,j,k,x,size,g_no;
	s8	attack_pos_2p[] = {12,40,68};
	s8	attack_col_table_2p[2] = {1,0};
	s8	attack_col[3];
	s8	attack_sp[2][2] = {
		{13,26},{12,24}
	};
	s8	attack_col_table_4p[4][3] = {
		{3,2,1},{0,3,2},{1,0,3},{2,1,0}
	};


	if( !dm_bubble[player_no].bubble_disp_flg ){
		//	表示中でない
		if( state -> chain_line > 1 ){
			dm_bubble[player_no].bubble_disp_flg = 1;
			if( state -> chain_line == 2 ){
				size = 12;
				g_no = bubble_small;
			}else	if( state -> chain_line == 3 ){
				size = 16;
				g_no = bubble_mid;
			}else	{
				size = 20;
				g_no = bubble_big;
			}
			if( flg == 0 ){	//	2PLAY
				dm_bubble[player_no].scis_y = 215;																//	シザリング範囲のＹ座表設定
				for( i = 0;i < 3;i ++ ){
					dm_bubble[player_no].bubble[i].bubble_size[bubble_w] = size;								//	横幅設定
					dm_bubble[player_no].bubble[i].bubble_size[bubble_h] = size;								//	縦幅設定
					dm_bubble[player_no].bubble[i].bubble_size[bubble_g] = g_no;								//	グラフィック番号設定
					dm_bubble[player_no].bubble[i].bubble_size[bubble_p] = attack_col_table_2p[flg];			//	色の設定( 2PLAY なので 青か赤 )
					dm_bubble[player_no].bubble[i].bubble_size[bubble_s] = attack_sp[0][set_flg] + 1;			//	移動値( set_flg が ０の場合、途中で落下してくるようになる )

					dm_bubble[player_no].bubble[i].bubble_pos[0] = state -> map_x + attack_pos_2p[i];			//	Ｘ座標の設定
					dm_bubble[player_no].bubble[i].bubble_pos[1] = state -> map_y + 160 - size + i * size;		//	Ｙ座標の設定
					dm_bubble[player_no].bubble[i].bubble_pos[2] = dm_bubble[player_no].bubble[i].bubble_pos[1] + size;	//	落下した場合の判定消滅Ｙ座標
					dm_bubble[player_no].bubble[i].bubble_flg[bubble_disp] = 1;									//	表示フラグを立てる
					dm_bubble[player_no].bubble[i].bubble_flg[bubble_attack] = attack_col_table_2p[flg];		//	攻撃目標の設定
				}
				//	残り火設定
				if( genrand( 6 ) == 1 ){

					i = genrand( 3 );

					dm_bubble[player_no].bubble_mini.bubble_size[bubble_w] = dm_bubble[player_no].bubble_mini.bubble_size[bubble_h] = 8;	//	縦幅・横幅設定
					dm_bubble[player_no].bubble_mini.bubble_size[bubble_g] = 3;									//	グラフィック番号設定
					dm_bubble[player_no].bubble_mini.bubble_size[bubble_p] = attack_col_table_2p[flg];			//	色の設定( 2PLAY なので 青か赤 )
					dm_bubble[player_no].bubble_mini.bubble_size[bubble_s] = 11;								//	移動値( 途中で落下してくるようになる )

					dm_bubble[player_no].bubble_mini.bubble_pos[0] = state -> map_x + attack_pos_2p[i];			//	Ｘ座標の設定
					dm_bubble[player_no].bubble_mini.bubble_pos[1] = state -> map_y + 190 - size + i * 20;		//	Ｙ座標の設定
					dm_bubble[player_no].bubble_mini.bubble_pos[2] = dm_bubble[player_no].bubble_mini.bubble_pos[1];	//	落下した場合の判定消滅Ｙ座標
					dm_bubble[player_no].bubble_mini.bubble_flg[bubble_disp] = 1;								//	表示フラグを立てる
				}
			}else{	//	4PLAY
				dm_bubble[player_no].scis_y = 188;																//	シザリング範囲のＹ座表設定
				attack_col[0] = col_r;																			//	赤色で攻撃したかどうかの設定
				attack_col[1] = col_y;																			//	黄色で攻撃したかどうかの設定
				attack_col[2] = col_b;																			//	青色で攻撃したかどうかの設定

				//	攻撃相手数から、Ｘ座標の数値を割り出す
				for( i = j = 0;i < 3;i++ ){
					if( state -> chain_color[3] & ( 1 << i ) ){
						j ++;
					}
				}
				if( j == 3 ){			//	３人攻撃
					x = 8;
				}else	if( j == 2 ){	//	２人攻撃
					x = 16;
				}else{					//	１人攻撃
					x = 32;
				}

				for( i = j = k = 0;i < 3;i++ ){
					if( state -> chain_color[3] & ( 1 << i ) ){																	//	赤・黄・青の順で攻撃したか調べる
						dm_bubble[player_no].bubble[j].bubble_size[bubble_w] = size;											//	横幅設定
						dm_bubble[player_no].bubble[j].bubble_size[bubble_h] = size;											//	縦幅設定
						dm_bubble[player_no].bubble[j].bubble_size[bubble_g] = g_no;											//	グラフィック番号設定
						dm_bubble[player_no].bubble[j].bubble_size[bubble_p] = attack_col_table_4p[player_no][i];				//	色の設定( 攻撃する相手の色 )
						dm_bubble[player_no].bubble[j].bubble_size[bubble_s] = attack_sp[1][attack_col[i]] + 1;					//	移動値( attack_col[i] が ０の場合、途中で落下してくるようになる )

						dm_bubble[player_no].bubble[j].bubble_pos[0] = state -> map_x + x;										//	Ｘ座標の設定
						dm_bubble[player_no].bubble[j].bubble_pos[1] = state -> map_y + 128 - size + j * size;					//	Ｙ座標の設定
						dm_bubble[player_no].bubble[j].bubble_pos[2] = dm_bubble[player_no].bubble[j].bubble_pos[1] + size;		//	落下した場合の判定消滅Ｙ座標
						dm_bubble[player_no].bubble[j].bubble_flg[bubble_disp] = 1;												//	表示フラグを立てる
						dm_bubble[player_no].bubble[i].bubble_flg[bubble_attack] = attack_col_table_4p[player_no][i];			//	攻撃目標の設定

						//	残り火設定
						if( k == 0 ){
							if( genrand( 6 ) == 1 ){
								k = 1;
								dm_bubble[player_no].bubble_mini.bubble_size[bubble_w] = dm_bubble[player_no].bubble_mini.bubble_size[bubble_h] = 8;	//	縦幅・横幅設定
								dm_bubble[player_no].bubble_mini.bubble_size[bubble_g] = 3;											//	グラフィック番号設定
								dm_bubble[player_no].bubble_mini.bubble_size[bubble_p] = attack_col_table_4p[player_no][i];			//	色の設定( 攻撃する相手の色 )
								dm_bubble[player_no].bubble_mini.bubble_size[bubble_s] = 12;										//	移動値( 途中で落下してくるようになる )

								dm_bubble[player_no].bubble_mini.bubble_pos[0] = state -> map_x + x;								//	Ｘ座標の設定
								dm_bubble[player_no].bubble_mini.bubble_pos[1] = state -> map_y + 148 - size + j * size;			//	Ｙ座標の設定
								dm_bubble[player_no].bubble_mini.bubble_pos[2] = dm_bubble[player_no].bubble_mini.bubble_pos[1];	//	落下した場合の判定消滅Ｙ座標
								dm_bubble[player_no].bubble_mini.bubble_flg[bubble_disp] = 1;										//	表示フラグを立てる
							}
						}
						j++;
						x += 32;
					}
				}
			}
		}
	}
}

/*--------------------------------------
	バブル 処理メイン関数
--------------------------------------*/
void	bubble_main( s8 player_no )
{
	s8	i,j;

	if( dm_bubble[player_no].bubble_disp_flg){
		//	表示中
		for( i = 0;i < 3;i++ ){
			dm_bubble[player_no].bubble[i].bubble_size[bubble_s] --;			//	移動値の減少

			dm_bubble[player_no].bubble[i].bubble_pos[1] -= dm_bubble[player_no].bubble[i].bubble_size[bubble_s];			//	Ｙ座標から移動値を引く
			if( dm_bubble[player_no].bubble[i].bubble_pos[1] <= -dm_bubble[player_no].bubble[i].bubble_size[bubble_h] ){	//	バブルが画面外に消えた場合( 上昇 )
				//	表示フラグを消す
				dm_bubble[player_no].bubble[i].bubble_flg[bubble_disp] = 0;
			}else	if( dm_bubble[player_no].bubble[i].bubble_pos[1] >= dm_bubble[player_no].bubble[i].bubble_pos[2] ){		//	落下した場合
				//	表示フラグを消す
				dm_bubble[player_no].bubble[i].bubble_flg[bubble_disp] = 0;
			}
		}
		//	粒の移動
		dm_bubble[player_no].bubble_mini.bubble_size[bubble_s] --;
		dm_bubble[player_no].bubble_mini.bubble_pos[1] -= dm_bubble[player_no].bubble_mini.bubble_size[bubble_s];
		if( dm_bubble[player_no].bubble_mini.bubble_pos[1] > dm_bubble[player_no].bubble_mini.bubble_pos[2] ){		//	落下した場合
			dm_bubble[player_no].bubble_mini.bubble_flg[bubble_disp] = 0;											//	表示フラグを消す
		}

		//	表示フラグが立っていないか調べる
		for( i = j = 0;i < 3;i++ ){
			if( dm_bubble[player_no].bubble[i].bubble_flg[bubble_disp] ){
				j ++;
			}
		}
		if( j == 0 ){	//	３つとも表示フラグが立っていなかったら
			//	表示終了
			dm_bubble[player_no].bubble_disp_flg = 0;

		}
	}
}

/*--------------------------------------
	バブル 描画メイン関数
--------------------------------------*/
void	bubble_draw( game_state *state , s8 player_no )
{
	s8	i,j,k;
	s8	size,g_no,flip;

	gDPSetScissor( gp++, G_SC_NON_INTERLACE,0,0, 319, dm_bubble[player_no].scis_y - 1 );	//	描画範囲指定
	gDPSetRenderMode( gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2 );	//	半透明指定
	gDPSetPrimColor(gp++,0,0,255,255,255,200);					//	透明度設定

	if( dm_bubble[player_no].bubble_disp_flg ){
		//	表示中

		for( i = 0;i < 3;i++ ){
			if( dm_bubble[player_no].bubble[i].bubble_flg[bubble_disp] ){
				flip = 0;
				if( dm_bubble[player_no].bubble[i].bubble_size[bubble_s] < 0 ){
					flip = flip_on;	//	落下の場合
				}
				load_TexPal( bubble_pal[ dm_bubble[player_no].bubble[i].bubble_size[bubble_p] ] );		//	パレットロード
				load_TexTile_4b( bubble_graphic[ dm_bubble[player_no].bubble[i].bubble_size[bubble_g] ],
									dm_bubble[player_no].bubble[i].bubble_size[bubble_w],dm_bubble[player_no].bubble[i].bubble_size[bubble_h],
									0,0,dm_bubble[player_no].bubble[i].bubble_size[bubble_w] - 1,dm_bubble[player_no].bubble[i].bubble_size[bubble_h] - 1 );		//	テクスチャロード
				draw_TexFlip( dm_bubble[player_no].bubble[i].bubble_pos[0] - ( dm_bubble[player_no].bubble[i].bubble_size[bubble_w] >> 1 ),dm_bubble[player_no].bubble[i].bubble_pos[1],
								dm_bubble[player_no].bubble[i].bubble_size[bubble_w],dm_bubble[player_no].bubble[i].bubble_size[bubble_h],0,0,flip_off,flip );
			}
		}
		if( dm_bubble[player_no].bubble_mini.bubble_flg[bubble_disp] ){
			load_TexPal( bubble_pal[ dm_bubble[player_no].bubble_mini.bubble_size[bubble_p] ] );		//	パレットロード
			load_TexTile_4b( bubble_graphic[ dm_bubble[player_no].bubble_mini.bubble_size[bubble_g] ],
								dm_bubble[player_no].bubble_mini.bubble_size[bubble_w],dm_bubble[player_no].bubble_mini.bubble_size[bubble_h],
								0,0,dm_bubble[player_no].bubble_mini.bubble_size[bubble_w] - 1,dm_bubble[player_no].bubble_mini.bubble_size[bubble_h] - 1 );		//	テクスチャロード
				draw_Tex( dm_bubble[player_no].bubble_mini.bubble_pos[0] - ( dm_bubble[player_no].bubble_mini.bubble_size[bubble_w] >> 1 ),dm_bubble[player_no].bubble_mini.bubble_pos[1],
						dm_bubble[player_no].bubble_mini.bubble_size[bubble_w],dm_bubble[player_no].bubble_mini.bubble_size[bubble_h],0,0 );
		}
	}

	//	被爆状態描画
	if(  state -> game_condition[dm_mode_now] == dm_cnd_wait ){
		for( i = 4;i >= 0;i-- ){	//	とりあえず４個まで
#ifdef	DAMAGE_TYPE
			if( state -> cap_attack_work[i][0] != 0x0000 ){	//	攻撃を受けている
				for( j = k = 0;j < 16;j += 2 ){
					if( state -> cap_attack_work[i][0] & ( 0x03 << j ) ){
						k++;
					}
				}
				if( k == 2 ){
					size = 12;
					g_no = bubble_small;
				}else	if( k == 3 ){
					size = 16;
					g_no = bubble_mid;
				}else	{
					size = 20;
					g_no = bubble_big;
				}

				load_TexPal( bubble_pal[ state -> cap_attack_work[i][1] ] );		//	パレットロード	攻撃設定者の色
				load_TexTile_4b( bubble_graphic[ g_no ],
									size,size,0,0,size - 1,size - 1);
				draw_TexFlip( state -> map_x + i * 20,30 - ( size >> 1 ),size,size,0,0, flip_off,flip_on);
			}
		}
#endif

	}
	//	元に戻す
	gDPSetScissor( gp++, G_SC_NON_INTERLACE,0,0, 319, 239 );	//	描画範囲指定
	gDPSetPrimColor(gp++,0,0,0xff,0xff,0xff,0xff);				//	透明度設定
}
/*--------------------------------------
	push_any_key 初期化関数
--------------------------------------*/
void	init_push_any_key(void)
{
	push_sin = 0;
	push_pos_y = 0;
}

/*--------------------------------------
	push_any_key 処理メイン関数
--------------------------------------*/
void	push_any_key_main(void)
{
	push_pos_y =(s8)(sinf( DEGREE( push_sin ) ) * 4 );
	push_sin += 10;
	if( push_sin >= 360 ){
		push_sin -= 360;
	}
}
/*--------------------------------------
	push_any_key 描画メイン関数
--------------------------------------*/
void	push_any_key_draw(s16 x_pos,s16 y_pos )
{
	load_TexPal( dm_push_any_key_bm0_0tlut );		//	パレットロード
	load_TexBlock_4b( dm_push_any_key_bm0_0,64,7 );
	draw_Tex( x_pos,y_pos + push_pos_y,64,7,0,0 );
}
/*--------------------------------------
	初期化関数
--------------------------------------*/
void	init_effect_all(void)
{
	init_chaine();			//	連鎖処理初期化
	init_lose();			//	LOSE 初期化
	init_win();				//	WIN 初期化
	init_draw();			//	DRAW 初期化
	init_game_over();		//	GAME OVER 初期化
	init_try_next_stage();	//	TRY NEXT STAGE 初期化
	init_shock();			//	SHOCK 初期化
	init_humming();			//	HUMMING	初期化
	init_bubble();			//	連鎖２処理初期化
	init_push_any_key();	//	PUSH ANY KEY 初期化
}


