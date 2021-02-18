// ///////////////////////////////////////////////////////////////////////////
//
//	Mode Select Program File
//
//	Author		: Katsuyuki Ohkura
//
//	Last Update	: 1999/11/23
//
//	modify		: 1999/12/11	Hiroyuki Watanabe
// ///////////////////////////////////////////////////////////////////////////
#include <ultra64.h>
#include <PR/gs2dex.h>
#include "def.h"
#include "vram.h"
#include "segment.h"
#include "message.h"
#include "nnsched.h"
#include "main.h"
#include "audio.h"
#include "graphic.h"
#include "joy.h"
#include "static.h"
#include "evsworks.h"
#include "dm_code_data.h"

#include "hardcopy.h"
#include "o_segment.h"
#include "o_static.h"
#include "lv_sel_data.h"
#include "cap_pos.dat"

#define	CS_4P_SELECT_TEAM	0
#define	CS_4P_SELECT_MAN	1
#define	CS_4P_SELECT_LEVEL	2

#define	CS_STORY_SERECT_LEVEL	0
#define	CS_STORY_SERECT_STAGE	1

static s8 main_60_out_flg;

typedef struct {
	s16			bx;				// 基準Ｘ座標
	s16			by;				// 基準Ｙ座標
	u8 			pos;			// 位置
	u8			com_lv;			// ＣＯＭレベル
	u8			stg_no;			// ステージ番号
	u8			mus_no;			// 曲番号
	s16			com_adj_y;		// ＣＯＭレベル用上下スクロール
	s16			stg_adj_y;		// ステージ用上下スクロール用
	s16			mus_adj_y;		// 曲用上下スクロール用
	u8			stg_tbl[16];	// ステージテーブル
	u8			stg_sel_num;	// 選択可能ステージ数
} UNDER_ITEMS;

typedef struct {
	s8			move;			// 移動中フラグ
	int			count;			// 移動用カウンタ
	float		sc[5];			// スケール
	int			pos_y[5];		// Ｙ座標位置
} Face_Data;

typedef struct {
	s16			bx;				// 基準Ｘ座標
	s16			by;				// 基準Ｙ座標
	u8			flag;			// フラグ
	s8			ok;				// 決定済み
	u8			play;			// 人間 or ＣＯＭ
	u8			lv;				// 選択レベル
	u8			speed;			// 選択スピード
	u8			chara;			// 選択キャラ
	u8			p_team;			// チーム編成
	Face_Data	fd;				// 顔データ
} Player_Data;

typedef struct {
	u8			flow;			// 流れ制御
	u8			sub_flow;		// サブの流れ制御
	u8 			max_chara;		// キャラクター数
	u8			now_com;		// ＣＯＭ決定用カウンタ
	s8			coms[5];		// ＣＯＭ番号控え
	s16			adj_x;			// 全体移動用
	s16			mv_x;			// 移動量
	u32			cnt;			// アニメ用カウンタ
	u8			max_lv;			// １Ｐ用ＭＡＸレベル
	u8			team;			//	チーム選択用
	Player_Data	pd[4];			// プレイヤーデータ
	UNDER_ITEMS	ui;				// 下段項目
} Mode_Sel;

Mode_Sel ms;

// 下段パネル
static PIC2 under_pl[3][3] = {
{{plate_env_bm0_0,plate_env_bm0_0tlut,256,42,0,0},				// ダミー
 {plate_e_a_bm0_0,plate_e_a_bm0_0tlut,256,42,0,0},				// ダミー
 {plate_e_p_bm0_0,plate_e_p_bm0_0tlut,128,34,0,0}},				// ダミー
{{plate_env_bm0_0,plate_env_bm0_0tlut,256,42,0,0},				// ステージ＆音楽
 {plate_e_a_bm0_0,plate_e_a_bm0_0tlut,256,42,0,0},				// ステージ＆音楽 暗大
 {plate_e_p_bm0_0,plate_e_p_bm0_0tlut,128,34,0,0}},				// ステージ＆音楽 暗小
{{plate_e_scm_bm0_0,   plate_e_scm_bm0_0tlut  ,262,42,0,0},		// ステージ＆ＣＯＭＬＶ＆音楽
 {plate_e_sm262_bm0_0, plate_e_sm262_bm0_0tlut,262,42,0,0},		// ステージ＆ＣＯＭＬＶ＆音楽 暗大
 {plate_e_sm082_bm0_0, plate_e_sm082_bm0_0tlut,82, 34,0,0}},	// ステージ＆ＣＯＭＬＶ＆音楽 暗小
 };

// 難易度（２Ｐ・４Ｐ用）
static PIC com_lv[3] = {
	p_hard_bm0_0,   p_hard_bm0_0tlut,
	p_normal_bm0_0, p_normal_bm0_0tlut,
	p_easy_bm0_0,   p_easy_bm0_0tlut,
};
// 音楽
static PIC music[4][2] = {
	{{mus_fev_bm0_0,mus_fev_bm0_0tlut},
	{mus_fev2_bm0_0, mus_fev2_bm0_0tlut}},
	{{mus_chi_bm0_0,mus_chi_bm0_0tlut},
	{mus_chi2_bm0_0, mus_chi2_bm0_0tlut}},
	{{mus_diz_bm0_0,mus_diz_bm0_0tlut},
	{mus_diz2_bm0_0, mus_diz2_bm0_0tlut}},
	{{mus_off_bm0_0,mus_off_bm0_0tlut},
	{mus_off2_bm0_0, mus_off2_bm0_0tlut}},
};


// ステージ
static PIC battle_stage_pic[] = {
	menu_st_name_bm0_0, menu_st_name_bm0_0tlut,
	menu_st_name_bm1_0, menu_st_name_bm0_0tlut,
	menu_st_name_bm2_0, menu_st_name_bm0_0tlut,
	menu_st_name_bm3_0, menu_st_name_bm0_0tlut,
	menu_st_name_bm4_0, menu_st_name_bm0_0tlut,
	menu_st_name_bm5_0, menu_st_name_bm0_0tlut,
	menu_st_name_bm6_0, menu_st_name_bm0_0tlut,
	menu_st_name_bm7_0, menu_st_name_bm0_0tlut,
	menu_st_name_bm8_0, menu_st_name_bm0_0tlut,
	menu_st_name_bm9_0, menu_st_name_bm0_0tlut,
	menu_st_name_bm10_0, menu_st_name_bm0_0tlut,
};

// ＰＬ or ＣＯＭ
static PIC2 plcom[4][2] = {
	pl_1p_bm0_0, pl_1p_bm0_0tlut, 16,8,0,0,
	com_1p_bm0_0,com_1p_bm0_0tlut,32,8,0,0,
	pl_2p_bm0_0, pl_2p_bm0_0tlut, 16,8,0,0,
	com_2p_bm0_0,com_2p_bm0_0tlut,32,8,0,0,
	pl_3p_bm0_0, pl_3p_bm0_0tlut, 16,8,0,0,
	com_3p_bm0_0,com_3p_bm0_0tlut,32,8,0,0,
	pl_4p_bm0_0, pl_4p_bm0_0tlut, 16,8,0,0,
	com_4p_bm0_0,com_4p_bm0_0tlut,32,8,0,0,
};
// 顔フレーム
static PIC face_frame[4] = {
	frame_1p_bm0_0, frame_1p_bm0_0tlut,
	frame_2p_bm0_0, frame_2p_bm0_0tlut,
	frame_3p_bm0_0, frame_3p_bm0_0tlut,
	frame_4p_bm0_0, frame_4p_bm0_0tlut,
};
// プレート（キャラクターセレクト）
static PIC plate4p_char[4] = {
	plate4p_char_bm0_0,plate4p_char_bm0_0tlut,
	plate4p_char_bm0_0,plate4p_char_bm1_0tlut,
	plate4p_char_bm0_0,plate4p_char_bm2_0tlut,
	plate4p_char_bm0_0,plate4p_char_bm3_0tlut,
};
// プレート（レベルセレクト）
static PIC plate4p_lv[4] = {
	plate4p_lv_bm0_0,plate4p_lv_bm0_0tlut,
	plate4p_lv_bm0_0,plate4p_lv_bm1_0tlut,
	plate4p_lv_bm0_0,plate4p_lv_bm2_0tlut,
	plate4p_lv_bm0_0,plate4p_lv_bm3_0tlut,
};
// スピード（２Ｐ用）
static PIC cap_sp[2][3] = {
	cap_1_l_bm0_0,cap_1_l_bm0_0tlut,
	cap_1_m_bm0_0,cap_1_m_bm0_0tlut,
	cap_1_h_bm0_0,cap_1_h_bm0_0tlut,
	cap_2_l_bm0_0,cap_2_l_bm0_0tlut,
	cap_2_m_bm0_0,cap_2_m_bm0_0tlut,
	cap_2_h_bm0_0,cap_2_h_bm0_0tlut,
};
// スピード（４Ｐ用・パターンデータ）
static u8 *speed4p_pat[3] = {
	speed4p_low_bm0_0,
	speed4p_med_bm0_0,
	speed4p_hi_bm0_0,
};
// スピード（４Ｐ用・パレットデータ）
static u16 *speed4p_pal[4] = {
	speed4p_low_bm0_0tlut,
	speed4p_low_bm1_0tlut,
	speed4p_low_bm2_0tlut,
	speed4p_low_bm3_0tlut,
};

//	?P or ?COM
static PIC team_4p_player[4][2] = {
	{{dm_ts_1p_bm0_0,dm_ts_1p_bm0_0tlut},{dm_ts_1p_com_bm0_0,dm_ts_1p_bm0_0tlut}},
	{{dm_ts_2p_bm0_0,dm_ts_2p_bm0_0tlut},{dm_ts_2p_com_bm0_0,dm_ts_2p_bm0_0tlut}},
	{{dm_ts_3p_bm0_0,dm_ts_3p_bm0_0tlut},{dm_ts_3p_com_bm0_0,dm_ts_3p_bm0_0tlut}},
	{{dm_ts_4p_bm0_0,dm_ts_4p_bm0_0tlut},{dm_ts_4p_com_bm0_0,dm_ts_4p_bm0_0tlut}},
};

//	チーム分けデータ
static	u8	team_data[4][4] = {
	{TEAM_MARIO,TEAM_ENEMY,TEAM_ENEMY2,TEAM_ENEMY3},
	{TEAM_MARIO,TEAM_MARIO,TEAM_ENEMY,TEAM_ENEMY},
	{TEAM_MARIO,TEAM_MARIO,TEAM_MARIO,TEAM_ENEMY},
	{TEAM_MARIO,TEAM_ENEMY,TEAM_ENEMY,TEAM_ENEMY}
};

//	難易度プレート( STORY用 )
static u8 *difficulty_plate[3] = {
	menu_st_level_easy_bm0_0,
	menu_st_level_normal_bm0_0,
	menu_st_level_hard_bm0_0,
};

// 顔
static PIC faces[] = {
	st_face_01_bm0_0, st_face_01_bm0_0tlut,
	st_face_02_bm0_0, st_face_02_bm0_0tlut,
	st_face_03_bm0_0, st_face_03_bm0_0tlut,
	st_face_04_bm0_0, st_face_04_bm0_0tlut,
	st_face_05_bm0_0, st_face_05_bm0_0tlut,
	st_face_06_bm0_0, st_face_06_bm0_0tlut,
	st_face_07_bm0_0, st_face_07_bm0_0tlut,
	st_face_08_bm0_0, st_face_08_bm0_0tlut,
	st_face_09_bm0_0, st_face_09_bm0_0tlut,
	st_face_10_bm0_0, st_face_10_bm0_0tlut,
	st_face_11_bm0_0, st_face_11_bm0_0tlut,
	st_face_12_bm0_0, st_face_12_bm0_0tlut,
};

// 拡縮対応テクスチャ表示
static void disp_tex_4bt_sc( u8 *pat, u16 *pal, u16 sizex, u16 sizey, s16 x, s16 y, f32 scx, f32 scy, u8 flag )
{
	s32 pos_x,pos_y,w,h,dsdx,dtdy;

	pos_x	= (s32)(x + (sizex - (f32)sizex * scx) * .5);
	w		= (s32)((f32)sizex * scx);
	dsdx	= (s32)(1024.0 / scx);

	pos_y	= (s32)(y + (sizey - (f32)sizey * scy) * .5);
	h		= (s32)((f32)sizey * scy);
	dtdy	= (s32)(1024.0 / scy);

	gDPLoadTLUT_pal256( gp++,pal );
	gDPLoadTextureTile_4b( gp++, pat, G_IM_FMT_CI,sizex,sizey,0,0,sizex-1,sizey-1,
					0, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK,G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
	gSPScisTextureRectangle( gp++, (pos_x)<<2, (pos_y)<<2, (pos_x+w)<<2, (pos_y+h)<<2,
					G_TX_RENDERTILE, 0, 0, dsdx, dtdy );
}


static	void reset_face_size( Face_Data *fd )
{
	fd->count		= 0;		//	移動(上下のスクロール)用カウンタのクリア
	fd->move		= 0;		//	移動(上下のスクロール)中フラグのクリア
	fd->sc[0]		= 0.25;		//	２つ上のスケール値の設定(1/4の大きさ)
	fd->sc[1]		= 0.5;		//	１つ上のスケール値の設定(1/2の大きさ)
	fd->sc[2]		= 1.0;		//	中央のスケール値の設定(普通の大きさ)
	fd->sc[3]		= 0.5;		//	１つ下のスケール値の設定(1/2の大きさ)
	fd->sc[4]		= 0.25;		//	２つ下のスケール値の設定(1/4の大きさ)
	fd->pos_y[0]	= -54;		//	２つ上の基準Ｙ座標の設定
	fd->pos_y[1]	= -36;		//	１つ上の基準Ｙ座標の設定
	fd->pos_y[2]	= 0;		//	中央の基準Ｙ座標の設定
	fd->pos_y[3]	= 36;		//	１つ下の基準Ｙ座標の設定
	fd->pos_y[4]	= 54;		//	２つ下の基準Ｙ座標の設定
}

static	void set_face_size( Player_Data *pd,Face_Data *fd,u32 cnt )
{
	float sc_s[] = {0.125, 0.149, 0.163, 0.176, 0.191, 0.204, 0.218, 0.232, 0.246, 0.250};	//	４段目から３段目までのスケール値
	float sc_m[] = {0.250, 0.288, 0.316, 0.343, 0.371, 0.399, 0.427, 0.454, 0.482, 0.500};	//	３段目から２段目までのスケール値
	float sc_b[] = {0.500, 0.566, 0.621, 0.677, 0.732, 0.788, 0.843, 0.899, 0.954, 1.000};	// 	２段目から１段目までのスケール値

	//	顔のスクロール処理
	switch( pd->flag ){
	case 0:		// 待ち
		break;
	case 1:		// 上に移動( スクロール方向は下 )
		if( !(cnt % 1) ){					//	２イント( １フレーム )で１スクロール
			fd->count ++;						//	処理カウンタの増加
			fd->pos_y[0] += 2;					//	２つ上の基準Ｙ座標の増加
			fd->pos_y[1] += 4;					//	１つ上の基準Ｙ座標の増加
			fd->pos_y[2] += 4;					//	中央の基準Ｙ座標の増加
			fd->pos_y[3] += 2;					//	１つ下の基準Ｙ座標の増加
			fd->pos_y[4] += 1;					//	２つ下の基準Ｙ座標の増加
			fd->sc[0] = sc_m[fd->count];		//	２つ上のスケール値の設定
			fd->sc[1] = sc_b[fd->count];		//	１つ上のスケール値の設定
			fd->sc[2] = sc_b[9-fd->count];		//	中央のスケール値の設定
			fd->sc[3] = sc_m[9-fd->count];		//	１つ下のスケール値の設定
			fd->sc[4] = sc_s[9-fd->count];		//	２つ下のスケール値の設定
			if( fd->count >= 9 ){				//	処理終了判定
				pd->flag = 0;					//	スクロール処理フラグのクリア( 停止 )
				reset_face_size( fd );			//	座標・スケールの初期化
			}
		}
		break;
	case 2:		// 下に移動( スクロール方向は上 )
		if( !(cnt % 1) ){					//	２イント( １フレーム )で１スクロール
			fd->count ++;						//	処理カウンタの増加
			fd->pos_y[0] -= 1;					//	２つ上の基準Ｙ座標の増加
			fd->pos_y[1] -= 2;					//	１つ上の基準Ｙ座標の増加
			fd->pos_y[2] -= 4;					//	中央の基準Ｙ座標の増加
			fd->pos_y[3] -= 4;					//	１つ下の基準Ｙ座標の増加
			fd->pos_y[4] -= 2;					//	２つ下の基準Ｙ座標の増加
			fd->sc[0] = sc_s[9-fd->count];		//	２つ上のスケール値の設定
			fd->sc[1] = sc_m[9-fd->count];		//	２つ上のスケール値の設定
			fd->sc[2] = sc_b[9-fd->count];		//	中央のスケール値の設定
			fd->sc[3] = sc_b[fd->count];		//	１つ下のスケール値の設定
			fd->sc[4] = sc_m[fd->count];		//	２つ下のスケール値の設定
			if( fd->count >= 9 ){				//	処理終了判定
				pd->flag = 0;					//	スクロール処理フラグのクリア( 停止 )
				reset_face_size( fd );			//	座標・スケールの初期化
			}
		}
		break;
	}
}

// STORY 用初期化
static void init_story_play( void )
{
	if( evs_one_game_flg ){
		if( evs_mem_data[evs_select_name_no[0]].mem_use_flg & DM_MEM_STORY_CLEAR ){
			//	ステージセレクトを選ぶことが出来た場合
			ms.flow			= CS_STORY_SERECT_STAGE;	//	ステージセレクト
			ms.adj_x		= -320;						//	画面Ｘ座標
		}else{
			ms.flow			= CS_STORY_SERECT_LEVEL;	//	難易度選択(通常)
			ms.adj_x		= 0;						//	画面Ｘ座標
		}
	}else{
		//	モードセレクトから来た場合
		ms.flow			= CS_STORY_SERECT_LEVEL;	//	難易度選択(通常)
		ms.adj_x		= 0;						//	画面Ｘ座標
	}
	ms.mv_x			= 20;	//	切り替え(ステージセレクト <-> 難易度選択)のスクロール速度
	ms.sub_flow		= 0;	//	内部制御処理をキー入力に設定
	ms.cnt			= 0;	//	内部カウンタをクリア

	//	LEVEL のみの基準座標
	ms.pd[0].bx		= 110;	//	難易度選択描画の基準になる座標( ms.adj_x + ms.pd[0].bx )
	ms.pd[0].by		= 47;	// 	Ｙ座標は固定
	ms.pd[0].lv 	= evs_mem_data[evs_select_name_no[0]].state_old.p_st_lv;	//	前回選択した難易度の設定

	//	LEVEL & STAGE の基準座標
	ms.pd[1].bx		= 383;	//	ステージセレクト描画の基準になる座標( ms.adj_x + ms.pd[1].bx )
	ms.pd[1].by		= 47;	// 	Ｙ座標は固定
	ms.pd[1].flag	= 0;	//	ステージセレクトの時、難易度設定かステージ設定のどちらを行っているかのフラグ(0:難易度設定 1:ステージ設定 )
	ms.pd[1].lv		= evs_mem_data[evs_select_name_no[0]].state_old.p_st_st_lv;		//	前回選択した難易度の設定
	ms.pd[1].speed	= evs_mem_data[evs_select_name_no[0]].state_old.p_st_st - 1;	//	前回選択したステージ番号の設定

	evs_one_game_flg = 0;	// ステージセレクトフラグのクリア
}

// STORY 用メイン
static void calc_story_play( void )
{
	switch( ms.flow ){
	case	CS_STORY_SERECT_LEVEL:	//	レベル選択のみ
		switch( ms.sub_flow ){
			case	0:
				ms.cnt ++;									//	内部カウンタの増加( 矢印の点滅等に利用される )
				if( joycur[main_joy[0]] & DM_KEY_UP ){
					if( ms.pd[0].lv < 2 ){
						dm_set_menu_se( SE_mLeftRight );	//	SE設定
						ms.pd[0].lv ++;						//	カーソル上移動
					}
				}else	if( joycur[main_joy[0]] & DM_KEY_DOWN ){
					if( ms.pd[0].lv > 0 ){
						dm_set_menu_se( SE_mLeftRight );	//	SE設定
						ms.pd[0].lv --;						//	カーソル下移動
					}
				}else	if( joyupd[main_joy[0]] & DM_KEY_R ){
					if( evs_mem_data[evs_select_name_no[0]].mem_use_flg & DM_MEM_STORY_CLEAR ){
						//	クリアしていた場合ステージセレクトへ行く
						ms.sub_flow = 253;					//	スクロール処理に設定
					}
				}else	if( joyupd[main_joy[0]] & DM_KEY_OK ){
					dm_set_menu_se( SE_mDecide );			//	SE設定
					ms.sub_flow = 254;						//	決定
				}else	if( joyupd[main_joy[0]] & DM_KEY_B ){
					ms.sub_flow = 255;						//	キャンセル( モードセレクトに戻る )
				}
				break;
			case 253:		// フレームアウト( スクロール処理 )
				if( ms.adj_x - ms.mv_x > -320 ){
					ms.adj_x -= ms.mv_x;
				} else {
					ms.adj_x -= ms.mv_x;
					ms.cnt = 0;							//	内部カウンタのクリア
					ms.sub_flow = 0;					//	内部制御処理をキー入力に設定
					ms.pd[1].flag = 0;					//	ステージセレクトの難易度設定処理に変更。
					ms.flow = CS_STORY_SERECT_STAGE;	//	処理をステージセレクトに切り替える
				}
				break;
			case 254:
				main_no = MAIN_50;						//	ストーリーモードへ
				main_60_out_flg = 0;					// メインループ脱出
				break;
			case 255:
				main_no = MAIN_MODE_SELECT;				//	モードセレクトに戻る
				main_60_out_flg = 0;					//	メインループ脱出
				break;
		}
		break;
	case	CS_STORY_SERECT_STAGE:	//	レベル選択 & ステージセレクト
		switch( ms.sub_flow ){
			case	0:	//	難易度設定
				ms.cnt ++;									//	内部カウンタの増加( 矢印の点滅等に利用される )
				if( joycur[main_joy[0]] & DM_KEY_UP ){
					if( ms.pd[1].lv < 2 ){
						dm_set_menu_se( SE_mLeftRight );	//	SE設定
						ms.pd[1].lv ++;						//	カーソル上移動
					}
				}else	if( joycur[main_joy[0]] & DM_KEY_DOWN ){
					if( ms.pd[1].lv > 0 ){
						dm_set_menu_se( SE_mLeftRight );	//	SE設定
						ms.pd[1].lv --;						//	カーソル下移動
					}
				}else	if( joyupd[main_joy[0]] & DM_KEY_RIGHT ){
					dm_set_menu_se( SE_mUpDown );			//	SE設定
					ms.pd[1].flag = 1;						//	内部処理をステージ設定に切り替える( 表示系 )
					ms.sub_flow = 1;						//	内部処理をステージ設定に切り替える( 処理系 )
				}else	if( joyupd[main_joy[0]] & DM_KEY_L ){
					ms.sub_flow = 253;						//	スクロール処理に設定(ノーマルモード(通常難易度設定)へ行く)
				}else	if( joyupd[main_joy[0]] & DM_KEY_OK ){
					dm_set_menu_se( SE_mDecide );			//	SE設定
					ms.sub_flow = 254;						//	決定
				}else	if( joyupd[main_joy[0]] & DM_KEY_B ){
					ms.sub_flow = 255;						//	キャンセル( モードセレクトに戻る )
				}
				break;
			case	1:	//	ステージ設定
				ms.cnt ++;
				if( joycur[main_joy[0]] & DM_KEY_DOWN ){
					if( ms.pd[1].speed > 1 ){
						dm_set_menu_se( SE_mLeftRight );	//	SE設定
						ms.pd[1].speed --;					//	ステージダウン上移動
					}
				}else	if( joycur[main_joy[0]] & DM_KEY_UP ){
					if( ms.pd[1].speed < 10 + evs_secret_flg ){	//	ピーチ姫が使用可能な場合は、ピーチ姫とも対戦できる。
						dm_set_menu_se( SE_mLeftRight );	//	SE設定
						ms.pd[1].speed ++;					//	ステージアップ下移動
					}
				}else	if( joyupd[main_joy[0]] & DM_KEY_LEFT ){
					dm_set_menu_se( SE_mUpDown );			//	SE設定
					ms.pd[1].flag = 0;						//	内部処理を難易度設定に切り替える( 表示系 )
					ms.sub_flow = 0;						//	内部処理を難易度設定に切り替える( 処理系 )
				}else	if( joyupd[main_joy[0]] & DM_KEY_L ){
					ms.sub_flow = 253;						//	スクロール処理に設定(ノーマルモード(通常難易度設定)へ行く)
				}else	if( joyupd[main_joy[0]] & DM_KEY_OK ){
					dm_set_menu_se( SE_mDecide );
					ms.sub_flow = 254;	//	決定
				}else	if( joyupd[main_joy[0]] & DM_KEY_B ){
					ms.sub_flow = 255;						//	キャンセル( モードセレクトに戻る )
				}
				break;
			case 253:		// フレームイン
				if( ms.adj_x + ms.mv_x < 0 ){
					ms.adj_x += ms.mv_x;
				} else {
					ms.adj_x += ms.mv_x;
					ms.cnt = 0;							//	内部カウンタのクリア
					ms.sub_flow = 0;					//	内部制御処理をキー入力に設定
					ms.flow = CS_STORY_SERECT_LEVEL;	//	処理を難易度設定(通常)に切り替える
				}
				break;
			case 254:		//	決定
				main_no = MAIN_12;						// ゲーム前設定へ
				main_60_out_flg = 0;					// メインループ脱出
				break;
			case 255:
				main_no = MAIN_MODE_SELECT;				//	モードセレクトに戻る
				main_60_out_flg = 0;					// メインループ脱出
				break;
		}
		break;
	}
}

// STORY 用描画
static void disp_story_play( void )
{
	s16	i,j;

	//	背景
	gSPDisplayList( gp++, Texture_TE_dl );

	load_TexPal(dm_bg_tile_pal_3_bm0_0tlut);
	load_TexTile_4b( &dm_bg_tile_data_bm0_0[0],20,20,0,0,19,19 );
	for(i = 0;i < 12;i++)	//	縦回数
	{
		for(j = 0;j < 16;j++)	//	横回数
		{
			draw_Tex(j * 20,i * 20,20,20,0,0);
		}
	}

	// タイトル
	disp_tex_4bt( title_2p_1_bm0_0, title_2p_1_bm0_0tlut, 160, 31, 32, 12,SET_PTD );	//	モードセレクト
	disp_tex_4bt( title_2p_2_bm0_0, title_2p_2_bm0_0tlut,  80, 32,208, 12,SET_PTD );	//	ボタン(Ａ：決定 Ｂ：キャンセル)

	//	難易度のみの描画
	load_TexPal( menu_st_stage_dodai_bm0_0tlut );	//	パレットデータの読込み(  ほとんど共通 )
	for(i = 0;i < 4;i++ ){
		load_TexTile_4b( &menu_st_level_dodai_bm0_0[i * 1600],100,32,0,0,99,31 );
		draw_Tex(ms.adj_x + ms.pd[0].bx,ms.pd[0].by + (i << 5),100,32,0,0 );
	}
	load_TexTile_4b( &menu_st_level_dodai_bm0_0[1600 << 2],100,2,0,0,99,1 );
	draw_Tex(ms.adj_x + ms.pd[0].bx,ms.pd[0].by + (32 << 2),100,2,0,0 );

	//	カーソル( EASY.NORMAL.HARD )
	load_TexBlock_4b( difficulty_plate[ms.pd[0].lv],80,16 );	//	ms.pd[0].lv がカーソル位置 と 難易度の値を持つ(0:EASY 1:NORMAL 2:HARD )
	draw_Tex(ms.adj_x + ms.pd[0].bx + 9,ms.pd[0].by + 101 - ms.pd[0].lv * 33,80,16,0,0 );


	//	難易度とステージ設定の描画
	for(i = 0;i < 8;i++ ){
		load_TexTile_4b( &menu_st_stage_dodai_bm0_0[i * 1552],194,16,0,0,193,15 );
		draw_Tex(ms.adj_x + ms.pd[1].bx,ms.pd[1].by + (i << 4),194,16,0,0 );
	}
	load_TexTile_4b( &menu_st_stage_dodai_bm0_0[1552 << 3],194,2,0,0,193,1 );
	draw_Tex(ms.adj_x + ms.pd[1].bx,ms.pd[1].by + (16 << 3),194,2,0,0 );

	//	カーソル( EASY.NORMAL.HARD )
	load_TexBlock_4b( difficulty_plate[ms.pd[1].lv],80,16 );	//	ms.pd[1].lv がカーソル位置 と 難易度の値を持つ(0:EASY 1:NORMAL 2:HARD )
	draw_Tex(ms.adj_x + ms.pd[1].bx + 9,ms.pd[1].by + 101 - ms.pd[1].lv * 33,80,16,0,0 );



	if( evs_mem_data[evs_select_name_no[0]].mem_use_flg & DM_MEM_STORY_CLEAR ){	//	もし、クリアしていた場合、矢印の描画を行う
		//	RL キー
		load_TexTile_4b(menu_st_rl_key_bm0_0,40,13,0,0,39,12 );
		draw_Tex(ms.adj_x + ms.pd[0].bx - 5,ms.pd[0].by + 137 ,20,13,0,0 );		//	Ｒボタン
		draw_Tex(ms.adj_x + ms.pd[1].bx + 84,ms.pd[1].by + 137,20,13,20,0 );	//	Ｌボタン

		//	切り替え矢印
		j = (s16)(sinf( ((ms.cnt*10) % 360) * 3.14159265 /180.0 ) * 8 ) + 84;
		load_TexTile_4b( menu_st_arrow_b_bm0_0 ,92,8,0,0,91,7 );
		draw_Tex( ms.adj_x + ms.pd[0].bx + 16,ms.pd[0].by + 142,8,8,0,0 );				//	LEVEL側 左端
		draw_ScaleTex( ms.adj_x + ms.pd[0].bx + 24,ms.pd[0].by + 142,80,8,j,8,2,0 );	//	LEVEL側 胴体

		draw_TexFlip( ms.adj_x + ms.pd[1].bx + 75,ms.pd[1].by + 142,8,8,0,0,flip_on,flip_off );	//	STAGE側 右端
		draw_ScaleTex( ms.adj_x + ms.pd[1].bx + 75 - j,ms.pd[1].by + 142,80,8,j,8,2,0 );		//	STAGE側 胴体

		load_TexTile_4b( menu_st_arrow_a_bm0_0 ,24,16,0,0,23,15 );	//	矢印部分
		draw_Tex( ms.adj_x + ms.pd[0].bx + 24 + j,ms.pd[0].by + 134,24,16,0,0 );						//	LEVEL側 左端
		draw_TexFlip( ms.adj_x + ms.pd[1].bx + 51 - j,ms.pd[1].by + 134,24,16,0,0,flip_on,flip_off );	//	STAGE側 右端

		//	文字
		load_TexTile_4b( menu_st_stage_str_bm0_0 ,84,10,0,0,83,9 );	//	STAGE SELECT
		draw_Tex( ms.adj_x + ms.pd[0].bx + 20 ,ms.pd[0].by + 137,84,10,0,0 );

		load_TexBlock_4b( menu_st_level_str_bm0_0 ,80,10 );	//	NORMAL GAME
		draw_Tex( ms.adj_x + ms.pd[1].bx - 4 ,ms.pd[1].by + 137,80,10,0,0 );


	}

	//	顔
	load_TexPal( faces[ms.pd[1].speed].pal );
	load_TexBlock_4b( faces[ms.pd[1].speed].pat ,48,48);
	draw_Tex(ms.adj_x + ms.pd[1].bx + 130,ms.pd[1].by + 69,48,48,0,0 );

	//	ステージ番号
	load_TexPal( lv_num_bm0_0tlut );
	load_TexTile_4b( &lv_num_bm0_0[0],8,120,0,0,7,119 );
	if( ms.pd[1].speed > 9 ){
		draw_Tex(ms.adj_x + ms.pd[1].bx + 157,ms.pd[1].by + 39,8,12,0,(ms.pd[1].speed / 10) * 12 );
		draw_Tex(ms.adj_x + ms.pd[1].bx + 165,ms.pd[1].by + 39,8,12,0,(ms.pd[1].speed % 10) * 12 );
	}else{
		draw_Tex(ms.adj_x + ms.pd[1].bx + 161,ms.pd[1].by + 39,8,12,0,ms.pd[1].speed * 12 );
	}


	//	ここからした半透明
	gDPSetRenderMode( gp++,G_RM_XLU_SURF, G_RM_XLU_SURF2 );
	gDPSetPrimColor( gp++,0,0,255,255,255,127);

	//	黒網( ms.pd[1].flag が 0 の場合	ステージ設定側が暗くなり、１の場合、難易度設定側が暗くなる。
	load_TexPal( menu_st_stage_dodai_bm0_0tlut );
	for(i = 0;i < 2;i++ ){
		load_TexBlock_4b( &menu_st_shadow_bm0_0[i * 2016],96,42 );
		draw_Tex(ms.adj_x + ms.pd[1].bx + 98 - 	ms.pd[1].flag * 95,ms.pd[1].by + 14 + i * 42,96,42,0,0 );
	}
	load_TexBlock_4b( &menu_st_shadow_bm0_0[2016 << 1],96,28 );
	draw_Tex(ms.adj_x + ms.pd[1].bx + 98 - ms.pd[1].flag * 95,ms.pd[1].by + 14 + (42 << 1),96,28,0,0 );

	j = (s16)(sinf( ((ms.cnt*10) % 360) * 3.14159265 /180.0 ) * 64.0) + 191;

	//	横矢印
	gDPSetPrimColor(gp++,0,0,j,j,j,0xff);
	load_TexPal( arrow_r_bm0_0tlut );
	load_TexTile_4b( arrow_r_bm0_0,8,11,0,0,7,10 );
	//	難易度のみの場合
	draw_Tex( ms.adj_x + ms.pd[0].bx + 2 + ((ms.cnt / 15) % 2),ms.pd[0].by + 104 - ms.pd[0].lv * 33,8,11,0,0 );

	//	ステージセレクトも含む場合
	switch( ms.pd[1].flag )
	{
	case	0:
		draw_Tex( ms.adj_x + ms.pd[1].bx + 2 + ((ms.cnt / 15) % 2),ms.pd[1].by + 104 - ms.pd[1].lv * 33,8,11,0,0 );
		break;
	case	1:
		//	上下矢印
		load_TexPal( arrow_u_bm0_0tlut );
		load_TexBlock_4b( arrow_u_bm0_0,16,7 );
		//	上矢印
		if( ms.pd[1].speed < 10 + evs_secret_flg ){
			draw_Tex( ms.adj_x + ms.pd[1].bx + 159,ms.pd[1].by + 30 - ((ms.cnt / 15) % 2),16,7,0,0 );
		}
		//	下矢印
		if( ms.pd[1].speed > 1 ){
			draw_TexFlip(ms.adj_x + ms.pd[1].bx + 159,ms.pd[1].by + 53 + ((ms.cnt / 15) % 2),16,7,0,0,flip_off,flip_on );
		}
		break;
	}
}


// １Ｐ(レベルセレクト)用初期化
static void init_1p_play( void )
{
	ms.flow			= 0;		//	初期の処理をレベル・スピード設定にする
	ms.cnt			= 0;		//	内部カウンタのクリア
	ms.ui.mus_no	= evs_mem_data[evs_select_name_no[0]].state_old.p_1p_m;	//	前回設定した音楽番号のセット
	ms.ui.mus_adj_y	= 0;		//	音楽表示Ｙ座標の初期化
	ms.ui.bx		= 96;		//	音楽プレートのＸ座標の設定
	ms.ui.by		= 181;		//	音楽プレートのＹ座標の設定

	if( evs_mem_data[evs_select_name_no[0]].level_data[0].c_level >= 20 ){	//	クリアレベルが 21 以上だった場合
		ms.max_lv	= 21;		//	選択可能最高レベルを 21 にする
	}else{						//	クリアレベルが 20 以下の場合
		ms.max_lv	= 20;		//	選択可能最高レベルを 20 にする
	}

	ms.pd[0].flag	= 0;		//	内部制御をレベル設定している。
	ms.pd[0].ok		= 0;		//	決定フラグのクリア
	ms.pd[0].lv		= evs_mem_data[evs_select_name_no[0]].state_old.p_1p_lv;	//	前回設定したレベルに設定している
	ms.pd[0].lv		= ( ms.pd[0].lv > ms.max_lv )?ms.max_lv:ms.pd[0].lv;		//	設定されたレベルが選択可能最高レベルを超えた場合、選択可能最高レベルにしている
	ms.pd[0].speed	= evs_mem_data[evs_select_name_no[0]].state_old.p_1p_sp;	//	前回設定した速度に設定している
	ms.pd[0].bx		= 96;		//	レベル選択プレートのＸ座標の設定
	ms.pd[0].by		= 47;		//	レベル選択プレートのＹ座標の設定
}

// １Ｐ用メイン
static void calc_1p_play( void )
{
	int i;
	Player_Data *pd;
	UNDER_ITEMS *ui;

	ui = &ms.ui;

	switch( ms.flow ){
		case 0:		// レベル・スピード設定
			pd = &ms.pd[0];
			if( !pd->ok ){
				if( joycur[main_joy[0]] & CONT_UP ){
					if( !pd->flag ){							//	レベル設定側の時
						if( pd -> lv < ms.max_lv ){				//	選択可能最高レベルより下の場合
							pd -> lv ++;						//	レベル上昇
							dm_set_menu_se( SE_mLeftRight );	//	SE設定
						}
					} else {									//	カプセルスピード設定側の時
						if( pd -> speed < 2 ){					//	最高速度より下だった場合
							pd -> speed ++;						//	加速
							dm_set_menu_se( SE_mLeftRight );	//	SE設定
						}
					}
				}else	if( joycur[main_joy[0]] & CONT_DOWN ){
					if( !pd->flag ){							//	レベル設定側の時
						if( pd -> lv > 0 ){						//	最低レベルより上だった場合
							pd -> lv --;						//	レベル下降
							dm_set_menu_se( SE_mLeftRight );	//	SE設定
						}
					} else {									//	カプセルスピード設定側の時
						if( pd -> speed > 0 ){					//	最低速度より上だった場合
							pd -> speed --;						//	減速
							dm_set_menu_se( SE_mLeftRight );	//	SE設定
						}
					}
				}else	if( joyupd[main_joy[0]] & CONT_RIGHT ){
					if( pd -> flag < 1 ){						//	レベル設定側の時
						pd -> flag ++;							//	カプセルスピード設定側に変更
						dm_set_menu_se( SE_mUpDown );			//	SE設定
					}
				}else	if( joyupd[main_joy[0]] & CONT_LEFT ){
					if( pd -> flag > 0 ){						//	カプセルスピード設定側の時
						pd -> flag --;							//	レベル設定側に変更
						dm_set_menu_se( SE_mUpDown );			//	SE設定
					}
				}else	if( joyupd[main_joy[0]] & DM_KEY_OK ){	//	決定
					pd->ok = 1;									//	決定フラグを立てる
					ms.flow = 1;								//	内部処理を音楽設定に変更
					dm_set_menu_se( SE_mDecide );				//	SE設定
					break;
				}else	if( joyupd[main_joy[0]] & CONT_B ){		//	キャンセル
					main_no = MAIN_MODE_SELECT;					//	モードセレクトに戻る
					main_60_out_flg = 0;						//	メインループ脱出
				}
			} else {
				// キャンセル
				if( joyupd[main_joy[0]] & CONT_B ){
					pd->ok = 0;						//	決定解除
				}
			}
			break;
		case 1:		// 音楽設定
			pd = &ms.pd[0];
			if( ui->mus_adj_y == 0 ){	//	音楽グラフィックがスクロール中でない場合
				if( joynew[main_joy[0]] & CONT_UP ){
					if( ui->mus_no > 0 ){						//	音楽番号が０より上の場合
						ui->mus_no --;							//	音楽番号の減少
						ui->mus_adj_y = -16;					//	スクロール値の設定
						dm_set_menu_se( SE_mLeftRight );		//	SE設定
					}
				}else	if( joynew[main_joy[0]] & CONT_DOWN ){
					if( ui->mus_no < 3 ){						//	音楽番号が３より下の場合
						ui->mus_no ++;							//	音楽番号の増加
						ui->mus_adj_y = 16;						//	スクロール値の設定
						dm_set_menu_se( SE_mLeftRight );		//	SE設定
					}
				}else	if( joyupd[main_joy[0]] & DM_KEY_OK ){	//	決定
					main_no = MAIN_12;							//	ゲーム前設定へ
					main_60_out_flg = 0;						//	メインループ脱出
					ms.flow = 2;								//	処理を何もしない(ループ脱出のため)
					dm_set_menu_se( SE_mDecide );				//	SE設定
					break;
				}
			}
			// キャンセル
			if( joyupd[main_joy[0]] & CONT_B ){
				pd->ok = 0;										//	決定フラグをクリアする
				ms.flow = 0;									//	内部処理をレベル・スピード設定に変更
			}
			break;
		case 2:		// 抜け処理
			break;
	}
	ui->mus_adj_y = ( ui->mus_adj_y-1 > 0 )?ui->mus_adj_y-1:( ui->mus_adj_y+1 < 0 )?ui->mus_adj_y+1:0;	//	音楽選択グラフィックのスクロール処理
	ms.cnt ++;		//	内部カウンタの増加( 矢印の点滅等に利用される )
}

// １Ｐ用描画
static void disp_1p_play( void )
{
	int i,j,k,l,mod,adj,col,st,en;
	char work1[10];
	Player_Data *pd;
	UNDER_ITEMS *ui;

	pd = &ms.pd[0];
	ui = &ms.ui;

	//	背景
	gSPDisplayList( gp++, Normal_TNZ_Texture_dl);
	gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );

	load_TexPal(dm_bg_tile_pal_3_bm0_0tlut);
	load_TexTile_4b( &dm_bg_tile_data_bm0_0[0],20,20,0,0,19,19 );
	for(i = 0;i < 12;i++)	//	縦回数
	{
		for(j = 0;j < 16;j++)	//	横回数
		{
			draw_Tex(j * 20,i * 20,20,20,0,0);
		}
	}

	// 下地関係

	disp_tex_4bt( title_2p_1_bm0_0, title_2p_1_bm0_0tlut, 160, 31, 32, 12,SET_PTD );				//	モードセレクト
	disp_tex_4bt( title_2p_2_bm0_0, title_2p_2_bm0_0tlut,  80, 32,208, 12,SET_PTD );				//	ボタン(Ａ：決定 Ｂ：キャンセル)
	disp_tex_4bt( plate_1p_bm0_0, plate_1p_bm0_0tlut, 128,130,ms.pd[0].bx, ms.pd[0].by,SET_PTD );	//	レベル・スピード設定プレート
	disp_tex_4bt( plate_env1_bm0_0,plate_env1_bm0_0tlut,128, 42, ui->bx,ui->by,SET_PTD );			//	音楽設定プレート

	// カプセル
	disp_tex_4bt( cap_sp[0][pd->speed].pat,cap_sp[0][pd->speed].pal,56, 24, pd->bx+65,pd->by+32+(2-pd->speed)*33, SET_PTD );	//	速度のカーソル

	// ウイルス
	for( k = 0; k < 3; k++ ){			//	各色分のループ
		mod = SET_PTD;					//	各色一回目の描画の時は、パレットとグラフィックをロードするように設定している
		for( l = 0; l < 8; l++ ){		//	横ループ
			for( j = 0; j < 13; j++ ){	//	縦ループ
				col = (cap_pos[pd->lv][j] >> ( 14-l*2 )) & 0x3;	//	座標データ( l,j )の配置色を調べる( 0:無し1:赤 2:青 3:黄色 )
				if( (col > 0) && (col-1 == k ) ){				// 	配置がありで、配置色 - 1 が K と同じ場合描画する
					disp_tex_4bt( &virus_bm0_0[k*20],virus_bm0_0tlut,8,5,pd->bx+17+l*5,pd->by+55+j*5,mod );
					mod = SET_D;		//	各色一回目以降の描画の時は、パレットをグラフィックをロードしないように設定している
				}
			}
		}
	}
	// レベル数値
	adj = ( pd->lv < 10 )?4:0;
	itoa( pd->lv,work1,10 );
	for( j = 0; work1[j] != '\0'; j++ ){
		disp_tex_4bt( &lv_num_bm0_0[(work1[j]-'0')*48],lv_num_bm0_0tlut,8,12,pd->bx+37+j*8+adj,pd->by+42,SET_PTD );
	}
	// レベルゲージ
	gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,pd->by+39+(20-pd->lv)*4,319,239);			//	ウイルスレベルゲージの上下は、シザリング(描画範囲指定)で行っている
	disp_tex_4bt( lv_guage_bm0_0,lv_guage_bm0_0tlut,8,84, pd->bx+6,pd->by+38,SET_PTD );
	gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,0,319,239);								//	シザリング(描画範囲指定)を元に戻す

	col = (s16)(sinf( ((ms.cnt*10) % 360) * 3.14159265 /180.0 ) * 64.0) + 191;	//	矢印点滅用数値の計算

	// 音楽パネル
	gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );
	gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,ui->by+18,319,ui->by+18+16);	//	スクロール処理用にシザリング(描画範囲指定)を設定している
	if( ms.flow == 1 || ui->mus_adj_y != 0 ){		//	内部処理が音楽設定かスクロール中の場合( ui->mus_adj_y != 0 )
		for( i = 0; i < 4; i ++ ){					//	音楽グラフィックを毎回４つ描画する( シザリングで範囲外は消える )
				//	15イントでアニメーションするようになっている((ms.cnt/30)%2)
			disp_tex_4bt( music[i][(ms.cnt/30)%2].pat,music[i][(ms.cnt/30)%2].pal,48,16,ui->bx+42,ui->by+18+(i-ui->mus_no)*16+ui->mus_adj_y,SET_PTD );
		}
	} else {	//	内部処理がレベル・スピード設定の場合、選択している音楽番号( ui->mus_no )のグラフィックだけを描画する(アニメーションしない)
		disp_tex_4bt( music[ui->mus_no][0].pat,music[ui->mus_no][0].pal,48,16,ui->bx+42,ui->by+18,SET_PTD );
	}

	gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,ui->by+13,319,ui->by+13+16);	//	シザリング(描画範囲指定)を元に戻す
	gSPDisplayList( gp++, Normal_XNZ_Texture_dl);
	switch( ms.flow ){
		case 0:
			// 音楽暗パネル
			gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,0,319,239);
			gDPSetPrimColor( gp++, 0,0, 255,255,255,127 );
			disp_tex_4bt( music_sm_bm0_0,music_sm_bm0_0tlut,128,42, ui->bx,ui->by,SET_PTD );	//	( 半透明黒幕をかける )

			// 上段暗パネル
			gDPSetPrimColor( gp++, 0,0, 255,255,255,127 );
			if( pd->ok ){	// あまり意味が無い
				disp_tex_4bt( plate_l_a_bm0_0,plate_l_a_bm0_0tlut,128,130, pd->bx,pd->by,SET_PTD );
			} else {
				//	pd->flag が ０ の時はスピード設定側が、１ の時はレベル設定側が暗くなる( 半透明黒幕をかける )
				disp_tex_4bt( plate_l_p_bm0_0,plate_l_p_bm0_0tlut,64,112, pd->bx+4+(1-pd->flag)*60, pd->by+14,SET_PTD );
			}
			// 上段用矢印
			if( !pd->ok ){
				gDPSetPrimColor( gp++, 0,0, col,col,col,255 );
				if( pd->flag ){
					disp_tex_4bt( arrow_r_bm0_0,arrow_r_bm0_0tlut,8,11, pd->bx+59+(ms.cnt/15)%2,pd->by+38+(2-pd->speed)*33,SET_PTD );
				} else {
					disp_tex_4bt( arrow_r_bm0_0,arrow_r_bm0_0tlut,8,11, pd->bx-2+(ms.cnt/15)%2,pd->by+34+(20-pd->lv)*4,SET_PTD );
				}
			}
			break;
		case 1:
			// 下段用矢印
			gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,0,319,239);
			gDPSetPrimColor( gp++, 0,0, col,col,col,255 );
			if( ui->mus_no > 0 ){
				disp_tex_4bt( arrow_u_bm0_0,arrow_u_bm0_0tlut,16,7,ui->bx+62,ui->by+11-(ms.cnt/15)%2,SET_PTD );
			}
			if( ui->mus_no < 3 ){
				disp_tex_4bt( arrow_d_bm0_0,arrow_d_bm0_0tlut,16,7,ui->bx+62,ui->by+34+(ms.cnt/15)%2,SET_PTD );
			}
			// 上段暗パネル
			gSPDisplayList( gp++, Normal_XNZ_Texture_dl);
			gDPSetPrimColor( gp++, 0,0, 255,255,255,127 );
			disp_tex_4bt( plate_l_a_bm0_0,plate_l_a_bm0_0tlut,128,130, pd->bx,pd->by,SET_PTD );
			break;
		case 2:
			// 音楽暗パネル
			gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,0,319,239);
			gDPSetPrimColor( gp++, 0,0, 255,255,255,127 );
			disp_tex_4bt( music_sm_bm0_0,music_sm_bm0_0tlut,128,42, ui->bx,ui->by,SET_PTD );
			// 上段暗パネル
			gDPSetPrimColor( gp++, 0,0, 255,255,255,127 );
			disp_tex_4bt( plate_l_a_bm0_0,plate_l_a_bm0_0tlut,128,130, pd->bx,pd->by,SET_PTD );
			break;
	}

	// ＯＫ
	gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );
	if( pd->ok ){
		disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+95,pd->by+4,SET_PTD );
	}
}

// ２Ｐ用初期化
static void init_2p_play( u8 flag, u8 mode )
{
	int i,j;

	if( flag ){

		if( main_old == MAIN_GAME){		//	ゲームから設定に来た場合
			ms.flow		= 1;			//	レベル・スピード・音楽設定
		}else{							//	ゲーム以外(モードセレクト、名前入力)から設定に来た場合
			ms.flow		= 0;			//	キャラ選択
		}

		ms.sub_flow		= 0;			//	内部制御処理を人間キー入力に設定
		ms.adj_x		= 0;			//	画面Ｘ座標
		ms.mv_x			= 20;			//	切り替え(キャラ選択 <-> レベル・スピード・音楽設定)のスクロール速度の設定
		ms.max_chara	= 11 + evs_secret_flg;	//	最大選択可能キャラ数の設定( 通常 11 だが ピーチが使用可能の場合(evs_secret_flg == 1) 12 )

		ms.ui.com_lv = 1;				//	CPU のレベルを NORMAL に設定している

		if( evs_gamesel == GSL_2PLAY ){	//	MAN VS MAN の場合
			for( i = 0;i < 2;i++ ){
				ms.pd[i].chara	= evs_mem_data[evs_select_name_no[i]].state_old.p_vm_no;	//	前回使ったキャラクター番号の設定
				ms.pd[i].lv		= evs_mem_data[evs_select_name_no[i]].state_old.p_vm_lv;	//	前回のウイルスレベルの設定
				ms.pd[i].speed	= evs_mem_data[evs_select_name_no[i]].state_old.p_vm_sp;	//	前回のウイルススピードの設定
			}
			ms.ui.stg_no = evs_mem_data[evs_select_name_no[0]].state_old.p_vm_st;			//	前回の背景番号の設定
			ms.ui.mus_no = evs_mem_data[evs_select_name_no[0]].state_old.p_vm_m;			//	前回の音楽の設定
		}else	if( evs_gamesel == GSL_VSCPU ){	//	MAN VS CPUの場合
			for( i = 0;i < 2;i++ ){
				ms.pd[i].chara	= evs_mem_data[evs_select_name_no[0]].state_old.p_vc_no[i];	//	前回使ったキャラクター番号の設定
				ms.pd[i].lv		= evs_mem_data[evs_select_name_no[0]].state_old.p_vc_lv[i];	//	前回のウイルスレベルの設定
				ms.pd[i].speed	= evs_mem_data[evs_select_name_no[0]].state_old.p_vc_sp[i];	//	前回のウイルススピードの設定
			}
			ms.ui.com_lv = evs_mem_data[evs_select_name_no[0]].state_old.p_vc_cp_lv;		//	前回の CPU難易度の設定
			ms.ui.stg_no = evs_mem_data[evs_select_name_no[0]].state_old.p_vc_st;			//	前回の背景番号の設定
			ms.ui.mus_no = evs_mem_data[evs_select_name_no[0]].state_old.p_vc_m;			//	前回の音楽の設定
		}

		// 人間 or ＣＯＭ
		ms.pd[0].play	= 0;		//	人かＣＯＭの判別フラグの設定
		ms.pd[1].play	= mode;		//	0:MAN 1:CPU

		// 下段諸設定( 音楽・背景の設定部分 )
		ms.ui.bx	= ( ms.pd[1].play )?30:33;	//	Ｘ座標 2PLAY と VSCPU で座標を変えている(表示物が異なるため)
		ms.ui.by	= 181;						//	Ｙ座標の設定
		ms.ui.pos	= 0;						//	下段パネルカーソル初期位置を左端に設定する
		ms.ui.com_adj_y = 0;					//	ＣＰＵ難易度選択表示のスクロール移動値のクリア
		ms.ui.stg_adj_y = 0;					//	背景選択表示のスクロール移動値のクリア
		ms.ui.mus_adj_y = 0;					//	音楽選択表示のスクロール移動値のクリア

		// 選択可能ステージチェック
		for( i = 0, ms.ui.stg_sel_num = 0; i < 11; i ++ ){
			if( evs_clear_stage_flg[i] ){
				ms.ui.stg_tbl[ms.ui.stg_sel_num++] = i;
			}
		}

		// 顔関係
		for( i = 0;i < 2;i++ ){
			reset_face_size( &ms.pd[i].fd );	// 座標初期化
		}
	}
	// 基本位置
	ms.pd[0].by = ms.pd[1].by = 47;		//	基本Ｙ座標は共通で固定
	ms.pd[1].bx = 161;					//	２Ｐ側の基本Ｘ座標の設定
	if( ms.flow == 0 ){
		ms.pd[0].bx = 97;				//	キャラ選択の場合の１Ｐ側の基本Ｘ座標の設定
	} else {
		ms.pd[0].bx = 33;				//	レベル・スピード設定の場合の１Ｐ側の基本Ｘ座標の設定
	}
	// カウンタ
	ms.cnt		= 0;					//	内部カウンタのクリア

	for( i = 0; i < 2; i ++ ){
		ms.pd[i].flag			= 0;	//	レベル・スピード設定の内部選択をレベル設定にする
		ms.pd[i].ok				= 0;	//	決定フラグのクリア
	}
}
// ２Ｐ用メイン
static void calc_2p_play( void )
{
	int i,j,fl;
	float sc_s[] = {0.125, 0.149, 0.163, 0.176, 0.191, 0.204, 0.218, 0.232, 0.246, 0.250};	//	４段目から３段目までのスケール値
	float sc_m[] = {0.250, 0.288, 0.316, 0.343, 0.371, 0.399, 0.427, 0.454, 0.482, 0.500};	//	３段目から２段目までのスケール値
	float sc_b[] = {0.500, 0.566, 0.621, 0.677, 0.732, 0.788, 0.843, 0.899, 0.954, 1.000};	// 	２段目から１段目までのスケール値

	Face_Data   *fd;
	Player_Data *pd;
	UNDER_ITEMS *ui;

	switch( ms.flow ){
		case 0:		// キャラ選択
			switch( ms.sub_flow ){
				case 0:			// 人間設定
					for( i = 0; i < 2; i ++ ){
						pd = &ms.pd[i];
						fd = &ms.pd[i].fd;
						if( !pd->play ){		// 人間のみ操作可
							if( !pd->ok ){			// 選択中
								if( !fd->move ){		//	顔のグラフィックがスクロール中でない場合( !fd->move )
									if( joynew[main_joy[i]] & CONT_UP ){
										dm_set_menu_se( SE_mLeftRight );									//	SE設定
										pd->chara = ( pd->chara - 1 >= 0 )?pd->chara - 1:ms.max_chara - 1;	//	キャラ番号が０以下の場合、最大選択キャラの番号に設定
										fd->move = -1;														//	スクロール中フラグの設定( スクロール方向も兼ねる )
										pd->flag = 1;														//	スクロール処理フラグを上方向スクロールに設定
									}else	if( joynew[main_joy[i]] & CONT_DOWN  ){
										dm_set_menu_se( SE_mLeftRight );									//	SE設定
										pd->chara = ( pd->chara + 1 < ms.max_chara )?pd->chara + 1:0;		//	キャラ番号が最大選択キャラの番号以上になった場合、０に設定
										fd->move = 1;														//	スクロール中フラグの設定( スクロール方向も兼ねる )
										pd->flag = 2;														//	スクロール処理フラグを下方向スクロールに設定
									}else	if( joyupd[main_joy[i]] & DM_KEY_OK  ){				// 決定
										for( j = 0,fl = 1; j < 2; j++ ){
											//	相手が決定状態で、相手の選択したキャラと自分の選択したキャラが同じだった場合、処理フラグをクリアする( fl = 0 )
											if( ms.pd[j].ok && pd->chara == ms.pd[j].chara ) fl = 0;
										}
										if( fl ){												//	自分のキャラが決定した場合( 相手と違うキャラか、相手がまだ決定状態でなかった場合 )
											dm_set_menu_se( SE_mDecide );						//	SE設定
											pd->ok = 1;											//	決定フラグを立てる

											for( j = 0,fl = 1; j < 2; j++ ){					//	決定状態を調べる
												//	プレイヤーが人間でまだ決定していなかった場合、処理フラグをクリアする( fl = 0 )
												if( !ms.pd[j].play && !ms.pd[j].ok ) fl = 0;
											}
											if( fl ){											//	処理を次に進める
												//	２ＰがＣＰＵの場合、ＣＰＵのキャラクター選択へ( ms.sub_flow = 1 ),２Ｐが人の場合、画面のスクロール処理へ( ms.sub_flow = 254 )
												ms.sub_flow = ( ms.pd[1].play == 0 )?254:1;
												break;
											}
										}
									}else	if( joyupd[main_joy[i]] & CONT_B ){	//	キャンセル
										ms.sub_flow = 255;		//	メインループ脱出処理に設定
									}
								}
								//	顔のスクロール処理
								set_face_size(pd,fd,ms.cnt);
							} else {
								// キャンセル
								if( joyupd[main_joy[i]] & CONT_B && !fd->move ){	//	顔のグラフィックがスクロール中でない場合( !fd->move )
									pd->ok = 0;										//	決定の解除
								}
							}
						}
					}
					break;
				case 1:			// ＣＯＭ設定
					pd = &ms.pd[1];
					fd = &ms.pd[1].fd;

					if( !fd->move ){	//	顔のグラフィックがスクロール中でない場合( !fd->move )
						if( joynew[main_joy[0]] & CONT_UP  ){
							dm_set_menu_se( SE_mLeftRight );									//	SE設定
							pd->chara = ( pd->chara - 1 >= 0 )?pd->chara - 1:ms.max_chara - 1;	//	キャラ番号が０以下の場合、最大選択キャラの番号に設定
							fd->move = -1;														//	スクロール中フラグの設定( スクロール方向も兼ねる )
							pd->flag = 1;														//	スクロール処理フラグを上方向スクロールに設定
						}else	if( joynew[main_joy[0]] & CONT_DOWN ){
							dm_set_menu_se( SE_mLeftRight );									//	SE設定
							pd->chara = ( pd->chara + 1 < ms.max_chara )?pd->chara + 1:0;		//	キャラ番号が最大選択キャラの番号以上になった場合、０に設定
							fd->move = 1;														//	スクロール中フラグの設定( スクロール方向も兼ねる )
							pd->flag = 2;														//	スクロール処理フラグを下方向スクロールに設定
						}else	if( joyupd[main_joy[0]] & DM_KEY_OK ){							//	決定
							if( pd->chara != ms.pd[0].chara ){									//	１Ｐが選択したキャラと違う場合
								dm_set_menu_se( SE_mDecide );									//	SE設定
								pd->ok = 1;														//	決定フラグの設定
								ms.sub_flow = 254;												//	画面のスクロール処理へ
							}
						}else	if( joyupd[main_joy[0]] & CONT_B ){								// キャンセル
							ms.pd[0].ok = 0;													//	１Ｐの決定フラグノクリア
							ms.sub_flow = 0;													//	１Ｐ(人間）のキャラ選択に戻る
						}
					}
					set_face_size(pd,fd,ms.cnt);												//	顔のスクロール処理
					break;
				case 253:								//	フレームイン( 顔のプレートが画面内に入ってくる処理 )
					if( ms.adj_x - ms.mv_x > 0 ){		//	指定座標まで移動していない場合
						ms.adj_x -= ms.mv_x;
					} else {							//	指定座標まで移動した場合
						ms.adj_x -= ms.mv_x;
						ms.sub_flow = 0;				//	処理をキャラ選択に変更
					}
					break;
				case 254:								//	フレームアウト( 顔のプレートが画面外に出て行く処理 )
					if( ms.adj_x + ms.mv_x < 320 ){		//	画面外に出ていっていない場合
						ms.adj_x += ms.mv_x;
					} else {							//	画面外に出ていった場合
						ms.adj_x += ms.mv_x;
						ms.flow = 1;					//	メインの処理をレベル・スピード設定へ
						ms.sub_flow = 253;				//	フレームイン( レベル・スピード設定のプレートが画面内に入ってくる )処理に変更
						init_2p_play(0,0);				//	設定初期化( 座標系と決定関係のフラグのみクリアされる )
					}
					break;
				case 255:								//	プレイヤー人数選択へ
					main_no = MAIN_MODE_SELECT;			//	モードセレクトに戻る
					main_60_out_flg = 0;				//	メインループ脱出
					break;
			}
			ms.cnt ++;									//	内部カウンタの増加( 矢印の点滅等に利用される )
			break;
		case 1:		// レベル選択
			ui = &ms.ui;

			switch( ms.sub_flow ){
				case 0:		// 人間設定
					for( i = 0; i < 2; i ++ ){
						pd = &ms.pd[i];
						if( !pd->play ){		// 人間のみ操作可
							if( !pd->ok ){			// 選択中
								if( joycur[main_joy[i]] & CONT_UP ){
									if( !pd->flag ){							//	レベル設定側の時
										if( pd -> lv < 20 ){					//	選択可能最高レベル( 20 )より下の場合
											pd -> lv ++;						//	レベル上昇
											dm_set_menu_se( SE_mLeftRight );	//	SE設定
										}
									} else {									//	カプセルスピード設定側の時
										if( pd -> speed < 2 ){					//	最高速度より下だった場合
											pd -> speed ++;						//	加速
											dm_set_menu_se( SE_mLeftRight );	//	SE設定
										}
									}
								}else	if( joycur[main_joy[i]] & CONT_DOWN	){
									if( !pd->flag ){							//	レベル設定側の時
										if( pd -> lv > 0 ){						//	最低レベルより上だった場合
											pd -> lv --;						//	レベル下降
											dm_set_menu_se( SE_mLeftRight );	//	SE設定
										}
									} else {									//	カプセルスピード設定側の時
										if( pd -> speed > 0 ){					//	最低速度より上だった場合
											pd -> speed --;						//	減速
											dm_set_menu_se( SE_mLeftRight );	//	SE設定
										}
									}
								}else	if( joyupd[main_joy[i]] & CONT_RIGHT ){
									if( pd->flag < 1 ){							//	レベル設定側の時
										pd->flag ++;							//	カプセルスピード設定側に変更
										dm_set_menu_se( SE_mUpDown );			//	SE設定
									}
								}else	if( joyupd[main_joy[i]] & CONT_LEFT ){
									if( pd->flag > 0 ){							//	カプセルスピード設定側の時
										pd->flag --;							//	レベル設定側に変更
										dm_set_menu_se( SE_mUpDown );			//	SE設定
									}
								}else	if( joyupd[main_joy[i]] & DM_KEY_OK ){	//	決定
									pd->ok = 1;									//	決定フラグを立てる
									dm_set_menu_se( SE_mDecide );				//	SE設定

									for( j = 0,fl = 1; j < 2; j++ ){
										//	プレイヤーが人間でまだ決定していなかった場合、処理フラグをクリアする( fl = 0 )
										if( !ms.pd[j].play && !ms.pd[j].ok ) fl = 0;
									}
									if( fl ){	//	人間が全員決定した場合
										//	２ＰがＣＰＵの場合、ＣＰＵのキャラクター選択へ( ms.sub_flow = 1 ),２Ｐが人の場合、音楽設定処理へ( ms.sub_flow = 2 )
										ms.sub_flow = ( ms.pd[1].play == 0 )?2:1;
										break;
									}
								}else	if( joyupd[main_joy[i]] & CONT_B ){		//	キャンセル
									ms.sub_flow = 254;							//	画面のスクロール処理へ
								}
							} else {				// 決定済み
								if( joyupd[main_joy[i]] & CONT_B ){				//	キャンセル
									pd->ok = 0;									//	決定フラグのクリア
								}
							}
						}
					}
					break;
				case 1:		// ＣＯＭ設定
					pd = &ms.pd[1];

					if( joycur[main_joy[0]] & CONT_UP ){
						if( !pd->flag ){							//	レベル設定側の時
							if( pd -> lv < 20 ){					//	選択可能最高レベル( 20 )より下の場合
								pd -> lv ++;						//	レベル上昇
								dm_set_menu_se( SE_mLeftRight );	//	SE設定
							}
						} else {									//	カプセルスピード設定側の時
							if( pd -> speed < 2 ){					//	最高速度より下だった場合
								pd -> speed ++;						//	加速
								dm_set_menu_se( SE_mLeftRight );	//	SE
							}
						}
					}else	if( joycur[main_joy[0]] & CONT_DOWN ){
						if( !pd->flag ){							//	レベル設定側の時
							if( pd -> lv > 0 ){						//	最低レベルより上だった場合
								pd -> lv --;						//	レベル下降
								dm_set_menu_se( SE_mLeftRight );	//	SE設定
							}
						} else {									//	カプセルスピード設定側の時
							if( pd -> speed > 0 ){					//	最低速度より上だった場合
								pd -> speed --;						//	減速
								dm_set_menu_se( SE_mLeftRight );	//	SE
							}
						}
					}else	if( joyupd[main_joy[0]] & CONT_RIGHT ){
						if( pd->flag < 1 ){							//	レベル設定側の時
							pd->flag ++;							//	カプセルスピード設定側に変更
							dm_set_menu_se( SE_mUpDown );			//	SE設定
						}
					}else	if( joyupd[main_joy[0]] & CONT_LEFT ){
						if( pd->flag > 0 ){							//	カプセルスピード設定側の時
							pd->flag --;							//	レベル設定側に変更
							dm_set_menu_se( SE_mUpDown );			//	SE設定
						}
					}else	if( joyupd[main_joy[0]] & DM_KEY_OK ){	//	決定
						pd->ok = 1;									//	決定フラグを立てる
						ms.sub_flow = 2;							//	音楽設定処理へ
						dm_set_menu_se( SE_mDecide );				//	SE設定
					}else	if( joyupd[main_joy[0]] & CONT_B ){		//	キャンセルチェック
						ms.pd[0].ok = 0;							//	１Ｐの決定フラグ解除
						ms.sub_flow = 0;							//	１Ｐ(人間）のレベル・スピード設定に戻る
					}
					break;
				case 2:		// 音楽設定
					pd = &ms.pd[0];

					if( !ms.pd[1].play ){		// 二人とも人間
						if( joyupd[main_joy[0]] & CONT_RIGHT ){
							if( ui -> pos < 1 ){				//	背景選択側の時
								ui -> pos ++;					//	音楽設定側に変更
								dm_set_menu_se( SE_mUpDown );	//	SE設定
							}
						}else	if( joyupd[main_joy[0]] & CONT_LEFT ){
							if( ui -> pos > 0 ){				//	音楽設定側の時
								ui -> pos --;					//	背景選択側に変更
								dm_set_menu_se( SE_mUpDown );	//	SE設定
							}
						}
						switch( ui->pos ){
							case 0:				//	ステージ( 背景選択 )
								if( ui->stg_adj_y == 0 ){	//	背景選択グラフィックがスクロールしていないとき
									if( joynew[main_joy[0]] & CONT_UP ){
										if( ui->stg_no > 0 ){					//	背景番号が１以上の時
											ui->stg_no --;						//	背景番号の減少
											ui->stg_adj_y = -16;				//	スクロール値の設定
											dm_set_menu_se( SE_mLeftRight );	//	SE設定
										}
									}else	if( joynew[main_joy[0]] & CONT_DOWN ){
										if( ui->stg_no < ui->stg_sel_num - 1){		//	背景番号が選択可能最大番号より下の時
											ui->stg_no ++;						//	背景番号の増加
											ui->stg_adj_y = 16;					//	スクロール値の設定
											dm_set_menu_se( SE_mLeftRight );	//	SE設定
										}
									}
								}
								break;
							case 1:				// 音楽選択
								if( ui->mus_adj_y == 0 ){	//	音楽グラフィックがスクロールしていないとき
									if( joynew[main_joy[0]] & CONT_UP ){
										if( ui->mus_no > 0 ){					//	音楽番号が０より上の場合
											ui->mus_no --;						//	音楽番号の減少
											ui->mus_adj_y = -16;				//	スクロール値の設定
											dm_set_menu_se( SE_mLeftRight );	//	SE
										}
									}else	if( joynew[main_joy[0]] & CONT_DOWN ){
										if( ui->mus_no < 3 ){					//	音楽番号が３より下の場合
											ui->mus_no ++;						//	音楽番号の増加
											ui->mus_adj_y = 16;					//	スクロール値の設定
											dm_set_menu_se( SE_mLeftRight );	//	SE設定
										}
									}
								}
								break;
						}
					} else {					// ２ＰがＣＯＭ
						if( joyupd[main_joy[0]] & CONT_RIGHT ){
							if( ui -> pos < 2 ){					//	カーソル位置が右端( 音楽選択 )でない時
								ui -> pos ++;						//	右に移動
								dm_set_menu_se( SE_mUpDown );		//	SE設定
							}
						}else	if( joyupd[main_joy[0]] & CONT_LEFT ){
							if( ui -> pos > 0 ){					//	カーソル位置が左端( 背景選択 )でない時
								ui -> pos --;						//	左に移動
								dm_set_menu_se( SE_mUpDown );		//	SE設定
							}
						}
						switch( ui->pos ){
							case 0:				//	ステージ( 背景選択 )
								if( ui->stg_adj_y == 0 ){	//	背景選択グラフィックがスクロールしていないとき
									if( joynew[main_joy[0]] & CONT_UP ){
										if( ui->stg_no > 0 ){					//	背景番号が１以上の時
											ui->stg_no --;						//	背景番号の減少
											ui->stg_adj_y = -16;				//	スクロール値の設定
											dm_set_menu_se( SE_mLeftRight );	//	SE設定
										}
									}else	if( joynew[main_joy[0]] & CONT_DOWN ){
										if( ui->stg_no < ui->stg_sel_num - 1 ){		//	背景番号が選択可能最大番号より下の時
											ui->stg_no ++;						//	背景番号の増加
											ui->stg_adj_y = 16;					//	スクロール値の設定
											dm_set_menu_se( SE_mLeftRight );	//	SE設定
										}
									}
								}
								break;
							case 1:				// ＣＯＭレベル
								if( ui->com_adj_y == 0 ){	//	ＣＯＭレベル選択グラフィックがスクロールしていないとき
									if( joynew[main_joy[0]] & CONT_UP ){
										if( ui->com_lv < 2 ){					//	難易度が NORMAL 以下の時
											ui->com_lv ++;						//	難易度の上昇
											ui->com_adj_y = -16;				//	スクロール値の設定
											dm_set_menu_se( SE_mLeftRight );	//	SE設定
										}
									}else	if( joynew[main_joy[0]] & CONT_DOWN ){
										if( ui->com_lv > 0 ){					//	難易度が NORMAL 以上の時
											ui->com_lv --;						//	難易度の減少
											ui->com_adj_y = 16;					//	スクロール値の設定
											dm_set_menu_se( SE_mLeftRight );	//	SE設定
										}
									}
								}
								break;
							case 2:				// 音楽
								if( ui->mus_adj_y == 0 ){	//	音楽グラフィックがスクロールしていないとき
									if( joynew[main_joy[0]] & CONT_UP ){
										if( ui->mus_no > 0 ){					//	音楽番号が０より上の場合
											ui->mus_no --;						//	音楽番号の減少
											ui->mus_adj_y = -16;				//	スクロール値の設定
											dm_set_menu_se( SE_mLeftRight );	//	SE
										}
									}else	if( joynew[main_joy[0]] & CONT_DOWN ){
										if( ui->mus_no < 3 ){					//	音楽番号が３より下の場合
											ui->mus_no ++;						//	音楽番号の増加
											ui->mus_adj_y = 16;					//	スクロール値の設定
											dm_set_menu_se( SE_mLeftRight );	//	SE設定
										}
									}
								}
								break;
						}
					}

					// キャンセルチェック
					for( i = 0; i < 2; i ++ ){
						if( !ms.pd[i].play && joyupd[main_joy[i]] & CONT_B ){	//	ボタンを押したプレイヤーが人だった場合
							ms.pd[i].ok = 0;		//	ボタンを押したプレイヤーの決定フラグの解除
							ms.sub_flow = 0;		//	レベル・スピード設定処理に戻る
							break;
						}
					}
					// 決定
					if( joyupd[main_joy[0]] & DM_KEY_OK && ui->stg_adj_y == 0 && ui->com_adj_y == 0 && ui->mus_adj_y == 0 ){
						dm_set_menu_se( SE_mDecide );	//	SE設定
						ms.sub_flow = 255;				//	ゲーム設定処理へ
						break;
					}
					break;
				case 253:								//	フレームイン( レベル・スピード設定のプレートが画面内に入ってくる )
					if( ms.adj_x - ms.mv_x > 0 ){		//	指定座標まで移動していない場合
						ms.adj_x -= ms.mv_x;
					} else {							//	指定座標まで移動した場合
						ms.adj_x -= ms.mv_x;
						ms.sub_flow = 0;				//	処理をレベル・スピード設定に変更
					}
					break;
				case 254:								//	フレームアウト( レベル・スピード設定のプレートが画面外に出て行く )
					if( ms.adj_x + ms.mv_x < 320 ){		//	画面外に出ていっていない場合
						ms.adj_x += ms.mv_x;
					} else {							//	画面外に出ていった場合
						ms.adj_x += ms.mv_x;
						ms.flow = 0;					//	メインの処理をキャラクタ選択へ
						ms.sub_flow = 253;				//	フレームイン( 顔のプレートが画面内に入ってくる処理 )
						init_2p_play(0,0);				//	設定初期化( 座標系と決定関係のフラグのみクリアされる )
					}
					break;
				case 255:		// ゲームへ
					main_no = MAIN_12;					//	ゲーム前設定へ
					main_60_out_flg = 0;				//	メインループ脱出
					break;
			}
			ui->stg_adj_y = ( ui->stg_adj_y-1 > 0 )?ui->stg_adj_y-1:( ui->stg_adj_y+1 < 0 )?ui->stg_adj_y+1:0;	//	背景選択グラフィックのスクロール処理
			ui->com_adj_y = ( ui->com_adj_y-1 > 0 )?ui->com_adj_y-1:( ui->com_adj_y+1 < 0 )?ui->com_adj_y+1:0;	//	CPU難易度選択グラフィックのスクロール処理
			ui->mus_adj_y = ( ui->mus_adj_y-1 > 0 )?ui->mus_adj_y-1:( ui->mus_adj_y+1 < 0 )?ui->mus_adj_y+1:0;	//	音楽選択グラフィックのスクロール処理
			ms.cnt ++;			//	内部カウンタの増加( 矢印の点滅等に利用される )
			break;
	}
}

// ２Ｐ用描画
static void disp_2p_play( void )
{
	int i,j,k,fl,adj,col,top,mx;
	int l,mod,st,en;
	char cwo[10];
	char work1[10];

	u8 *smb;
	u8 *sms;
	u16 *smb_p;
	u16 *sms_p;

	PIC2 *pc;
	Player_Data *pd;
	Face_Data	*fd;
	UNDER_ITEMS *ui;

	ui = &ms.ui;
	col = (s16)(sinf( ((ms.cnt*10) % 360) * 3.14159265 /180.0 ) * 64.0) + 191;

	// 背景
	gSPDisplayList( gp++, Normal_TNZ_Texture_dl);
	gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );

	load_TexPal(dm_bg_tile_pal_3_bm0_0tlut);
	load_TexTile_4b( &dm_bg_tile_data_bm0_0[0],20,20,0,0,19,19 );
	for(i = 0;i < 12;i++)	//	縦回数
	{
		for(j = 0;j < 16;j++)	//	横回数
		{
			draw_Tex(j * 20,i * 20,20,20,0,0);
		}
	}

	// タイトル
	disp_tex_4bt( title_2p_1_bm0_0, title_2p_1_bm0_0tlut, 160, 31, 32, 12,SET_PTD );	//	モードセレクト
	disp_tex_4bt( title_2p_2_bm0_0, title_2p_2_bm0_0tlut,  80, 32,208, 12,SET_PTD );	//	ボタン(Ａ：決定 Ｂ：キャンセル)

	switch( ms.flow ){
		case 0:		// キャラ選択
			for( i = 0; i < 2; i ++ ){
				pd = &ms.pd[i];
				fd = &ms.pd[i].fd;
				pc = &plcom[i][pd->play];

				// パネル
				gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,0,319,239 );
				gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );
				disp_tex_4bt( plate4p_char[i].pat, plate4p_char[i].pal, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
				disp_tex_4bt( pc->pat, pc->pal, pc->width, pc->height, pd->bx+4+ms.adj_x, pd->by+4,SET_PTD );

				// 顔
				gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,pd->by+25,319,pd->by+121 );
				gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );
				top = ( pd->chara >= 2 )?pd->chara-2:ms.max_chara-( 2-pd->chara );	//	一番最初に描画する顔の設定
				if( fd->move == 1 ){							//	上方向にスクロールしている場合
					top = ( top - 1 >= 0 )?top-1:ms.max_chara-1;
				} else if( fd->move == -1 ){					//	下方向にスクロールしている場合
					top = ( top + 1 < ms.max_chara )?top+1:0;
				}
				for( j = 0; j < 5; j ++ ){
					for( k = 0, fl = 1; k < 4; k ++ ){
						if( ms.pd[k].ok && ms.pd[k].chara == ((top+j)%ms.max_chara) && i != k )fl = 0;	//	相手が決定していて同じキャラか調べている
					}
					if( !fl || (pd->ok && j != 2) ){					//	相手の決定したキャラと同じキャラの場合
						gDPSetPrimColor( gp++, 0,0, 100,100,100,255 );	//	暗く描画する
					} else {
						gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );	//	明るく描画する
					}
					disp_tex_4bt_sc( faces[(top+j)%ms.max_chara].pat, faces[(top+j)%ms.max_chara].pal, 48, 48, pd->bx+7+ms.adj_x, pd->by+49+fd->pos_y[j], 1.0, fd->sc[j], 0 );
				}
				// 顔枠
				if( (ms.sub_flow == 0 && !pd->ok && !pd->play) || (ms.sub_flow == 1 && !pd->ok) ){	//	まだ決定していない場合
					j = ( ( ms.cnt/30 ) % 2 ) * 127 + 128;											//	点滅させる
				} else {																			//	決定していない場合
					j = 255;																		//	点滅させない
				}
				gDPSetPrimColor( gp++, 0,0, j,j,j,255 );
				disp_tex_4bt( face_frame[i].pat, face_frame[i].pal, 48, 48, ms.pd[i].bx+7+ms.adj_x, ms.pd[i].by+49,SET_PTD );
			}

			// 顔グラデ(インテンシティ使用)
			gSPDisplayList( gp++, Intensity_XNZ_Texture_dl);
			gDPSetPrimColor( gp++,0, 0, 0, 0, 0, 255);
			gDPSetEnvColor( gp++, 196, 196, 196, 255);
			for( i = 0; i < 2; i ++ ){
				pd = &ms.pd[i];

				gDPLoadTextureTile_4b( gp++, face4p_sm_bm0_0, G_IM_FMT_I,48,24,0,0,48-1,24-1,
								0, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK,G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
				gSPScisTextureRectangle( gp++, (pd->bx+7+ms.adj_x)<<2, (pd->by+25)<<2, (pd->bx+7+48+ms.adj_x)<<2, (pd->by+25+24)<<2,
								G_TX_RENDERTILE, 0, 0, 1<<10, 1<<10);

				gDPLoadTextureTile_4b( gp++, face4p_sm_bm0_0, G_IM_FMT_I,48,24,0,0,47,23,
								0, G_TX_CLAMP, G_TX_MIRROR, G_TX_NOMASK,5, G_TX_NOLOD, G_TX_NOLOD);
				gSPScisTextureRectangle( gp++, (pd->bx+7+ms.adj_x)<<2, (pd->by+97)<<2, (pd->bx+7+48+ms.adj_x)<<2, (pd->by+97+24)<<2,
								G_TX_RENDERTILE, 0, (32+8)<<5, 1<<10, 1<<10);
			}
			gSPDisplayList( gp++, Normal_XNZ_Texture_dl);

			switch( ms.sub_flow ){
				case 0:		// 人間選択
				case 253:	// フレームイン
				case 255:	// プレイヤー人数選択へ
					for( i = 0; i < 2; i ++ ){
						pd = &ms.pd[i];
						if( !pd->play ){
							if( !pd->ok ){		// 選択中
								// 矢印
								gDPSetPrimColor( gp++,0, 0, col, col, col, 255);
								disp_tex_4bt( arrow_u_bm0_0,arrow_u_bm0_0tlut,16,7,pd->bx+26+ms.adj_x,pd->by+41-(ms.cnt/15)%2,SET_PTD );
								disp_tex_4bt( arrow_d_bm0_0,arrow_d_bm0_0tlut,16,7,pd->bx+26+ms.adj_x,pd->by+98+(ms.cnt/15)%2,SET_PTD );
							} else {			// 決定
								// 暗パネル
								gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
								disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
								// ＯＫ
								gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 255);
								disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+42+ms.adj_x,pd->by+4,SET_PTD );
							}
						} else {				// ＣＯＭは待ち
							// 暗パネル
							gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
							disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
						}
					}
					break;
				case 1:		// ＣＯＭ選択
					pd = &ms.pd[0];
					// 暗パネル
					gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
					disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
					// ＯＫ
					gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 255);
					disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+42+ms.adj_x,pd->by+4,SET_PTD );

					pd = &ms.pd[1];
					// 矢印
					gDPSetPrimColor( gp++,0, 0, col, col, col, 255);
					disp_tex_4bt( arrow_u_bm0_0,arrow_u_bm0_0tlut,16,7,pd->bx+26+ms.adj_x,pd->by+41-(ms.cnt/15)%2,SET_PTD );
					disp_tex_4bt( arrow_d_bm0_0,arrow_d_bm0_0tlut,16,7,pd->bx+26+ms.adj_x,pd->by+98+(ms.cnt/15)%2,SET_PTD );
					break;
				case 254: 	// フレームアウト
					for( i = 0; i < 2; i ++ ){
						pd = &ms.pd[i];
						// 暗パネル
						gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
						disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
						// ＯＫ
						gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 255);
						disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+42+ms.adj_x,pd->by+4,SET_PTD );
					}
					break;
			}
			break;

		case 1:		// レベル選択

			// 上段パネル
			disp_tex_4bt( plate_1p_bm0_0, plate_1p_bm0_0tlut, 128,130,ms.pd[0].bx+ms.adj_x, ms.pd[0].by,SET_PTD );	//	１Ｐレベル・スピード設定プレート描画
			disp_tex_4bt( plate_2p_bm0_0, plate_2p_bm0_0tlut, 128,130,ms.pd[1].bx+ms.adj_x, ms.pd[1].by,SET_PTD );	//	２Ｐレベル・スピード設定プレート描画
			if( !ms.pd[1].play ){	//	２ＰがＭＡＮ( 人間 )の場合
				pc = under_pl[1];	//	下のパネルを 背景・音楽 のグラフィックにする
			} else {				//	２ＰがＣＰＵの場合
				//	２Ｐレベル・スピード設定プレートの２Ｐの部分に ＣＯＭＰＵＴＥＲ と描画する
				disp_tex_4bt( plate_computer_bm0_0, plate_computer_bm0_0tlut, 58,8,ms.pd[1].bx+11+ms.adj_x, ms.pd[1].by+4,SET_PTD );
				pc = under_pl[2];	//	下のパネルを 背景・ＣＰＵレベル・音楽 のグラフィックにする
			}
			// 下段パネル
			disp_tex_4bt( pc[0].pat,pc[0].pal,pc[0].width,pc[0].height, ui->bx+ms.adj_x,ui->by,SET_PTD );

			for( i = 0; i < 2; i ++ ){

				pd = &ms.pd[i];

				// ウイルス
				for( k = 0; k < 3; k++ ){			//	各色分のループ
					mod = SET_PTD;					//	各色一回目の描画の時は、パレットとグラフィックをロードするように設定している
					for( l = 0; l < 8; l++ ){		//	横ループ
						for( j = 0; j < 13; j++ ){	//	縦ループ
							col = (cap_pos[pd->lv][j] >> ( 14-l*2 )) & 0x3;	//	座標データ( l,j )の配置色を調べる( 0:無し1:赤 2:青 3:黄色 )
							if( (col > 0) && (col-1 == k ) ){				// 	配置がありで、配置色 - 1 が K と同じ場合描画する
								disp_tex_4bt( &virus_bm0_0[k*20],virus_bm0_0tlut,8,5,pd->bx+17+l*5+ms.adj_x,pd->by+55+j*5,mod );
								mod = SET_D;		//	各色一回目以降の描画の時は、パレットをグラフィックをロードしないように設定している
							}
						}
					}
				}
				// レベル数値
				adj = ( pd->lv < 10 )?4:0;
				itoa( pd->lv,work1,10 );
				for( j = 0; work1[j] != '\0'; j++ ){
					disp_tex_4bt( &lv_num_bm0_0[(work1[j]-'0')*48],lv_num_bm0_0tlut,8,12,pd->bx+37+j*8+adj+ms.adj_x,pd->by+42,SET_PTD );
				}
				// レベルゲージ
				gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,pd->by+39+(20-pd->lv)*4,319,239);						//	ウイルスレベルゲージの上下は、シザリング(描画範囲指定)で行っている
				disp_tex_4bt( lv_guage_bm0_0,lv_guage_bm0_0tlut,8,84, pd->bx+6+ms.adj_x,pd->by+38,SET_PTD );
				gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,0,319,239);											//	シザリング(描画範囲指定)を元に戻す

				// カプセル
				disp_tex_4bt( cap_sp[i][pd->speed].pat,cap_sp[i][pd->speed].pal,56, 24, pd->bx+65+ms.adj_x,pd->by+32+(2-pd->speed)*33, SET_PTD );
			}

			col = (s16)(sinf( ((ms.cnt*10) % 360) * 3.14159265 /180.0 ) * 64.0) + 191;

			gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );
			gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,ui->by+18,319,ui->by+18+16);
			if( ui->stg_adj_y == 0 ){
				st = ui->stg_no;
				en = ui->stg_no;
			} else if( ui->stg_adj_y > 0 ){
				st = ui->stg_no-1;
				en = ui->stg_no;
			} else {
				st = ui->stg_no;
				en = ui->stg_no+1;
			}
			if( !ms.pd[1].play ){		// 二人とも人間
				// ステージ
				for( i = st; i <= en; i ++ ){
					disp_tex_4bt( battle_stage_pic[ui->stg_tbl[i]].pat,battle_stage_pic[ui->stg_tbl[i]].pal,64,16,ui->bx+31+ms.adj_x,ui->by+18+(i-ui->stg_no)*16+ui->stg_adj_y,SET_PTD );
				}
				// 音楽
				if( (ms.sub_flow == 2 && ui->pos == 1 ) || ui->mus_adj_y != 0 ){
					for( i = 0; i < 4; i ++ ){
						disp_tex_4bt( music[i][(ms.cnt/30)%2].pat,music[i][(ms.cnt/30)%2].pal,48,16,ui->bx+165+ms.adj_x,ui->by+18+(i-ui->mus_no)*16+ui->mus_adj_y,SET_PTD );
					}
				} else {
					disp_tex_4bt( music[ui->mus_no][0].pat,music[ui->mus_no][0].pal,48,16,ui->bx+165+ms.adj_x,ui->by+18,SET_PTD );
				}
			} else {					// ２ＰがＣＯＭ
				// ステージ
				for( i = st; i <= en; i ++ ){
					disp_tex_4bt( battle_stage_pic[ui->stg_tbl[i]].pat,battle_stage_pic[ui->stg_tbl[i]].pal,64,16,ui->bx+12+ms.adj_x,ui->by+18+(i-ui->stg_no)*16+ui->stg_adj_y,SET_PTD );
				}
				// ＣＯＭレベル
				for( i = 0; i < 3; i ++ ){
					disp_tex_4bt( com_lv[i].pat,com_lv[i].pal,80,16,ui->bx+91+ms.adj_x,ui->by+18+(i+ui->com_lv-2)*16+ui->com_adj_y,SET_PTD );
				}
				// 音楽
				if( (ms.sub_flow == 2 && ui->pos == 2 ) || ui->mus_adj_y != 0 ){
					for( i = 0; i < 4; i ++ ){
						disp_tex_4bt( music[i][(ms.cnt/30)%2].pat,music[i][(ms.cnt/30)%2].pal,48,16,ui->bx+192+ms.adj_x,ui->by+18+(i-ui->mus_no)*16+ui->mus_adj_y,SET_PTD );
					}
				} else {
					disp_tex_4bt( music[ui->mus_no][0].pat,music[ui->mus_no][0].pal,48,16,ui->bx+192+ms.adj_x,ui->by+18,SET_PTD );
				}
			}

			// 下段暗パネル
			gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,0,319,239);
			gSPDisplayList( gp++, Normal_XNZ_Texture_dl);
			gDPSetPrimColor( gp++, 0,0, 255,255,255,127 );
			if( ms.sub_flow != 2 ){		// 下段選択時以外ならでっかいやつ
				disp_tex_4bt( pc[1].pat,pc[1].pal,pc[1].width,pc[1].height, ui->bx+ms.adj_x,ui->by,SET_PTD );
			} else {					// 下段選択時は小さいやつ
				if( !ms.pd[1].play ){
					disp_tex_4bt( pc[2].pat,pc[2].pal,pc[2].width,pc[2].height, ui->bx+5+(1-ui->pos)*124+ms.adj_x,ui->by+4,SET_PTD );
				} else {
					for( i = 0; i < 3; i++ ){
						if( i != ui->pos ){
							disp_tex_4bt( pc[2].pat,pc[2].pal,pc[2].width,pc[2].height, ui->bx+i*85+5+ms.adj_x,ui->by+4,SET_PTD );
						}
					}
				}
			}

			switch( ms.sub_flow ){
				case 0:		// 人間選択
				case 253:	// フレームイン
				case 254: 	// フレームアウト
					for( i = 0; i < 2; i ++ ){
						pd = &ms.pd[i];
						if( !pd->play ){
							if( !pd->ok ){
								// 上段暗パネル
								gDPSetPrimColor( gp++, 0,0, 255,255,255,127 );
								disp_tex_4bt( plate_l_p_bm0_0,plate_l_p_bm0_0tlut,64,112, pd->bx+4+(1-pd->flag)*60+ms.adj_x, pd->by+14,SET_PTD );

								// 上段矢印
								gDPSetPrimColor( gp++, 0,0, col,col,col,255 );
								if( pd->flag ){
									disp_tex_4bt( arrow_r_bm0_0,arrow_r_bm0_0tlut,8,11, pd->bx+59+(ms.cnt/15)%2+ms.adj_x,pd->by+38+(2-pd->speed)*33,SET_PTD );
								} else {
									disp_tex_4bt( arrow_r_bm0_0,arrow_r_bm0_0tlut,8,11, pd->bx-2+(ms.cnt/15)%2+ms.adj_x,pd->by+34+(20-pd->lv)*4,SET_PTD );
								}
							} else {
								// 上段暗パネル
								gDPSetPrimColor( gp++, 0,0, 255,255,255,127 );
								disp_tex_4bt( plate_l_a_bm0_0,plate_l_a_bm0_0tlut,128,130, pd->bx+ms.adj_x,pd->by,SET_PTD );
								// ＯＫ
								gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );
								disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+95+ms.adj_x,pd->by+4,SET_PTD );
							}
						} else {
							// 上段暗パネル
							gDPSetPrimColor( gp++, 0,0, 255,255,255,127 );
							disp_tex_4bt( plate_l_a_bm0_0,plate_l_a_bm0_0tlut,128,130, pd->bx+ms.adj_x,pd->by,SET_PTD );
						}
					}
					break;
				case 1:		// ＣＯＭ選択
					// 上段暗パネル
					pd = &ms.pd[0];
					gDPSetPrimColor( gp++, 0,0, 255,255,255,127 );
					disp_tex_4bt( plate_l_a_bm0_0,plate_l_a_bm0_0tlut,128,130, pd->bx+ms.adj_x,pd->by,SET_PTD );
					// ＯＫ
					gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );
					disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+95+ms.adj_x,pd->by+4,SET_PTD );

					// 上段暗パネル
					pd = &ms.pd[1];
					gDPSetPrimColor( gp++, 0,0, 255,255,255,127 );
					disp_tex_4bt( plate_l_p_bm0_0,plate_l_p_bm0_0tlut,64,112, pd->bx+4+(1-pd->flag)*60+ms.adj_x, pd->by+14,SET_PTD );
					// 上段矢印
					if( !pd->ok ){
						gDPSetPrimColor( gp++, 0,0, col,col,col,255 );
						if( pd->flag ){
							disp_tex_4bt( arrow_r_bm0_0,arrow_r_bm0_0tlut,8,11, pd->bx+59+(ms.cnt/15)%2+ms.adj_x,pd->by+38+(2-pd->speed)*33,SET_PTD );
						} else {
							disp_tex_4bt( arrow_r_bm0_0,arrow_r_bm0_0tlut,8,11, pd->bx-2+(ms.cnt/15)%2+ms.adj_x,pd->by+34+(20-pd->lv)*4,SET_PTD );
						}
					}
					break;
				case 2:		// 下段
					pd = &ms.pd[0];
					gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,0,319,239);
					gDPSetPrimColor( gp++, 0,0, col,col,col,255 );

					// 矢印
					if( !ms.pd[1].play ){		// 二人とも人間
						switch( ui->pos ){
							case 0:					// ステージ用矢印
								if( ui->stg_no > 0 ){
									disp_tex_4bt( arrow_u_bm0_0,arrow_u_bm0_0tlut,16,7,ui->bx+58+ms.adj_x,ui->by+11-(ms.cnt/15)%2,SET_PTD );
								}
								if( ui->stg_no < ui->stg_sel_num-1 ){
									disp_tex_4bt( arrow_d_bm0_0,arrow_d_bm0_0tlut,16,7,ui->bx+58+ms.adj_x,ui->by+34+(ms.cnt/15)%2,SET_PTD );
								}
								break;
							case 1:					// 音楽用矢印
								if( ui->mus_no > 0 ){
									disp_tex_4bt( arrow_u_bm0_0,arrow_u_bm0_0tlut,16,7,ui->bx+185+ms.adj_x,ui->by+11-(ms.cnt/15)%2,SET_PTD );
								}
								if( ui->mus_no < 3 ){
									disp_tex_4bt( arrow_d_bm0_0,arrow_d_bm0_0tlut,16,7,ui->bx+185+ms.adj_x,ui->by+34+(ms.cnt/15)%2,SET_PTD );
								}
								break;
						}
					} else {					// ２ＰがＣＯＭ
						switch( ui->pos ){
							case 0:					// ステージ用矢印
								if( ui->stg_no > 0 ){
									disp_tex_4bt( arrow_u_bm0_0,arrow_u_bm0_0tlut,16,7,ui->bx+41+ms.adj_x,ui->by+11-(ms.cnt/15)%2,SET_PTD );
								}
								if( ui->stg_no < ui->stg_sel_num-1 ){
									disp_tex_4bt( arrow_d_bm0_0,arrow_d_bm0_0tlut,16,7,ui->bx+41+ms.adj_x,ui->by+34+(ms.cnt/15)%2,SET_PTD );
								}
								break;
							case 1:					// ＣＯＭレベル用矢印
								if( ui->com_lv < 2 ){
									disp_tex_4bt( arrow_u_bm0_0,arrow_u_bm0_0tlut,16,7,ui->bx+126+ms.adj_x,ui->by+11-(ms.cnt/15)%2,SET_PTD );
								}
								if( ui->com_lv > 0 ){
									disp_tex_4bt( arrow_d_bm0_0,arrow_d_bm0_0tlut,16,7,ui->bx+126+ms.adj_x,ui->by+34+(ms.cnt/15)%2,SET_PTD );
								}
								break;
							case 2:					// 音楽用矢印
								if( ui->mus_no > 0 ){
									disp_tex_4bt( arrow_u_bm0_0,arrow_u_bm0_0tlut,16,7,ui->bx+211+ms.adj_x,ui->by+11-(ms.cnt/15)%2,SET_PTD );
								}
								if( ui->mus_no < 3 ){
									disp_tex_4bt( arrow_d_bm0_0,arrow_d_bm0_0tlut,16,7,ui->bx+211+ms.adj_x,ui->by+34+(ms.cnt/15)%2,SET_PTD );
								}
								break;
						}
					}
					// 暗パネル
					gSPDisplayList( gp++, Normal_XNZ_Texture_dl);
					gDPSetPrimColor( gp++, 0,0, 255,255,255,127 );
					disp_tex_4bt( plate_l_a_bm0_0,plate_l_a_bm0_0tlut,128,130, pd->bx,pd->by,SET_PTD );
					disp_tex_4bt( plate_l_a_bm0_0,plate_l_a_bm0_0tlut,128,130, ms.pd[1].bx,ms.pd[1].by,SET_PTD );

					// ＯＫ
					gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );
					for( i = 0; i < 2; i ++ ){
						pd = &ms.pd[i];
						disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+95,pd->by+4,SET_PTD );
					}
					break;
				case 255:	// プレイヤー人数選択へ
					// 暗パネル
					gSPDisplayList( gp++, Normal_XNZ_Texture_dl);
					gDPSetPrimColor( gp++, 0,0, 255,255,255,127 );
					disp_tex_4bt( plate_l_a_bm0_0,plate_l_a_bm0_0tlut,128,130, ms.pd[0].bx+ms.adj_x,ms.pd[0].by,SET_PTD );
					disp_tex_4bt( plate_l_a_bm0_0,plate_l_a_bm0_0tlut,128,130, ms.pd[1].bx+ms.adj_x,ms.pd[1].by,SET_PTD );

					// ＯＫ
					gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );
					for( i = 0; i < 2; i ++ ){
						pd = &ms.pd[i];
						disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+95+ms.adj_x,pd->by+4,SET_PTD );
					}
					break;
			}
			break;
	}
}


// ４Ｐ用初期化
static void init_4p_play( u8 flag )
{
	int i,j;

	if( flag ){
		if( main_old == MAIN_GAME){						//	ゲームから設定に来た場合
			ms.flow			= CS_4P_SELECT_LEVEL;		//	レベル・スピード・音楽設定
		}else{											//	ゲーム以外(モードセレクト、名前入力)から設定に来た場合
			ms.flow			= CS_4P_SELECT_TEAM;		//	チーム選択
		}

		ms.adj_x		= 0;							//	画面Ｘ座標
		ms.mv_x			= 20;							//	切り替えのスクロール速度の設定
		ms.max_chara	= 11 + evs_secret_flg;			//	最大選択可能キャラ数の設定( 通常 11 だが ピーチが使用可能の場合(evs_secret_flg == 1) 12 )
		ms.ui.pos	= 0;								//	下段パネルのカーソル初期位置を左端に設定する

		ms.ui.mus_no 	= evs_4p_state_old.p_4p_m;		//	前回の音楽の設定
		ms.ui.com_lv	= evs_4p_state_old.p_4p_c_lv;	//	前回の CPU難易度の設定
		ms.team			= evs_4p_state_old.p_4p_t_m;	//	前回のゲームモードの設定 0:バトルロイヤル 1:チームバトル
		ms.sub_flow		= 0;							//	内部制御処理を人間キー入力に設定

		for( i = 0;i < 4;i++ ){
			ms.pd[i].chara	= evs_4p_state_old.p_4p_no[i];					//	前回使ったキャラクター番号の設定
			ms.pd[i].lv		= evs_4p_state_old.p_4p_lv[i];					//	前回のウイルスレベルの設定
			ms.pd[i].speed	= evs_4p_state_old.p_4p_sp[i];					//	前回のウイルススピードの設定
			ms.pd[i].p_team	= evs_4p_state_old.p_4p_team[i];				//	前回のチーム番号の設定
			ms.pd[i].play	= game_state_data[i].player_state[PS_PLAYER];	//	前回のプレイヤー情報( 0: 人間 1:ＣＰＵ )
		}
		ms.pd[0].by = ms.pd[1].by = ms.pd[2].by = ms.pd[3].by = 47;			//	キャラ選択Ｙ座標の設定( 固定 )
		ms.pd[0].bx	= 33;													//	キャラ選択１ＰＸ座標の設定
		ms.pd[1].bx = 97;													//	キャラ選択２ＰＸ座標の設定
		ms.pd[2].bx = 161;													//	キャラ選択３ＰＸ座標の設定
		ms.pd[3].bx = 225;													//	キャラ選択４ＰＸ座標の設定

		for( i = 0, j = 0; i < 4; i ++ ){
			if( ms.pd[i].play ) ms.coms[j++] = i;							//	ＣＰＵの配列番号を設定
		}

		ms.coms[j]	= -1;								//	ＣＰＵ打ち止め(判定用)の設定

		ms.ui.bx	= ( ms.coms[0] == -1 )?96:33;		//	下段パネルのＸ座標の設定( ＣＰＵがいた場合 33 いない場合 96 )
		ms.ui.by	= 181;								//	下段パネルのＹ座標の設定
		ms.ui.pos	= 0;								//	下段パネルカーソル初期位置を左端に設定する
		ms.ui.com_adj_y = 0;							//	ＣＰＵ難易度選択表示のスクロール移動値のクリア
		ms.ui.mus_adj_y = 0;							//	音楽選択表示のスクロール移動値のクリア

	}

	ms.cnt		= 0;			//	内部カウンタのクリア
	ms.now_com	= 0;			// ＣＯＭ決定用カウンタのクリア

	for( i = 0; i < 4; i ++ ){
		ms.pd[i].flag			= 0;		//	レベル・スピード設定の内部選択をレベル設定にする
		ms.pd[i].ok				= 0;		//	決定フラグのクリア
		reset_face_size( &ms.pd[i].fd );	//	顔関係座標・スケール初期化
	}
}

// ４Ｐ用メイン
static void calc_4p_play( void )
{
	int i,j,k,fl;
	s8	work,work_ok;

	Face_Data   *fd;
	Player_Data *pd;
	UNDER_ITEMS *ui;

	ui = &ms.ui;

	switch( ms.flow ){
		case CS_4P_SELECT_TEAM:		//	チーム選択
			switch( ms.sub_flow ){
				case	0:			//	バトルロイヤルかチームバトルを選択する処理
					ms.cnt ++;									//	内部カウンタの増加( 矢印の点滅等に利用される )
					if( joyupd[main_joy[0]] & DM_KEY_DOWN ){
						if( ms.team < 1 ){							//	バトルロイヤル選択側の時
							ms.team = 1;							//	チームバトル選択側に変更
							dm_set_menu_se( SE_mUpDown );			//	SE設定
						}
					}else	if( joyupd[main_joy[0]] & DM_KEY_UP ){
						if( ms.team > 0 ){							//	チームバトル選択側の時
							ms.team = 0;							//	バトルロイヤル選択側に変更
							dm_set_menu_se( SE_mUpDown );			//	SE設定
						}
					}else	if( joyupd[main_joy[0]] & DM_KEY_OK ){
						if( ms.team == 0 ){							//	バトルロイヤル選択側の場合
							ms.sub_flow = 254;						//	画面のスクロール処理へ
						}else{										//	チームバトル選択側の場合
							ms.sub_flow = 1;						//	チーム分け処理へ
						}
						dm_set_menu_se( SE_mDecide );				//	SE設定
					}else	if( joyupd[main_joy[0]] & DM_KEY_B ){
						ms.sub_flow = 255;							//	キャンセル( モードセレクトに戻る )
					}
					break;
				case	1:	//	チーム分けの処理( 人間 )
					ms.cnt ++;			//	内部カウンタの増加( 矢印の点滅等に利用される )
					for( i = 0;i < 4;i++ ){
						pd = &ms.pd[i];
						fd = &ms.pd[i].fd;
						if( !pd->play ){		// 人間のみ操作可
							if( !pd->ok ){			//	選択中
								if( joyupd[i] & DM_KEY_LEFT ){
									if( pd -> p_team ){						//	ＴＥＡＭ Ｂの場合
										pd -> p_team = 0;					//	ＴＥＡＭ Ａに変更
										dm_set_menu_se( SE_mLeftRight );	//	SE設定
									}
								}else	if( joyupd[i] & DM_KEY_RIGHT ){
									if( !pd -> p_team ){					//	ＴＥＡＭ Ａの場合
										pd -> p_team = 1;					//	ＴＥＡＭ Ｂに変更
										dm_set_menu_se( SE_mLeftRight );	//	SE設定
									}
								}else	if( joyupd[i] & DM_KEY_OK ){		//	決定
									//	選択したチームで決定できるか調べる処理
									k = pd -> p_team;
									for(j = fl = 0;j < 4;j++){						//	チーム人数計算
										if( ms.pd[j].p_team == k && ms.pd[j].ok ){	//	チームが同じ(ms.pd[j].p_team == k)で決定していた場合( ms.pd[j].ok )
											fl ++;									//	チーム人数増加
										}
									}
									if( fl < 3 ){							//	選択したチームの人数が２人以下の場合
										dm_set_menu_se( SE_mDecide );		//	SE設定
										pd -> ok = 1;						//	決定フラグを立てる
									}else{									//	選択したチームの人数が３人以上の場合
																			//	決定できません
										dm_set_menu_se( SE_mError );		//	SE設定
									}
									//	次の処理に移るための判定処理
									for(j = 0,fl = 1;j < 4;j++){
										if( !ms.pd[j].play && !ms.pd[j].ok ){	//	プレイヤー( 人間 )でまだ決まっていない人がいた場合
											fl = 0;
										}
									}
									if( fl ){	// プレイヤー( 人間 )が全て決まった場合
										if( ms.coms[0] == -1 ){	//	CPU がいない場合
											ms.sub_flow = 254;	//	画面のスクロール処理へ
										}else{					//	CPU がいる場合
											ms.sub_flow = 2;	//	CPU のチーム設定へ
										}
									}
								}else	if( joyupd[i] & DM_KEY_B ){
									if( main_joy[0] == i ){
										ms.sub_flow = 0;		//	キャンセル( チームバトルかバトルロイヤルか選択する処理に戻る )
									}
								}
							}else{
								if( joyupd[i] & DM_KEY_B ){		//	キャンセル
									pd -> ok = 0;				//	決定状態のキャンセル
								}
							}
						}
					}
					break;
				case	2:	//	CPUのチーム設定
					ms.cnt ++;		//	内部カウンタの増加( 矢印の点滅等に利用される )
					pd = &ms.pd[ms.coms[ms.now_com]];
					fd = &ms.pd[ms.coms[ms.now_com]].fd;
					if( joyupd[main_joy[0]] & DM_KEY_LEFT ){
						if( pd -> p_team ){								//	ＴＥＡＭ Ｂの場合
							pd -> p_team = 0;							//	ＴＥＡＭ Ａに変更
							dm_set_menu_se( SE_mLeftRight );			//	SE設定
						}
					}else	if( joyupd[main_joy[0]] & DM_KEY_RIGHT ){
						if( !pd -> p_team ){							//	ＴＥＡＭ Ａの場合
							pd -> p_team = 1;							//	ＴＥＡＭ Ｂに変更
							dm_set_menu_se( SE_mLeftRight );			//	SE設定
						}
					}else	if( joyupd[main_joy[0]] & DM_KEY_OK ){		//	決定
						//	選択したチームで決定できるか調べる処理
						k = pd -> p_team;
						for(j = fl = 0;j < 4;j++){						//	チーム人数計算
							if( ms.pd[j].p_team == k && ms.pd[j].ok ){	//	チームが同じ(ms.pd[j].p_team == k)で決定していた場合( ms.pd[j].ok )
								fl ++;									//	チーム人数増加
							}
						}
						if( fl < 3 ){							//	選択したチームの人数が２人以下の場合
							dm_set_menu_se( SE_mDecide );		//	SE設定
							pd -> ok = 1;						//	決定フラグを立てる
						}else{									//	選択したチームの人数が３人以上の場合
																			//	決定できません
							dm_set_menu_se( SE_mError );		//	SE設定
						}
						//	次の処理に移るための判定処理
						for(j = 0,fl = 1;j < 4;j++){
							if( !ms.pd[j].ok ){	//	決まっていない人がいた場合
								fl = 0;
							}
						}

						if( ms.coms[ms.now_com+1] != -1 ){	//	まだＣＰＵが残っている
							ms.now_com ++;					//	ＣＰＵの番号をずらす
						} else {
							if( fl ){						//	全員決定している
								ms.sub_flow = 254;			//	画面のスクロール処理へ
							}
							break;
						}
					}
					// キャンセル
					for( i = 0; i < 4; i ++ ){
						if( !ms.pd[i].play && joyupd[i] & DM_KEY_B ){
							if( i == 0 ){								//	メインのコントローラの場合
								if( ms.now_com > 0 ){					//	ＣＰＵの設定を行っていた場合
									ms.now_com --;						//	ＣＰＵの前のＣＰＵにずらす
									ms.pd[ms.coms[ms.now_com]].ok = 0;	//	前のＣＰＵの決定フラグ解除
								} else {								//	ＣＰＵの設定ではなかった場合
									ms.pd[i].ok = 0;					//	決定フラグ解除
									ms.now_com = 0;						//	ＣＰＵの番号をクリア
									ms.sub_flow = 1;					//	人間のチーム選択処理に戻る
								}
							} else {
								ms.pd[i].ok = 0;						//	押した人の決定フラグ解除
								for( j = 0; j < ms.now_com; j ++ ){
									ms.pd[ms.coms[j]].ok = 0;			//	ＣＰＵの決定フラグ解除
								}
								ms.now_com = 0;							//	ＣＰＵの番号をクリア
								ms.sub_flow = 1;						//	人間のチーム選択処理に戻る
							}
							break;
						}
					}

					break;
				case 253:								//	フレームイン( チーム選択のプレートが画面内に入ってくる )
					if( ms.adj_x - ms.mv_x > 0 ){		//	指定座標まで移動していない場合
						ms.adj_x -= ms.mv_x;
					} else {							//	指定座標まで移動した場合
						ms.adj_x -= ms.mv_x;
						ms.sub_flow = ms.team;			//	チーム選択かゲームモード選択(バトルロイヤル か チームバトル )処理に変更
					}
					break;
				case 254:								//	フレームアウト( チーム選択のプレートが画面外に出て行く )
					if( ms.adj_x + ms.mv_x < 320 ){		//	画面外に出ていっていない場合
						ms.adj_x += ms.mv_x;
					} else {							//	画面外に出ていった場合
						ms.adj_x += ms.mv_x;
						ms.flow = CS_4P_SELECT_MAN;		//	メインの処理をキャラクタ選択へ
						ms.sub_flow = 253;				//	フレームイン( 顔のプレートが画面内に入ってくる処理 )
						init_4p_play(0);				//	設定初期化( 座標系と決定関係のフラグのみクリアされる )
					}
					break;
				case 255:								// プレイヤー人数選択へ
					main_no = MAIN_MODE_SELECT;			//	モードセレクトに戻る
					main_60_out_flg = 0;				//	メインループ脱出
					break;
			}
			break;
		case CS_4P_SELECT_MAN:		// キャラ選択
			switch( ms.sub_flow ){
				case 0:			// 人間設定
					for( i = 0; i < 4; i ++ ){
						pd = &ms.pd[i];
						fd = &ms.pd[i].fd;
						if( !pd->play ){		// 人間のみ操作可
							if( !pd->ok ){			//	選択中
								if( !fd->move ){		//	顔のグラフィックがスクロール中でない場合( !fd->move )
									if( joynew[i] & CONT_UP ){
										dm_set_menu_se( SE_mLeftRight );									//	SE設定
										pd->chara = ( pd->chara - 1 >= 0 )?pd->chara - 1:ms.max_chara - 1;	//	キャラ番号が０以下の場合、最大選択キャラの番号に設定
										fd->move = -1;														//	スクロール中フラグの設定( スクロール方向も兼ねる )
										pd->flag = 1;														//	スクロール処理フラグを上方向スクロールに設定
									}else	if( joynew[i] & CONT_DOWN  ){
										dm_set_menu_se( SE_mLeftRight );									//	SE設定
										pd->chara = ( pd->chara + 1 < ms.max_chara )?pd->chara + 1:0;		//	キャラ番号が最大選択キャラの番号以上になった場合、０に設定
										fd->move = 1;														//	スクロール中フラグの設定( スクロール方向も兼ねる )
										pd->flag = 2;														//	スクロール処理フラグを下方向スクロールに設定
									}else	if( joyupd[i] & DM_KEY_OK  ){				// 決定
										for( j = 0,fl = 1; j < 4; j++ ){
											//	相手が決定状態で、相手の選択したキャラと自分の選択したキャラが同じだった場合、処理フラグをクリアする( fl = 0 )
											if( ms.pd[j].ok && pd->chara == ms.pd[j].chara ) fl = 0;
										}
										if( fl ){												//	自分のキャラが決定した場合( 相手と違うキャラか、相手がまだ決定状態でなかった場合 )
											dm_set_menu_se( SE_mDecide );						//	SE設定
											pd->ok = 1;											//	決定フラグを立てる

											for( j = 0,fl = 1; j < 4; j++ ){					//	決定状態を調べる
												//	プレイヤーが人間でまだ決定していなかった場合、処理フラグをクリアする( fl = 0 )
												if( !ms.pd[j].play && !ms.pd[j].ok ) fl = 0;
											}
											if( fl ){											//	処理を次に進める
												//	ＣＰＵがいる場合( ms.sub_flow = 1 ),ＣＰＵがいない場合、画面のスクロール処理へ( ms.sub_flow = 254 )
												ms.sub_flow = ( ms.coms[0] == -1 )?254:1;
												break;
											}
										}
									}else	if( joyupd[i] & CONT_B  ){	//	キャンセル
										ms.sub_flow = 255;				//	画面スクロール処理へ( チーム選択の処理に戻る )
									}
								}
								//	顔のスクロール処理
								set_face_size(pd,fd,ms.cnt);
							} else {
								// キャンセル
								if( joyupd[i] & CONT_B && !fd->move ){	//	顔のグラフィックがスクロール中でない場合( !fd->move )
									pd->ok = 0;							//	決定の解除
								}
							}
						}
					}
					break;
				case 1:			// ＣＯＭ設定
					pd = &ms.pd[ms.coms[ms.now_com]];
					fd = &ms.pd[ms.coms[ms.now_com]].fd;

					if( !fd->move ){	//	顔のグラフィックがスクロール中でない場合( !fd->move )
						if( joynew[main_joy[0]] & CONT_UP  ){
							dm_set_menu_se( SE_mLeftRight );									//	SE設定
							pd->chara = ( pd->chara - 1 >= 0 )?pd->chara - 1:ms.max_chara - 1;	//	キャラ番号が０以下の場合、最大選択キャラの番号に設定
							fd->move = -1;														//	スクロール中フラグの設定( スクロール方向も兼ねる )
							pd->flag = 1;														//	スクロール処理フラグを上方向スクロールに設定
						}else	if( joynew[main_joy[0]] & CONT_DOWN ){
							dm_set_menu_se( SE_mLeftRight );									//	SE設定
							pd->chara = ( pd->chara + 1 < ms.max_chara )?pd->chara + 1:0;		//	キャラ番号が最大選択キャラの番号以上になった場合、０に設定
							fd->move = 1;														//	スクロール中フラグの設定( スクロール方向も兼ねる )
							pd->flag = 2;														//	スクロール処理フラグを下方向スクロールに設定
						}else	if( joyupd[main_joy[0]] & DM_KEY_OK ){
							for( j = 0,fl = 1; j < 4; j++ ){
								//	相手が決定状態で、相手の選択したキャラと自分の選択したキャラが同じだった場合、処理フラグをクリアする( fl = 0 )
								if( ms.pd[j].ok && pd->chara == ms.pd[j].chara ) fl = 0;
							}
							if( fl ){
								dm_set_menu_se( SE_mDecide );		//	SE設定
								pd->ok = 1;							//	決定フラグを立てる

								//	次の処理に移るための判定処理
								for(j = 0,fl = 1;j < 4;j++){
									if( !ms.pd[j].ok ){	//	決まっていない人がいた場合
										fl = 0;
									}
								}

								if( ms.coms[ms.now_com+1] != -1 ){	//	まだＣＰＵが残っている
									ms.now_com ++;					//	ＣＰＵの番号をずらす
								} else {
									if( fl ){						//	全員決定している
										ms.sub_flow = 254;			//	画面のスクロール処理へ
									}
									break;
								}
							}
						}
											// キャンセル
						for( i = 0; i < 4; i ++ ){
							if( !ms.pd[i].play && joyupd[i] & CONT_B  ){
								if( i == 0 ){								//	メインのコントローラの場合
									if( ms.now_com > 0 ){					//	ＣＰＵの設定を行っていた場合
										ms.now_com --;						//	ＣＰＵの前のＣＰＵにずらす
										ms.pd[ms.coms[ms.now_com]].ok = 0;	//	前のＣＰＵの決定フラグ解除
									} else {								//	ＣＰＵの設定ではなかった場合
										ms.pd[i].ok = 0;					//	決定フラグ解除
										ms.now_com = 0;						//	ＣＰＵの番号をクリア
										ms.sub_flow = 0;					//	人間のキャラ選択処理に戻る
									}
								} else {
									ms.pd[i].ok = 0;						//	押した人の決定フラグ解除
									for( j = 0; j < ms.now_com; j ++ ){
										ms.pd[ms.coms[j]].ok = 0;			//	ＣＰＵの決定フラグ解除
									}
									ms.now_com = 0;							//	ＣＰＵの番号をクリア
									ms.sub_flow = 0;						//	人間のキャラ選択処理に戻る
								}
								break;
							}
						}
					}
					//	顔のスクロール処理
					set_face_size(pd,fd,ms.cnt);

					break;
				case 253:								//	フレームイン( 顔のプレートが画面内に入ってくる処理 )
					if( ms.adj_x - ms.mv_x > 0 ){		//	指定座標まで移動していない場合
						ms.adj_x -= ms.mv_x;
					} else {							//	指定座標まで移動した場合
						ms.adj_x -= ms.mv_x;
						ms.sub_flow = 0;				//	処理をキャラ選択に変更
					}
					break;
				case 254:								//	フレームアウト( 顔のプレートが画面外に出て行く処理 )
					if( ms.adj_x + ms.mv_x < 320 ){		//	画面外に出ていっていない場合
						ms.adj_x += ms.mv_x;
					} else {							//	画面外に出ていった場合
						ms.adj_x += ms.mv_x;
						ms.flow = CS_4P_SELECT_LEVEL;	//	メインの処理をレベル・スピード設定へ
						ms.sub_flow = 253;				//	フレームイン( レベル・スピード設定のプレートが画面内に入ってくる )処理に変更
						init_4p_play(0);				//	設定初期化( 座標系と決定関係のフラグのみクリアされる )
					}
					break;
				case 255:								//	フレームアウト( 顔のプレートが画面外に出て行く処理 )
					if( ms.adj_x + ms.mv_x < 320 ){		//	画面外に出ていっていない場合
						ms.adj_x += ms.mv_x;
					} else {							//	画面外に出ていった場合
						ms.adj_x += ms.mv_x;
						ms.flow = CS_4P_SELECT_TEAM;	//	メインの処理をチーム選択
						ms.sub_flow = 253;				//	フレームイン( チーム選択のプレートが画面内に入ってくる )処理に変更
						init_4p_play(0);				//	設定初期化( 座標系と決定関係のフラグのみクリアされる )
					}
					break;
			}
			ms.cnt ++;	//	内部カウンタの増加( 矢印の点滅等に利用される )
			break;

		case CS_4P_SELECT_LEVEL:	// レベル選択
			switch( ms.sub_flow ){
				case 0:		// 人間設定
					for( i = 0; i < 4; i ++ ){
						pd = &ms.pd[i];
						if( !pd->play ){		// 人間のみ操作可
							if( !pd->ok ){			// 選択中
								if( joycur[i] & CONT_UP ){
									if( !pd->flag ){							//	レベル設定側の時
										if( pd -> lv < 20 ){					//	選択可能最高レベル( 20 )より下の場合
											pd -> lv ++;						//	レベル上昇
											dm_set_menu_se( SE_mLeftRight );	//	SE設定
										}
									} else {									//	カプセルスピード設定側の時
										if( pd -> speed < 2 ){					//	最高速度より下だった場合
											pd -> speed ++;						//	加速
											dm_set_menu_se( SE_mLeftRight );	//	SE設定
										}
									}
								}else	if( joycur[i] & CONT_DOWN ){
									if( !pd->flag ){							//	レベル設定側の時
										if( pd -> lv > 0 ){						//	最低レベルより上だった場合
											pd -> lv --;						//	レベル下降
											dm_set_menu_se( SE_mLeftRight );	//	SE設定
										}
									} else {									//	カプセルスピード設定側の時
										if( pd -> speed > 0 ){					//	最低速度より上だった場合
											pd -> speed --;						//	減速
											dm_set_menu_se( SE_mLeftRight );	//	SE設定
										}
									}
								}else	if( joyupd[i] & CONT_RIGHT ){
									if( pd->flag < 1 ){							//	レベル設定側の時
										pd->flag ++;							//	カプセルスピード設定側に変更
										dm_set_menu_se( SE_mUpDown );			//	SE設定
									}
								}else	if( joyupd[i] & CONT_LEFT ){
									if( pd->flag > 0 ){							//	カプセルスピード設定側の時
										pd->flag --;							//	レベル設定側に変更
										dm_set_menu_se( SE_mUpDown );			//	SE設定
									}
								}else	if( joyupd[i] & DM_KEY_OK ){			//	決定
									pd->ok = 1;									//	決定フラグを立てる
									dm_set_menu_se( SE_mDecide );				//	SE設定

									for( j = 0,fl = 1; j < 4; j++ ){
										//	プレイヤーが人間でまだ決定していなかった場合、処理フラグをクリアする( fl = 0 )
										if( !ms.pd[j].play && !ms.pd[j].ok ) fl = 0;
									}
									if( fl ){	//	人間が全員決定した場合
										//	ＣＰＵがいた場合、ＣＰＵのキャラクター選択へ( ms.sub_flow = 1 ),ＣＰＵがいない場合、音楽設定処理へ( ms.sub_flow = 2 )
										ms.sub_flow = ( ms.coms[0] == -1 )?2:1;
										break;
									}
								}else	if( joyupd[i] & CONT_B ){				//	キャンセル
									ms.sub_flow = 254;							//	画面のスクロール処理へ
								}
							} else {				// 決定済み
								// キャンセル
								if( joyupd[i] & CONT_B ){						//	キャンセル
									pd->ok = 0;									//	決定フラグのクリア
								}
							}
						}
					}
					break;
				case 1:		// ＣＯＭ設定
					pd = &ms.pd[ms.coms[ms.now_com]];

					if( joycur[main_joy[0]] & CONT_UP ){
						if( !pd->flag ){							//	レベル設定側の時
							if( pd -> lv < 20 ){					//	選択可能最高レベル( 20 )より下の場合
								pd -> lv ++;						//	レベル上昇
								dm_set_menu_se( SE_mLeftRight );	//	SE設定
							}
						} else {									//	カプセルスピード設定側の時
							if( pd -> speed < 2 ){					//	最高速度より下だった場合
								pd -> speed ++;						//	加速
								dm_set_menu_se( SE_mLeftRight );	//	SE
							}
						}
					}else	if( joycur[main_joy[0]] & CONT_DOWN ){
						if( !pd->flag ){							//	レベル設定側の時
							if( pd -> lv > 0 ){						//	最低レベルより上だった場合
								pd -> lv --;						//	レベル下降
								dm_set_menu_se( SE_mLeftRight );	//	SE設定
							}
						} else {									//	カプセルスピード設定側の時
							if( pd -> speed > 0 ){					//	最低速度より上だった場合
								pd -> speed --;						//	減速
								dm_set_menu_se( SE_mLeftRight );	//	SE
							}
						}
					}else	if( joyupd[main_joy[0]] & CONT_RIGHT ){
						if( pd->flag < 1 ){							//	レベル設定側の時
							pd->flag ++;							//	カプセルスピード設定側に変更
							dm_set_menu_se( SE_mUpDown );			//	SE設定
						}
					}else	if( joyupd[main_joy[0]] & CONT_LEFT ){
						if( pd->flag > 0 ){							//	カプセルスピード設定側の時
							pd->flag --;							//	レベル設定側に変更
							dm_set_menu_se( SE_mUpDown );			//	SE設定
						}
					}else	if( joyupd[main_joy[0]] & DM_KEY_OK ){	//	決定
						dm_set_menu_se( SE_mDecide );				//	SE設定
						pd->ok = 1;									//	決定フラグを立てる

						//	次の処理に移るための判定処理
						for(j = 0,fl = 1;j < 4;j++){
							if( !ms.pd[j].ok ){	//	決まっていない人がいた場合
								fl = 0;
							}
						}

						if( ms.coms[ms.now_com+1] != -1 ){	//	まだＣＰＵが残っている
							ms.now_com ++;					//	ＣＰＵの番号をずらす
						} else {
							if( fl ){						//	全員決定している
								ms.sub_flow = 2;			//	音楽設定処理へ
							}
							break;
						}
					}
					// キャンセルチェック
					for( i = 0; i < 4; i ++ ){
						if( !ms.pd[i].play && joyupd[i] & CONT_B ){
							if( i == 0 ){								//	メインのコントローラの場合
								if( ms.now_com > 0 ){					//	ＣＰＵの設定を行っていた場合
									ms.now_com --;						//	ＣＰＵの前のＣＰＵにずらす
									ms.pd[ms.coms[ms.now_com]].ok = 0;	//	前のＣＰＵの決定フラグ解除
								} else {								//	ＣＰＵの設定ではなかった場合
									ms.pd[i].ok = 0;					//	決定フラグ解除
									ms.now_com = 0;						//	ＣＰＵの番号をクリア
									ms.sub_flow = 0;					//	人間のレベル・スピード選択処理に戻る
								}
							} else {
								ms.pd[i].ok = 0;						//	押した人の決定フラグ解除
								for( j = 0; j < ms.now_com; j ++ ){
									ms.pd[ms.coms[j]].ok = 0;			//	ＣＰＵの決定フラグ解除
								}
								ms.now_com = 0;							//	ＣＰＵの番号をクリア
								ms.sub_flow = 0;						//	人間のレベル・スピード選択処理に戻る
							}
							break;
						}
					}
					break;
				case 2:		// 音楽設定
					pd = &ms.pd[0];

					if( ms.coms[0] == -1 ){		//	ＣＰＵがいない場合の処理
						if( ui->mus_adj_y == 0 ){	//	音楽グラフィックがスクロールしていないとき
							if( joynew[main_joy[0]] & CONT_UP ){
								if( ui->mus_no > 0 ){					//	音楽番号が０より上の場合
									ui->mus_no --;						//	音楽番号の減少
									ui->mus_adj_y = -16;				//	スクロール値の設定
									dm_set_menu_se( SE_mLeftRight );	//	SE
								}
							}else	if( joynew[main_joy[0]] & CONT_DOWN ){
								if( ui->mus_no < 3 ){					//	音楽番号が３より下の場合
									ui->mus_no ++;						//	音楽番号の増加
									ui->mus_adj_y = 16;					//	スクロール値の設定
									dm_set_menu_se( SE_mLeftRight );	//	SE設定
								}
							}
						}
					} else {					//	ＣＰＵがいる場合の処理
						if( joyupd[main_joy[0]] & CONT_RIGHT ){
							if( ui -> pos < 1 ){						//	ＣＰＵ難易度選択側の時
								ui->pos = 1;							//	音楽設定側に変更
								dm_set_menu_se( SE_mUpDown );			//	SE設定
							}
						}else	if( joyupd[main_joy[0]] & CONT_LEFT ){
							if( ui -> pos > 0){							//	音楽設定側の時
								ui->pos = 0;							//	ＣＰＵ難易度選択側に変更
								dm_set_menu_se( SE_mUpDown );			//	SE設定
							}
						}
						if( !ui->pos ){		//	ＣＰＵ難易度選択側の時
							if( ui->com_adj_y == 0 ){	//	ＣＯＭレベル選択グラフィックがスクロールしていないとき
								if( joynew[main_joy[0]] & CONT_UP ){
									if( ui->com_lv < 2 ){					//	難易度が NORMAL 以下の時
										ui->com_lv ++;						//	難易度の上昇
										ui->com_adj_y = -16;				//	スクロール値の設定
										dm_set_menu_se( SE_mLeftRight );	//	SE設定
									}
								}else	if( joynew[main_joy[0]] & CONT_DOWN ){
									if( ui->com_lv > 0 ){					//	難易度が NORMAL 以上の時
										ui->com_lv --;						//	難易度の減少
										ui->com_adj_y = 16;					//	スクロール値の設定
										dm_set_menu_se( SE_mLeftRight );	//	SE設定
									}
								}
							}
						} else {			//	音楽設定側の時
							if( ui->mus_adj_y == 0 ){	//	音楽グラフィックがスクロールしていないとき
								if( joynew[main_joy[0]] & CONT_UP ){
									if( ui->mus_no > 0 ){					//	音楽番号が０より上の場合
										ui->mus_no --;						//	音楽番号の減少
										ui->mus_adj_y = -16;				//	スクロール値の設定
										dm_set_menu_se( SE_mLeftRight );	//	SE
									}
								}else	if( joynew[main_joy[0]] & CONT_DOWN ){
									if( ui->mus_no < 3 ){					//	音楽番号が３より下の場合
										ui->mus_no ++;						//	音楽番号の増加
										ui->mus_adj_y = 16;					//	スクロール値の設定
										dm_set_menu_se( SE_mLeftRight );	//	SE設定
									}
								}
							}
						}
					}

					// キャンセルチェック
					for( i = 0; i < 4; i ++ ){
						if( !ms.pd[i].play && joyupd[i] & CONT_B ){	//	ボタンを押したプレイヤーが人だった場合
							ms.pd[i].ok = 0;						//	ボタンを押したプレイヤーの決定フラグの解除
							for( j = 0; ms.coms[j] != -1; j ++ ){
								ms.pd[ms.coms[j]].ok = 0;			//	ＣＰＵの決定フラグ解除
							}
							ms.now_com = 0;							//	ＣＰＵの番号をクリア
							ms.sub_flow = 0;						//	人間のレベル・スピード選択処理に戻る
							break;
						}
					}
					// 決定
					if( joyupd[main_joy[0]] & DM_KEY_OK && ui->com_adj_y == 0 && ui->mus_adj_y == 0 ){
						dm_set_menu_se( SE_mDecide );				//	SE設定
						ms.sub_flow = 255;							//	ゲーム設定処理へ
						break;
					}
					break;
				case 253:								//	フレームイン( レベル・スピード設定のプレートが画面内に入ってくる )
					if( ms.adj_x - ms.mv_x > 0 ){		//	指定座標まで移動していない場合
						ms.adj_x -= ms.mv_x;
					} else {							//	指定座標まで移動した場合
						ms.adj_x -= ms.mv_x;
						ms.sub_flow = 0;				//	処理をレベル・スピード設定に変更
					}
					break;
				case 254:								//	フレームアウト( レベル・スピード設定のプレートが画面外に出て行く )
					if( ms.adj_x + ms.mv_x < 320 ){		//	画面外に出ていっていない場合
						ms.adj_x += ms.mv_x;
					} else {							//	画面外に出ていった場合
						ms.adj_x += ms.mv_x;
						ms.flow = CS_4P_SELECT_MAN;		//	メインの処理をキャラクタ選択へ
						ms.sub_flow = 253;				//	フレームイン( 顔のプレートが画面内に入ってくる処理 )
						init_4p_play(0);				//	設定初期化( 座標系と決定関係のフラグのみクリアされる )
					}
					break;
				case 255:		// ゲームへ
					main_no = MAIN_12;					//	ゲーム前設定へ
					main_60_out_flg = 0;				//	メインループ脱出
					break;
			}
			ui->com_adj_y = ( ui->com_adj_y-1 > 0 )?ui->com_adj_y-1:( ui->com_adj_y+1 < 0 )?ui->com_adj_y+1:0;	//	CPU難易度選択グラフィックのスクロール処理
			ui->mus_adj_y = ( ui->mus_adj_y-1 > 0 )?ui->mus_adj_y-1:( ui->mus_adj_y+1 < 0 )?ui->mus_adj_y+1:0;	//	音楽選択グラフィックのスクロール処理
			ms.cnt ++;			//	内部カウンタの増加( 矢印の点滅等に利用される )
			break;
	}
}
// ４Ｐ用描画
static void disp_4p_play( void )
{
	int i,j,k,fl,adj,col,top,mx;
	s8	a_fl[2],b_fl[2];
	s8	cwo[10];
	PIC2 *pc;
	Player_Data *pd;
	Face_Data	*fd;
	UNDER_ITEMS *ui;

	ui = &ms.ui;
	col = (s16)(sinf( ((ms.cnt*10) % 360) * 3.14159265 /180.0 ) * 64.0) + 191;

	// 背景
	gSPDisplayList( gp++, Normal_TNZ_Texture_dl);
	gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );

	load_TexPal(dm_bg_tile_pal_3_bm0_0tlut);
	load_TexTile_4b( &dm_bg_tile_data_bm0_0[0],20,20,0,0,19,19 );
	for(i = 0;i < 12;i++)	//	縦回数
	{
		for(j = 0;j < 16;j++)	//	横回数
		{
			draw_Tex(j * 20,i * 20,20,20,0,0);
		}
	}

	// タイトル
	disp_tex_4bt( title_2p_1_bm0_0, title_2p_1_bm0_0tlut, 160, 31, 32, 12,SET_PTD );	//	モードセレクト
	disp_tex_4bt( title_2p_2_bm0_0, title_2p_2_bm0_0tlut,  80, 32,208, 12,SET_PTD );	//	ボタン(Ａ：決定 Ｂ：キャンセル)

	switch( ms.flow ){
		case CS_4P_SELECT_TEAM:		//	チーム選択

			//	パネル
			load_TexPal( dm_ts_pal_bm0_0tlut );
			load_TexTile_4b( dm_ts_waku_a_bm0_0,236,17,0,0,235,16 );

			switch( ms.team ){
			case	0:	//	NORMAL
				//	プレート描画
				draw_Tex( ms.adj_x + 42,47,236,17,0,0 );				//	天井部分 NOMAL 側
				draw_Tex( ms.adj_x + 42,100,236,14,0,0 );				//	天井部分 TEAM 側

				//	NOMAL 側
				load_TexTile_4b( dm_ts_waku_b_bm0_0,236,8,0,0,235,7 );
				draw_Tex( ms.adj_x + 42,64,236,7,0,0 );					//	上の部分	NOMAL 側
				draw_ScaleTex( ms.adj_x + 42,71,236,6,236,10,0,1);		// 	中間の部分	NOMAL 側
				draw_Tex( ms.adj_x + 42,81,236,7,0,1 );					//	下の部分	NOMAL 側

				load_TexTile_4b( dm_ts_waku_c_bm0_0,236,4,0,0,235,3 );
				draw_Tex( ms.adj_x + 42,88,236,4,0,0 );					//	床部分 NOMAL 側
				draw_Tex( ms.adj_x + 42,114,236,4,0,0 );				//	床部分 TEAM 側

				//	NORMAL の文字
				load_TexTile_4b( dm_ts_team_normal_bm0_0,46,10,0,0,45,9 );
				draw_Tex( ms.adj_x + 137,51,45,10,0,0 );
				//	TEAM の文字
				load_TexTile_4b( dm_ts_team_4p_bm0_0,30,10,0,0,29,9 );
				draw_Tex( ms.adj_x + 145,103,30,10,0,0 );

				//	vs の文字
				load_TexBlock_4b( dm_ts_vs_bm0_0,16,10 );
				for( i = 0;i < 3;i++ ){
					draw_Tex( ms.adj_x + 92 + i * 60,71,16,10,0,0 );
				}
				//	1P vs 2P vs 3P vs 4P( 人( P ) か CPU( COM ) )
				for( i = 0;i < 4;i++ ){
					load_TexPal( team_4p_player[i][0].pal );
					load_TexTile_4b( team_4p_player[i][ms.pd[i].play].pat,42,16,0,0,41,15 );
					draw_Tex( ms.adj_x + 49 + i * 60,68,42,16,0,0 );

				}

				//	覆面で覆う
				gSPDisplayList( gp++, Texture_NZ_dl);

				gDPSetPrimColor( gp++, 0,0, 128,128,128,128 );	//	暗く
				load_TexPal( dm_ts_waku_a_b_bm0_0tlut );
				load_TexTile_4b( dm_ts_waku_a_bm0_0,236,17,0,0,235,16 );
				draw_Tex( ms.adj_x + 42,100,236,14,0,0 );
				load_TexTile_4b( dm_ts_waku_c_bm0_0,236,4,0,0,235,3 );
				draw_Tex( ms.adj_x + 42,114,236,4,0,0 );

				// 矢印
				gDPSetPrimColor( gp++,0, 0, col, col, col, 255);
				load_TexPal( arrow_u_bm0_0tlut );
				load_TexBlock_4b( arrow_u_bm0_0,16,7 );
				draw_TexFlip(ms.adj_x + 154 ,92 + (ms.cnt/15)%2,16,7,0,0, flip_off,flip_on );
				break;
			case	1:	//	TEAM
				//	プレート描画
				draw_Tex( ms.adj_x + 42,47,236,14,0,0 );				//	天井部分 NOMAL 側
				draw_Tex( ms.adj_x + 42,73,236,17,0,0 );				//	天井部分 TEAM 側

				load_TexTile_4b( dm_ts_waku_b_bm0_0,236,8,0,0,235,7 );
				draw_Tex( ms.adj_x + 42,90,236,7,0,0 );					//	上の部分	TEAM 側
				draw_ScaleTex( ms.adj_x + 42,97,236,6,236,97,0,1);		// 	中間の部分	TEAM 側
				draw_Tex( ms.adj_x + 42,194,236,7,0,1 );				//	下の部分	TEAM 側

				load_TexTile_4b( dm_ts_waku_c_bm0_0,236,4,0,0,235,3 );
				draw_Tex( ms.adj_x + 42,61,236,4,0,0 );					//	床部分 NOMAL 側
				draw_Tex( ms.adj_x + 42,201,236,4,0,0 );				//	床部分 TEAM 側

				//	NORMAL の文字
				load_TexTile_4b( dm_ts_team_normal_bm0_0,46,10,0,0,45,9 );
				draw_Tex( ms.adj_x + 137,51,45,10,0,0 );
				//	TEAM の文字
				load_TexTile_4b( dm_ts_team_4p_bm0_0,30,10,0,0,29,9 );
				draw_Tex( ms.adj_x + 145,77,30,10,0,0 );

				//	VS の文字
				load_TexBlock_4b( dm_ts_vs_bm0_0,16,10 );
				draw_Tex( ms.adj_x + 152,136,16,10,0,0 );

				//	TEAM A 下地
				load_TexBlock_4b( dm_ts_team_a_bm0_0,64,50 );
				draw_Tex( ms.adj_x + 84,94,64,50,0,0 );
				load_TexBlock_4b( &dm_ts_team_a_bm0_0[1600],64,53 );
				draw_Tex( ms.adj_x + 84,144,64,53,0,0 );

				//	TEAM B 下地
				load_TexBlock_4b( dm_ts_team_b_bm0_0,64,50 );
				draw_Tex( ms.adj_x + 171,94,64,50,0,0 );
				load_TexBlock_4b( &dm_ts_team_b_bm0_0[1600],64,53 );
				draw_Tex( ms.adj_x + 171,144,64,53,0,0 );

				//	1P vs 2P vs 3P vs 4P( 人( P ) か CPU( COM ) )

				//	TEAM A SIDE
				for(i = 0;i < 4;i++){
					if( !ms.pd[i].p_team ){	//	選択チームが ＴＥＡＭ Ａの場合
						load_TexPal( team_4p_player[i][0].pal );
						load_TexTile_4b( team_4p_player[i][ms.pd[i].play].pat,42,16,0,0,41,15 );
						draw_Tex( ms.adj_x + 96,113 + i * 20,42,16,0,0 );
					}
				}

				//	TEAM B SIDE
				for(i = 0;i < 4;i++){
					if( ms.pd[i].p_team ){	//	選択チームが ＴＥＡＭ Ｂの場合
						load_TexPal( team_4p_player[i][0].pal );
						load_TexTile_4b( team_4p_player[i][ms.pd[i].play].pat,42,16,0,0,41,15 );
						draw_Tex( ms.adj_x + 183,113 + i * 20,42,16,0,0 );
					}
				}

				//	覆面で覆う
				gSPDisplayList( gp++, Texture_NZ_dl);

				gDPSetPrimColor( gp++, 0,0, 128,128,128,128 );	//	暗く
				load_TexPal( dm_ts_waku_a_b_bm0_0tlut );		//	黒色パレットのロード
				load_TexTile_4b( dm_ts_waku_a_bm0_0,236,17,0,0,235,16 );
				draw_Tex( ms.adj_x + 42,47,236,14,0,0 );
				load_TexTile_4b( dm_ts_waku_c_bm0_0,236,4,0,0,235,3 );
				draw_Tex( ms.adj_x + 42,61,236,4,0,0 );

				load_TexTile_4b( team_4p_player[0][0].pat,42,16,0,0,41,15 );
				switch( ms.sub_flow ){
				case	255:	//	モードセレクトに戻る
				case 	253:	//	フレームイン
				case	0:	//	チーム分け
				case	1:	//	人間選択
					//	TEAM A SIDE
					for(i =  0;i < 4;i++){
						if( !ms.pd[i].p_team ){
							if( !ms.pd[i].play ){	//	プレイヤー
								if( ms.pd[i].ok ){	//	決定している場合
									draw_Tex( ms.adj_x + 96,113 + i * 20,42,16,0,0 );
								}
							}else{	//	CPU
								draw_Tex( ms.adj_x + 96,113 + i * 20,42,16,0,0 );
							}
						}
					}
					//	TEAM B SIDE
					for(i = 0;i < 4;i++){
						if( ms.pd[i].p_team ){
							if( !ms.pd[i].play ){	//	プレイヤー
								if( ms.pd[i].ok ){	//	決定している場合
									draw_Tex( ms.adj_x + 183,113 + i * 20,42,16,0,0 );
								}
							}else{	//	CPU
								draw_Tex( ms.adj_x + 183,113 + i * 20,42,16,0,0 );
							}
						}
					}
					break;
				case	2://	ＣＰＵ選択
					//	TEAM A SIDE
					for(i = 0;i < 4;i++){
						if( !ms.pd[i].p_team ){
							if( ms.pd[i].play ){	//	CPU
								if( i != ms.coms[ms.now_com] ){	//	選択中のＣＯＭでなかったら
									draw_Tex( ms.adj_x + 96,113 + i * 20,42,16,0,0 );
								}
							}else{	//	プレイヤーの場合
								draw_Tex( ms.adj_x + 96,113 + i * 20,42,16,0,0 );
							}
						}
					}
					//	TEAM B SIDE
					for(i = 0;i < 4;i++){
						if( ms.pd[i].p_team ){
							if( ms.pd[i].play ){	//	CPU
								if( i != ms.coms[ms.now_com] ){	//	選択中のＣＯＭでなかったら
									draw_Tex( ms.adj_x + 183,113 + i * 20,42,16,0,0 );
								}
							}else{	//	プレイヤーの場合
								draw_Tex( ms.adj_x + 183,113 + i * 20,42,16,0,0 );	//	CPU
							}
						}
					}
					break;
				case	254://	FRAME OUＴ( フレームアウト中は全てに黒幕をつける )
					//	TEAM A SIDE
					for(i = 0;i < 4;i++){
						if( !ms.pd[i].p_team ){
							draw_Tex( ms.adj_x + 96,113 + i * 20,42,16,0,0 );
						}
					}
					//	TEAM B SIDE
					for(i = 0;i < 4;i++){
						if( ms.pd[i].p_team ){
							draw_Tex( ms.adj_x + 183,113 + i * 20,42,16,0,0 );
						}
					}
					break;
				}

				// 上矢印
				gDPSetPrimColor( gp++,0, 0, col, col, col, 255);
				if( ms.sub_flow == 0 ){
					load_TexPal( arrow_u_bm0_0tlut );
					load_TexBlock_4b( arrow_u_bm0_0,16,7 );
					draw_TexFlip(ms.adj_x + 154 ,65 + (ms.cnt/15)%2,16,7,0,0, flip_off,flip_off);
				}

				//	右矢印
				load_TexPal( arrow_r_bm0_0tlut );
				load_TexTile_4b( arrow_r_bm0_0,8,11,0,0,7,10 );

				switch( ms.sub_flow ){
				case	1:	//	人間選択
					//	TEAM A SIDE
					for(i = 0;i < 4;i++){
						if( !ms.pd[i].p_team ){
							if( !ms.pd[i].play ){	//	プレイヤー
								if( !ms.pd[i].ok ){	//	未決定
									draw_Tex( ms.adj_x + 138 + (ms.cnt/15)%2 ,115 + i * 20,8,11,0,0 );	//	矢印描画
								}
							}
						}
					}
					break;
				case	2:	//	CPU選択
					//	TEAM A SIDE
					for(i = j = 0;i < 4;i++){
						if( !ms.pd[i].p_team ){
							if( ms.pd[i].play ){	//	CPU
								if( i == ms.coms[ms.now_com] ){	//	選択中のCPUの場合
									draw_Tex( ms.adj_x + 138 + (ms.cnt/15)%2 ,115 + i * 20,8,11,0,0 );	//	矢印描画
								}
							}
						}
					}
					break;
				}

				switch( ms.sub_flow ){
				case	1:	//	人間選択
					//	TEAM B SIDE
					for(i = j = 0;i < 4;i++){
						if( ms.pd[i].p_team ){
							if( !ms.pd[i].play ){	//	プレイヤー
								if( !ms.pd[i].ok ){	//	未決定
									draw_TexFlip( ms.adj_x + 174 - (ms.cnt/15)%2 ,115 + i * 20,8,11,0,0,flip_on,flip_off );	//	矢印描画
								}
							}
						}
					}
					break;
				case	2:
					//	TEAM B SIDE
					for(i = j = 0;i < 4;i++){
						if( ms.pd[i].p_team ){
							if( ms.pd[i].play ){	//	CPU
								if( i == ms.coms[ms.now_com] ){	//	選択中のCPUの場合
									draw_TexFlip( ms.adj_x + 174 - (ms.cnt/15)%2 ,115 + i * 20,8,11,0,0,flip_on,flip_off );	//	矢印描画
								}
							}
						}
					}
					break;
				}
				break;
			}

			break;
		case CS_4P_SELECT_MAN:		// キャラ選択
			for( i = 0; i < 4; i ++ ){
				pd = &ms.pd[i];
				fd = &ms.pd[i].fd;
				pc = &plcom[i][pd->play];

				// パネル
				gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,0,319,239 );
				gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );
				disp_tex_4bt( plate4p_char[i].pat, plate4p_char[i].pal, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
				disp_tex_4bt( pc->pat, pc->pal, pc->width, pc->height, pd->bx+4+ms.adj_x, pd->by+4,SET_PTD );

				//	チーム表示
				if( ms.team ){
					load_TexPal( dm_ts_pal_bm0_0tlut );
					load_TexTile_4b( dm_ts_team_ab_bm0_0,44,13,0,0,43,12 );
					draw_Tex( pd->bx+ms.adj_x+36,pd->by+1,22,13,ms.pd[i].p_team * 22,0 );
				}

				// 顔
				gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,pd->by+25,319,pd->by+121 );
				gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );
				top = ( pd->chara >= 2 )?pd->chara-2:ms.max_chara-( 2-pd->chara );
				if( fd->move == 1 ){
					top = ( top - 1 >= 0 )?top-1:ms.max_chara-1;
				} else if( fd->move == -1 ){
					top = ( top + 1 < ms.max_chara )?top+1:0;
				}
				for( j = 0; j < 5; j ++ ){
					for( k = 0, fl = 1; k < 4; k ++ ){
						if( ms.pd[k].ok && ms.pd[k].chara == ((top+j)%ms.max_chara) && i != k )fl = 0;
					}
					if( !fl || (pd->ok && j != 2) ){
						gDPSetPrimColor( gp++, 0,0, 100,100,100,255 );
					} else {
						gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );
					}
					disp_tex_4bt_sc( faces[(top+j)%ms.max_chara].pat, faces[(top+j)%ms.max_chara].pal, 48, 48, pd->bx+7+ms.adj_x, pd->by+49+fd->pos_y[j], 1.0, fd->sc[j], 0 );
				}
				// 顔枠
				if( (!ms.sub_flow && !pd->ok && !pd->play) || (ms.sub_flow == 1 && i == ms.coms[ms.now_com]) ){
					j = ( ( ms.cnt/30 ) % 2 ) * 127 + 128;
				} else {
					j = 255;
				}
				gDPSetPrimColor( gp++, 0,0, j,j,j,255 );
				disp_tex_4bt( face_frame[i].pat, face_frame[i].pal, 48, 48, ms.pd[i].bx+7+ms.adj_x, ms.pd[i].by+49,SET_PTD );
			}

			// 顔グラデ
			gSPDisplayList( gp++, Intensity_XNZ_Texture_dl);
			gDPSetPrimColor( gp++,0, 0, 0, 0, 0, 255);
			gDPSetEnvColor( gp++, 196, 196, 196, 255);
			for( i = 0; i < 4; i ++ ){
				pd = &ms.pd[i];

				gDPLoadTextureTile_4b( gp++, face4p_sm_bm0_0, G_IM_FMT_I,48,24,0,0,48-1,24-1,
								0, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK,G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
				gSPScisTextureRectangle( gp++, (pd->bx+7+ms.adj_x)<<2, (pd->by+25)<<2, (pd->bx+7+48+ms.adj_x)<<2, (pd->by+25+24)<<2,
								G_TX_RENDERTILE, 0, 0, 1<<10, 1<<10);

				gDPLoadTextureTile_4b( gp++, face4p_sm_bm0_0, G_IM_FMT_I,48,24,0,0,47,23,
								0, G_TX_CLAMP, G_TX_MIRROR, G_TX_NOMASK,5, G_TX_NOLOD, G_TX_NOLOD);
				gSPScisTextureRectangle( gp++, (pd->bx+7+ms.adj_x)<<2, (pd->by+97)<<2, (pd->bx+7+48+ms.adj_x)<<2, (pd->by+97+24)<<2,
								G_TX_RENDERTILE, 0, (32+8)<<5, 1<<10, 1<<10);
			}
			gSPDisplayList( gp++, Normal_XNZ_Texture_dl);

			switch( ms.sub_flow ){
				case 0:		// 人間選択
				case 253:	// フレームイン
				case 255:	// プレイヤー人数選択へ
					for( i = 0; i < 4; i ++ ){
						pd = &ms.pd[i];
						if( !pd->play ){
							if( !pd->ok ){		// 選択中
								// 矢印
								gDPSetPrimColor( gp++,0, 0, col, col, col, 255);
								disp_tex_4bt( arrow_u_bm0_0,arrow_u_bm0_0tlut,16,7,pd->bx+26+ms.adj_x,pd->by+41-(ms.cnt/15)%2,SET_PTD );
								disp_tex_4bt( arrow_d_bm0_0,arrow_d_bm0_0tlut,16,7,pd->bx+26+ms.adj_x,pd->by+98+(ms.cnt/15)%2,SET_PTD );
							} else {			// 決定
								// 暗パネル
								gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
								disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
								// ＯＫ
								gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 255);
								disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+42+ms.adj_x,pd->by+4,SET_PTD );
							}
						} else {				// ＣＯＭは待ち
							// 暗パネル
							gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
							disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
						}
					}
					break;
				case 1:		// ＣＯＭ選択
					for( i = 0; i < 4; i ++ ){
						pd = &ms.pd[i];

						if( !pd->play ){		// 人間
							// 暗パネル
							gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
							disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
							// ＯＫ
							gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 255);
							disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+42+ms.adj_x,pd->by+4,SET_PTD );
						} else {				// ＣＯＭ
							if( i == ms.coms[ms.now_com] ){
								// 矢印
								gDPSetPrimColor( gp++,0, 0, col, col, col, 255);
								disp_tex_4bt( arrow_u_bm0_0,arrow_u_bm0_0tlut,16,7,pd->bx+26+ms.adj_x,pd->by+41-(ms.cnt/15)%2,SET_PTD );
								disp_tex_4bt( arrow_d_bm0_0,arrow_d_bm0_0tlut,16,7,pd->bx+26+ms.adj_x,pd->by+98+(ms.cnt/15)%2,SET_PTD );
							} else {
								// 暗パネル
								gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
								disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
								if( pd->ok ){
									// ＯＫ
									gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 255);
									disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+42+ms.adj_x,pd->by+4,SET_PTD );
								}
							}
						}
					}
					break;
				case 254: 	// フレームアウト
					for( i = 0; i < 4; i ++ ){
						pd = &ms.pd[i];
						// 暗パネル
						gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
						disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
						// ＯＫ
						gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 255);
						disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+42+ms.adj_x,pd->by+4,SET_PTD );
					}
					break;
			}
			break;

		case CS_4P_SELECT_LEVEL:		// レベル選択

			pd = &ms.pd[0];

			// 下段パネル
			if( ms.coms[0] == -1 ){
				mx = ui->bx;
				disp_tex_4bt( plate_env1_bm0_0,plate_env1_bm0_0tlut,128, 42, ui->bx+ms.adj_x,ui->by,SET_PTD );
			} else {
				mx = ui->bx + 124;
				disp_tex_4bt( com_music_bm0_0,com_music_bm0_0tlut,256, 42, ui->bx+ms.adj_x,ui->by,SET_PTD );
			}

			// ＣＯＭレベル
			gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,ui->by+18,319,ui->by+34 );
			if( ms.coms[0] != -1 ){
				for( i = 0; i < 3; i ++ ){
					disp_tex_4bt( com_lv[i].pat,com_lv[i].pal,80,16,ui->bx+26+ms.adj_x,ui->by+18+(i+ui->com_lv-2)*16+ui->com_adj_y,SET_PTD );
				}
			}
			// 音楽
			if( ms.coms[0] == -1 ){
				if( ms.sub_flow == 2 || ui->mus_adj_y != 0 ){
					for( i = 0; i < 4; i ++ ){
						disp_tex_4bt( music[i][(ms.cnt/30)%2].pat,music[i][(ms.cnt/30)%2].pal,48,16,mx+41+ms.adj_x,ui->by+18+(i-ui->mus_no)*16+ui->mus_adj_y,SET_PTD );
					}
				}else{
					disp_tex_4bt( music[ui->mus_no][0].pat,music[ui->mus_no][0].pal,48,16,mx+41+ms.adj_x,ui->by+18,SET_PTD );
				}
			}else{
				if( (ms.sub_flow == 2 && ui->pos == 1) || ui->mus_adj_y != 0 ){
					for( i = 0; i < 4; i ++ ){
						disp_tex_4bt( music[i][(ms.cnt/30)%2].pat,music[i][(ms.cnt/30)%2].pal,48,16,mx+41+ms.adj_x,ui->by+18+(i-ui->mus_no)*16+ui->mus_adj_y,SET_PTD );
					}
				} else {
					disp_tex_4bt( music[ui->mus_no][0].pat,music[ui->mus_no][0].pal,48,16,mx+41+ms.adj_x,ui->by+18,SET_PTD );
				}
			}
			gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,0,319,239 );

			for( i = 0; i < 4; i ++ ){
				pd = &ms.pd[i];
				fd = &ms.pd[i].fd;
				pc = &plcom[i][pd->play];

				// パネル
				disp_tex_4bt( plate4p_lv[i].pat, plate4p_lv[i].pal, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );

				// Player ＆ COM
				disp_tex_4bt( pc->pat, pc->pal, pc->width, pc->height, pd->bx+4+ms.adj_x, pd->by+4,SET_PTD );

				// 選択スピード
				disp_tex_4bt( speed4p_pat[pd->speed], speed4p_pal[i], 28, 24, pd->bx+33+ms.adj_x, pd->by+32+(2-pd->speed)*33,SET_PTD );

				// レベルゲージ
				gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,pd->by+(20-pd->lv)*4+41,319,239);
				disp_tex_4bt( lv_guage_bm0_0,lv_guage_bm0_0tlut,8,84, pd->bx+15+ms.adj_x, pd->by+40,SET_PTD );
				gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,0,319,239 );

				// レベル数値
				adj = ( pd->lv < 10 )?4:0;
				itoa( pd->lv,cwo,10 );
				for( j = 0; cwo[j] != '\0'; j++ ){
					disp_tex_4bt( &lv_num_bm0_0[(cwo[j]-'0')*48],lv_num_bm0_0tlut,8,12,pd->bx+14+j*8+adj+ms.adj_x,pd->by+27,SET_PTD );
				}
			}
			//	チーム表示
			if( ms.team ){
				load_TexPal( dm_ts_pal_bm0_0tlut );
				load_TexTile_4b( dm_ts_team_ab_bm0_0,44,13,0,0,43,12 );
				for(i = 0;i < 4;i++){
					pd = &ms.pd[i];
					draw_Tex( pd->bx+ms.adj_x+36,pd->by+1,22,13,ms.pd[i].p_team * 22,0 );
				}
			}

			gDPSetRenderMode( gp++, G_RM_XLU_SURF,G_RM_XLU_SURF2 );

			switch( ms.sub_flow ){
				case 0:		// 人間設定
				case 253:	// フレームイン
				case 254:	// フレームアウト
					for( i = 0; i < 4; i ++ ){
						pd = &ms.pd[i];
						if( !pd->play ){
							if( !pd->ok ){		// 選択中
								// 暗パネル
								gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
								disp_tex_4bt( plate4p_sms_bm0_0, plate4p_sms_bm0_0tlut, 28, 112, pd->bx+3+(1-pd->flag)*29+ms.adj_x, pd->by+14,SET_PTD );
								// 矢印
								gDPSetPrimColor( gp++,0, 0, col, col, col, 255);
								if( !pd->flag ){
									disp_tex_4bt( arrow_r_bm0_0,arrow_r_bm0_0tlut,8,11, pd->bx+6+(ms.cnt/15)%2+ms.adj_x,pd->by+36+(20-pd->lv)*4,SET_PTD );
								} else {
									disp_tex_4bt( arrow_r_bm0_0,arrow_r_bm0_0tlut,8,11,pd->bx+25+(ms.cnt/15)%2+ms.adj_x,pd->by+38+(2-pd->speed)*33,SET_PTD );
								}
							} else {			// 決定
								// 暗パネル
								gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
								disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
								// ＯＫ
								gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 255);
								disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+42+ms.adj_x,pd->by+4,SET_PTD );
							}
						} else {				// ＣＯＭは待ち
							// 暗パネル
							gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
							disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
						}
					}
					// 下段暗パネル
					gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
					if( ms.coms[0] == -1 ){
						disp_tex_4bt( music_sm_bm0_0,music_sm_bm0_0tlut,128,42,ui->bx+ms.adj_x,ui->by,SET_PTD );
					} else {
						disp_tex_4bt( plate_e_a_bm0_0,plate_e_a_bm0_0tlut,256,42, ui->bx+ms.adj_x,ui->by,SET_PTD );
					}
					break;
				case 1:		// ＣＯＭ設定
					for( i = 0; i < 4; i ++ ){
						pd = &ms.pd[i];

						if( !pd->play ){
							// 暗パネル
							gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
							disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
							// ＯＫ
							gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 255);
							disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+42+ms.adj_x,pd->by+4,SET_PTD );
						} else {
							if( i == ms.coms[ms.now_com] ){
								// 暗パネル
								gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
								disp_tex_4bt( plate4p_sms_bm0_0, plate4p_sms_bm0_0tlut, 28, 112, pd->bx+3+(1-pd->flag)*29+ms.adj_x, pd->by+14,SET_PTD );
								// 矢印
								gDPSetPrimColor( gp++,0, 0, col, col, col, 255);
								if( !pd->flag ){
									disp_tex_4bt( arrow_r_bm0_0,arrow_r_bm0_0tlut,8,11, pd->bx+6+(ms.cnt/15)%2+ms.adj_x,pd->by+36+(20-pd->lv)*4,SET_PTD );
								} else {
									disp_tex_4bt( arrow_r_bm0_0,arrow_r_bm0_0tlut,8,11,pd->bx+25+(ms.cnt/15)%2+ms.adj_x,pd->by+38+(2-pd->speed)*33,SET_PTD );
								}
							} else {
								// 暗パネル
								gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
								disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
								if( pd->ok ){
									// ＯＫ
									gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 255);
									disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+42+ms.adj_x,pd->by+4,SET_PTD );
								}
							}
						}
					}
					// 下段暗パネル
					gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
					if( ms.coms[0] == -1 ){
						disp_tex_4bt( music_sm_bm0_0,music_sm_bm0_0tlut,128,42,ui->bx+ms.adj_x,ui->by,SET_PTD );
					} else {
						disp_tex_4bt( plate_e_a_bm0_0,plate_e_a_bm0_0tlut,256,42, ui->bx+ms.adj_x,ui->by,SET_PTD );
					}
					break;
				case 2:		// ＣＯＭレベル・音楽設定
					// 矢印
					gDPSetPrimColor( gp++,0, 0, col, col, col, 255);
					if( ms.coms[0] == -1 ){		// ＣＯＭがいないときはこっち
						if( ui->mus_no > 0 ){
							disp_tex_4bt( arrow_u_bm0_0,arrow_u_bm0_0tlut,16,7,mx+61+ms.adj_x,ui->by+11-(ms.cnt/15)%2,SET_PTD );
						}
						if( ui->mus_no < 3 ){
							disp_tex_4bt( arrow_d_bm0_0,arrow_d_bm0_0tlut,16,7,mx+61+ms.adj_x,ui->by+34+(ms.cnt/15)%2,SET_PTD );
						}
					} else {					// ＣＯＭがいるときはこっち
						pd = &ms.pd[0];
						if( !ui->pos ){			// ＣＯＭＬＶ用矢印
							if( ui->com_lv < 2 ){
								disp_tex_4bt( arrow_u_bm0_0,arrow_u_bm0_0tlut,16,7,ui->bx+61+ms.adj_x,ui->by+11-(ms.cnt/15)%2,SET_PTD );
							}
							if( ui->com_lv > 0 ){
								disp_tex_4bt( arrow_d_bm0_0,arrow_d_bm0_0tlut,16,7,ui->bx+61+ms.adj_x,ui->by+34+(ms.cnt/15)%2,SET_PTD );
							}
						} else {				// 音楽用矢印
							if( ui->mus_no > 0 ){
								disp_tex_4bt( arrow_u_bm0_0,arrow_u_bm0_0tlut,16,7,mx+61+ms.adj_x,ui->by+11-(ms.cnt/15)%2,SET_PTD );
							}
							if( ui->mus_no < 3 ){
								disp_tex_4bt( arrow_d_bm0_0,arrow_d_bm0_0tlut,16,7,mx+61+ms.adj_x,ui->by+34+(ms.cnt/15)%2,SET_PTD );
							}
						}
						// 暗パネル
						gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
						disp_tex_4bt( plate_e_p_bm0_0,plate_e_p_bm0_0tlut,128,34,ui->bx+5+(1-ui->pos)*124, ui->by+4,SET_PTD );
					}
					for( i = 0; i < 4; i ++ ){
						pd = &ms.pd[i];
						// 暗パネル
						gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
						disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
						// ＯＫ
						gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 255);
						disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+42+ms.adj_x,pd->by+4,SET_PTD );
					}
					break;
				case 255:		// ゲームへ
					for( i = 0; i < 4; i ++ ){
						pd = &ms.pd[i];
						// 暗パネル
						gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
						disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
						// ＯＫ
						gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 255);
						disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+42+ms.adj_x,pd->by+4,SET_PTD );
					}
					// 音楽暗パネル
					gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
					if( ms.coms[0] == -1 ){
						disp_tex_4bt( music_sm_bm0_0,music_sm_bm0_0tlut,128,42,ui->bx+ms.adj_x,ui->by,SET_PTD );
					} else {
						disp_tex_4bt( plate_e_a_bm0_0,plate_e_a_bm0_0tlut,256,42, ui->bx+ms.adj_x,ui->by,SET_PTD );
					}
					break;
			}
			break;
	}
}

void main60(NNSched* sched)
{
	OSMesgQueue msgQ;
	OSMesg		msgbuf[MAX_MESGS];
	NNScClient	client;
	int i;
	game_state_old_sub	*config;

	osCreateMesgQueue(&msgQ, msgbuf, MAX_MESGS);	// create message queue for VI retrace
	nnScAddClient(sched, &client, &msgQ);			// add client to shceduler


	// データ読み込み
	auRomDataRead( (u32)_lv_sel_dataSegmentRomStart, gfx_freebuf[GFX_SPRITE_PAGE], (u32)(_lv_sel_dataSegmentRomEnd - _lv_sel_dataSegmentRomStart));

	for(i = 0;i < MAXCONTROLLERS;i ++){	//	キーリピート対応キーの設定
		joyflg[i] = 0x0000 | DM_KEY_UP | DM_KEY_DOWN | DM_KEY_LEFT | DM_KEY_RIGHT ;
	}
	joycur1 = 24;
	joycur2 = 6;
	joystp = FALSE;			// start button pause ... off.
	#ifdef STICK_3D_ON
	joystk = TRUE;			// joy stick key.
	#endif

	main_60_out_flg = 1;

	evs_seqence = TRUE;			//	音楽フラグを立てる
	evs_seqnumb = SEQ_Menu;		//	曲の設定
	dm_init_se();				//	効果音の初期化

	// 初期化
	switch( evs_gamesel ){
		case GSL_1PLAY:
			init_1p_play();
			break;
		case GSL_2PLAY:
			init_2p_play(1,0);
			break;
		case GSL_4PLAY:
			init_4p_play(1);
			break;
		case GSL_VSCPU:
			if( evs_story_flg ){
				init_story_play();
			}else{
				init_2p_play(1,1);
			}
			break;
	}

	//	画面切り替わり処理
	if( main_old == MAIN_GAME ){
		 HCEffect_Init(gfx_freebuf[0], HC_SPHERE);
	}else{
		 HCEffect_Init(gfx_freebuf[0], HC_CURTAIN);
	}

#ifdef	DM_DEBUG_FLG
	osSyncPrintf("Start Main60 \n");
#endif

	graphic_no = GFX_60;

	// メインループ
	while( main_60_out_flg ) {
		(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);	// VI retrace wait.
		if(	!HCEffect_Main() ){		//	画面切り替え処理
			switch( evs_gamesel ){
				case GSL_1PLAY:
					calc_1p_play();	//	１PLAY用処理
					break;
				case GSL_2PLAY:
					calc_2p_play();	//	２PLAY用処理
					break;
				case GSL_4PLAY:
					calc_4p_play();	//	４PLAY用処理
					break;
				case GSL_VSCPU:
					if( evs_story_flg ){
						calc_story_play();	//	STORY用処理
					}else{
						calc_2p_play();		//	２PLAY用処理( 内部でCPU用の処理を行っている )
					}
					break;
			}
		}
		dm_menu_se_play();	//	効果音再生
		#ifndef THREAD_JOY
		joyProcCore();
		#endif
	}
	if( main_no == MAIN_12 ){											// ゲームに行く場合、曲の再生を停止する
		auSeqPlayerStop(0);												// stop the audio.
		while( auSeqPlayerState(0) != AL_STOPPED ) {
			(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
		}
	}
	graphic_no = GFX_NULL;												// graphics finish.
	while( pendingGFX != 0 ) {
		(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
	}

	// code of charm. 980213.
	nnScRemoveClient(sched, &client);									// remove client to shceduler

	// 後処理
	main_old = MAIN_60;													//	現在のモードの保存
	switch( evs_gamesel ){
		case GSL_1PLAY:
			// スピード
			config = &evs_mem_data[evs_select_name_no[0]].state_old;

			game_state_data[0].cap_def_speed			= ms.pd[0].speed;	//	カプセル落下速度の設定
			config -> p_1p_sp							= ms.pd[0].speed;	//	カプセル落下速度の設定( 設定の保存 )

			// ウィルス
			game_state_data[0].virus_level				= ms.pd[0].lv;		//	ウイルスレベルの設定
			config -> p_1p_lv 							= ms.pd[0].lv;		//	ウイルスレベルの設定( 設定の保存 )

			//	音楽
			config -> p_1p_m 							= ms.ui.mus_no;		//	曲番号の設定( 設定の保存 )
			break;
		case GSL_2PLAY:
			for( i = 0; i < 2; i++ ){
				config = &evs_mem_data[evs_select_name_no[i]].state_old;

				// キャラ
				config -> p_vm_no								= ms.pd[i].chara;	//	キャラクタ番号の設定( 設定の保存 )

				game_state_data[i].anime.cnt_charcter_no		= ms.pd[i].chara;	//	キャラクタ番号の設定( 表示用 )
				game_state_data[i].player_state[PS_THINK_FLG]	= ms.pd[i].chara;	//	キャラクタ番号の設定( 思考用 )

				// スピード
				game_state_data[i].cap_def_speed				= ms.pd[i].speed;	//	カプセル落下速度の設定
				config -> p_vm_sp								= ms.pd[i].speed;	//	カプセル落下速度の設定( 設定の保存 )

				// ウィルス
				game_state_data[i].virus_level					= ms.pd[i].lv;		//	ウイルスレベルの設定
				config -> p_vm_lv								= ms.pd[i].lv;		//	ウイルスレベルの設定( 設定の保存 )

				//	背景
				config -> p_vm_st								= ms.ui.stg_tbl[ms.ui.stg_no];	//	背景番号設定( 設定の保存 )

				//	音楽
				config -> p_vm_m 								= ms.ui.mus_no;		//	曲番号の設定( 設定の保存 )
			}
			// ステージ
			evs_stage_no = ms.ui.stg_tbl[ms.ui.stg_no];			//	背景番号設定
			break;
		case GSL_4PLAY:
			for( i = 0; i < 4; i ++ ){
				// キャラ
				game_state_data[i].anime.cnt_charcter_no		= ms.pd[i].chara;	//	キャラクタ番号の設定( 表示用 )
				game_state_data[i].player_state[PS_THINK_FLG]	= ms.pd[i].chara;	//	キャラクタ番号の設定( 思考用 )
				evs_4p_state_old.p_4p_no[i]						= ms.pd[i].chara;	//	キャラクタ番号の設定( 設定の保存 )

				// スピード
				game_state_data[i].cap_def_speed				= ms.pd[i].speed;	//	カプセル落下速度の設定
				evs_4p_state_old.p_4p_sp[i]						= ms.pd[i].speed;	//	カプセル落下速度の設定( 設定の保存 )

				// ウィルス
				game_state_data[i].virus_level					= ms.pd[i].lv;		//	ウイルスレベルの設定
				evs_4p_state_old.p_4p_lv[i]						= ms.pd[i].lv;		//	ウイルスレベルの設定( 設定の保存 )

				// ＣＯＭレベル
				if( ms.pd[i].play ){
					game_state_data[i].player_state[PS_CPU_LEVEL]	= ms.ui.com_lv;	//	CPUレベルの設定
				}

				//	チーム分け
				if( ms.team == 0){	//	バトルロイヤルの場合
					game_state_data[i].player_state[PS_TEAM_FLG]	= i;	//	チーム番号の設定( 全員異なる( 0,1,2,3 ) )
				}else{				//	チーム戦の場合
					game_state_data[i].player_state[PS_TEAM_FLG]	= ms.pd[i].p_team;	//	チーム番号の設定( 0 か 1 )
					evs_4p_state_old.p_4p_team[i]					= ms.pd[i].p_team;	//	チーム番号の設定( 設定の保存 )
				}
			}
			evs_4p_state_old.p_4p_t_m	= ms.team;			//	ゲームモード( チーム戦かバトルロイヤル )
			evs_4p_state_old.p_4p_m 	= ms.ui.mus_no;		//	曲番号の設定( 設定の保存 )
			evs_4p_state_old.p_4p_c_lv	= ms.ui.com_lv;		//	CPUレベルの設定( 設定の保存 )
			break;
		case GSL_VSCPU:
			config = &evs_mem_data[evs_select_name_no[0]].state_old;
			if( evs_story_flg ){	//	ストーリーモードの場合
				switch( ms.flow ){
				case	CS_STORY_SERECT_LEVEL:	//	通常の場合
					evs_story_level 	= ms.pd[0].lv;		//	レベル設定( EASY/NORMAL/HARD )
					config -> p_st_lv 	= ms.pd[0].lv;		//	レベル設定( 設定の保存 )
					evs_story_no 		= 0;				//	ストーリー番号を０( 1面 )にする
					evs_one_game_flg	= 0;				//	特殊処理フラグを消す
					break;
				case	CS_STORY_SERECT_STAGE:	//	ステージセレクトの場合
					evs_story_level 		= ms.pd[1].lv;			//	レベル設定
					evs_story_no 			= ms.pd[1].speed;		//	ステージ設定
					config -> p_st_st_lv 	= ms.pd[1].lv;			//	レベル設定( 設定の保存 )
					config -> p_st_st 		= ms.pd[1].speed + 1;	//	ステージ設定( 設定の保存 )

					evs_one_game_flg = 1;							//	特殊処理フラグを立てる
					break;
				}
				main_old = MAIN_60;
			}else{				//	ＶＳＣＰＵの場合
				for( i = 0; i < 2; i++ ){
					// キャラ
					game_state_data[i].anime.cnt_charcter_no		= ms.pd[i].chara;	//	キャラクタ番号の設定( 表示用 )
					game_state_data[i].player_state[PS_THINK_FLG]	= ms.pd[i].chara;	//	キャラクタ番号の設定( 思考用 )

					// スピード
					game_state_data[i].cap_def_speed				= ms.pd[i].speed;	//	カプセル落下速度の設定

					// ウィルス
					game_state_data[i].virus_level					= ms.pd[i].lv;		//	ウイルスレベルの設定

					// ＣＯＭレベル
					if( ms.pd[i].play ){
						game_state_data[i].player_state[PS_CPU_LEVEL]	= ms.ui.com_lv;	//	CPUレベルの設定
					}
					config -> p_vc_lv[i] 	= ms.pd[i].lv;					//	ウイルスレベルの設定( 設定の保存 )
					config -> p_vc_sp[i] 	= ms.pd[i].speed;				//	カプセル落下速度の設定( 設定の保存 )
					config -> p_vc_no[i] 	= ms.pd[i].chara;				//	キャラクタ番号の設定( 設定の保存 )
				}
				config -> p_vc_cp_lv 		= ms.ui.com_lv;					//	CPUレベルの設定( 設定の保存 )
				config -> p_vc_st 			= ms.ui.stg_tbl[ms.ui.stg_no];	//	背景番号設定( 設定の保存 )
				config -> p_vm_m 			= ms.ui.mus_no;					//	曲番号の設定( 設定の保存 )


				// ステージ
				evs_stage_no = ms.ui.stg_tbl[ms.ui.stg_no];	//	背景番号設定

			}
			break;
	}

	for( i = 0;i < 4;i++ ){
		game_state_data[i].game_score = 0;	//	得点の初期化
		game_state_data[i].game_retry = 0;	//	リトライ回数の初期化
	}

	evs_game_time = 0;	//	共通ゲームタイム初期化

	//	音楽設定
	if( evs_one_game_flg ){
		evs_seqnumb = evs_story_no % 3;		//	曲番号の設定
		evs_seqence = TRUE;					//	ＢＧＭを鳴らす
	}else{
		if ( ms.ui.mus_no < 3) {
			evs_seqnumb = ms.ui.mus_no;		//	曲番号の設定
			evs_seqence = TRUE;				//	ＢＧＭを鳴らす
		} else {
			evs_seqence = FALSE;			//	ＢＧＭを鳴らさない
		}
	}
	if( main_no == MAIN_MODE_SELECT ){		//	モードセレクトに戻る
		evs_one_game_flg	= 0;			//	特殊処理フラグを消す
	}


	return;
}

// グラフィック
void graphic60(void)
{
	NNScTask *t;

	// graphic display list buffer. task buffer.
	gp = &gfx_glist[gfx_gtask_no][0];
	t  = &gfx_task[gfx_gtask_no];

	// RSP initialize.

	gSPSegment(gp++, 0, 0x0);
	gSPSegment(gp++, OBJ_SEGMENT,	osVirtualToPhysical(gfx_freebuf[0]));
	gSPSegment(gp++, BG_SEGMENT,	osVirtualToPhysical((void *)gfx_freebuf[GFX_SPRITE_PAGE]));

	F3RCPinitRtn();
	F3ClearFZRtn(0);


	S2RDPinitRtn(TRUE);

	// モード別分岐
	switch( evs_gamesel ){
		case GSL_1PLAY:
			disp_1p_play();
			break;
		case GSL_2PLAY:
			disp_2p_play();
			break;
		case GSL_4PLAY:
			disp_4p_play();
			break;
		case GSL_VSCPU:
			if( evs_story_flg ){
				disp_story_play();
			}else{
				disp_2p_play();
			}
			break;
	}
	HCEffect_Grap(&gp);

	// end of display list & go the graphic task.
	gDPFullSync(gp++);
	gSPEndDisplayList(gp++);
	osWritebackDCacheAll();
	gfxTaskStart(t, gfx_glist[gfx_gtask_no], (s32)(gp - gfx_glist[gfx_gtask_no]) * sizeof(Gfx), GFX_GSPCODE_F3DEX, NN_SC_SWAPBUFFER);
}



