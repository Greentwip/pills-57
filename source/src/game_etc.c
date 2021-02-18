#include <ultra64.h>
#include <PR/gs2dex.h>
#include <math.h>
#include "def.h"
#include "segment.h"
#include "message.h"
#include "nnsched.h"
#include "main.h"
#include "audio.h"
#include "graphic.h"
#include "joy.h"
#include "static.h"
#include "evsworks.h"

#include "font.h"
#include "msgwnd.h"

#include "comp.h"
#include "main_story.h"
#include "game_etc.h"
#include "tex_func.h"
#include "calc.h"
#include "lws.h"

#include "dm_game_main.h"

extern	void	init_objMtx(void);

#define	win_2P_scene	0
#define	win_4P_scene	1
#define	lose_2P_scene	2
#define	lose_4P_scene	3
#define	draw_2P_scene	4
#define	draw_4P_scene	5
#define	retire_scene	6
#define	pause_scene		7
#define	gameover_scene	8
#define	ready_3sec_scene	9
#define	stageclear_scene	10
#define	trynext_scene		11
#define	timeover_scene		12
#define	allclear_scene		13

#define	OFS_pausemenu				0
#define	OFS_pausemenu_alpha			1
#define	OFS_pausemenu_alpha2		2
#define	OFS_pausemenu_cursor		3
#define	OFS_pausemenu2				4
#define	OFS_pausemenu2_alpha		5
#define	OFS_pausemenu3				6
#define	OFS_pausemenu4				7
#define	OFS_pausemenu5				8
#define	OFS_pausemenu6				9
#define	OFS_stop_a					10
#define	OFS_stop_ai					11
#define	OFS_stop_b					12
#define	OFS_etc_gbi					13
#define	OFS_msg_seg					14
#define	OFS_attack					15
//#define	OFS_coin_00					9


static	int pause_table[][6] = {
	{ 2, OFS_pausemenu5, OFS_pausemenu_alpha,  PAUSE_RET_CONTINUE, PAUSE_RET_EXIT, PAUSE_RET_EXIT },
	{ 3, OFS_pausemenu4, OFS_pausemenu_alpha2, PAUSE_RET_CONTINUE, PAUSE_RET_RETRY, PAUSE_RET_EXIT },
};
static	int cont_table[][6] = {
	{ 2, OFS_pausemenu , OFS_pausemenu_alpha,  CONTINUE_RET_REPLAY, CONTINUE_RET_EXIT, CONTINUE_RET_EXIT },
	{ 3, OFS_pausemenu3, OFS_pausemenu_alpha2, CONTINUE_RET_REPLAY, CONTINUE_RET_RETRY, CONTINUE_RET_EXIT },
	{ 2, OFS_pausemenu2, OFS_pausemenu_alpha,  CONTINUE_RET_REPLAY, CONTINUE_RET_CONTINUE, CONTINUE_RET_CONTINUE },
	{ 3, OFS_pausemenu6, OFS_pausemenu_alpha2, CONTINUE_RET_REPLAY, CONTINUE_RET_CONTINUE, CONTINUE_RET_EXIT },
};
static	u32	etc_parts_tbl[] = {
#if LOCAL==CHINA
#include "texture/story_bmp/etc/etc_gbi.tbl"
#include "texture/story_bmp/etc/etc.tbl"
#else
0x00000000,  //C:\Ultra\kaz\dm64\texture\story_bmp\etc\al_pausemenu.gbi			0
0x00001310,  //C:\Ultra\kaz\dm64\texture\story_bmp\etc\al_pausemenu_alpha.gbi	1
0x00001960,  //C:\Ultra\kaz\dm64\texture\story_bmp\etc\al_pausemenu_alpha2.gbi	2
0x000021b0,  //C:\Ultra\kaz\dm64\texture\story_bmp\etc\al_pausemenu_cursor.gbi	3
0x00002620,  //C:\Ultra\kaz\dm64\texture\story_bmp\etc\al_pausemenu2.gbi		4
0x00003930,  //C:\Ultra\kaz\dm64\texture\story_bmp\etc\al_pausemenu2_alpha.gbi	5
0x00003f80,  //C:\Ultra\kaz\dm64\texture\story_bmp\etc\al_pausemenu3.gbi		6
0x00005a90,  //C:\Ultra\kaz\dm64\texture\story_bmp\etc\al_pausemenu4.gbi		7
0x000075a0,  //C:\Ultra\kaz\dm64\texture\story_bmp\etc\al_pausemenu5.gbi		8
0x000088b0,  //C:\Ultra\kaz\dm64\texture\story_bmp\etc\al_pausemenu6.gbi		9
0x0000a3c0,  //C:\Ultra\kaz\dm64\texture\story_bmp\etc\stop_a.gbi				10
0x0000a7d0,  //C:\Ultra\kaz\dm64\texture\story_bmp\etc\stop_a_i.gbi				11
0x0000abe0,  //C:\Ultra\kaz\dm64\texture\story_bmp\etc\stop_b.gbi				12
0x00000000,  //C:\Ultra\kaz\dm64\texture\story_bmp\etc\etc_gbi.bin		13
0x0000af58,  //C:\Ultra\kaz\dm64\texture\story_bmp\etc\mes.seg			14
0x00024198,  //C:\Ultra\kaz\dm64\texture\story_bmp\etc\sp.bin			15
#endif
};


//-------------------------------------------------------------------------
static	s32		x2p[] = { 68-160, 252-160 };
static	s32		x4p[] = { 52-160, 124-160, 196-160, 268-160 };
static	int		binCount;
static	int		cont_bg_flg;
//-------------------------------------------------------------------------
// 表示データ処理ワーク
#define		EtcCount	8	// １プレイヤーで表示する最大数
typedef struct {
	float	x0, y0;
	float	xs, ys;
	int		timer[EtcCount];
	float	px[EtcCount], py[EtcCount];
	int		r[EtcCount], g[EtcCount], b[EtcCount], a[EtcCount];
} G_ETC;
//-------------------------------------------------------------------------
// 攻撃エフェクト処理ワーク
#define		AttackSP_0	8		// 本体のスプライト数
#define		AttackSP_1	4		// 破片のスプライト数
#define		AttackTime	48		// 移動時間
#define		AttackTimeF	8		// フェード時間
#define		AttackCount	16
typedef struct {
	int		x, y;				// 位置
	float	sx, sy;				// 始点
	float	ex, ey;				// 終点
	float	vx, vy;				// ベクトル
	int		time;				// 時間
	int		cnt;				// アニメ番号
	int		player;				// プレイヤー番号
} G_ATTACK;
#define		AtkEffCount	128
typedef struct {
	int		x, y, col;			// 座標、色
	int		no, time;			// スプライト番号、index
} G_A_SPRITE;

G_ATTACK	attack_effect[AttackCount];
G_A_SPRITE	attack_sprite[AtkEffCount];
int			attack_effect_idx;
int			attack_sprite_idx;
u32			attack_sprite_address;


//-------------------------------------------------------------------------
// ワーク
static	u32		etcTexAddress;
static	SCENE_DATA	**etcLwsAddress;
static	SCENE_DATA	*etcLwsTbl[16];

static	u32		logo_ofsY;
static	int		etc_mode;
static	G_ETC	g_etc_work[4];
static	int		*heart_x_tbl;
static	int		*heart_y_tbl;
static	int		heart_time[8];
static	int		coin_count;
static	int		coin_time[4];
static	Mtx		etc_viewMtx;
static	Vp	etc_vp = {
	SCREEN_WD*2, SCREEN_HT*2, G_MAXZ/2, 0,
	SCREEN_WD*2, SCREEN_HT*2, G_MAXZ/2, 0,
};
static	Gfx etc_setup[] = {
	gsDPPipeSync(),

	gsSPViewport(&etc_vp),
	gsDPSetBlendColor(0, 0, 0, 4),

	gsDPSetScissor(G_SC_NON_INTERLACE, 0, 0, SCREEN_WD-1, SCREEN_HT-1),
	gsDPSetCycleType(G_CYC_1CYCLE),
	gsDPPipelineMode(G_PM_1PRIMITIVE),
	gsDPSetRenderMode(G_RM_AA_OPA_SURF, G_RM_AA_OPA_SURF2),
	gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),

	gsSPClearGeometryMode(G_SHADING_SMOOTH | G_CULL_BOTH | G_ZBUFFER |
							G_FOG | G_LIGHTING | G_TEXTURE_GEN |
							G_TEXTURE_GEN_LINEAR | G_LOD ),
	gsSPSetGeometryMode( G_SHADE | G_CULL_BACK ),
	gsSPTexture(0, 0, 0, 0, G_OFF),

	gsDPSetTextureLOD(G_TL_TILE),
	gsDPSetTextureDetail(G_TD_CLAMP),
	gsDPSetTextureConvert(G_TC_FILT),
	gsDPSetTextureLUT(G_TT_NONE),
	gsDPSetTextureFilter(G_TF_BILERP),
	gsDPSetTexturePersp(G_TP_NONE),

	gsDPPipeSync(),
	gsSPEndDisplayList(),
};



//-------------------------------------------------------------------------
//	初期化
//		u32 address		テクスチャーデータの先頭アドレス
//		int bin_count	表示しているビンの数 ( 1, 2 or 4 )
void	initEtcWork(u32 address, int bin_count)
{
	int		i, n;
	u32		tmp;

	if ( evs_gamemode == GMD_TIME_ATTACK ) {
		logo_ofsY = 16;
	} else {
		logo_ofsY = 0;
	}

	binCount = bin_count;
	etcTexAddress = address;
	etcLwsAddress = (SCENE_DATA **)( address + etc_parts_tbl[OFS_msg_seg] );
	attack_sprite_address = address + etc_parts_tbl[OFS_attack];

	for ( i = 0; i < 14; i++ ) {
		tmp = (u32)etcLwsAddress[i];
		tmp = ( tmp & 0x00ffffff ) + address + etc_parts_tbl[OFS_msg_seg];
		etcLwsTbl[i] = (SCENE_DATA *)tmp;
	}

	switch ( binCount ) {	//evs_playcnt
		case 1:
		case 2:
			etc_mode = 0;
			break;
		case 4:
			etc_mode = 1;
			break;
	}

	guOrtho(&etc_viewMtx,
		-(float)SCREEN_WD/2.0F, (float)SCREEN_WD/2.0F,
		-(float)SCREEN_HT/2.0F, (float)SCREEN_HT/2.0F,
		1.0, 2000.0, 1.0);

	for ( i = 0; i < binCount; i++ ) {	//evs_playcnt
		g_etc_work[i].x0 = (float)game_state_data[i].map_x;
		g_etc_work[i].y0 = (float)game_state_data[i].map_y;
		g_etc_work[i].xs = (float)game_state_data[i].map_item_size * GAME_MAP_W;
		g_etc_work[i].ys = (float)game_state_data[i].map_item_size * ( GAME_MAP_H + 1);

		for ( n = 0; n < EtcCount; n++ ) {
			g_etc_work[i].timer[n] = 0;
			g_etc_work[i].px[n] = g_etc_work[i].py[n] = 0;
			g_etc_work[i].r[n] = g_etc_work[i].g[n] = g_etc_work[i].b[n] = g_etc_work[i].a[n] = 255;
		}
	}

	attack_effect_idx = 0;
	for ( i = 0; i < AttackCount; i++ ) {
		attack_effect[i].time = 0;
	}
	attack_sprite_idx = 0;
	for ( i = 0; i < AtkEffCount; i++ ) {
		attack_sprite[i].time = 0;
	}
}



//-------------------------------------------------------------------------
// ポーズ表時の時のみ、ワーク初期化
void	init_pause_disp(void)
{
	int		i, n;

	for ( i = 0; i < 4; i++ ) {
		for ( n = 0; n < EtcCount; n++ ) {
			g_etc_work[i].timer[n] = 0;
			g_etc_work[i].px[n] = g_etc_work[i].py[n] = 0;
			g_etc_work[i].r[n] = g_etc_work[i].g[n] = g_etc_work[i].b[n] = g_etc_work[i].a[n] = 255;
		}
	}
}



//-------------------------------------------------------------------------
//	表示のセットアップ
void	disp_logo_setup(Gfx **glp)
{
	Gfx			*pgfx;

	pgfx = *glp;

	init_objMtx();
	gSPMatrix(pgfx++, OS_K0_TO_PHYSICAL(&etc_viewMtx),
			G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);
	gSPSegment(pgfx++, ST_SEGMENT, osVirtualToPhysical((void *)etcLwsAddress));

	*glp = pgfx;
}



//-------------------------------------------------------------------------
//	カウントダウンの表示
//		int player	プレイヤー番号
//		int count	フレームカウント ( 0 ～ ??? )
//	return:
//		TRUE  : 終了
//		FALSE : アニメ中
int		disp_count_logo(Gfx **glp, int player, int count)
{
	Gfx			*pgfx;
	G_ETC		*pge;
	LMatrix 	pm;
	s32			x, y;
	int			ret;

	pgfx = *glp;

	gSPDisplayList(pgfx++, etc_setup);

	switch ( binCount ) {
		case 1:
			x = 0;
			y = 0;
			break;
		case 2:
			x = x2p[player];
			y = 0;
			break;
		default:
			x = x4p[player];
			y = 0;
			break;
	}

	makeTransrateMatrix(pm, x * FIXPOINT, y * FIXPOINT, -1000 * FIXPOINT);
	ret = lws_anim(&pgfx, pm, etcLwsTbl[ready_3sec_scene], count, (u32)etcLwsAddress);

	*glp = pgfx;

	return(ret);
}



//-------------------------------------------------------------------------
//	ステージクリア
//		int player	プレイヤー番号
void	disp_clear_logo(Gfx **glp, int player, int flg)
{
	Gfx			*pgfx;
	G_ETC		*pge;
	LMatrix 	pm;
	s32			x, y;

	pgfx = *glp;
	pge = &g_etc_work[player];

	gSPDisplayList(pgfx++, etc_setup);

	switch ( binCount ) {
		case 1:
			x = -4;
			y = 0;
			break;
		case 2:
			x = x2p[player] - 4;
			y = 0;
			break;
		default:
			x = x4p[player] - 4;
			y = 0;
			break;
	}

	makeTransrateMatrix(pm, x * FIXPOINT, y * FIXPOINT, -1000 * FIXPOINT);

	// StageClear
	if ( lws_anim(&pgfx, pm, etcLwsTbl[stageclear_scene], pge->timer[0], (u32)etcLwsAddress) == TRUE ) {
		pge->timer[0] = 40-1;
	}
	pge->timer[0]++;

	// TryNext
	if ( flg ) {
		if ( lws_anim(&pgfx, pm, etcLwsTbl[trynext_scene], pge->timer[1], (u32)etcLwsAddress) == TRUE ) {
			pge->timer[1] = 60-1;
		}
		pge->timer[1]++;
	}

	*glp = pgfx;
}



//-------------------------------------------------------------------------
//	オールクリア
//		int player	プレイヤー番号
void	disp_allclear_logo(Gfx **glp, int player, int flg)
{
	Gfx			*pgfx;
	G_ETC		*pge;
	LMatrix 	pm;
	s32			x, y;

	pgfx = *glp;
	pge = &g_etc_work[player];

	gSPDisplayList(pgfx++, etc_setup);

	switch ( binCount ) {
		case 1:
			x = -4;
			y = 0;
			break;
		case 2:
			x = x2p[player] - 4;
			y = 0;
			break;
		default:
			x = x4p[player] - 4;
			y = 0;
			break;
	}

	makeTransrateMatrix(pm, x * FIXPOINT, y * FIXPOINT, -1000 * FIXPOINT);

	// AllClear
	if ( lws_anim(&pgfx, pm, etcLwsTbl[allclear_scene], pge->timer[1], (u32)etcLwsAddress) == TRUE ) {
		pge->timer[1] = 40-1;
	}
	pge->timer[1]++;

	// TryNext
	if ( flg ) {
		if ( lws_anim(&pgfx, pm, etcLwsTbl[trynext_scene], pge->timer[2], (u32)etcLwsAddress) == TRUE ) {
			pge->timer[2] = 60-1;
		}
		pge->timer[2]++;
	}

	*glp = pgfx;
}



//-------------------------------------------------------------------------
//	ＷＩＮの表示
//		int player	プレイヤー番号
void	disp_win_logo(Gfx **glp, int player)
{
	Gfx			*pgfx;
	G_ETC		*pge;
	LMatrix 	pm;
	s32			x, y;

	pgfx = *glp;
	pge = &g_etc_work[player];

	gSPDisplayList(pgfx++, etc_setup);

	switch ( binCount ) {
		case 1:
			x = 0;
			y = 0;
			break;
		case 2:
			x = x2p[player];
			y = 0;
			break;
		default:
			x = x4p[player];
			y = 2;
			break;
	}

	makeTransrateMatrix(pm, x * FIXPOINT, (y+logo_ofsY) * FIXPOINT, -1000 * FIXPOINT);
	if ( binCount != 4 ) {
		// 2P
		if ( lws_anim(&pgfx, pm, etcLwsTbl[win_2P_scene], pge->timer[0], (u32)etcLwsAddress) == TRUE ) {
			pge->timer[0] = 64-1;
		}
	} else {
		// 4P
		if ( lws_anim(&pgfx, pm, etcLwsTbl[win_4P_scene], pge->timer[0], (u32)etcLwsAddress) == TRUE ) {
			pge->timer[0] = 64-1;
		}
	}

	pge->timer[0]++;

	*glp = pgfx;
}



//-------------------------------------------------------------------------
//	ＬＯＳＥの表示
//		int player	プレイヤー番号
void	disp_lose_logo(Gfx **glp, int player)
{
	Gfx			*pgfx;
	G_ETC		*pge;
	LMatrix 	pm;
	s32			x, y;

	pgfx = *glp;
	pge = &g_etc_work[player];

	gSPDisplayList(pgfx++, etc_setup);

	switch ( binCount ) {
		case 1:
			x = 0;
			y = 0;
			break;
		case 2:
			x = x2p[player];
			y = 0;
			break;
		default:
			x = x4p[player];
			y = 2;
			break;
	}

	makeTransrateMatrix(pm, x * FIXPOINT, (y+logo_ofsY) * FIXPOINT, -1000 * FIXPOINT);
	if ( binCount != 4 ) {
		// 2P
		if ( lws_anim(&pgfx, pm, etcLwsTbl[lose_2P_scene], pge->timer[0], (u32)etcLwsAddress) == TRUE ) {
			pge->timer[0] = 56-1;
		}
	} else {
		// 4P
		if ( lws_anim(&pgfx, pm, etcLwsTbl[lose_4P_scene], pge->timer[0], (u32)etcLwsAddress) == TRUE ) {
			pge->timer[0] = 56-1;
		}
	}

	pge->timer[0]++;

	*glp = pgfx;
}



//-------------------------------------------------------------------------
//	ＤＲＡＷの表示
//		int player	プレイヤー番号
void	disp_draw_logo(Gfx **glp, int player)
{
	Gfx			*pgfx;
	G_ETC		*pge;
	LMatrix 	pm;
	s32			x, y;

	pgfx = *glp;
	pge = &g_etc_work[player];

	gSPDisplayList(pgfx++, etc_setup);

	switch ( binCount ) {
		case 1:
			x = 0;
			y = 0;
			break;
		case 2:
			x = x2p[player];
			y = 0;
			break;
		default:
			x = x4p[player];
			y = 2;
			break;
	}

	makeTransrateMatrix(pm, x * FIXPOINT, (y+logo_ofsY) * FIXPOINT, -1000 * FIXPOINT);
	if ( binCount != 4 ) {
		// 2P
		if ( lws_anim(&pgfx, pm, etcLwsTbl[draw_2P_scene], pge->timer[0], (u32)etcLwsAddress) == TRUE ) {
			pge->timer[0] = 16-1;
		}
	} else {
		// 4P
		if ( lws_anim(&pgfx, pm, etcLwsTbl[draw_4P_scene], pge->timer[0], (u32)etcLwsAddress) == TRUE ) {
			pge->timer[0] = 16-1;
		}
	}

	pge->timer[0]++;

	*glp = pgfx;
}



//-------------------------------------------------------------------------
//	ＰＡＵＳＥの表示
//		int player	プレイヤー番号
//		int score	スコア			-1:NotDisplayScore
//		int player	カーソル位置	0:continue  1:retry  2:exit    -1:NotDisplayMenu
int	disp_pause_logo(Gfx **glp, int player, int score, int cursor, int type)
{
#define	PAUSE_TIME	16
#define	PAUSE_YS2	35
#define	PAUSE_YS3	50
#define	PAUSE_SOFY0	10	// 大瓶  SCORE
#define	PAUSE_SOFY1	10	// 小瓶
#define	PAUSE_4PY	40	// 4Pの送り説明
	Gfx			*pgfx;
	G_ETC		*pge;
	GBI_DATA	gbi, gbia;
	LMatrix 	pm;
	float		sx, sy, s;
	float		fx, fy;
	int			i, a, t[5], x, y;

	pgfx = *glp;
	pge = &g_etc_work[player];

	// ＳＣＯＲＥの１文字目
	pge->px[7] = pge->x0 + pge->xs / 2.0 - 20;
	pge->py[7] = pge->y0 + ( etc_mode == 0 ? PAUSE_SOFY0 : PAUSE_SOFY1 );
	// メニューの位置
	pge->px[2] = pge->x0 + pge->xs / 2.0 - 62/2;
	if ( pause_table[type][0] == 2 ) {
		pge->py[2] = pge->y0 + pge->ys - PAUSE_YS2 - 2;
	} else {
		pge->py[2] = pge->y0 + pge->ys - PAUSE_YS3 - 2;
	}

	// Pause Logo
	gSPDisplayList(pgfx++, etc_setup);
	switch ( binCount ) {
		case 1:
			x = 0;
			y = 0;
			break;
		case 2:
			x = x2p[player];
			y = 0;
			break;
		default:
			x = x4p[player];
			y = 8;
			break;
	}
	makeTransrateMatrix(pm, x * FIXPOINT, y * FIXPOINT, -1000 * FIXPOINT);
	if ( binCount != 4 ) {
		// 2P
		if ( lws_anim(&pgfx, pm, etcLwsTbl[pause_scene], pge->timer[7], (u32)etcLwsAddress) == TRUE ) {
			pge->timer[7] = 26-1;
		}
	} else {
		// 4P
		if ( lws_anim(&pgfx, pm, etcLwsTbl[pause_scene], pge->timer[7], (u32)etcLwsAddress) == TRUE ) {
			pge->timer[7] = 26-1;
		}
	}

	// メニュー描画
	gSPDisplayList(pgfx++, alpha_texture_init_dl);
	a = pge->timer[6] - ( PAUSE_TIME );
	if ( cursor >= 0 && a > 0 ) {
		// メニュー
		get_gbi_stat(&gbi, etcTexAddress + etc_parts_tbl[pause_table[type][1]]);
		get_gbi_stat(&gbia, etcTexAddress + etc_parts_tbl[pause_table[type][2]]);
		StretchAlphaTexBlock(&pgfx,		// 出力先Gfxバッファ変数のアドレス
			gbi.x, gbi.y,				// アライン部分を除いたテクスチャのサイズ
			gbi.tex_addr, gbi.x,		// カラーテクスチャのアドレスと幅
			gbia.tex_addr, gbia.x,		// アルファテクスチャのアドレスと幅
			pge->px[2], pge->py[2],		// 描画座標
			(float)(gbi.x), (float)(gbi.y));	// 描画範囲, 負の値を指定すると反転描画
		// カーソル
		if ( pause_table[type][0] == 2 ) {
			switch ( cursor ) {
				case 0:
					fx = pge->px[2];
					fy = pge->py[2] + 0 + 1;
					break;
				case 1:
					fx = pge->px[2];
					fy = pge->py[2] + 16 - 2;
					break;
			}
		} else {
			switch ( cursor ) {
				case 0:
					fx = pge->px[2];
					fy = pge->py[2] + 0 - 1;
					break;
				case 1:
					fx = pge->px[2];
					fy = pge->py[2] + 16 - 1;
					break;
				case 2:
					fx = pge->px[2];
					fy = pge->py[2] + 31 - 1;
					break;
			}
		}
		if ( pge->timer[5] & 0x010 ) {
			// 点滅
			gSPDisplayList(pgfx++, normal_texture_init_dl);
			get_gbi_stat(&gbi, etcTexAddress + etc_parts_tbl[OFS_pausemenu_cursor]);
			StretchTexBlock4(&pgfx,
				gbi.x, gbi.y, gbi.lut_addr, gbi.tex_addr,
				fx, fy, gbi.x, gbi.y);
		}
	}

	// カウンタ
	pge->timer[5]++;
	pge->timer[6]++;
	pge->timer[7]++;

	gSPDisplayList(pgfx++, normal_texture_init_dl);

	*glp = pgfx;

	return(pause_table[type][cursor+3]);
}



//-------------------------------------------------------------------------
//	ｃｏｎｔｉｎｕｅ時の表示
int	etc_continue_logo(Gfx **glp, int player, int cursor, int type)
{
#define	CONT_YS2	35			//パネルのサイズ
#define	CONT_YS3	50			//パネルのサイズ
	Gfx			*pgfx;
	G_ETC		*pge;
	GBI_DATA	gbi, gbia;
	float		fx, fy;
	int			i, a;

	pge = &g_etc_work[player];
	pgfx = *glp;

	// メニュー描画
	if ( cursor >= 0 ) {
		a = pge->timer[6] * 12;
		if ( a > 255 ) a = 255;

		if ( cont_bg_flg && ( binCount == 2 ) ) {
			// 2Pスコアアタックの場合影をつける
			gSPDisplayList(pgfx++, normal_texture_init_dl);
			gDPSetRenderMode(pgfx++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
			gDPSetCombineLERP(pgfx++, 0,0,0,PRIMITIVE,   PRIMITIVE,0,TEXEL0,0,    0,0,0,PRIMITIVE,   PRIMITIVE,0,TEXEL0,0);
			gDPSetTextureLUT(pgfx++, G_TT_NONE);
			gDPSetPrimColor(pgfx++, 0, 0, 0, 0, 0, 160);
			get_gbi_stat(&gbia, etcTexAddress + etc_parts_tbl[cont_table[type][2]]);
			StretchTexBlock4i(&pgfx,
				gbia.x, gbia.y, gbia.tex_addr,
				pge->px[6]+6, pge->py[6]+6, gbia.x, gbia.y);
		}

		gSPDisplayList(pgfx++, alpha_texture_init_dl);

		// メニュー
		get_gbi_stat(&gbi, etcTexAddress + etc_parts_tbl[cont_table[type][1]]);
		get_gbi_stat(&gbia, etcTexAddress + etc_parts_tbl[cont_table[type][2]]);
		StretchAlphaTexBlock(&pgfx,		// 出力先Gfxバッファ変数のアドレス
			gbi.x, gbi.y,				// アライン部分を除いたテクスチャのサイズ
			gbi.tex_addr, gbi.x,		// カラーテクスチャのアドレスと幅
			gbia.tex_addr, gbia.x,		// アルファテクスチャのアドレスと幅
			pge->px[6], pge->py[6],		// 描画座標
			(float)(gbi.x), (float)(gbi.y));	// 描画範囲, 負の値を指定すると反転描画
		// カーソル
		if ( cont_table[type][0] == 2 ) {
			switch ( cursor ) {
				case 0:
					fx = pge->px[6];
					fy = pge->py[6] + 0 + 1;
					break;
				case 1:
					fx = pge->px[6];
					fy = pge->py[6] + 16 - 2;
					break;
			}
		} else {
			switch ( cursor ) {
				case 0:
					fx = pge->px[6];
					fy = pge->py[6] + 0 - 1;
					break;
				case 1:
					fx = pge->px[6];
					fy = pge->py[6] + 16 - 1;
					break;
				case 2:
					fx = pge->px[6];
					fy = pge->py[6] + 31 - 1;
					break;
			}
		}
		if ( pge->timer[6] & 0x010 ) {
			// 点滅
			gSPDisplayList(pgfx++, normal_texture_init_dl);
			get_gbi_stat(&gbi, etcTexAddress + etc_parts_tbl[OFS_pausemenu_cursor]);
			StretchTexBlock4(&pgfx,
				gbi.x, gbi.y, gbi.lut_addr, gbi.tex_addr,
				fx, fy, gbi.x, gbi.y);
		}
	}

	// カウンタ
	pge->timer[6]++;

	gSPDisplayList(pgfx++, normal_texture_init_dl);

	*glp = pgfx;

	return(cont_table[type][cursor+3]);
}



//-------------------------------------------------------------------------
//	ｃｏｎｔｉｎｕｅ時の表示
//		int player	プレイヤー番号
//		int cursor	カーソル位置	0:continue  1:exit
int	disp_continue_logo(Gfx **glp, int player, int cursor, int type)
{
	G_ETC		*pge;

	cont_bg_flg = FALSE;
	pge = &g_etc_work[player];

	// メニューの位置
	pge->px[6] = pge->x0 + pge->xs / 2.0 - 62/2;
	if ( cont_table[type][0] == 2 ) {
		pge->py[6] = pge->y0 + pge->ys - CONT_YS2 - 2;
	} else {
		pge->py[6] = pge->y0 + pge->ys - CONT_YS3 - 2;
	}

	return( etc_continue_logo(glp, player, cursor, type) );
}



//-------------------------------------------------------------------------
//	ｃｏｎｔｉｎｕｅ時の表示（スコアアタック用）
//		int player	プレイヤー番号
//		int cursor	カーソル位置	0:continue  1:exit
int	disp_continue_logo_score(Gfx **glp, int player, int cursor, int type)
{
	G_ETC		*pge;

	cont_bg_flg = TRUE;
	pge = &g_etc_work[player];

	// メニューの位置
	pge->px[6] = 160 - 62/2;
	if ( cont_table[type][0] == 2 ) {
		pge->py[6] = pge->y0 + pge->ys - CONT_YS2 - 2;
	} else {
		pge->py[6] = pge->y0 + pge->ys - CONT_YS3 - 2;
	}

	return( etc_continue_logo(glp, player, cursor, type) );
}



//-------------------------------------------------------------------------
//	ＧａｍｅＯｖｅｒの表示
//		int player	プレイヤー番号
void	disp_gameover_logo(Gfx **glp, int player)
{
	Gfx			*pgfx;
	G_ETC		*pge;
	LMatrix 	pm;
	s32			x, y;

	pgfx = *glp;
	pge = &g_etc_work[player];

	gSPDisplayList(pgfx++, etc_setup);

	switch ( binCount ) {
		case 1:
			x = -2;
			y = 0;
			break;
		case 2:
			x = x2p[player] - 2;
			y = 0;
			break;
		default:
			x = x4p[player] - 2;
			y = 0;
			break;
	}

	makeTransrateMatrix(pm, x * FIXPOINT, y * FIXPOINT, -1000 * FIXPOINT);

	if ( lws_anim(&pgfx, pm, etcLwsTbl[gameover_scene], pge->timer[1], (u32)etcLwsAddress) == TRUE ) {
		pge->timer[1] = 58-1;
	}

	pge->timer[1]++;

	*glp = pgfx;
}
//-------------------------------------------------------------------------
//	ＴｉｍｅＯｖｅｒの表示
//		int player	プレイヤー番号
void	disp_timeover_logo(Gfx **glp, int player)
{
	Gfx			*pgfx;
	G_ETC		*pge;
	LMatrix 	pm;
	s32			x, y;

	pgfx = *glp;
	pge = &g_etc_work[player];

	gSPDisplayList(pgfx++, etc_setup);

	switch ( binCount ) {
		case 1:
			x = -2;
			y = 0;
			break;
		case 2:
			x = x2p[player] - 2;
			y = 0;
			break;
		default:
			x = x4p[player] - 2;
			y = 0;
			break;
	}

	makeTransrateMatrix(pm, x * FIXPOINT, y * FIXPOINT, -1000 * FIXPOINT);

	if ( lws_anim(&pgfx, pm, etcLwsTbl[timeover_scene], pge->timer[0], (u32)etcLwsAddress) == TRUE ) {
		pge->timer[0] = 58-1;
	}

	pge->timer[0]++;

	*glp = pgfx;
}



//-------------------------------------------------------------------------
//	Ｒｅｔｉｒｅの表示
//		int player	プレイヤー番号
void	disp_retire_logo(Gfx **glp, int player)
{
	Gfx			*pgfx;
	G_ETC		*pge;
	LMatrix 	pm;
	s32			x, y;

	pgfx = *glp;
	pge = &g_etc_work[player];

	gSPDisplayList(pgfx++, etc_setup);

	switch ( binCount ) {
		case 1:
			x = -2;
			y = 0;
			break;
		case 2:
			x = x2p[player];
			y = 0;
			break;
		default:
			x = x4p[player];
			y = 0;
			break;
	}

	makeTransrateMatrix(pm, x * FIXPOINT, y * FIXPOINT, -1000 * FIXPOINT);

	if ( lws_anim(&pgfx, pm, etcLwsTbl[retire_scene], pge->timer[2], (u32)etcLwsAddress) == TRUE ) {
		pge->timer[2] = 140-1;
	}

	pge->timer[2]++;

	*glp = pgfx;
}



//-------------------------------------------------------------------------
//	ＳＴＯＰの表示
//		int player	プレイヤー番号
void	disp_timestop_logo(Gfx **glp, int player)
{
#define	STOP_X		(59-16)
#define	STOP_Y		(175-18)
#define	STOP_X2		(59-22)
#define	STOP_Y2		(175+0)
	Gfx			*pgfx;
	G_ETC		*pge;
	GBI_DATA	gbi;

	pgfx = *glp;
	pge = &g_etc_work[player];

	// 描画
	get_gbi_stat(&gbi, etcTexAddress + etc_parts_tbl[OFS_stop_ai]);
	gSPDisplayList(pgfx++, normal_texture_init_dl);
	gDPSetRenderMode(pgfx++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
	gDPSetCombineLERP(pgfx++, 0,0,0,0,   0,0,0,TEXEL0,    0,0,0,0,   0,0,0,TEXEL0);
	gDPSetTextureLUT(pgfx++, G_TT_NONE);
	StretchTexBlock4i(&pgfx,
		gbi.x, gbi.y, gbi.tex_addr,
 		STOP_X, STOP_Y, gbi.x, gbi.y);

	get_gbi_stat(&gbi, etcTexAddress + etc_parts_tbl[OFS_stop_a]);
	gSPDisplayList(pgfx++, normal_texture_init_dl);
	StretchTexBlock4(&pgfx,
		gbi.x, gbi.y, gbi.lut_addr, gbi.tex_addr,
		STOP_X, STOP_Y, gbi.x, gbi.y);

	if ( pge->timer[4] & 0x010 ) {
		// 点滅
		get_gbi_stat(&gbi, etcTexAddress + etc_parts_tbl[OFS_stop_b]);
		StretchTexBlock4(&pgfx,
			gbi.x, gbi.y, gbi.lut_addr, gbi.tex_addr,
			STOP_X2, STOP_Y2, gbi.x, gbi.y);
	}

	// カウンタ
	pge->timer[4]++;

	*glp = pgfx;

	return;
}



//-------------------------------------------------------------------------
// 攻撃エフェクトの追加
void	add_attack_effect(int player, int sx, int sy, int ex, int ey)
{
	attack_effect[attack_effect_idx].player = player;
	attack_effect[attack_effect_idx].time = AttackTime;
	attack_effect[attack_effect_idx].cnt = 0;

	attack_effect[attack_effect_idx].x = sx;
	attack_effect[attack_effect_idx].y = sy;
	attack_effect[attack_effect_idx].sx = (float)sx;
	attack_effect[attack_effect_idx].sy = (float)sy;
	attack_effect[attack_effect_idx].ex = (float)ex;
	attack_effect[attack_effect_idx].ey = (float)ey;

	attack_effect[attack_effect_idx].vx = sx;
	attack_effect[attack_effect_idx].vy = sy;

	attack_effect_idx++;
	if ( attack_effect_idx >= AttackCount ) attack_effect_idx = 0;
}



//-------------------------------------------------------------------------
// 攻撃エフェクトの表示
void	disp_attack_effect(Gfx **glp)
{
#define	ATK_EFF_ALPHA	240
#define	ATK_EFF_Y2		50
#define	ATK_EFF_Y4		30
	static	int	col_prim[][3] = {
		// Red
		{ 255,   0,  48 },
		// Blue
		{  64,  64, 255 },
		// Yellow
		{ 255, 200,   0 },
		// Green
		{   0, 255,   0 },
	};
	static	int	col_env[][3] = {
		// Red
		{ 255, 192, 210 },
		// Blue
		{ 220, 220, 255 },
		// Yellow
		{ 255, 255, 220 },
		// Green
		{ 210, 255, 210 },
	};
	int		i, tmp, a;
	u32		addr;
	Gfx		*pgfx;
	float	x, y, t, scl;

	pgfx = *glp;

	// DL設定
	gSPDisplayList(pgfx++, normal_texture_init_dl);
	gDPSetTextureLUT(pgfx++, G_TT_NONE);
	gDPSetAlphaCompare(pgfx++, G_AC_NONE);
	gDPSetRenderMode(pgfx++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
	gDPSetCombineLERP(pgfx++,
		ENVIRONMENT, PRIMITIVE, TEXEL0, PRIMITIVE,
		ENVIRONMENT, 0, TEXEL0, 0, 
		ENVIRONMENT, PRIMITIVE, TEXEL0, PRIMITIVE,
		ENVIRONMENT, 0, TEXEL0, 0);

	// 飛んでいく曲線のＹスケール
	switch ( binCount ) {
		case 1:
		case 2:
			scl = ATK_EFF_Y2;
			break;
		case 4:
		default:
			scl = ATK_EFF_Y4;
			break;
	}

	// 玉を表示
	tmp = -1;
	for ( i = 0; i < AttackCount; i++ ) {
		if ( attack_effect[i].time > 0 ) {
			attack_effect[i].time--;
			// 移動
			t = ( (float)attack_effect[i].time ) / AttackTime;
			x = attack_effect[i].ex - ( t * ( attack_effect[i].ex - attack_effect[i].sx ) );
			y = attack_effect[i].ey - ( t * ( attack_effect[i].ey - attack_effect[i].sy ) );
			y = y - scl * sinf( DEG2RAD( t * 180.0 ) );
			attack_effect[i].x = (int)( x + 0.5 );
			attack_effect[i].y = (int)( y + 0.5 );
			// 色の設定
			a = ATK_EFF_ALPHA;
			if ( ( AttackTime - AttackTimeF ) < attack_effect[i].time ) {
				a = AttackTime - attack_effect[i].time;
				a = ( ATK_EFF_ALPHA * a ) / AttackTimeF;
			}
			if ( AttackTimeF > attack_effect[i].time ) {
				a = ( ATK_EFF_ALPHA * attack_effect[i].time ) / AttackTimeF;
			}
			if ( attack_effect[i].player != tmp ) {
				gDPSetPrimColor(pgfx++, 0, 0,
					col_prim[attack_effect[i].player][0],
					col_prim[attack_effect[i].player][1],
					col_prim[attack_effect[i].player][2],
					255 );
				gDPSetEnvColor(pgfx++,
					col_env[attack_effect[i].player][0],
					col_env[attack_effect[i].player][1],
					col_env[attack_effect[i].player][2],
					a );
				tmp = attack_effect[i].player;
			}
			// 表示するスプライトのアドレス
			addr = attack_sprite_address + 0x1800 + 0x200 * attack_effect[i].cnt;
			// 描画
			StretchTexBlock4i(&pgfx, 32, 32, (void *)addr, x, y, 32, 32);
			attack_effect[i].cnt++;
			if ( attack_effect[i].cnt >= AttackSP_0 ) attack_effect[i].cnt = 0;
			// 粉を定義
			if ( ( attack_effect[i].time & 3 ) == 3 ) {
				attack_sprite[attack_sprite_idx].time = AttackSP_1 * 4;
				attack_sprite[attack_sprite_idx].col = attack_effect[i].player;
//				attack_sprite[attack_sprite_idx].no = random(3);
//				attack_sprite[attack_sprite_idx].x = attack_effect[i].x + random(4);
//				attack_sprite[attack_sprite_idx].y = attack_effect[i].y + random(4);
				attack_sprite[attack_sprite_idx].no = rand() % 3;
				attack_sprite[attack_sprite_idx].x = attack_effect[i].x + rand() % 4;
				attack_sprite[attack_sprite_idx].y = attack_effect[i].y + rand() % 4;
				attack_sprite_idx++;
				if ( attack_sprite_idx >= AtkEffCount ) attack_sprite_idx = 0;
			}
		}
	}

	// 粉を表示
	tmp = -1;
	for ( i = 0; i < AtkEffCount; i++ ) {
		if ( attack_sprite[i].time > 0 ) {
			attack_sprite[i].time--;
			// 色の設定
			if ( attack_sprite[i].col != tmp ) {
				gDPSetPrimColor(pgfx++, 0, 0,
					col_prim[attack_sprite[i].col][0],
					col_prim[attack_sprite[i].col][1],
					col_prim[attack_sprite[i].col][2],
					255 );
				gDPSetEnvColor(pgfx++,
					col_env[attack_sprite[i].col][0],
					col_env[attack_sprite[i].col][1],
					col_env[attack_sprite[i].col][2],
					ATK_EFF_ALPHA );
				tmp = attack_sprite[i].col;
			}
			// 表示するスプライトのアドレス
			addr = attack_sprite_address + 0x200 * ( attack_sprite[i].no * AttackSP_1 + attack_sprite[i].time/2 );
			// 描画
			StretchTexBlock4i(&pgfx,
				32, 32, (void *)addr,
				(float)attack_sprite[i].x, (float)attack_sprite[i].y,
		 		32, 32);
		}
	}

	// 終わり
	*glp = pgfx;
}
