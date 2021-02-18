/*--------------------------------------
	filename	:	dm_mode_select_main.c

	create		:	1999/10/28
	modify		:	1999/12/02

	created		:	Hiroyuki Watanabe
--------------------------------------*/
#define F3DEX_GBI
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
#include "audio.h"
#include "joy.h"
#include "evsworks.h"
#include "static.h"
#include "sprite.h"
#include "record.h"

#include	"hardcopy.h"

#include	"dm_nab_include.h"

//#define	_IA_TEST_

static	mode_select_control		dm_ms_control;
static	mode_select_control		*ms_p	=	&dm_ms_control;
static	ms_title_pos			*ms_tp	=	&dm_ms_control.title_pos;
static	ms_mes_win_pos			*ms_mp	=	&dm_ms_control.ms_mes_win;
static	ms_cap_pos				*ms_cp	=	&dm_ms_control.cap_pos;

/*--------------------------------------
	モードセレクト用 EEP 書き込み関数
--------------------------------------*/
void	dm_ms_eep_write(u32 err_add)
{
	EepRomErr err = EepRomErr_NoError;

	err = EepRom_WriteSystem();

	*(EepRomErr *)err_add = err;
}

/*--------------------------------------
	モードセレクト初期化関数
--------------------------------------*/
void	init_dm_ms(void)
{
	s8	i;


	for( i = 0;i < 4;i++ ){
		joyflg[i] = 0x0000 | DM_KEY_UP | DM_KEY_DOWN | DM_KEY_LEFT | DM_KEY_RIGHT ;
	}

	ms_tp -> t_pos[MS_X_P]			=	32;		//	タイトル の初期Ｘ座標設定
	ms_tp -> t_pos[MS_Y_P]			=	-40;	//	タイトル の初期Ｙ座標設定
	ms_tp -> k_pos[MS_X_P]			=	328;	//	キー 	 の初期Ｘ座標設定
	ms_tp -> k_pos[MS_Y_P]			=	12;		//	キー 	 の初期Ｙ座標設定
	ms_tp -> p_pos[MS_X_P]			=	350;	//	キノピオ の初期Ｘ座標設定
	ms_tp -> p_pos[MS_Y_P]			=	140;	//	キノピオ の初期Ｙ座標設定
	ms_tp -> p_mouth_flg			=	0;		//	キノピオクチパクフラグ 		の設定
	ms_tp -> p_mouth_count			=	1;		//	キノピオクチパクカウンタ	の設定
	ms_tp -> p_eye_flg				=	0;		//	キノピオ目パチフラグ 		の設定
	ms_tp -> p_eye_count			=	0;		//	キノピオ目パチカウンタ		の設定

	ms_mp -> mes_win_pos[MS_X_P]	=	256;	//	Message Window の初期Ｘ座標設定
	ms_mp -> mes_win_pos[MS_Y_P]	=	172;	//	Message Window の初期Ｙ座標設定
	ms_mp -> mes_win_siz[MS_X_SIZ]	=	0;		//	Message Window の初期横幅設定
	ms_mp -> mes_win_siz[MS_Y_SIZ]	=	2;		//	Message Window の初期縦幅設定
	ms_mp -> mes_win_flg			=	0;		//	Message Window の表示無しに設定
	ms_mp -> mes_mes_flg			=	1;		//	Message 表示無しに設定
	ms_mp -> mes_next_flg			=	0;		//	Message 改行表示無し設定
	ms_mp -> mes_mes_len			=	0;		//	Message 長さを０に設定
	ms_mp -> mes_advance_count		=	0;		//	Message 進行カウンタの設定


	ms_p -> scrool_x				= 	MS_SCROOL_SPEED;	//	移動速度
	ms_p -> black_alpha				= 	0xff;				//	黒幕
	ms_cp -> c_count				=	0;					//	キー入力待ちカウントの初期化
	ms_cp -> c_pos_size				=	0;					//	操作説明選択時のカーソルのサイズの増減値を０にする
	ms_cp -> c_pos					=	0;					//	カプセルの揺れの大きさ
	ms_cp -> main_c_pos				=	0;					//	メインのカーソル位置設定
	ms_cp -> sub_c_pos				=	0;					//	過去のカーソル位置保存
	for( i = 0;i < 2;i++ ){
		ms_cp -> c_alpha[i]			=	0xff;				//	カーソルの点滅
		ms_cp -> name_c_pos[i]		=	4;					//	カーソル位置をゲストに設定
		ms_cp -> name_c_flg[i]		=	0;					//	カーソル決定フラグを未決定に設定
	}

	switch(main_old)
	{
	case	MAIN_TITLE:			//	タイトルからきた
		ms_p -> main_type		= 	MS_M_SCROOL;		//	MAIN 制御設定
		ms_p -> main_type_next	= 	MS_M_MAIN;			//	次の MAIN 制御設定
		ms_p -> sub_type		= 	MS_S_FADE_IN_A;		//	MS_M_SCROOL 内での最初の処理設定
		ms_p -> main_grp_type	=	MS_M_MAIN;			//	MAIN 描画設定
		ms_p ->	main_x			=	-274;				//	基準座標設定
		ms_p ->	stop_x			=	23;					//	移動目標座標設定
		break;
	case	MAIN_60:			//	ゲーム設定から来た
	case	MAIN_NAME_ENTRY:	//	名前入力からきた場合
		ms_p -> main_type		= 	MS_M_SCROOL;		//	MAIN 制御設定
		ms_p -> sub_type		= 	MS_S_FADE_IN_A;		//	MS_M_SCROOL 内での最初の処理設定
		switch( evs_gamesel ){
		case	GSL_4PLAY:		//	4PLAY
			ms_p -> main_type_next	= 	MS_M_MAIN_C;				//	次の MAIN 制御設定
			ms_p -> main_grp_type	=	MS_M_MAIN_C;				//	MAIN 描画設定
			ms_p ->	main_x			=	-274;						//	基準座標設定
			ms_p ->	stop_x			=	23;							//	移動目標座標設定
			ms_cp -> main_c_pos		=	evs_4p_state_old.p_4p_p_m;	//	メインのカーソル位置設定
			break;
		case	GSL_VSCPU:		//	VSCOM
			if( evs_story_flg ){
				ms_p -> main_type_old	=	MS_M_MAIN_A;		//	以前の MAIN 制御設定
				ms_cp -> sub_c_pos		=	0;					//	メインのカーソル位置設定
			}else{
				ms_p -> main_type_old	=	MS_M_MAIN_B;		//	以前の MAIN 制御設定
				ms_cp -> sub_c_pos		=	1;					//	メインのカーソル位置設定
			}
			ms_p -> main_type_next		= 	MS_M_NAME_A;		//	次の MAIN 制御設定
			ms_p -> main_grp_type		=	MS_M_NAME_A;		//	MAIN 描画設定
			ms_p ->	main_x				=	-300;				//	基準座標設定
			ms_p ->	stop_x				=	0;					//	移動目標座標設定

			break;
		case	GSL_2PLAY:
			ms_cp -> sub_c_pos			=	0;					//	メインのカーソル位置設定
			ms_p -> main_type_old		=	MS_M_MAIN_B;		//	以前の MAIN 制御設定
			ms_p -> main_type_next		= 	MS_M_NAME_B;		//	次の MAIN 制御設定
			ms_p -> main_grp_type		=	MS_M_NAME_B;		//	MAIN 描画設定
			ms_p ->	main_x				=	-300;				//	基準座標設定
			ms_p ->	stop_x				=	0;					//	移動目標座標設定
			break;
		case	GSL_1PLAY:
			ms_p -> main_type_old		=	MS_M_MAIN_A;		//	以前の MAIN 制御設定
			ms_p -> main_type_next		= 	MS_M_NAME_A;		//	次の MAIN 制御設定
			ms_p -> main_grp_type		=	MS_M_NAME_A;		//	MAIN 描画設定
			ms_p ->	main_x				=	-300;				//	基準座標設定
			ms_p ->	stop_x				=	0;					//	移動目標座標設定
			ms_cp -> sub_c_pos			=	1;					//	メインのカーソル位置設定
			break;
		case	GSL_OPTION:
			ms_p -> main_type_old		=	MS_M_MAIN_D;		//	以前の MAIN 制御設定
			ms_p -> main_type_next		= 	MS_M_NAME_C;		//	次の MAIN 制御設定
			ms_p -> main_grp_type		=	MS_M_NAME_C;		//	MAIN 描画設定
			ms_p ->	main_x				=	-300;				//	基準座標設定
			ms_p ->	stop_x				=	0;					//	移動目標座標設定
			ms_cp -> sub_c_pos			=	2;					//	メインのカーソル位置設定
			break;
		}
		break;
	case	MAIN_RANK:			//	記録から来た
		ms_p -> main_type		= 	MS_M_SCROOL;		//	MAIN 制御設定
		ms_p -> main_type_next	= 	MS_M_MAIN_D;		//	次の MAIN 制御設定
		ms_p -> sub_type		= 	MS_S_FADE_IN_A;		//	MS_M_SCROOL 内での最初の処理設定
		ms_p -> main_grp_type	=	MS_M_MAIN_D;		//	MAIN 描画設定
		ms_p ->	main_x			=	-320;				//	基準座標設定
		ms_p ->	stop_x			=	20;					//	移動目標座標設定
		break;
	case	MAIN_MANUAL:		//	操作説明から来た
		ms_p -> main_type		= 	MS_M_SCROOL;		//	MAIN 制御設定
		ms_p -> main_type_next	= 	MS_M_MAIN_D_A;		//	次の MAIN 制御設定
		ms_p -> main_grp_type	=	MS_M_MAIN_D_A;		//	MAIN 描画設定
		ms_p -> sub_type		= 	MS_S_FADE_IN_B;		//	MS_M_SCROOL 内での最初の処理設定
		ms_p ->	main_x			=	-246;				//	基準座標設定
		ms_p ->	stop_x			=	37;					//	移動目標座標設定
		ms_cp -> main_c_pos		=	evs_manual_no;		//	カーソル位置を選択した操作説明にする
		break;
	}
}



/*--------------------------------------
	モードセレクト処理メイン関数
--------------------------------------*/
int		dm_mode_select_main(NNSched*	sched)
{
	OSMesgQueue	msgQ;
	OSMesg		msgbuf[MAX_MESGS];
	NNScClient	client;
	EepRomErr	eep_err;
	int	ret;
	s16	i,j,max,omake_time = 0;
	s16	sin_f = 180,sin_f_c = 180;
	s8	sec_30_flg = 0;
	s8	sec_str_flg = 0;
	s8	out_flg = 1;	//	ループ脱出用フラグ
	u8	main_cont[2];
	s8	k[2];

	//	メッセージキューの作成
  	osCreateMesgQueue(&msgQ, msgbuf, MAX_MESGS);	//	メッセージＱの割り当て

	//	クライアントに登録
	nnScAddClient(sched, &client, &msgQ);


	//	グラフィックデータの読込み
	auRomDataRead((u32)_dm_mode_select_dataSegmentRomStart,(void *)_dm_mode_select_dataSegmentStart	, (u32)(_dm_mode_select_dataSegmentRomEnd - _dm_mode_select_dataSegmentRomStart));

	//	変数初期化
	init_dm_ms();

	//	画面切り替え処理初期化
//	init_change_scene();

	//	画面切り替え初期化
	if( main_old != MAIN_TITLE ){
	    HCEffect_Init(gfx_freebuf[0], HC_SPHERE);
	}


	//	コントローラ接続状況確認
	evs_playmax = joyResponseCheck();
	for(i = 0;i < 2;i++){
		main_cont[i] = main_joy[i];
	}

	//	リトレースメッセージ（保険）
	(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);


	dm_init_se();
	evs_seqence = TRUE;	//	音楽フラグを立てる
	evs_seqnumb = SEQ_Menu;	//	再生曲のセット


	//	グラフィックの設定
	graphic_no = GFX_MODE_SELECT;


	//	メインループ

	while( out_flg )
	{
		#ifndef THREAD_JOY
		joyProcCore();
		#endif

		//	リトレースメッセージ
		(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
//		calc_change_scene();	//	画面切り替え処理

		sec_30_flg ^= 1;

		//	目パチ
		if( ms_tp -> p_eye_flg ){
			ms_tp -> p_eye_count ++;
			if( ms_tp -> p_eye_count >= 3 ){	//	目パチ中止
				ms_tp -> p_eye_flg		=	0;
				ms_tp -> p_eye_count	=	0;
			}
		}

		//	各アニメーション処理
		if( ms_p -> sub_type == MS_S_DEFAULT ){
			//	文章をすすめるフラグ
			ms_mp -> mes_advance_count ++;
			if( ms_mp -> mes_advance_count >= 2 ){
				ms_mp -> mes_advance_count = 0;
				//	文字列進行の設定
				ms_mp -> mes_mes_len ++;
				if( ms_mp -> mes_mes_len > MS_STR_LEN_MAX ){
					ms_mp -> mes_mes_len = MS_STR_LEN_MAX;
				}
				//	口パクアニメーションの設定
				if( ms_tp -> p_mouth_count == 1 ){
					if( ms_mp -> mes_mes_len % 2 ){
						ms_tp -> p_mouth_flg ^= 1;
					}
				}else{
					ms_tp -> p_mouth_flg = 0;
				}
				//	目パチアニメーションの設定
				if( !ms_tp -> p_eye_flg ){
					if( (RAND(100) % 100) == 99){
						ms_tp -> p_eye_flg = 1;
					}
				}
			}

			//	カプセルの上下の揺れ
			sin_f -= 4;
			if( sin_f <= 0 ){
				sin_f += 360;
			}
			ms_cp -> c_pos = sinf(DEGREE(sin_f)) * 10;

			//	カーソルの点滅
			sin_f_c -= 10;
			if(sin_f_c <= 0){
				sin_f_c += 360;
			}
			ms_cp -> c_alpha[0] = ms_cp -> c_alpha[1] = 200 + (sinf(DEGREE(sin_f_c)) * 55);

			//	カーソルの拡大縮小
			ms_cp -> c_pos_size = (s8)(sinf( DEGREE( ms_cp -> c_pos_size_count ) ) * 8 );
			ms_cp -> c_pos_size_count += 4;
			if( ms_cp -> c_pos_size_count >= 360 ){
				ms_cp -> c_pos_size_count -= 360;
			}

		}else{
			//	アニメーションしない
			ms_tp -> p_mouth_flg	= 0;
			ms_mp -> mes_mes_len	= 0;
			ms_cp -> c_alpha[0]		= 0xff;
			ms_cp -> c_alpha[1]		= 0xff;
		}

		switch( ms_p -> main_type )
		{
		case	MS_M_SCROOL:
			switch( ms_p -> sub_type )
			{
			case	MS_S_FADE_IN_A:		//	FADE in	A
				if( !HCEffect_Main() ){
					ms_p -> black_alpha = 0;
					ms_p -> sub_type = MS_S_TITLE_IN_A;//	処理をタイトルとキーとキノピオの移動へ
				}
				break;
			case	MS_S_FADE_IN_B:		//	FADE in	A
				if( !HCEffect_Main() ){
					ms_p -> black_alpha = 0;
					ms_p -> sub_type = MS_S_TITLE_IN_B;	//	処理をタイトルとキーの移動へ
				}
				break;
			case	MS_S_TITLE_IN_A:	//	TITLE type  A
				ms_tp -> p_pos[MS_X_P] -= 8;		//	キノピオの移動
				ms_tp -> k_pos[MS_X_P] -= 8;		//	キーの移動
				ms_tp -> t_pos[MS_Y_P] += 4;		//	タイトルの移動

				if( ms_tp -> t_pos[MS_Y_P] >= 12 ){		//	タイトルの移動修正
					ms_tp -> t_pos[MS_Y_P] = 12;
				}
				if( ms_tp -> k_pos[MS_X_P] <= 208 ){	//	キーの移動修正
					ms_tp -> k_pos[MS_X_P] = 208;
				}
				if( ms_tp -> p_pos[MS_X_P] <= 232 ){	//	キノピオの移動修正
					ms_tp -> p_pos[MS_X_P] 			=	232;
					ms_mp -> mes_win_siz[MS_SIZ_P]	=	0;
					ms_mp -> mes_win_flg			=	1;					//	Message Window の表示ありに設定
					ms_mp -> mes_mes_flg			=	1;					//	Message の表示ありに設定
					ms_p -> sub_type 				=	MS_S_MES_WIN_IN_A;	//	Message Window の移動・拡大処理に
				}
				break;
			case	MS_S_TITLE_IN_B:	//	TITLE type  B
				ms_tp -> k_pos[MS_X_P] -= 8;			//	キーの移動
				ms_tp -> t_pos[MS_Y_P] += 4;			//	タイトルの移動

				if( ms_tp -> t_pos[MS_Y_P] >= 12 ){		//	タイトルの移動修正
					ms_tp -> t_pos[MS_Y_P] = 12;
				}
				if( ms_tp -> k_pos[MS_X_P] <= 208 ){		//	キーの移動修正
					ms_tp -> k_pos[MS_X_P] = 208;
					ms_p -> sub_type = MS_S_SCROOL_IN_A;	//	アイテム(カプセルや黒板)が画面内に移動する処理に
				}
				break;
			case	MS_S_MES_WIN_IN_A:	//	MESSAGE WIN type A
				ms_mp -> mes_win_pos[MS_X_P]	= ms_win_pos_x[ms_mp -> mes_win_siz[MS_SIZ_P]];	//	Ｘ座標の設定
				ms_mp -> mes_win_pos[MS_Y_P]	= ms_win_pos_y[ms_mp -> mes_win_siz[MS_SIZ_P]];	//	Ｙ座標の設定
				ms_mp -> mes_win_siz[MS_X_SIZ]	= ms_win_siz_w[ms_mp -> mes_win_siz[MS_SIZ_P]];	//	横幅の設定
				ms_mp -> mes_win_siz[MS_Y_SIZ]	= ms_win_siz_h[ms_mp -> mes_win_siz[MS_SIZ_P]];	//	縦幅の設定
				if(	sec_30_flg  ){
					ms_mp -> mes_win_siz[MS_SIZ_P] ++;
				}
				if( ms_mp -> mes_win_siz[MS_SIZ_P] > 7 ){	//	移動・拡大終了
					ms_mp -> mes_win_siz[MS_SIZ_P] = 7;
					ms_p -> sub_type = MS_S_SCROOL_IN_A;	//	アイテム(カプセルや黒板)が画面内に移動する処理に
				}
				break;
			case	MS_S_SCROOL_IN_A:	//	ITEM in
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM 移動修正
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	ms_p -> main_type_next;	//	次のメイン処理の設定
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	次のサブ処理の設定
				}
				break;
			case	MS_S_MES_WIN_OUT_A:
				ms_mp -> mes_win_pos[MS_X_P]	= ms_win_pos_x[ms_mp -> mes_win_siz[MS_SIZ_P]];	//	Ｘ座標の設定
				ms_mp -> mes_win_pos[MS_Y_P]	= ms_win_pos_y[ms_mp -> mes_win_siz[MS_SIZ_P]];	//	Ｙ座標の設定
				ms_mp -> mes_win_siz[MS_X_SIZ]	= ms_win_siz_w[ms_mp -> mes_win_siz[MS_SIZ_P]];	//	横幅の設定
				ms_mp -> mes_win_siz[MS_Y_SIZ]	= ms_win_siz_h[ms_mp -> mes_win_siz[MS_SIZ_P]];	//	縦幅の設定
				if(	sec_30_flg  ){
					ms_mp -> mes_win_siz[MS_SIZ_P] --;
				}
				if( ms_mp -> mes_win_siz[MS_SIZ_P] <= 0 ){	//	Message Window の移動・縮小処理終了
					ms_mp -> mes_win_siz[MS_SIZ_P] = 0;
					ms_mp -> mes_win_flg = 0;
					ms_mp -> mes_mes_flg = 0;
					ms_p -> sub_type = MS_S_SCROOL_OUT_A;	//	キノピオの画面外移動
				}
				break;
			case	MS_S_SCROOL_OUT_A:
				ms_tp -> p_pos[MS_X_P] += 8;			//	キノピオの移動
				if( ms_tp -> p_pos[MS_X_P] >= 320 ){	//	キノピオの移動修正
					ms_tp -> p_pos[MS_X_P] 			=	320;
					ms_p -> main_grp_type			=	MS_M_MAIN_D_A;			//	描画処理を操作説明選択
					ms_p -> main_type_next			=	MS_M_MAIN_D_A;			//	メイン処理を操作説明選択
					ms_p -> sub_type 				=	MS_S_SCROOL_IN_A;		//	アイテム(カプセルや黒板)が画面内に移動する処理に
					ms_p -> main_x					=	-246;					//	基準座標設定
					ms_p -> stop_x					=	37;						//	停止座標設定
				}
				break;
			case	MS_S_MENU_OUT_START:	//	モードセレクト終了処理開始
				out_flg = 0;
				ms_p -> sub_type = MS_S_MENU_OUT_END;
				break;
			case	MS_S_MENU_OUT_END:
				break;
			}
			break;
		case	MS_M_MAIN:	//	1P/2P/4P/その他
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY 操作
				if( joycur[main_cont[0]] & DM_KEY_LEFT) {
					dm_set_menu_se( SE_mUpDown );	//	SE 設定
					ms_cp -> main_c_pos --;
					ms_mp -> mes_mes_len = 0;
					if( ms_cp -> main_c_pos < 0 ){
						ms_cp -> main_c_pos = 3;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_RIGHT) {
					dm_set_menu_se( SE_mUpDown );	//	SE 設定
					ms_cp -> main_c_pos ++;
					ms_mp -> mes_mes_len = 0;
					if( ms_cp -> main_c_pos > 3 ){
						ms_cp -> main_c_pos = 0;
					}
				}else	if( joyupd[main_cont[0]] & DM_KEY_B ){	//	メニュー終了処理(タイトルに戻る)
					ms_p -> main_type	= MS_M_SCROOL;
					ms_p -> sub_type	= MS_S_MENU_OUT_START;
					ret = MAIN_TITLE;	//	タイトルに戻る
				}else	if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	決定
					dm_set_menu_se( SE_mDecide );					//	SE 設定
					ms_p -> sub_type	=	 MS_S_SCROOL_OUT_NEXT;	//	スクロールアウト処理設定
				}
				break;
			case	MS_S_SCROOL_OUT_NEXT:	//	カプセルスクロールアウト
				evs_story_flg = 0;
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_cp -> sub_c_pos 			=	ms_cp -> main_c_pos;	//	カーソル位置保存
					ms_cp -> main_c_pos			=	0;						//	カーソル位置設定
					switch( ms_cp -> sub_c_pos ){
					case	0:	//	1PLAY
						ms_p -> main_x			=	-224;			//	基準座標設定
						ms_p -> stop_x			=	48;				//	停止座標設定
						ms_p -> main_grp_type	=	MS_M_MAIN_A;	//	描画の設定
						break;
					case	1:	//	2PLAY
						ms_p -> main_x			=	-224;			//	基準座標設定
						ms_p -> stop_x			=	48;				//	停止座標設定
						ms_p -> main_grp_type	=	MS_M_MAIN_B;	//	描画の設定
						if( evs_playmax < 2 ){
							ms_cp -> main_c_pos	=	1;				//	カーソル位置設定
						}
						break;
					case	2:	//	4PLAY
						ms_p -> main_x			=	-274;			//	基準座標設定
						ms_p -> stop_x			=	23;				//	停止座標設定
						ms_p -> main_grp_type	=	MS_M_MAIN_C;	//	描画の設定
						ms_cp -> main_c_pos		=	4 - evs_playmax;//	カーソル位置設定
						break;
					case	3:	//	OTHER
						ms_p -> main_x			=	-320;			//	基準座標設定
						ms_p -> stop_x			=	20;				//	停止座標設定
						ms_p -> main_grp_type	=	MS_M_MAIN_D;	//	描画の設定
						break;
					}
					ms_p -> sub_type		 	=	MS_S_SCROOL_IN_NEXT;	//	スクロールイン処理設定
				}
				break;
			case	MS_S_SCROOL_IN_NEXT:
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM 移動修正
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	ms_p -> main_grp_type;	//	次のメイン処理の設定
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	次のサブ処理の設定
				}
				break;
			}
			break;
		case	MS_M_MAIN_A:	//	1PLAY
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY 操作
				if( joycur[main_cont[0]] & DM_KEY_LEFT) {
					dm_set_menu_se( SE_mUpDown );	//	SE 設定
					ms_cp -> main_c_pos --;
					ms_mp -> mes_mes_len = 0;
					if( ms_cp -> main_c_pos < 0 ){
						ms_cp -> main_c_pos = 1;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_RIGHT) {
					dm_set_menu_se( SE_mUpDown );	//	SE 設定
					ms_cp -> main_c_pos ++;
					ms_mp -> mes_mes_len = 0;
					if( ms_cp -> main_c_pos > 1 ){
						ms_cp -> main_c_pos = 0;
					}
				}else	if( joyupd[main_cont[0]] & DM_KEY_B ){	//	メニュー終了処理(タイトルに戻る)
					ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;
				}else	if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	決定
					dm_set_menu_se( SE_mDecide );		//	SE 設定
					ms_p -> sub_type = MS_S_SCROOL_OUT_NEXT;	//	スクロールアウト処理設定

				}
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	カプセルスクロールアウト
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-274;					//	基準座標設定
					ms_p -> stop_x			=	23;						//	停止座標設定
					ms_p -> main_grp_type	=	MS_M_MAIN;				//	描画の設定
					ms_cp -> main_c_pos		=	0;						//	カーソル位置設定
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_BACK;	//	スクロールイン処理設定
				}
				break;
			case	MS_S_SCROOL_IN_BACK:		//	カプセルスクロールイン
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM 移動修正
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_MAIN;				//	次のメイン処理の設定
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	次のサブ処理の設定
				}
				break;
			case	MS_S_SCROOL_OUT_NEXT:
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> sub_type		=	MS_S_SCROOL_IN_NEXT;	//	次のサブ処理の設定
					ms_p -> main_type_old	=	ms_p -> main_type;		//	現在の処理の保存
					ms_p -> main_grp_type	=	MS_M_NAME_A;			//	描画の設定
					ms_p -> main_x			=	-300;					//	基準座標設定
					ms_p -> stop_x			=	0;						//	停止座標設定
					ms_cp -> sub_c_pos		=	ms_cp -> main_c_pos;	//	現在のカーソル位置の保存
					ms_cp -> name_c_pos[0]	=	4;						//	カーソル位置をゲストに設定
					ms_cp -> name_c_flg[0]	=	0;						//	カーソル決定フラグを未決定に設定
					evs_game_time			=	0;						//	プレイ時間の初期化
					switch( ms_cp -> sub_c_pos){
					case	0:	//	STORY
						evs_story_flg	=	1;
						evs_gamesel 	=	GSL_VSCPU;				//	ゲームモードをVSCPUへ変更
						break;
					case	1:	//	LEVEL
						evs_gamesel 	=	GSL_1PLAY;				//	ゲームモードを1PLAYへ変更
						break;
					}
				}
				break;
			case	MS_S_SCROOL_IN_NEXT:
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM 移動修正
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_NAME_A;		//	次のメイン処理の設定( 名前選択 )
					ms_p -> sub_type	=	MS_S_DEFAULT;		//	次のサブ処理の設定
				}
				break;
			}
			break;
		case	MS_M_MAIN_B:	//	2PLAY
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY 操作
				if( joycur[main_cont[0]] & DM_KEY_LEFT) {
					i = 1;
					if( ms_cp -> main_c_pos == 1 ){	//	カーソル位置が MAN vs COM
						if( evs_playmax < 2 ){		//	コントローラーが１つのみの場合
							i = 0;
						}
					}
					if( i ){
						dm_set_menu_se( SE_mUpDown );	//	SE 設定
						ms_cp -> main_c_pos --;
						ms_mp -> mes_mes_len = 0;
						if( ms_cp -> main_c_pos < 0 ){
							ms_cp -> main_c_pos = 1;
						}
					}

				}else	if( joycur[main_cont[0]] & DM_KEY_RIGHT) {
					i = 1;
					if( ms_cp -> main_c_pos == 1 ){	//	カーソル位置が MAN vs COM
						if( evs_playmax < 2 ){		//	コントローラーが１つのみの場合
							i = 0;
						}
					}
					if( i ){
						dm_set_menu_se( SE_mUpDown );	//	SE 設定
						ms_cp -> main_c_pos ++;
						ms_mp -> mes_mes_len = 0;
						if( ms_cp -> main_c_pos > 1 ){
							ms_cp -> main_c_pos = 0;
						}
					}
				}else	if( joyupd[main_cont[0]] & DM_KEY_B ){	//	メニュー終了処理(タイトルに戻る)
					ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;
				}else	if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	決定
					dm_set_menu_se( SE_mDecide );		//	SE 設定
					if( ms_cp -> main_c_pos == 0 ){		//	VSMAN の場合
						if( evs_playmax >= 2 ){			//	2CON 以上でないと次に進めない
							ms_p -> sub_type = MS_S_SCROOL_OUT_NEXT;	//	スクロールアウト処理設定
						}
					}else{
						ms_p -> sub_type = MS_S_SCROOL_OUT_NEXT;	//	スクロールアウト処理設定
					}
				}
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	カプセルスクロールアウト
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-274;					//	基準座標設定
					ms_p -> stop_x			=	23;						//	停止座標設定
					ms_p -> main_grp_type	=	MS_M_MAIN;				//	描画の設定
					ms_cp -> main_c_pos		=	1;						//	カーソル位置設定
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_BACK;	//	スクロールイン処理設定
				}
				break;
			case	MS_S_SCROOL_IN_BACK:		//	カプセルスクロールイン
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM 移動修正
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_MAIN;				//	次のメイン処理の設定
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	次のサブ処理の設定
				}
				break;
			case	MS_S_SCROOL_OUT_NEXT:
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> sub_type		=	MS_S_SCROOL_IN_NEXT;	//	次のサブ処理の設定
					ms_p -> main_type_old	=	ms_p -> main_type;		//	現在の処理の保存
					ms_p -> main_x			=	-300;					//	基準座標設定
					ms_p -> stop_x			=	0;						//	停止座標設定
					ms_cp -> sub_c_pos		=	ms_cp -> main_c_pos;	//	現在のカーソル位置の保存
					evs_game_time			=	0;						//	プレイ時間の初期化
					for( i = 0;i < 2;i++ ){
						ms_cp -> name_c_pos[i]	=	4;					//	カーソル位置をゲストに設定
						ms_cp -> name_c_flg[i]	=	0;					//	カーソル決定フラグを未決定に設定
					}

					switch( ms_cp -> sub_c_pos){
					case	0:	//	VSMAN
						evs_gamesel 	=	GSL_2PLAY;				//	ゲームモードを2PLAYへ変更
						ms_p -> main_grp_type	=	MS_M_NAME_B;	//	描画の設定
						break;
					case	1:	//	VSCPU
						evs_gamesel 	=	GSL_VSCPU;				//	ゲームモードをVSCPUへ変更
						ms_p -> main_grp_type	=	MS_M_NAME_A;	//	描画の設定
						break;
					}
				}
				break;
			case	MS_S_SCROOL_IN_NEXT:
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM 移動修正
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	ms_p -> main_grp_type;	//	次のメイン処理の設定
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	次のサブ処理の設定
				}
				break;
			}
			break;
		case	MS_M_MAIN_C:	//	4PLAY
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY 操作
				if( joycur[main_cont[0]] & DM_KEY_LEFT) {
					j = ms_cp -> main_c_pos;	//	移動したか調べるためカーソル位置の保存
					if( evs_playmax < (( 4 - ms_cp -> main_c_pos ) + 1 ) ){
						ms_cp -> main_c_pos = 3;
					}else{
						ms_cp -> main_c_pos --;
					}
					if( j != ms_cp -> main_c_pos ){
						dm_set_menu_se( SE_mUpDown );	//	SE 設定
						ms_mp -> mes_mes_len = 0;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_RIGHT) {
					j = ms_cp -> main_c_pos;	//	移動したか調べるためカーソル位置の保存
					ms_cp -> main_c_pos ++;
					if( ms_cp -> main_c_pos > 3 ){
						ms_cp -> main_c_pos = 4 - evs_playmax;
					}
					if( j != ms_cp -> main_c_pos ){
						dm_set_menu_se( SE_mUpDown );	//	SE 設定
						ms_mp -> mes_mes_len = 0;
					}
				}else	if( joyupd[main_cont[0]] & DM_KEY_B ){	//	メニュー終了処理(タイトルに戻る)
					ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;
				}else	if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	決定
					evs_gamesel	=	GSL_4PLAY;	//	ゲームモードを４人用へ変更
					ret			=	MAIN_60;	//	ゲーム設定へ
					evs_4p_state_old.p_4p_p_m = ms_cp -> main_c_pos;	//	選択したスタイルの保存
					switch( ms_cp -> main_c_pos ){
					case	0:	//	4MAN
						if( evs_playmax == 4){
							dm_set_menu_se( SE_mDecide );		//	SE 設定
							ms_p -> main_type	=	MS_M_SCROOL;
							ms_p -> sub_type	=	MS_S_MENU_OUT_START;
							for( i = 0;i < 4;i++ ){
								game_state_data[i].player_state[PS_PLAYER]				= 	PUF_PlayerMAN;	//	全員人
							}
						}
						break;
					case	1:	//	3MAN & 1COM
						if( evs_playmax >= 3){
							dm_set_menu_se( SE_mDecide );		//	SE 設定
							ms_p -> main_type	=	MS_M_SCROOL;
							ms_p -> sub_type	=	MS_S_MENU_OUT_START;
							for(i = j = 0;i < 4;i++ ){
								if( link_joy[i] ){
									if( j < 3 ){
										game_state_data[i].player_state[PS_PLAYER]				=	PUF_PlayerMAN;	//	人
										j++;
									}else{
										game_state_data[i].player_state[PS_PLAYER] 				=	PUF_PlayerCPU;	//	CPU
									}
								}else{
									game_state_data[i].player_state[PS_PLAYER]					=	PUF_PlayerCPU;	//	CPU
								}
							}
						}
						break;
					case	2:	//	2MAN & 2COM
						if( evs_playmax >= 2){
							dm_set_menu_se( SE_mDecide );		//	SE 設定
							ms_p -> main_type	=	MS_M_SCROOL;
							ms_p -> sub_type	=	MS_S_MENU_OUT_START;
							for(i = j = 0;i < 4;i++ ){
								if( link_joy[i] ){
									if( j < 2 ){
										game_state_data[i].player_state[PS_PLAYER] 				=	PUF_PlayerMAN;	//	人
										j++;
									}else{
										game_state_data[i].player_state[PS_PLAYER]				=	PUF_PlayerCPU;	//	CPU
									}
								}else{
									game_state_data[i].player_state[PS_PLAYER]					=	PUF_PlayerCPU;	//	CPU
								}
							}
						}
						break;
					case	3:	//	1MAN & 3COM
						dm_set_menu_se( SE_mDecide );		//	SE 設定
						ms_p -> main_type	=	MS_M_SCROOL;
						ms_p -> sub_type	=	MS_S_MENU_OUT_START;
						for(i = j = 0;i < 4;i++ ){
							if( link_joy[i] ){
								if( j < 1 ){
									game_state_data[i].player_state[PS_PLAYER]				=	PUF_PlayerMAN;	//	人
									j++;
								}else{
									game_state_data[i].player_state[PS_PLAYER]				=	PUF_PlayerCPU;	//	CPU
								}
							}else{
								game_state_data[i].player_state[PS_PLAYER]					=	PUF_PlayerCPU;	//	CPU
							}
						}
						break;
					}
				}
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	カプセルスクロールアウト
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-274;					//	基準座標設定
					ms_p -> stop_x			=	23;						//	停止座標設定
					ms_p -> main_grp_type	=	MS_M_MAIN;				//	描画の設定
					ms_cp -> main_c_pos		=	2;						//	カーソル位置設定
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_BACK;	//	スクロールイン処理設定
				}
				break;
			case	MS_S_SCROOL_IN_BACK:		//	カプセルスクロールイン
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM 移動修正
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_MAIN;				//	次のメイン処理の設定
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	次のサブ処理の設定
				}
				break;
			}
			break;
		case	MS_M_MAIN_D:	//	OTHER
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY 操作
				if( joycur[main_cont[0]] & DM_KEY_LEFT) {
					dm_set_menu_se( SE_mUpDown );	//	SE 設定
					ms_cp -> main_c_pos --;
					ms_mp -> mes_mes_len = 0;
					if( ms_cp -> main_c_pos < 0 ){
						ms_cp -> main_c_pos = 4;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_RIGHT) {
					dm_set_menu_se( SE_mUpDown );	//	SE 設定
					ms_cp -> main_c_pos ++;
					ms_mp -> mes_mes_len = 0;
					if( ms_cp -> main_c_pos > 4 ){
						ms_cp -> main_c_pos = 0;
					}
				}else	if( joyupd[main_cont[0]] & DM_KEY_B ){	//	メニュー終了処理(タイトルに戻る)
					ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;
				}else	if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	決定
					dm_set_menu_se( SE_mDecide );		//	SE 設定
					ms_p -> sub_type = MS_S_SCROOL_OUT_NEXT;	//	スクロールアウト処理設定
				}
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	カプセルスクロールアウト
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-274;					//	基準座標設定
					ms_p -> stop_x			=	23;						//	停止座標設定
					ms_p -> main_grp_type	=	MS_M_MAIN;				//	描画の設定
					ms_cp -> main_c_pos		=	3;						//	カーソル位置設定
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_BACK;	//	スクロールイン処理設定
				}
				break;
			case	MS_S_SCROOL_IN_BACK:		//	カプセルスクロールイン
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM 移動修正
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_MAIN;				//	次のメイン処理の設定
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	次のサブ処理の設定
				}
				break;
			case	MS_S_SCROOL_OUT_NEXT:
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> sub_type		=	MS_S_SCROOL_IN_NEXT;	//	次のサブ処理の設定
					ms_p -> main_type_old	=	ms_p -> main_type;		//	現在の処理の保存
					ms_cp -> sub_c_pos		=	ms_cp -> main_c_pos;	//	現在のカーソル位置の保存
					ms_cp -> main_c_pos		=	0;

					switch( ms_cp -> sub_c_pos )
					{
					case	0:	//	記録を見る
						ms_p -> main_type = MS_M_SCROOL;
						ms_p -> sub_type = MS_S_MENU_OUT_START;
						ret = MAIN_RANK;	//	記録モードに行く
						break;
					case	1:	//	遊び方
						ms_p -> main_type 		= 	MS_M_SCROOL;
						ms_p -> sub_type 		= 	MS_S_MES_WIN_OUT_A;
						break;
					case	2:	//	名前
						ms_p -> main_x			=	-300;					//	基準座標設定
						ms_p -> stop_x			=	0;						//	停止座標設定
						for( i = 0;i < 2;i++ ){
							ms_cp -> name_c_pos[i]	=	4;					//	カーソル位置をゲストに設定
							ms_cp -> name_c_flg[i]	=	0;					//	カーソル決定フラグを未決定に設定
						}
						ms_p -> main_grp_type	=	MS_M_NAME_C;	//	描画の設定
						break;
					case	3:	//	サウンド
						ms_p -> main_x			=	-224;			//	基準座標設定
						ms_p -> stop_x			=	48;				//	停止座標設定
						ms_p -> main_grp_type	=	MS_M_MAIN_D_C;	//	描画の設定
						break;
					case	4:	//	バックアップ
						ms_p -> main_x			=	-224;			//	基準座標設定
						ms_p -> stop_x			=	48;				//	停止座標設定
						ms_p -> main_grp_type	=	MS_M_MAIN_D_D;	//	描画の設定
						break;
					}
				}
				break;
			case	MS_S_SCROOL_IN_NEXT:
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM 移動修正
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	ms_p -> main_grp_type;	//	次のメイン処理の設定
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	次のサブ処理の設定
				}
				break;
			}
			break;
		case	MS_M_MAIN_D_A:
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY 操作
				if( joycur[main_cont[0]] & DM_KEY_UP) {
					if( ms_cp -> main_c_pos > 0 ){
						dm_set_menu_se( SE_mUpDown );	//	SE 設定
						ms_cp -> main_c_pos --;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_DOWN) {
					if( ms_cp -> main_c_pos < 5 ){
						dm_set_menu_se( SE_mUpDown );	//	SE 設定
						ms_cp -> main_c_pos ++;
					}
				}else	if( joyupd[main_cont[0]] & DM_KEY_B ){	//	メニュー終了処理(タイトルに戻る)
					ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;
				}else	if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	決定
					dm_set_menu_se( SE_mDecide );		//	SE 設定
					evs_manual_no 		= ms_cp -> main_c_pos;
					ms_p -> main_type	= MS_M_SCROOL;
					ms_p -> sub_type 	= MS_S_MENU_OUT_START;
					ret = MAIN_MANUAL;	//	操作説明に行く
				}
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	カプセルスクロールアウト
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-320;				//	基準座標設定
					ms_p -> stop_x			=	20;					//	停止座標設定
					ms_p -> main_type_next	=	MS_M_MAIN_D;		//	次のメイン処理の設定
					ms_p -> main_grp_type	=	MS_M_MAIN_D;		//	描画の設定
					ms_cp -> main_c_pos		=	1;					//	カーソル位置設定
					ms_p ->	sub_type		=	MS_S_TITLE_IN_A;	//	スクロールイン処理設定
					ms_p ->	main_type		=	MS_M_SCROOL;		//	スクロールイン処理設定
				}
				break;
			}
			break;
		case	MS_M_MAIN_D_C:	//	サウンド
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY 操作
				if( joycur[main_cont[0]] & DM_KEY_LEFT) {
					dm_set_menu_se( SE_mUpDown );	//	SE 設定
					ms_cp -> main_c_pos --;
					if( ms_cp -> main_c_pos < 0 ){
						ms_cp -> main_c_pos = 1;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_RIGHT) {
					dm_set_menu_se( SE_mUpDown );	//	SE 設定
					ms_cp -> main_c_pos ++;
					if( ms_cp -> main_c_pos > 1 ){
						ms_cp -> main_c_pos = 0;
					}
				}else	if( joyupd[main_cont[0]] & DM_KEY_B ){	//	メニュー終了処理(タイトルに戻る)
					ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;
				}else	if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	決定
					dm_set_menu_se( SE_mDecide );		//	SE 設定
					ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;	//	スクロールアウト処理設定
				}
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	カプセルスクロールアウト
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-320;			//	基準座標設定
					ms_p -> stop_x			=	20;				//	停止座標設定
					ms_p -> main_grp_type	=	MS_M_MAIN_D;	//	描画の設定
					ms_cp -> main_c_pos		=	3;				//	カーソル位置設定
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_BACK;	//	スクロールイン処理設定
				}
				break;
			case	MS_S_SCROOL_IN_BACK:		//	カプセルスクロールイン
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM 移動修正
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_MAIN_D;			//	次のメイン処理の設定
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	次のサブ処理の設定
				}
				break;
			}
			break;
		case	MS_M_MAIN_D_D:	//	バックアップ消去確認
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY 操作
				if( joycur[main_cont[0]] & DM_KEY_LEFT) {
					dm_set_menu_se( SE_mUpDown );	//	SE 設定
					ms_cp -> main_c_pos --;
					if( ms_cp -> main_c_pos < 0 ){
						ms_cp -> main_c_pos = 1;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_RIGHT) {
					dm_set_menu_se( SE_mUpDown );	//	SE 設定
					ms_cp -> main_c_pos ++;
					if( ms_cp -> main_c_pos > 1 ){
						ms_cp -> main_c_pos = 0;
					}
				}else	if( joyupd[main_cont[0]] & DM_KEY_B ){	//	メニュー終了処理
					ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;
				}else	if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	決定
					dm_set_menu_se( SE_mDecide );		//	SE 設定
					switch( ms_cp -> main_c_pos ){
					case	0:	//	キャンセル
						ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;	//	スクロールアウト処理設定
						break;
					case	1:
						//	初期化

						for(i = 0;i < 9;i++)
						{
							dm_init_save_mem( &evs_mem_data[i], i );
						}
						dm_init_config_4p_save();
						ms_mp -> mes_mes_len 	= 	0;
#ifdef	_DM_EEP_ROM_USE_
						EepRom_CreateThread();
						EepRom_SendTask(dm_ms_eep_write, (u32)&eep_err);
						ms_p -> sub_type		=	MS_S_EEP_WAIT;		//	次の処理
						break;

#else
						ms_p -> main_type 		=	MS_M_MAIN_D_D_A;	//	消去
						ms_p -> main_grp_type	=	MS_M_MAIN_D_D_A;	//	描画の設定
						break;
#endif	//	_DM_EEP_ROM_USE_

					}
				}
				break;
			case	MS_S_EEP_WAIT:
#ifdef	_DM_EEP_ROM_USE_
				if( EepRom_GetTaskCount() == 0 ){	//	EEP 書き込み完了
					EepRom_DestroyThread();
					ms_p -> main_type		=	MS_M_MAIN_D_D_A;	//	次の処理
					ms_p -> main_grp_type	=	MS_M_MAIN_D_D_A;	//	次の処理
				}
#endif	//	_DM_EEP_ROM_USE_
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	カプセルスクロールアウト
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-320;			//	基準座標設定
					ms_p -> stop_x			=	20;				//	停止座標設定
					ms_p -> main_grp_type	=	MS_M_MAIN_D;	//	描画の設定
					ms_cp -> main_c_pos		=	4;				//	カーソル位置設定
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_BACK;	//	スクロールイン処理設定
				}
				break;
			case	MS_S_SCROOL_IN_BACK:		//	カプセルスクロールイン
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM 移動修正
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_MAIN_D;			//	次のメイン処理の設定
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	次のサブ処理の設定
				}
				break;
			}
			break;
		case	MS_M_MAIN_D_D_A:	//	BACKUP ERACE
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY 操作
				if( ms_tp -> p_mouth_count == -1 ){	//	文字列終了
					ms_cp -> c_count = (ms_cp -> c_count + 1) % 24;
					if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	メニュー終了処理(タイトルに戻る)
						ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;
					}
				}
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	カプセルスクロールアウト
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-320;			//	基準座標設定
					ms_p -> stop_x			=	20;				//	停止座標設定
					ms_p -> main_grp_type	=	MS_M_MAIN_D;	//	描画の設定
					ms_cp -> main_c_pos		=	4;				//	カーソル位置設定
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_BACK;	//	スクロールイン処理設定
				}
				break;
			case	MS_S_SCROOL_IN_BACK:		//	カプセルスクロールイン
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM 移動修正
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_MAIN_D;			//	次のメイン処理の設定
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	次のサブ処理の設定
				}
				break;
			}
			break;
		case	MS_M_NAME_A:	//	NAME SELECT 1P
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY 操作
				if( joycur[main_cont[0]] & DM_KEY_LEFT) {
					if( ( ms_cp -> name_c_pos[0] % 3) != 0 ){
						dm_set_menu_se( SE_mLeftRight );	//	SE 設定
						ms_cp -> name_c_pos[0] --;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_RIGHT) {
					if( ( ms_cp -> name_c_pos[0] % 3) != 2 ){
						dm_set_menu_se( SE_mLeftRight );	//	SE 設定
						ms_cp -> name_c_pos[0] ++;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_UP) {
					if( ms_cp -> name_c_pos[0] > 2 ){
						dm_set_menu_se( SE_mLeftRight );	//	SE 設定
						ms_cp -> name_c_pos[0] -= 3;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_DOWN) {
					if( ms_cp -> name_c_pos[0] < 6 ){
						dm_set_menu_se( SE_mLeftRight );	//	SE 設定
						ms_cp -> name_c_pos[0] += 3;
					}
				}else	if( joyupd[main_cont[0]] & DM_KEY_B ){	//	メニュー終了処理(タイトルに戻る)
					ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;
				}else	if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	決定
					dm_set_menu_se( SE_mDecide );		//	SE 設定
					if( ms_cp -> name_c_pos[0] == 4 ){	//	ゲスト
						j = -1;
						evs_select_name_no[0] = DM_MEM_GUEST;
					}else	if( ms_cp -> name_c_pos[0] < 4 ){
						evs_select_name_no[0] = j = ms_cp -> name_c_pos[0];
					}else	if( ms_cp -> name_c_pos[0] > 4 ){
						j = ms_cp -> name_c_pos[0] - 1;
						evs_select_name_no[0] = j;
					}
					if( j == -1 ){
						ret = MAIN_60;	//	ゲーム設定へ
					}else{
						if(	evs_mem_data[j].mem_use_flg & DM_MEM_USE ){	//	名前有り
							ret = MAIN_60;	//	ゲーム設定へ
						}else	if( ( evs_mem_data[j].mem_use_flg & DM_MEM_USE ) == 0 ){	//	名前無し
							evs_neme_entry_flg[0] = 1;
							ret = MAIN_NAME_ENTRY;
						}
					}
					ms_p -> sub_type	= MS_S_MENU_OUT_START;	//	終了
					ms_p -> main_type	= MS_M_SCROOL;			//	終了
				}
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	カプセルスクロールアウト
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-224;					//	基準座標設定
					ms_p -> stop_x			=	48;						//	停止座標設定
					ms_p -> main_grp_type	=	ms_p -> main_type_old;	//	描画の設定
					ms_cp -> main_c_pos		=	ms_cp -> sub_c_pos;		//	カーソル位置設定
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_BACK;	//	スクロールイン処理設定
				}
				break;
			case	MS_S_SCROOL_IN_BACK:		//	カプセルスクロールイン
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM 移動修正
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	ms_p -> main_type_old;	//	次のメイン処理の設定
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	次のサブ処理の設定
				}
				break;
			}
			break;
		case	MS_M_NAME_B:	//	NAME SELECT	2P
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY 操作
				for(i = 0;i < 2;i++ ){
					if( !ms_cp -> name_c_flg[i] ){	//	未決定
						if( joycur[main_cont[i]] & DM_KEY_LEFT) {
							if( ( ms_cp -> name_c_pos[i] % 3 ) == 2 ){
								dm_set_menu_se( SE_mLeftRight );	//	SE 設定
								if( ms_cp -> name_c_pos[i] - 1 == ms_cp -> name_c_pos[i ^ 1] ){
									//	移動先に相手がいた場合
									if( ms_cp -> name_c_pos[i ^ 1] == 4 ){
										//	相手が真ん中だった場合
										ms_cp -> name_c_pos[i] --;
									}else{
										ms_cp -> name_c_pos[i] -= 2;
									}
								}else{
									//	いない場合
									ms_cp -> name_c_pos[i] --;
								}
							}else	if( ( ms_cp -> name_c_pos[i] % 3) == 1 ){
								if( ms_cp -> name_c_pos[i] - 1 != ms_cp -> name_c_pos[i ^ 1] ){
									//	移動先に相手がいない場合
									dm_set_menu_se( SE_mLeftRight );	//	SE 設定
									ms_cp -> name_c_pos[i] --;
								}
							}
						}else	if( joycur[main_cont[i]] & DM_KEY_RIGHT ) {
							if( ( ms_cp -> name_c_pos[i] % 3) == 0 ){
								dm_set_menu_se( SE_mLeftRight );	//	SE 設定
								if( ms_cp -> name_c_pos[i] + 1 == ms_cp -> name_c_pos[i ^ 1] ){
									//	移動先に相手がいた場合
									if( ms_cp -> name_c_pos[i ^ 1] == 4 ){
										//	相手が真ん中だった場合
										ms_cp -> name_c_pos[i] ++;
									}else{
										ms_cp -> name_c_pos[i] += 2;
									}
								}else{
									//	いない場合
									ms_cp -> name_c_pos[i] ++;
								}
							}else	if( ( ms_cp -> name_c_pos[i] % 3) == 1 ){
								if( ms_cp -> name_c_pos[i] + 1 != ms_cp -> name_c_pos[i ^ 1] ){
									//	移動先に相手がいない場合
									dm_set_menu_se( SE_mLeftRight );	//	SE 設定
									ms_cp -> name_c_pos[i] ++;
								}
							}
						}else	if( joycur[main_cont[i]] & DM_KEY_UP) {
							if( ms_cp -> name_c_pos[i] > 5 ){
								dm_set_menu_se( SE_mLeftRight );	//	SE 設定
								if( ms_cp -> name_c_pos[i] - 3 == ms_cp -> name_c_pos[i ^ 1] ){
									//	移動先に相手がいた場合
									if( ms_cp -> name_c_pos[i ^ 1] == 4 ){
										//	相手が真ん中だった場合
										ms_cp -> name_c_pos[i] -= 3;
									}else{
										ms_cp -> name_c_pos[i] -= 6;
									}
								}else{
									ms_cp -> name_c_pos[i] -= 3;
								}
							}else	if( ms_cp -> name_c_pos[i] > 2 ){
								if( ms_cp -> name_c_pos[i] - 3 != ms_cp -> name_c_pos[i ^ 1] ){
									dm_set_menu_se( SE_mLeftRight );	//	SE 設定
									ms_cp -> name_c_pos[i] -= 3;
								}
							}
						}else	if( joycur[main_cont[i]] & DM_KEY_DOWN) {
							if( ms_cp -> name_c_pos[i] < 3 ){
								dm_set_menu_se( SE_mLeftRight );	//	SE 設定
								if( ms_cp -> name_c_pos[i] + 3 == ms_cp -> name_c_pos[i ^ 1] ){
									//	移動先に相手がいた場合
									if( ms_cp -> name_c_pos[i ^ 1] == 4 ){
										//	相手が真ん中だった場合
										ms_cp -> name_c_pos[i] += 3;
									}else{
										ms_cp -> name_c_pos[i] += 6;
									}
								}else{
									ms_cp -> name_c_pos[i] += 3;
								}
							}else	if( ms_cp -> name_c_pos[i] < 6 ){
								if( ms_cp -> name_c_pos[i] + 3 != ms_cp -> name_c_pos[i ^ 1] ){
									dm_set_menu_se( SE_mLeftRight );	//	SE 設定
									ms_cp -> name_c_pos[i] += 3;
								}
							}
						}else	if( joyupd[main_cont[i]] & DM_KEY_B ){	//	メニュー終了処理(タイトルに戻る)
							ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;
						}else	if( joyupd[main_cont[i]] & DM_KEY_OK ){	//	決定
							dm_set_menu_se( SE_mDecide );		//	SE 設定
							ms_cp -> name_c_flg[i] = 1;
						}
					}else{
						if( joyupd[main_cont[i]] & DM_KEY_B ){	//	決定キャンセル
							ms_cp -> name_c_flg[i] = 0;
						}
					}
				}
				if( ms_cp -> name_c_flg[0] == 1 && ms_cp -> name_c_flg[1] == 1){
					//	両方決定した場合
					for( i = 0;i < 2;i++ ){
						evs_neme_entry_flg[i] = 0;
						if( ms_cp -> name_c_pos[i] == 4 ){
							k[i] = -1;
							evs_select_name_no[i] = DM_MEM_GUEST;	//	ゲストは８番
						}else{
							if( ms_cp -> name_c_pos[i] < 4 ){
								k[i] = evs_select_name_no[i] = ms_cp -> name_c_pos[i];
							}else	if( ms_cp -> name_c_pos[i] > 4 ){
								k[i] = evs_select_name_no[i] = ms_cp -> name_c_pos[i] - 1;
							}
						}
					}
					for( i = 0;i < 2;i++ ){
						if( k[i] >= 0 ){
							if( (evs_mem_data[evs_select_name_no[i]].mem_use_flg & DM_MEM_USE) == 0 ){
								evs_neme_entry_flg[i] = 1;
							}
						}
					}
					if( evs_neme_entry_flg[0] == 0 && evs_neme_entry_flg[1] == 0 ){
						ret = MAIN_60;
					}else{
						ret = MAIN_NAME_ENTRY;
					}
					ms_p -> sub_type		=	MS_S_MENU_OUT_START;	//	終了
					ms_p -> main_type		=	MS_M_SCROOL;			//	終了
				}
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	カプセルスクロールアウト
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-224;					//	基準座標設定
					ms_p -> stop_x			=	48;						//	停止座標設定
					ms_p -> main_grp_type	=	MS_M_MAIN_B;			//	描画の設定
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_BACK;	//	スクロールイン処理設定
					ms_cp -> main_c_pos		=	0;						//	カーソル位置設定
				}
				break;
			case	MS_S_SCROOL_IN_BACK:		//	カプセルスクロールイン
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM 移動修正
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_MAIN_B;			//	次のメイン処理の設定
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	次のサブ処理の設定
				}
				break;
			}
			break;			break;
		case	MS_M_NAME_C:	//	OTHER
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY 操作
				if( joycur[main_cont[0]] & DM_KEY_LEFT) {
					if( ( ms_cp -> name_c_pos[0] % 3) != 0 ){
						dm_set_menu_se( SE_mLeftRight );	//	SE 設定
						ms_cp -> name_c_pos[0] --;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_RIGHT) {
					if( ( ms_cp -> name_c_pos[0] % 3) != 2 ){
						dm_set_menu_se( SE_mLeftRight );	//	SE 設定
						ms_cp -> name_c_pos[0] ++;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_UP) {
					if( ms_cp -> name_c_pos[0] > 2 ){
						dm_set_menu_se( SE_mLeftRight );	//	SE 設定
						ms_cp -> name_c_pos[0] -= 3;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_DOWN) {
					if( ms_cp -> name_c_pos[0] < 6 ){
						dm_set_menu_se( SE_mLeftRight );	//	SE 設定
						ms_cp -> name_c_pos[0] += 3;
					}
				}else	if( joyupd[main_cont[0]] & DM_KEY_B ){	//	メニュー終了処理(タイトルに戻る)
					ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;
				}else	if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	決定
					dm_set_menu_se( SE_mDecide );		//	SE 設定
					if( ms_cp -> name_c_pos[0] == 4 ){
						ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;	//	スクロールアウト処理設定
					}else{
						if( ms_cp -> name_c_pos[0] < 4 ){
							ms_cp -> neme_d_pos = ms_cp -> name_c_pos[0];
						}else	if( ms_cp -> name_c_pos[0] > 4 ){
							ms_cp -> neme_d_pos = ms_cp -> name_c_pos[0] - 1;
						}
						if( ( evs_mem_data[ms_cp -> neme_d_pos].mem_use_flg & DM_MEM_USE ) == 0 ){	//	名前無し
							evs_gamesel				=	GSL_OPTION;				//	オプションモード
							evs_neme_entry_flg[0]	=	1;
							evs_select_name_no[0]	=	ms_cp -> neme_d_pos;
							ms_p -> sub_type		=	MS_S_MENU_OUT_START;	//	終了
							ms_p -> main_type		=	MS_M_SCROOL;			//	終了
							ret = MAIN_NAME_ENTRY;	//	名前入力
						}else{
							ms_p -> sub_type = MS_S_SCROOL_OUT_NEXT;	//	スクロールアウト処理設定
						}
					}
				}
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	カプセルスクロールアウト
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-320;					//	基準座標設定
					ms_p -> stop_x			=	20;						//	停止座標設定
					ms_p -> main_grp_type	=	MS_M_MAIN_D;			//	描画の設定
					ms_cp -> main_c_pos		=	2;						//	カーソル位置設定
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_BACK;	//	スクロールイン処理設定
				}
				break;
			case	MS_S_SCROOL_IN_BACK:		//	カプセルスクロールイン
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM 移動修正
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_MAIN_D;			//	次のメイン処理の設定
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	次のサブ処理の設定
				}
				break;
			case	MS_S_SCROOL_OUT_NEXT:
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_grp_type	=	MS_M_MAIN_D_B;			//	描画の設定
					ms_p -> main_x			=	-224;					//	基準座標設定
					ms_p -> stop_x			=	48;						//	停止座標設定
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_NEXT;	//	スクロールイン処理設定
				}
				break;
			case	MS_S_SCROOL_IN_NEXT:
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM 移動修正
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_MAIN_D_B;		//	次のメイン処理の設定( 名前をどうするか )
					ms_p -> sub_type	=	MS_S_DEFAULT;		//	次のサブ処理の設定
				}
				break;
			}
			break;
		case	MS_M_MAIN_D_B:
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY 操作
				if( joycur[main_cont[0]] & DM_KEY_LEFT) {
					dm_set_menu_se( SE_mUpDown );	//	SE 設定
					ms_cp -> main_c_pos --;
					if( ms_cp -> main_c_pos < 0 ){
						ms_cp -> main_c_pos = 1;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_RIGHT) {
					dm_set_menu_se( SE_mUpDown );	//	SE 設定
					ms_cp -> main_c_pos ++;
					if( ms_cp -> main_c_pos > 1 ){
						ms_cp -> main_c_pos = 0;
					}
				}else	if( joyupd[main_cont[0]] & DM_KEY_B ){	//	メニュー終了処理(タイトルに戻る)
					ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;
				}else	if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	決定
					dm_set_menu_se( SE_mDecide );		//	SE 設定
					switch( ms_cp -> main_c_pos ){
					case	0:	//	変更
						evs_gamesel				=	GSL_OPTION;				//	オプションモード
						evs_neme_entry_flg[0]	=	1;
						evs_select_name_no[0]	=	ms_cp -> neme_d_pos;
						ms_p -> sub_type		=	MS_S_MENU_OUT_START;	//	終了
						ms_p -> main_type		=	MS_M_SCROOL;			//	終了
						ret = MAIN_NAME_ENTRY;	//	名前入力
						break;
					case	1:	//	削除
						ms_p -> sub_type		=	MS_S_SCROOL_OUT_NEXT;	//	次の処理
						ms_cp -> main_c_pos 	=	0;
						break;
					}
				}
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	カプセルスクロールアウト
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-300;					//	基準座標設定
					ms_p -> stop_x			=	0;						//	停止座標設定
					ms_p -> main_grp_type	=	MS_M_NAME_C;			//	描画の設定
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_BACK;	//	スクロールイン処理設定
					for( i = 0;i < 2;i++ ){
						ms_cp -> name_c_pos[i]	=	4;					//	カーソル位置をゲストに設定
						ms_cp -> name_c_flg[i]	=	0;					//	カーソル決定フラグを未決定に設定
					}
				}
				break;
			case	MS_S_SCROOL_IN_BACK:		//	カプセルスクロールイン
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM 移動修正
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_NAME_C;			//	次のメイン処理の設定
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	次のサブ処理の設定
				}
				break;
			case	MS_S_SCROOL_OUT_NEXT:
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_grp_type	=	MS_M_MAIN_D_B_A;		//	描画の設定
					ms_p -> main_x			=	-224;					//	基準座標設定
					ms_p -> stop_x			=	48;						//	停止座標設定
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_NEXT;	//	スクロールイン処理設定
				}
				break;
			case	MS_S_SCROOL_IN_NEXT:
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM 移動修正
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_MAIN_D_B_A;	//	次のメイン処理の設定( 名前をどうするか )
					ms_p -> sub_type	=	MS_S_DEFAULT;		//	次のサブ処理の設定
				}
				break;
			}
			break;
		case	MS_M_MAIN_D_B_A:
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY 操作
				if( joycur[main_cont[0]] & DM_KEY_LEFT) {
					dm_set_menu_se( SE_mUpDown );	//	SE 設定
					ms_cp -> main_c_pos --;
					if( ms_cp -> main_c_pos < 0 ){
						ms_cp -> main_c_pos = 1;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_RIGHT) {
					dm_set_menu_se( SE_mUpDown );	//	SE 設定
					ms_cp -> main_c_pos ++;
					if( ms_cp -> main_c_pos > 1 ){
						ms_cp -> main_c_pos = 0;
					}
				}else	if( joyupd[main_cont[0]] & DM_KEY_B ){	//	メニュー終了処理(タイトルに戻る)
					ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;
				}else	if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	決定
					dm_set_menu_se( SE_mDecide );		//	SE 設定
					switch( ms_cp -> main_c_pos ){
					case	0:	//	キャンセル
						ms_p -> sub_type		=	MS_S_SCROOL_OUT_BACK;
						break;
					case	1:	//	削除
						ms_mp -> mes_mes_len	=	0;
						for( i = 0;i < 8;i++ ){
							evs_mem_data[i].vs_data[ms_cp -> neme_d_pos][0] = 0;
							evs_mem_data[i].vs_data[ms_cp -> neme_d_pos][1] = 0;
						}
						dm_init_save_mem( &evs_mem_data[ms_cp -> neme_d_pos],ms_cp -> neme_d_pos  );
#ifdef	_DM_EEP_ROM_USE_
						EepRom_CreateThread();
						EepRom_SendTask(dm_ms_eep_write, (u32)&eep_err);
						ms_p -> sub_type		=	MS_S_EEP_WAIT;		//	次の処理
						break;

#else
						ms_p -> main_type		=	MS_M_MAIN_D_B_B;	//	次の処理
						ms_p -> main_grp_type	=	MS_M_MAIN_D_B_B;	//	次の処理
						break;
#endif	//	_DM_EEP_ROM_USE_
					}
				}
				break;
			case	MS_S_EEP_WAIT:
#ifdef	_DM_EEP_ROM_USE_
				if( EepRom_GetTaskCount() == 0 ){	//	EEP 書き込み完了
					EepRom_DestroyThread();
					ms_p -> main_type		=	MS_M_MAIN_D_B_B;	//	次の処理
					ms_p -> main_grp_type	=	MS_M_MAIN_D_B_B;	//	次の処理
				}
#endif	//	_DM_EEP_ROM_USE_
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	カプセルスクロールアウト
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-300;					//	基準座標設定
					ms_p -> stop_x			=	0;						//	停止座標設定
					ms_p -> main_grp_type	=	MS_M_NAME_C;			//	描画の設定
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_BACK;	//	スクロールイン処理設定
					for( i = 0;i < 2;i++ ){
						ms_cp -> name_c_pos[i]	=	4;					//	カーソル位置をゲストに設定
						ms_cp -> name_c_flg[i]	=	0;					//	カーソル決定フラグを未決定に設定
					}
				}
				break;
			case	MS_S_SCROOL_IN_BACK:		//	カプセルスクロールイン
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM 移動修正
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_NAME_C;			//	次のメイン処理の設定
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	次のサブ処理の設定
				}
				break;
			case	MS_S_SCROOL_OUT_NEXT:
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_grp_type	=	MS_M_MAIN_D_B_B;		//	描画の設定
					ms_p -> main_x			=	-224;					//	基準座標設定
					ms_p -> stop_x			=	48;						//	停止座標設定
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_NEXT;	//	スクロールイン処理設定
				}
				break;
			case	MS_S_SCROOL_IN_NEXT:
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM 移動修正
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_MAIN_D_B_B;	//	次のメイン処理の設定( 名前をどうするか )
					ms_p -> sub_type	=	MS_S_DEFAULT;		//	次のサブ処理の設定
				}
				break;
			}
			break;
		case	MS_M_MAIN_D_B_B:	//	名前削除しました
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:
				if( ms_tp -> p_mouth_count == -1 ){	//	文字列終了
					ms_cp -> c_count = (ms_cp -> c_count + 1) % 24;
					if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	メニュー終了処理(タイトルに戻る)
						ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;

					}
				}
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	カプセルスクロールアウト
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-300;					//	基準座標設定
					ms_p -> stop_x			=	0;						//	停止座標設定
					ms_p -> main_grp_type	=	MS_M_NAME_C;			//	描画の設定
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_BACK;	//	スクロールイン処理設定
					for( i = 0;i < 2;i++ ){
						ms_cp -> name_c_pos[i]	=	4;					//	カーソル位置をゲストに設定
						ms_cp -> name_c_flg[i]	=	0;					//	カーソル決定フラグを未決定に設定
					}
				}
				break;
			case	MS_S_SCROOL_IN_BACK:		//	カプセルスクロールイン
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM 移動修正
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_NAME_C;			//	次のメイン処理の設定
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	次のサブ処理の設定
				}
				break;
			}
			break;
		}

		//	名前選択で決定している場合
		for( i = 0;i < 2;i++ ){
			if( ms_cp -> name_c_flg[i] ){
				ms_cp -> c_alpha[i] = 0xff;
			}
		}

		dm_menu_se_play();	//	SE再生
	}

	//	表示停止
	graphic_no = GFX_NULL;

	//	グラフィック終了待ち
	while( pendingGFX != 0 ) {
		(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
	}

	//	クライアントの抹消
	nnScRemoveClient(sched, &client);

	main_old = MAIN_MODE_SELECT;

	return	ret;
}

/*--------------------------------------
	モードセレクト描画メイン関数
--------------------------------------*/
void	dm_mode_select_graphic(void)
{
	NNScTask	*gt;
	s16			i,j,k,p;
	s16			x_pos,y_pos;
	s16			skip,max;
	s16			n_x_pos[] = {36,128,220,36,128,220,36,128,220};
	s16			n_y_pos[] = {52,52,52,84,84,84,116,116,116};
	s8			cursor_x[2],cursor_h[2],cursor_g[2][2];
	u8			f_alpha[] = {128,255};
	u8			**menu_p;
	u16			*str_p;


	// ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄﾊﾞｯﾌｧ･ﾀｽｸﾊﾞｯﾌｧの指定
	gp = &gfx_glist[wb_flag][0];
	gt = &gfx_task[wb_flag];

	gSPSegment(gp++, 0, 0x0);
	gSPSegment(gp++, OBJ_SEGMENT, osVirtualToPhysical(gfx_freebuf[0]));	//	アイテム（カプセル等）のセグメント設定

	F3RCPinitRtn();
	F3ClearFZRtn(0);

	gSPDisplayList(gp++, S2Spriteinit_dl);

	//	描画範囲の指定
	gDPSetScissor(gp++,G_SC_NON_INTERLACE, 0, 0, SCREEN_WD-1, SCREEN_HT-1);

	//	描画設定
	gSPDisplayList(gp++, Texture_TE_dl );

	//	背景描画
	load_TexPal(dm_bg_tile_pal_2_bm0_0tlut);
	load_TexTile_4b( &dm_bg_tile_data_bm0_0[0],20,20,0,0,19,19 );
	for(i = 0;i < 12;i++)	//	縦回数
	{
		for(j = 0;j < 16;j++)	//	横回数
		{
			draw_Tex(j * 20,i * 20,20,20,0,0);
		}
	}

	//	キノピオ描画
	load_TexPal( dm_mode_select_pino_bm0_0tlut );
	load_TexBlock_4b( &dm_mode_select_pino_bm0_0[0],64,42 );
	draw_Tex( ms_tp -> p_pos[MS_X_P],ms_tp -> p_pos[MS_Y_P],64,42,0,0 );
	load_TexBlock_4b( &dm_mode_select_pino_bm0_0[1344],64,42 );
	draw_Tex( ms_tp -> p_pos[MS_X_P],ms_tp -> p_pos[MS_Y_P] + 42,64,42,0,0 );
	if( ms_tp -> p_mouth_flg ){	//	口
		load_TexTile_4b( &dm_mode_select_pino_mouth_bm0_0[0],8,5,0,0,7,4 );
		draw_Tex( ms_tp -> p_pos[MS_X_P] + 24,ms_tp -> p_pos[MS_Y_P] + 34,8,5,0,0 );
	}
	if( ms_tp -> p_eye_flg ){	//	目
		load_TexTile_4b( &dm_mode_select_pino_eye_bm0_0[0],12,7,0,0,11,6 );
		draw_Tex( ms_tp -> p_pos[MS_X_P] + 24,ms_tp -> p_pos[MS_Y_P] + 28,12,7,0,0 );
	}

	//	ボタン説明の描画
	load_TexPal(dm_mode_select_bt_ab_bm0_0tlut);
	load_TexBlock_4b(dm_mode_select_bt_ab_bm0_0,80,32);
	draw_Tex( ms_tp -> k_pos[MS_X_P],ms_tp -> k_pos[MS_Y_P],80,32,0,0);

	//	メニュー名の描画

	load_TexPal(dm_mode_select_menu_bm0_0tlut);
	load_TexBlock_4b(dm_mode_select_menu_bm0_0,160,25);
	draw_Tex( ms_tp -> t_pos[MS_X_P],ms_tp -> t_pos[MS_Y_P],160,25,0,0);
	load_TexBlock_4b(&dm_mode_select_menu_bm0_0[2000],160,6);
	draw_Tex( ms_tp -> t_pos[MS_X_P],ms_tp -> t_pos[MS_Y_P] + 25,160,6,0,0);




	if( ms_mp -> mes_win_flg ){
		gSPDisplayList(gp++, prev_halftrans_Ab_dl );

		//	メッセージウインドウの描画
		disp_rect( 64,64,64,176,ms_mp -> mes_win_pos[MS_X_P],ms_mp -> mes_win_pos[MS_Y_P],
									ms_mp -> mes_win_siz[MS_X_SIZ],ms_mp -> mes_win_siz[MS_Y_SIZ] );
	}

	//	名前用下地
	switch( ms_p -> main_grp_type )
	{
	case	MS_M_NAME_A://	名前選択	１人用
	case	MS_M_NAME_B://	名前選択	２人用
	case	MS_M_NAME_C://	名前選択	その他用
		for(i = 0;i < 4;i++){
			disp_rect( 64,64,64,176,ms_p -> main_x + n_x_pos[i],n_y_pos[i],64,16 );
			disp_rect( 64,64,64,176,ms_p -> main_x + n_x_pos[i + 5],n_y_pos[i + 5],64,16 );
		}
		break;
	case	MS_M_MAIN_D_B:		//	名前選択	その他用
	case	MS_M_MAIN_D_B_A:	//	その他で選択した名前
	case	MS_M_MAIN_D_B_B:	//	その他で選択した削除
		disp_rect(64,64,64,176,ms_p -> main_x + n_x_pos[1] - 48,n_y_pos[1],64,16);
		break;
	}


	//	描画設定
	gSPDisplayList(gp++, Texture_TE_dl );

	max = skip = 0;
	switch( ms_p -> main_grp_type )
	{
	case	MS_M_MAIN:
		//	メインメニュー
		max 	= 4;									//	カプセル表示数
		skip 	= 70;									//	描画間隔
		menu_p 	= ms_cap_256_a;							//	グラフィックデータのセット
		str_p	= ms_str_a[ms_cp -> main_c_pos];			//	文章データのセット
		break;
	case	MS_M_MAIN_A:	//	1PALY
		max 	= 2;									//	カプセル表示数
		skip	= 160;									//	描画間隔
		menu_p 	= ms_cap_256_aa;						//	グラフィックデータのセット
		str_p	= ms_str_b[ms_cp -> main_c_pos];		//	文章データのセット
		break;
	case	MS_M_MAIN_B:	//	2PLAY
		max 	= 2;									//	カプセル表示数
		skip	= 160;									//	描画間隔
		menu_p 	= ms_cap_256_b;							//	グラフィックデータのセット
		str_p	= ms_str_c[ms_cp -> main_c_pos];		//	文章データのセット
		break;
	case	MS_M_MAIN_C:	//	4PLAY
		max 	= 4;									//	カプセル表示数
		skip	= 70;									//	描画間隔
		menu_p 	= ms_cap_256_c;							//	グラフィックデータのセット
		str_p	= ms_str_d[ms_cp -> main_c_pos];		//	文章データのセット
		break;
	case	MS_M_MAIN_D:	//	OTHER
		max 	= 5;									//	カプセル表示数
		skip	= 54;									//	描画間隔
		menu_p 	= ms_cap_256_d;							//	グラフィックデータのセット
		str_p	= ms_str_e[ms_cp -> main_c_pos];		//	文章データのセット
		break;
	case	MS_M_MAIN_D_C:	//	SOUND
		max 	= 2;									//	カプセル表示数
		skip	= 160;									//	描画間隔
		menu_p 	= ms_cap_256_sound;							//	グラフィックデータのセット
		str_p	= mode_select_str_sound;				//	文章データのセット
		break;
	case	MS_M_MAIN_D_D:	//	BACKUP
		max 	= 2;									//	カプセル表示数
		skip	= 160;									//	描画間隔
		menu_p 	= ms_cap_256_backup;					//	グラフィックデータのセット
		str_p	= mode_select_str_check;				//	文章データのセット
		break;
	case	MS_M_NAME_A:
	case	MS_M_NAME_B:
		str_p	= mode_select_str_name;					//	名前選択用文章のセット
		break;
	case	MS_M_NAME_C:
		str_p	= mode_select_ext_str_name;				//	その他用名前選択用文章のセット
		break;
	case	MS_M_MAIN_D_B:
		max 	= 2;									//	カプセル表示数
		skip 	= 160;									//	描画間隔
		menu_p 	= ms_cap_256_name;						//	グラフィックデータのセット
		str_p	= mode_select_str_do_name;				//	名前をどうするかの文章データのセット
		break;
	case	MS_M_MAIN_D_B_A:	//	その他で選択した名前
		max 	= 2;									//	カプセル表示数
		skip 	= 160;									//	描画間隔
		menu_p 	= ms_cap_256_backup;					//	グラフィックデータのセット
		str_p	= mode_select_str_check;				//	名前をどうするかの文章データのセット
		break;
	case	MS_M_MAIN_D_D_A:	//	バックアップ削除
		max 	= 2;									//	カプセル表示数
		skip 	= 160;									//	描画間隔
		menu_p 	= ms_cap_256_backup;					//	グラフィックデータのセット
		str_p	= mode_select_str_erace_backup;			//	名前をどうするかの文章データのセット
		break;
	case	MS_M_MAIN_D_B_B:	//	その他で選択した名前
		max 	= 2;									//	カプセル表示数
		skip 	= 160;									//	描画間隔
		menu_p 	= ms_cap_256_backup;					//	グラフィックデータのセット
		str_p	= mode_select_str_erace_name;			//	名前をどうするかの文章データのセット
		break;
	}

	//	カプセル描画
	if( max ){
		load_TexPal( dm_ms_capsel_pal_bm0_0tlut );
		x_pos = ms_p -> main_x;	//	基準座標の設定
		for( i = 0;i < max;i++ ){
			switch( ms_p -> main_grp_type )
			{
			case	MS_M_MAIN_B:	//	2PLAY
				if( i == 0 ){
					if( evs_playmax < 2 ){
						gDPSetPrimColor(gp++,0,0,128,128,128,255);
					}else{
						gDPSetPrimColor(gp++,0,0,255,255,255,255);
					}
				}else{
					gDPSetPrimColor(gp++,0,0,255,255,255,255);
				}
				break;
			case	MS_M_MAIN_C:	//	4PLAY
				if( i < (4 - evs_playmax ) ){
					gDPSetPrimColor(gp++,0,0,128,128,128,255);
				}else{
					gDPSetPrimColor(gp++,0,0,255,255,255,255);
				}

				break;
			}

			if( ms_cp -> main_c_pos != i ){
				//	縮小描画
				y_pos = 83;
				for( j = 0;j < 2;j++ ){
					load_TexBlock_8b( menu_p[i] + 2048 * j,64,32);
					draw_ScaleTex(x_pos + 16,y_pos,64,32,32,16,0,0);
					y_pos += 16;
				}
			}else	if( ms_cp -> main_c_pos == i ){
				//	通常描画
				y_pos = 67 + ms_cp -> c_pos;
				for( j = 0;j < 2;j++ ){
					load_TexBlock_8b( menu_p[i] + 2048 * j,64,32);
					draw_Tex(x_pos,y_pos,64,32,0,0);
					y_pos += 32;
				}
			}
			x_pos += skip;
		}
	}

	if( ms_mp -> mes_win_flg ){
		load_TexPal( dm_ms_mes_win_side_a_bm0_0tlut);
		load_TexTile_4b(dm_ms_mes_win_side_b_bm0_0,4,3,0,0,3,2);	//	上下枠
		draw_Tex( ms_mp -> mes_win_pos[MS_X_P],ms_mp -> mes_win_pos[MS_Y_P] - 3,ms_mp -> mes_win_siz[MS_X_SIZ],3,0,0);
		draw_Tex( ms_mp -> mes_win_pos[MS_X_P],ms_mp -> mes_win_pos[MS_Y_P] + ms_mp -> mes_win_siz[MS_Y_SIZ],ms_mp -> mes_win_siz[MS_X_SIZ],3,0,0);

		load_TexTile_4b(dm_ms_mes_win_side_a_bm0_0,4,80,0,0,3,79);	//	左右枠
		draw_Tex( ms_mp -> mes_win_pos[MS_X_P] - 3,ms_mp -> mes_win_pos[MS_Y_P] - 3,3,ms_mp -> mes_win_siz[MS_Y_SIZ] + 6,0,0);
		draw_TexFlip( ms_mp -> mes_win_pos[MS_X_P] + ms_mp -> mes_win_siz[MS_X_SIZ] -1,ms_mp -> mes_win_pos[MS_Y_P] - 3,3,ms_mp -> mes_win_siz[MS_Y_SIZ] + 6,0,0,flip_on,flip_off);
	}

	//	名前用プレート
	load_TexPal(dm_mode_select_name_plate_bm0_0tlut);
	gDPLoadTextureTile_4b(gp++,dm_mode_select_name_gest[2], G_IM_FMT_CI,72,24,0,0,71,23,0, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK,G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
	switch( ms_p -> main_grp_type )
	{
	case	MS_M_NAME_A://	名前選択	１人用
	case	MS_M_NAME_B://	名前選択	２人用
	case	MS_M_NAME_C://	名前選択	その他用
		for(i = 0;i < 4;i++){
			draw_Tex( ms_p -> main_x + n_x_pos[i] - 4,n_y_pos[i] - 4,72,24,0,0 );
			draw_Tex( ms_p -> main_x + n_x_pos[i + 5] - 4,n_y_pos[i + 5] - 4,72,24,0,0 );
		}
		break;
	case	MS_M_MAIN_D_A:
		load_TexPal( dm_mode_select_black_board_bm0_0tlut );	//	パレットロード
		load_TexTile_4b( &dm_mode_select_black_board_bm0_0[0],246,16,0,0,245,15 );
		draw_Tex( ms_p -> main_x,51,246,16,0,0 );	//	上部
		draw_ScaleTex( ms_p -> main_x,67,246,16,246,103,0,10 );	//	中部
		draw_TexFlip( ms_p -> main_x,170,246,16,0,0,flip_off,flip_on );	//	下部
		break;
	case	MS_M_MAIN_D_B:		//	その他で選択した名前
	case	MS_M_MAIN_D_B_A:	//	その他で選択した名前
	case	MS_M_MAIN_D_B_B:	//	その他で選択した名前
		draw_Tex( ms_p -> main_x + n_x_pos[1] - 52,n_y_pos[i] - 4,72,24,0,0 );
		break;
	}

	switch( ms_p -> main_grp_type ){
	case	MS_M_MAIN_D_D_A:
	case	MS_M_MAIN_D_B_B:	//	その他で選択した名前
		if( ms_tp -> p_mouth_count == -1 ){	//	文字列終了
			if( ms_p -> sub_type == MS_S_DEFAULT ){
				load_TexPal( w_button_bm0_0tlut );	//	パレットロード
				load_TexTile_4b( &w_button_bm0_0[32*(ms_cp -> c_count / 8)],8,8,0,0,7,7 );
				draw_Tex( ms_p -> main_x + 150,210,8,8,0,0 );	//	上部
			}
		}
		break;
	}

	//	インテンシティを使う準備
	gSPDisplayList(gp++, Intensity_XNZ_Texture_dl);
	gDPSetEnvColor(gp++,64,64,64,255);
	gDPSetPrimColor(gp++,0,0,255,255,255,255);

	//	名前描画
	switch( ms_p -> main_grp_type )
	{
	case	MS_M_NAME_A:	//	名前選択	１人用
	case	MS_M_NAME_B:	//	名前選択	２人用
	case	MS_M_NAME_C:	//	名前選択	その他用
		for(i = 0;i < 4;i++){
			draw_font_name( ms_p -> main_x + n_x_pos[i] ,n_y_pos[i],evs_mem_data[i].mem_name);
			draw_font_name( ms_p -> main_x + n_x_pos[i + 5] ,n_y_pos[i + 5],evs_mem_data[i + 4].mem_name);
		}
		break;
	case	MS_M_MAIN_D_B:		//	その他で選択した名前
	case	MS_M_MAIN_D_B_A:	//	その他で選択した名前
	case	MS_M_MAIN_D_B_B:	//	その他で選択した名前
		draw_font_name( ms_p -> main_x + n_x_pos[1] - 48,n_y_pos[0],evs_mem_data[ms_cp -> neme_d_pos].mem_name);
		break;
	}

	//	文字列描画
	if( ms_mp -> mes_win_flg ){
		if( ms_mp -> mes_mes_flg ){
			ms_tp -> p_mouth_count = draw_font_speed_str(30,150,str_p,ms_mp -> mes_mes_len);
		}
	}

	//	半透明設定
	gSPDisplayList(gp++, prev_halftrans_Ab_dl );

	switch( ms_p -> main_grp_type ){
	case	MS_M_NAME_A:	//	名前選択	１人用
	case	MS_M_NAME_B:	//	名前選択	２人用
	case	MS_M_NAME_C:	//	名前選択	その他用
		for(i = 0;i < 2;i++){
			cursor_g[i][0] 	= 0;							//	普通のカーソル
			cursor_g[i][1] 	= i;							//	１Ｐor２Ｐ
			cursor_h[i] 	= 24;							//	カーソルの高さ
		}
		j 	=	0;	//	ゲスト
		cursor_x[0]			= -12;						//	カーソルのずれ
		switch( ms_p -> main_grp_type ){
		case	MS_M_NAME_C:	//	名前選択	その他用
			j 	=	1;							//	終わり
		case	MS_M_NAME_A:	//	名前選択	１人用
			max 				= 0;						//	最大表示数の設定
			if( ms_cp -> name_c_pos[0] == 4){
				//	カーソル位置がゲスト又は終わりの場合
				cursor_g[0][0] 	= 1 + j;					//	ゲストカーソルか終わりカーソル
				cursor_g[0][1] 	= 0;						//	１Ｐ
			}
			break;
		case	MS_M_NAME_B:	//	名前選択	２人用
			max					= 1;						//	最大表示数の設定
			cursor_x[1]			= -4;						//	カーソルのずれ（２Ｐ）
			if( ms_cp -> name_c_pos[0] == ms_cp -> name_c_pos[1] ){
				//	もしカーソルが重なっていた場合（ゲストの場合のみ）
				cursor_g[0][0] 	= 1;						//	ゲストカーソル
				cursor_g[0][1] 	= 2;						//	３Ｐ（重なり用カーソル）
				cursor_g[1][0] 	= 1;						//	ゲストカーソル
				cursor_h[0]		= 16;						//	カーソルの高さ（１Ｐ）
			}else{
				for(i = 0;i < 2;i++){
					if( ms_cp -> name_c_pos[i] == 4){
						cursor_g[i][0] 	= 1;				//	ゲストカーソル
					}
				}
			}
			break;
		}
		//	ゲスト文字
		load_TexPal( dm_mode_select_name_gest_bm0_0tlut );
		load_TexTile_4b( dm_ms_gest[j],72,24,0,0,71,23 );
		draw_Tex( ms_p -> main_x + n_x_pos[4] - 4,n_y_pos[4] - 4,72,24,0,0 );

		//	カーソル描画
		for(i = max;i >= 0;i--)
		{
			gDPSetPrimColor(gp++,0,0,ms_cp -> c_alpha[i],ms_cp -> c_alpha[i],ms_cp -> c_alpha[i],255);
			load_TexPal( dm_ms_cursor_pal_all[cursor_g[i][0]][cursor_g[i][1]]);
			load_TexBlock_4b( dm_ms_cousor_all[cursor_g[i][0]][cursor_g[i][1]],80,cursor_h[i] );
			draw_Tex( ms_p -> main_x + n_x_pos[ms_cp -> name_c_pos[i]] + cursor_x[i],
							n_y_pos[ms_cp -> name_c_pos[i]] - 4,80,cursor_h[i],0,0 );
		}

		break;
	case	MS_M_MAIN_D_A:	//	操作説明選択
		load_TexPal( dm_mode_select_rule_a_bm0_0tlut );	//	パレットロード
		gDPSetPrimColor(gp++,0,0,128,128,128,255);
		for( i = 0;i < 6;i++ ){
			//	選択していないメニューの描画
			if( ms_cp -> main_c_pos != i ){
				load_TexTile_4b( dm_ms_manual_menu[i],dm_ms_manual_menu_size[i],16,0,0,dm_ms_manual_menu_size[i] - 1,15 );
				draw_Tex( ms_p -> main_x + dm_ms_manual_menu_pos[i][0],51 + dm_ms_manual_menu_pos[i][1],dm_ms_manual_menu_size[i],16,0,0 );
			}
		}
		gDPSetPrimColor(gp++,0,0,255,255,255,255);
		load_TexTile_4b( dm_ms_manual_menu[ms_cp -> main_c_pos],dm_ms_manual_menu_size[ms_cp -> main_c_pos],16,0,0,dm_ms_manual_menu_size[ms_cp -> main_c_pos] - 1,15 );
		k = dm_ms_manual_menu_size[ms_cp -> main_c_pos] >> 1;
		j = ms_cp -> c_pos_size	>> 1;	//	増減値 / 2
		draw_ScaleTex( ms_p -> main_x + dm_ms_manual_menu_pos[ms_cp -> main_c_pos][0] + k - (j + k),
						51 + dm_ms_manual_menu_pos[ms_cp -> main_c_pos][1] + 8 - (8 + j),dm_ms_manual_menu_size[ms_cp -> main_c_pos],16,
							dm_ms_manual_menu_size[ms_cp -> main_c_pos] + ms_cp -> c_pos_size,16 + ms_cp -> c_pos_size,0,0 );

		break;

	}

	//	画面切り替え処理関数
	//disp_change_scene();
	HCEffect_Grap(&gp);


	// ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄ作成終了, ｸﾞﾗﾌｨｯｸﾀｽｸの開始
	gDPFullSync(gp++);			// ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄ最終処理
	gSPEndDisplayList(gp++);	// ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄ終端
	osWritebackDCacheAll();		// dinamicｾｸﾞﾒﾝﾄのﾌﾗｯｼｭ
	gfxTaskStart(gt, gfx_glist[wb_flag], (s32)(gp - gfx_glist[wb_flag]) * sizeof(Gfx), GFX_GSPCODE_F3DEX, NN_SC_SWAPBUFFER);
}
