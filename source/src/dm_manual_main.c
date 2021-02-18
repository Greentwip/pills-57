//////////////////////////////////////////////////////////////////////////////
//	filename	:	dm_manual_main/c
//	create		:	1999/09/04
//	modify		:	1999/12/13
//	created		:	Hiroyuki Watanabe
//////////////////////////////////////////////////////////////////////////////

#include <ultra64.h>
#include <PR/ramrom.h>
#include <PR/gs2dex.h>
#include <assert.h>

#include "def.h"
#include "segment.h"
#include "message.h"
#include "nnsched.h"
#include "main.h"
#include "graphic.h"
#include "audio.h"
#include "joy.h"
#include "static.h"
#include "util.h"
#include "msgwnd.h"
#include "tex_func.h"

#include "evsworks.h"
#include "main_story.h"
#include "dm_game_main.h"
#include "dm_virus_init.h"
#include "dm_manual_main.h"

#include "debug.h"
#include "local.h"

//////////////////////////////////////////////////////////////////////////////
//{### リソース

#include "texture/tutorial/tutorial_data.h"
#include "texture/menu/menu_kasa.h"

// テクスチャデータ
static STexInfo *_texAll;
static STexInfo *_texKaSa;

//////////////////////////////////////////////////////////////////////////////
//{### メッセージウィンドウ

typedef struct STutolWnd {
	int pos[2];
	float alpha;
	float alphaStep;
	SMsgWnd msgWnd[1];
} STutolWnd;

//## 位置を設定
static void tutolWnd_setPos(STutolWnd *wnd, int x, int y)
{
	wnd->pos[0] = x;
	wnd->pos[1] = y;
#if LOCAL==JAPAN
	wnd->msgWnd->posX = x + 7;
#elif LOCAL==AMERICA
	wnd->msgWnd->posX = x + 8;
#elif LOCAL==CHINA
	wnd->msgWnd->posX = x + 8;
#endif
	wnd->msgWnd->posY = y + 8;
}

//## アルファを設定
static void tutolWnd_setAlpha(STutolWnd *wnd, int alpha)
{
}

//## 初期化
static void tutolWnd_init(STutolWnd *wnd, void **heap)
{
	wnd->alpha = 0;
	wnd->alphaStep = -1.0 / 16.0;

	// メッセージウィンドウ本体
#if LOCAL==JAPAN
	msgWnd_init(wnd->msgWnd, heap, 13, 4, 32, 32);
	wnd->msgWnd->fntW = 8;
	wnd->msgWnd->fntH = 12;
	wnd->msgWnd->colStep = 4;
	wnd->msgWnd->rowStep = 13;
#elif LOCAL==AMERICA
//	msgWnd_init(wnd->msgWnd, heap, 13, 5, 32, 32);
	msgWnd_init(wnd->msgWnd, heap, 13, 4, 32, 32);
	wnd->msgWnd->fntW = 10;
	wnd->msgWnd->fntH = 12;
	wnd->msgWnd->colStep = 5;
//	wnd->msgWnd->rowStep = 11;
	wnd->msgWnd->rowStep = 13;
#elif LOCAL==CHINA
//	msgWnd_init(wnd->msgWnd, heap, 13, 5, 32, 32);
	msgWnd_init(wnd->msgWnd, heap, 13, 4, 32, 32);
	wnd->msgWnd->fntW = 10;
	wnd->msgWnd->fntH = 12;
	wnd->msgWnd->colStep = 5;
//	wnd->msgWnd->rowStep = 11;
	wnd->msgWnd->rowStep = 14;
#endif
	wnd->msgWnd->msgSpeed = 1.0 / 6.0;

	tutolWnd_setPos(wnd, 32, 32);
}

//## 更新
static void tutolWnd_update(STutolWnd *wnd)
{
	wnd->alpha = CLAMP(0, 1, wnd->alpha + wnd->alphaStep);

	if(wnd->alpha == 1.0) {
		msgWnd_update(wnd->msgWnd);
	}
}

//## 描画
static void tutolWnd_draw(STutolWnd *wnd, Gfx **gpp)
{
	Gfx *gp = *gpp;
	STexInfo *texC;
	int alpha = wnd->alpha * 255;

	if(alpha == 0) return;

	gSPDisplayList(gp++, normal_texture_init_dl);
	gDPSetCombineMode(gp++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
	if(alpha < 255) {
		gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
	}
	gDPSetPrimColor(gp++, 0,0, 255,255,255, alpha);

	// 下地
	texC = _texAll + TEX_account_message;
	tiStretchTexBlock(&gp, texC, false,
		wnd->pos[0], wnd->pos[1], texC->size[0], texC->size[1]);

	if(wnd->alpha != 0) {
		wnd->msgWnd->alpha = alpha;
		msgWnd_draw(wnd->msgWnd, &gp);
	}

	*gpp = gp;
}

//## メッセージバッファをクリア
static void tutolWnd_clear(STutolWnd *wnd)
{
	msgWnd_clear(wnd->msgWnd);
}

//## メッセージバッファに文字列を追加
static void tutolWnd_addStr(STutolWnd *wnd, const char *str)
{
	msgWnd_addStr(wnd->msgWnd, str);
}

//## フェードイン
static void tutolWnd_fadeIn(STutolWnd *wnd)
{
	if(wnd->alphaStep < 0) {
		wnd->alphaStep = -wnd->alphaStep;
	}
}

//## フェードアウト
static void tutolWnd_fadeOut(STutolWnd *wnd)
{
	if(wnd->alphaStep > 0) {
		wnd->alphaStep = -wnd->alphaStep;
	}
}

//## メッセージが終了しているかどうか調べる
static int tutolWnd_isEnd(STutolWnd *wnd)
{
	return msgWnd_isEnd(wnd->msgWnd);
}

//## メッセージ進行中？
static int tutolWnd_isSpeaking(STutolWnd *wnd)
{
	return msgWnd_isSpeaking(wnd->msgWnd);
}

//////////////////////////////////////////////////////////////////////////////
//{### マクロ

#define KEY_FLASH_START   8
#define KEY_FLASH_END   -20
#define KEY_FLASH_STEP   -1

#define dm_manual_rr 0x00 // 操作説明マガジン作成用( 赤：赤 )
#define dm_manual_ry 0x01 // 操作説明マガジン作成用( 赤：黄 )
#define dm_manual_rb 0x02 // 操作説明マガジン作成用( 赤：青 )
#define dm_manual_yy 0x10 // 操作説明マガジン作成用( 黄：黄 )
#define dm_manual_yr 0x12 // 操作説明マガジン作成用( 黄：赤 )
#define dm_manual_yb 0x11 // 操作説明マガジン作成用( 黄：青 )
#define dm_manual_br 0x21 // 操作説明マガジン作成用( 青：赤 )
#define dm_manual_by 0x22 // 操作説明マガジン作成用( 青：黄 )
#define dm_manual_bb 0x20 // 操作説明マガジン作成用( 青：青 )

typedef enum {
	MODE_CAPSEL_PROC = 777,
	MODE_WAIT_MESSAGE,
	MODE_WAIT_CAPSEL_FALL,
	MODE_EXIT,
} MODE;

//////////////////////////////////////////////////////////////////////////////
//{### 操作説明メイン構造体

typedef struct {
	// 背景の取り込みを行うフラグ
//	int bg_snap_flg;

	// キノピオ
	int pino_dir;    // キノピオの向き
	int pino_pos[2]; // キノピオ表示座標

	// フェードイン・アウト制御
	int fade_count;
	int fade_step;

//	int flash_alpha;  // 点滅用フラグ
//	int flash_pos[2]; // 点滅範囲開始座標

	// ＢＧＭ再生用変数
	int bgm_timer;   // 再生時間カウンタ
	int bgm_timeout; // 再生時間設定変数

	// 
	int key_flash_count[4]; // キーの点滅時間

	// win, lose, game over などの timer
	int logo_timer;

	// カプセル処理の流れ
	int capsel_flow;

	// メッセージウィンドウ
	STutolWnd msgWnd[1];

	// 巨大ウイルスの座標
	int big_virus_pos[3][3];

	// 操作説明用攻撃カプセル
	game_a_cap attack_cap[4][4];

	u8  big_virus_count[4];  // 巨大ウイルスの座標
	int big_virus_flg[3][2]; // 0:消滅アニメーションを一回だけ設定するフラグ
	                         // 1:消滅音を一回だけ設定するフラグ

	MODE mode;               // 内部制御変数
	MODE next_mode;          // 
	int mode_stop_flg;       // 内部処理進行を制御するフラグ
	int last3_flg;           // ウイルスが残り３個のときに鳴らすためのフラグ
	int attack_cap_count[4]; // 操作説明用攻撃カプセル落下カウンタ

//	// Mtx buffer
//	Mtx  mtxBuf[GFX_GTASK_NUM][16];
//	Mtx *mtxPtr[GFX_GTASK_NUM];
//
//	// Vtx buffer
//	Vtx  vtxBuf[GFX_GTASK_NUM][128];
//	Vtx *vtxPtr[GFX_GTASK_NUM];

	// アニメーションカウンタ
	int animeCount;
} Manual;

//////////////////////////////////////////////////////////////////////////////
//{### 変数

Manual *watchManual;

// 説明[レベルセレクト]のメッセージウィンドウ座標
static const int _posLvMsgWnd[] = { 200, 40 };
static const int _posP2MsgWnd[] = { 108,  40 };
static const int _posP4MsgWnd[][2] = { { 36, 40 }, { 180, 40 } };

static const int _posLvMario[] = { 216, 112 };
static const int _posP2Mario[] = { 124, 112 };
static const int _posP4Mario[][2] = { { 52, 112 }, { 196, 112 } };

//////////////////////////////////////////////////////////////////////////////
//{### カプセル設定

//////////////////////////////////////
//## 操作説明攻撃カプセル設定関数
void dm_manual_set_attack_capsel(game_a_cap *cap, int pos_x, int color)
{
	cap->pos_a_x = pos_x;					// Ｘ座標の設定
	cap->pos_a_y = 1;						// Ｙ座標の設定
	cap->capsel_a_p = color;				// 色の設定
	cap->capsel_a_flg[cap_disp_flg] = 1;	// 表示フラグを立てる
}

//////////////////////////////////////
//## 操作説明攻撃カプセル落下処理関数
void dm_manual_attack_capsel_down()
{
	Manual *st = watchManual;
	int i, j, flg;

	// 攻撃カプセル落下処理
	flg = 0;
	for(i = 0; i < 4; i++) {
		st->attack_cap_count[i]++;
		if(st->attack_cap_count[i] < dm_down_speed) {
			continue;
		}
		st->attack_cap_count[i] = 0;

		for(j = 0; j < 4; j++) {
			if(!st->attack_cap[i][j].capsel_a_flg[cap_disp_flg]) {
				continue;
			}

			// 落下カプセルとマップ情報の判定
			if(get_map_info(game_map_data[i],
				st->attack_cap[i][j].pos_a_x,
				st->attack_cap[i][j].pos_a_y + 1))
			{
				// もしカプセルの下にアイテムがあった場合

				// マップ情報へ変更する
				set_map(game_map_data[i],
					st->attack_cap[i][j].pos_a_x,
					st->attack_cap[i][j].pos_a_y,
					capsel_b, st->attack_cap[i][j].capsel_a_p);

				// 表示フラグの消去
				st->attack_cap[i][j].capsel_a_flg[cap_disp_flg] = 0;
			}
			else {
				flg = 1;
				if(st->attack_cap[i][j].pos_a_y < 16) {
					st->attack_cap[i][j].pos_a_y++;
				}

				// もし最下段だった場合
				if(st->attack_cap[i][j].pos_a_y == 16) {
					// マップ情報へ変更する
					set_map(game_map_data[i],
						st->attack_cap[i][j].pos_a_x,
						st->attack_cap[i][j].pos_a_y,
						capsel_b, st->attack_cap[i][j].capsel_a_p);

					// 表示フラグの消去
					st->attack_cap[i][j].capsel_a_flg[cap_disp_flg] = 0;
				}
			}
		}
	}

	if(flg) {
		dm_snd_play(SE_gFreeFall);
	}
}

//////////////////////////////////////////////////////////////////////////////
//{### ゲーム画面処理を進行

#include "dm_manual_data.h"
#if LOCAL==JAPAN
#include "dm_manual.mes"
#elif LOCAL==AMERICA
#include "dm_manual_am.mes"
#elif LOCAL==CHINA
#include "dm_manual_zh.mes"
#endif

//////////////////////////////////////
//## 操作説明用演出処理関数
void dm_manual_effect_cnt(game_state *state, game_map *map, int player_no)
{
	// 黒上がり処理
	if(state->cnd_now != dm_cnd_wait && state->cnd_now != dm_cnd_pause) {
		dm_black_up(state, map);
	}
}

//////////////////////////////////////
//## ウィルスアニメーションを更新

static void dm_manual_update_virus_anime(game_state *state)
{
	Manual *st = watchManual;
	int i, j;

	// 巨大ウイルスアニメーション
	for(i = 0;i < 3;i++){
		animeState_update(get_virus_anime_state(i));
		animeSmog_update(get_virus_smog_state(i));
	}

	dm_calc_big_virus_pos(state);
	return;

	// 巨大ウイルスアニメーション
	for(i = j = 0;i < 3;i++){
		// 負けアニメーションの場合
		if(get_virus_anime_state(i)->animeSeq->animeNo == ASEQ_LOSE) {
			j++;
			// 巨大ウイルス消滅 SE を再生していない？
			if(!st->big_virus_flg[i][1]) {

				// ２回鳴らさないためのフラグセット
				st->big_virus_flg[i][1] = 1;

				// 巨大ウイルス消滅 SE セット
				if(state->virus_number != 0) {
					dm_snd_play( SE_gVrsErsAl );
				}
			}
		}
		// 通常アニメーションの場合
		else if(get_virus_anime_state(i)->animeSeq->animeNo != ASEQ_NORMAL) {
			j++;
		}
	}

	// 回転（円を描く）処理
	if(j == 0) {
		st->big_virus_count[3]++;

		if(st->big_virus_count[3] >= 10) {
			st->big_virus_count[3] = 0;

			for(i = 0;i < 3;i++){
				st->big_virus_pos[i][2]++;
				if(st->big_virus_pos[i][2] >= 360) {
					st->big_virus_pos[i][2] = 0;
				}
				st->big_virus_pos[i][0] = sinf(DEGREE(st->big_virus_pos[i][2])) *  20 +  45;
				st->big_virus_pos[i][1] = cosf(DEGREE(st->big_virus_pos[i][2])) * -20 + 155;
			}
		}
	}
}

//////////////////////////////////////
//## ゲーム処理関数
DM_GAME_RET dm_manual_main_cnt(game_state *state, game_map *map, u8 player_no, u8 flg)
{
	Manual *st = watchManual;
	int i, j, out;

	switch(state->mode_now) {
	// ウイルス配置
	case dm_mode_init:
		return dm_ret_virus_wait;

	case dm_mode_wait:
		return dm_ret_virus_wait;

	// カプセル落下
	case dm_mode_down:
		dm_capsel_down(state, map);
		break;

	// 着地ウェイト
	case dm_mode_down_wait:
		if(dm_check_game_over(state, map)) {
			// 積みあがり(ゲームオーバー)
			state->cnd_now = dm_cnd_game_over;
			state->mode_now = dm_mode_game_over;
			return dm_ret_game_over;
		}
		else {
			if(dm_h_erase_chack( map ) != cap_flg_off || dm_w_erase_chack( map ) != cap_flg_off) {
				// リタイアしていない
				if(!state->flg_game_over) {
					// 消滅判定
					state->mode_now = dm_mode_erase_chack;
				}
				// リタイアしている(練習中)
				else if(state->flg_game_over) {
					// 消滅判定
					state->mode_now = dm_mode_tr_erase_chack;
				}
				state->cap_speed_count = 0;
			}
			else {
				// リタイアしていない
				if(!state->flg_game_over) {
					// カプセルセット
					state->mode_now = dm_mode_cap_set;
				}
				// リタイアしている(練習中)
				else if(state->flg_game_over) {
					// カプセルセット
					state->mode_now = dm_mode_tr_cap_set;
				}
			}
		}
		break;

	// 消滅判定
	case dm_mode_erase_chack:
		state->cap_speed_count++;
		if(state->cap_speed_count >= dm_bound_wait) {
			// 消滅開始
			state->cap_speed_count = 0;
			state->mode_now = dm_mode_erase_anime;

			// 縦消し判定 // 横消し判定
			dm_h_erase_chack_set(state, map);
			dm_w_erase_chack_set(state, map);

			// 縦粒化現象 // 横粒化現象
			dm_h_ball_chack(map);
			dm_w_ball_chack(map);

			state->virus_number = get_virus_color_count(map,
				&st->big_virus_count[0], &st->big_virus_count[1], &st->big_virus_count[2]);

			switch(evs_manual_no) {
			case 0:
			case 3:
				for(i = 0;i < 3;i++ ) {
					if(st->big_virus_count[i] == 0 ) {
						if(!st->big_virus_flg[i][0] ) {
							st->big_virus_flg[i][0] = 1;
							// 巨大ウイルス消滅アニメーションセット
							animeState_set(get_virus_anime_state(i), ASEQ_LOSE);
							animeSmog_start(get_virus_smog_state(i));

							if(state->virus_number != 0) {
								// 巨大ウイルスダメージ
								dm_snd_play( SE_gVirusStruggle );
							}
						}
					}
					else {
						if(state->chain_color[3] & (0x10 << i) ) {
							// 巨大ウイルスやられアニメーションセット
							animeState_set(get_virus_anime_state(i), ASEQ_DAMAGE);

							// 巨大ウイルスダメージ
							dm_snd_play( SE_gVirusStruggle );
						}
					}
				}
			}

			// 巨大ウイルスアニメーションリセット
			state->chain_color[3] &= 0x0f;

			// ウイルス消滅
			if(state->virus_number == 0) {
				// 得点計算
				dm_make_score(state);

				// ステージクリア
				state->cnd_now = dm_cnd_stage_clear;
				state->mode_now = dm_mode_stage_clear;
				return dm_ret_clear;
			}
			else if(state->virus_number > 0 && state->virus_number < 4) {
				// ウイルスが残り３個以下のとき警告音を鳴らす
				if(!st->last3_flg) {
					// １回ならしたら以後鳴らさない
					st->last3_flg = cap_flg_on;

					// ウイルス残り３個ＳＥ再生
					dm_snd_play( SE_gReach );

					// hi tempo version のシーケンスに切り替える
					dm_seq_play_in_game((evs_seqnumb << 1) + 1);
				}
			}

			state ->chain_count++;
			if(state->chain_color[3] & 0x08) {
				// ウイルスを含む消滅のリセット
				state->chain_color[3] &= 0xF7;

				// ウイルスを含む消滅ＳＥ再生
				dm_snd_play( SE_gVrsErase );
			}
			else {
				// カプセルのみの消滅ＳＥ再生
				dm_snd_play( SE_gCapErase );
			}
		}
		break;

	// 消滅アニメーション
	case dm_mode_erase_anime:
		dm_capsel_erase_anime( state,map );
		break;

	// カプセル(粒)落下
	case dm_mode_ball_down:
		go_down( state,map,dm_down_speed );
		break;

	// 新カプセルセット
	case dm_mode_cap_set:
		out = 1;

		// 攻撃ＳＥ再生
		dm_attack_se( state,player_no );

		// 積みあがりチェック
		dm_warning_h_line( state,map );

		// 落下前思考追加位置
		aifMakeFlagSet(state);

		if(out) {
			dm_set_capsel(state);
			dm_capsel_speed_up(state);    // 落下カプセル速度の計算
			state->chain_line = 0;        // 消滅ライン数のリセット
			state->chain_count = 0;       // 連鎖数のリセット
			state->erase_virus_count = 0; // 消滅ウイルス数のリセット

			// 消滅色のリセット
			for(i = 0; i < 4; i++) {
				state->chain_color[i] = 0;
			}

			// カプセル落下処理へ
			state->mode_now = dm_mode_down;
		}
		break;

	// ステージクリア演出
	case dm_mode_stage_clear:
//		i = try_next_stage_main();
		if(++st->logo_timer > 120) {
			st->logo_timer = 0;
			return dm_ret_next_stage;
		}
		break;

	// ゲームオーバー演出
	case dm_mode_game_over:
//		i = game_over_main();
		if(++st->logo_timer > 120) {
			st->logo_timer = 0;
			return dm_ret_end;
		}
		break;

	// WIN演出
	case dm_mode_win:
//		i = win_main( center[flg][player_no] , player_no );
		if(++st->logo_timer > 120) {
			st->logo_timer = 0;
			return dm_ret_end;
		}
		break;

	// LOSE演出
	case dm_mode_lose:
//		i = lose_main( player_no,center[flg][player_no] );
		if(++st->logo_timer > 120) {
			st->logo_timer = 0;
			return dm_ret_end;
		}
		break;

	// DRAW演出
	case dm_mode_draw:
//		i = draw_main( player_no,center[flg][player_no] );
		if(++st->logo_timer > 120) {
			st->logo_timer = 0;
			return dm_ret_end;
		}
		break;

	// 練習確認
	case dm_mode_tr_chaeck:
		break;

	case dm_mode_tr_erase_chack:
		state->cap_speed_count++;
		if(state->cap_speed_count >= dm_bound_wait) {
			state->cap_speed_count = 0;
			state->mode_now = dm_mode_erase_anime;

			// 縦消し判定 // 横消し判定
			dm_h_erase_chack_set(state, map);
			dm_w_erase_chack_set(state, map);

			// 縦粒化現象 // 横粒化現象
			dm_h_ball_chack(map);
			dm_w_ball_chack(map);

			state ->chain_count++;
			if(state->chain_color[3] & 0x08) {
				// ウイルスフラグ
				state->chain_color[3] &= 0xF7;
			}
			else{
				// カプセルのみの消滅ＳＥ再生
				dm_snd_play(SE_gCapErase);
			}
		}
		break;

	case dm_mode_tr_cap_set:
		out = 1;
		dm_attack_se( state,player_no ); // 攻撃ＳＥ再生
		dm_warning_h_line( state,map );  // 積みあがりチェック

		// 攻撃あり
		if(game_state_data[i].flg_training) {
			// やられ判定
			if(dm_broken_set(state, map)) {
				// 粒落下処理へ
				state->mode_now = dm_mode_ball_down;
				out = 0;
			}
		}

		if(out) {
			dm_set_capsel(state);
			dm_capsel_speed_up(state);    // 落下カプセル速度の計算
			state->chain_line = 0;        // 消滅ライン数のリセット
			state->chain_count = 0;       // 連鎖数のリセット
			state->erase_virus_count = 0; // 消滅ウイルス数のリセット

			// 消滅色のリセット
			for(i = 0; i < 4; i++) {
				state->chain_color[i] = 0;
			}

			// カプセル落下処理へ
			state->mode_now = dm_mode_down;
		}
		break;
	}

	return dm_ret_null;
}

//////////////////////////////////////
//## キー操作処理関数

static void dm_manual_make_key(game_state *state, game_map *map)
{
	Manual *st = watchManual;
	game_cap *cap = &state->now_cap;
	u32 key;
	int i;

	// AIからキーを出力
	aifKeyOut(state);
	key = joygam[state->player_no];

	// 左回転
	if(key & DM_ROTATION_L) {
		rotate_capsel(map, cap, cap_turn_l);
		st->key_flash_count[3] = KEY_FLASH_START;
	}
	// 右回転
	else if(key & DM_ROTATION_R) {
		rotate_capsel(map, cap, cap_turn_r);
		st->key_flash_count[2] = KEY_FLASH_START;
	}

	// 左移動
	if(key & DM_KEY_LEFT) {
		translate_capsel(map, state, cap_turn_l, main_joy[state->player_no]);
		st->key_flash_count[0] = KEY_FLASH_START;
	}
	// 右移動
	else if(key & DM_KEY_RIGHT) {
		translate_capsel(map, state, cap_turn_r, main_joy[state->player_no]);
		st->key_flash_count[1] = KEY_FLASH_START;
	}

	state->cap_speed_vec = 1;

	// 高速落下
	if((key & DM_KEY_DOWN) && cap->pos_y[0] > 0) {
		i = FallSpeed[state->cap_speed];
		i = (i >> 1) + (i & 1);
		state->cap_speed_vec = i;
	}
}

//////////////////////////////////////////////////////////////////////////////
//{### 操作説明その１処理メイン関数

static bool dm_manual_1_main()
{
	Manual *st = watchManual;
	game_state *state;
	game_map *map;
	game_cap *cap;
	DM_GAME_RET ret;
	int i, j;
	bool res = true;

	state = &game_state_data[0];
	map = game_map_data[0];
	cap = &state->now_cap;

	// キー点滅カウンター
	for(i = 0; i < ARRAY_SIZE(st->key_flash_count); i++) {
		if(st->key_flash_count[i] != KEY_FLASH_END) {
			st->key_flash_count[i] += KEY_FLASH_STEP;
		}
	}

	// 落下や着地判定等を行っている
	if(st->mode_stop_flg) {
		ret = dm_manual_main_cnt(state, map, 0, 0);

		// キー操作
		dm_manual_make_key(state, map);
	}
	dm_manual_effect_cnt(state, map, 0); // 連鎖等の特種表示の処理

	// ウイルスアニメーション
	dm_virus_anime(state, map);

	// 巨大ウイルスアニメーション
	dm_manual_update_virus_anime(state);

	// つみあがり警告音を再生
	dm_warning_h_line_se();

	switch(st->mode) {
	// 初期化
	case 0:
		tutolWnd_setPos(st->msgWnd, _posLvMsgWnd[0], _posLvMsgWnd[1]);
		st->pino_dir = 1;					// キノピオの向き（左向き）
		st->pino_pos[0] = _posLvMario[0];	// キノピオの座標
		st->pino_pos[1] = _posLvMario[1];

		// メッセージウィンドウをフェードアウト
		tutolWnd_fadeOut(st->msgWnd);

		// 次の処理に
		st->mode++;

		// 落下カプセル作成
		for(i = 0; i < ARRAY_SIZE(capsel_1_1); i++) {
			CapsMagazine[i + 1] = capsel_1_1[i];
		}
		state->cap_magazine_cnt = 1; // マガジン残照を1にする
		dm_set_capsel(state);        // カプセル設定

		break;

	// ウイルス配置
	case 1:
		set_virus(map,
			virus_1_1[state->virus_number][1], // ウイルスＸ座標
			virus_1_1[state->virus_number][2], // ウイルスＹ座標
			virus_1_1[state->virus_number][0], // ウイルスの色
			virus_anime_table[virus_1_1[state->virus_number][0]][state->virus_anime]); // アニメ番号

		state->virus_number++;

		if(state->virus_number >= ARRAY_SIZE(virus_1_1)) {
			// 次の処理に
			st->mode = 10;
		}
		break;

	// 文書1
	case 10:
		// メッセージウィンドウにメッセージを追加
		tutolWnd_fadeIn(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_1_1);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 20;
		break;

	// 文書2
	case 20:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_1_2);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 30;
		break;

	// 文書3
	case 30:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_1_3);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 40;
		break;

	// 文章4
	case 40:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_1_4);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 41;
		break;

	case 41:
		st->mode_stop_flg = cap_flg_on;
		state->mode_now = dm_mode_down;

		aifMake2(state, 3, 12, 0, 0);
		st->mode++;
		break;

	// 回転
	case 42:
		// 強制的に高速落下を行わないようにする
		state->cap_speed_vec = 1;

		switch(cap->pos_y[0]) {
		case 4:
			if(cap->pos_y[0] == cap->pos_y[1]) {
				rotate_capsel(map, cap, cap_turn_l);
				st->key_flash_count[3] = KEY_FLASH_START;
			}
			break;
		case 6:
			if(cap->pos_y[0] != cap->pos_y[1]) {
				rotate_capsel(map, cap, cap_turn_l);
				st->key_flash_count[3] = KEY_FLASH_START;
			}
			break;
		case 8:
			if(cap->pos_y[0] == cap->pos_y[1]) {
				rotate_capsel(map, cap, cap_turn_r);
				st->key_flash_count[2] = KEY_FLASH_START;
			}
			break;
		case 10:
			if(cap->pos_y[0] != cap->pos_y[1]) {
				rotate_capsel(map, cap, cap_turn_r);
				st->key_flash_count[2] = KEY_FLASH_START;
			}
			break;
		case 12:
			aifMakeFlagSet(state);
			aifMake2(state, 4, 14, 0, 0);
			st->mode++;
			break;
		}
		break;

	case 43:
		if(!state->ai.aiok) {
			st->mode = 50;
		}
		break;

	// 文章5
	case 50:
		st->mode_stop_flg = cap_flg_off;
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_1_5);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 51;
		break;

	// x:2, y:12
	case 51:
		st->mode_stop_flg = cap_flg_on;
		aifMake2(state, 2, 12, 1, 0);
		st->mode++;
		break;

	// x:5, y:13
	case 52:
		if(!state->ai.aiok) {
			aifMake2(state, 5, 13, 1, 0);
			st->mode++;
		}
		break;

	case 53:
		if(!state->ai.aiok) {
			st->mode = 60;
		}
		break;

	// 文章6
	case 60:
		st->mode_stop_flg = cap_flg_off;
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_1_6);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 61;
		break;

	// x:2, y:16
	case 61:
		st->mode_stop_flg = cap_flg_on;
		aifMake2(state, 2, 16, 0, 0);
		st->mode++;
		break;

	case 62:
		if(ret == dm_ret_clear) {
			dm_seq_play(SEQ_End_A);
			st->bgm_timer = 0;
			st->bgm_timeout = 120;
			st->mode++;
		}
		break;

	case 63:
		if(ret == dm_ret_next_stage) {
			clear_map_all(map);
			st->mode++;

			state->virus_level = 15;
			_dm_virus_init(GMD_NORMAL, state, virus_map_data[0], virus_map_disp_order[0], 1);

			for(i = 0; i < 3; i++) {
				animeState_set(get_virus_anime_state(i), ASEQ_NORMAL);
				animeSmog_stop(get_virus_smog_state(i));
			}

			for(i = 0; i < ARRAY_SIZE(capsel_1_2); i++) {
				CapsMagazine[i + 1] = capsel_1_2[i];
			}
			state->cap_magazine_cnt = 1;
			dm_set_capsel(state);

			state->mode_now = dm_mode_init;
			state->cnd_now = dm_cnd_init;

			state->virus_number = 0;
		}
		break;

	case 64:
		i = virus_map_disp_order[0][state->virus_number];
		set_virus(map,
			virus_map_data[0][i].x_pos,
			virus_map_data[0][i].y_pos,
			virus_map_data[0][i].virus_type,
			virus_anime_table[virus_map_data[0][i].virus_type][state->virus_anime]);

		state->virus_number++;

		if(state->virus_number >= dm_get_first_virus_count(evs_gamemode, state)) {
			// 次の処理に
			st->mode = 70;
		}
		break;

	case 70:
		// メッセージウィンドウにメッセージを追加
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_1_7);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 71;
		break;

	case 71:
		st->mode_stop_flg = cap_flg_on;
		state->mode_now = dm_mode_down;
		aifMakeFlagSet(state);
		st->mode++;
		break;

	case 72:
		i = state->cap_magazine_cnt - 2;
		if(i < ARRAY_SIZE(position_1_1)) {
			aifMake2(state,
				position_1_1[i][0], position_1_1[i][1],
				position_1_1[i][2], position_1_1[i][3]);
		}
		else {
			st->mode++;
		}
		break;

	case 73:
		if(ret == dm_ret_game_over) {
			dm_seq_play(SEQ_End_C);
//			st->bgm_timer = 0;
//			st->bgm_timeout = 120;
			st->mode++;
		}
		break;

	case 74:
		if(ret == dm_ret_end) {
			// 次の処理に
			st->mode = 80;
		}
		break;

	case 80:
		// メッセージウィンドウにメッセージを追加
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_1_8);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = MODE_EXIT;
		break;

	case MODE_WAIT_MESSAGE:
		if(tutolWnd_isEnd(st->msgWnd)) {
			st->mode = st->next_mode;
		}
		break;

	case MODE_EXIT:
		res = false;
		break;
	}

	return res;
}

//////////////////////////////////////////////////////////////////////////////
//{### 操作説明その２処理メイン関数

static bool dm_manual_2_main()
{
	Manual *st = watchManual;
	DM_GAME_RET ret;
	int i, j;
	bool res = true;
	game_state *state;
	game_map *map;

	state = &game_state_data[0];
	map = game_map_data[0];

	if(st->mode_stop_flg) {
		ret = dm_manual_main_cnt(state, map, 0, 1);
		dm_manual_main_cnt(&game_state_data[1], game_map_data[1], 1, 1);

		// キー操作
		dm_manual_make_key(state, map);
	}
	dm_manual_effect_cnt(state, map, 0); // 連鎖等の特種表示の処理
	dm_manual_attack_capsel_down();      // 攻撃カプセルの落下処理

	// ウイルスアニメーション
	for(i = 0; i < 2; i++){
		dm_virus_anime(&game_state_data[i], game_map_data[i]);
	}

	// つみあがり警告音を再生
	dm_warning_h_line_se();

	switch(st->mode) {
	// 初期化
	case 0:
		tutolWnd_setPos(st->msgWnd, _posP2MsgWnd[0], _posP2MsgWnd[1]);
		st->pino_dir = 1;                 // キノピオの向き（左向き）
		st->pino_pos[0] = _posP2Mario[0]; // キノピオの座標
		st->pino_pos[1] = _posP2Mario[1];

		// メッセージウィンドウをフェードアウト
		tutolWnd_fadeOut(st->msgWnd);

		// 次の処理に
		st->mode = 1;

		// 落下カプセル作成
		for(i = 0; i < ARRAY_SIZE(capsel_2_1); i++) {
			CapsMagazine[i + 1] = capsel_2_1[i];
		}
		state->cap_magazine_cnt = 1; // マガジン残照を1にする
		dm_set_capsel(state);        // カプセル設定

		break;

	// ウイルス配置
	case 1:
		set_virus(map,
			virus_2_1[state->virus_number][1],
			virus_2_1[state->virus_number][2],
			virus_2_1[state->virus_number][0],
			virus_anime_table[virus_2_1[state->virus_number][0]][state->virus_anime]);

		state->virus_number++;

		if(state->virus_number >= ARRAY_SIZE(virus_2_1)) {
			// 次の処理に
			st->mode = 10;
		}
		break;

	case 10:
		tutolWnd_fadeIn(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_2_1);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 20;
		break;

	case 20:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_2_2);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 30;
		break;

	case 30:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_2_3);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 40;
		break;

	case 40:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_2_4);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 50;
		break;

	case 50:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_2_5);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 60;
		break;

	case 60:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_2_6);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 61;
		break;

	case 61:
		state->mode_now = dm_mode_down;
		st->mode = MODE_CAPSEL_PROC;
		st->next_mode = 70;
		break;

	case 70:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_2_7);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 71;
		break;

	case 71:
		st->mode = MODE_CAPSEL_PROC;
		st->next_mode = 80;
		break;

	case 80:
		st->mode = MODE_CAPSEL_PROC;
		st->next_mode = 81;
		break;

	case 81:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_2_8);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 90;
		break;

	case 90:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_2_9);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = MODE_EXIT;
		break;

	case MODE_CAPSEL_PROC:
		st->mode_stop_flg = cap_flg_on;

		i = state->cap_magazine_cnt - 2;
		if(i < ARRAY_SIZE(position_2_1)) {
			aifMake2(state,
				position_2_1[i][0], position_2_1[i][1],
				position_2_1[i][2], position_2_1[i][3]);
		}

		switch(st->capsel_flow) {
		case 0:
			if(i == 2) {
				dm_manual_set_attack_capsel(&st->attack_cap[1][0], 2, capsel_blue);
				dm_manual_set_attack_capsel(&st->attack_cap[1][1], 6, capsel_red);
				st->attack_cap_count[1] = 0;

				st->mode = MODE_WAIT_CAPSEL_FALL;
				st->capsel_flow++;
			}
			break;

		case 1:
			if(i == 5) {
				dm_manual_set_attack_capsel(&st->attack_cap[1][0], 2, capsel_blue);
				dm_manual_set_attack_capsel(&st->attack_cap[1][1], 4, capsel_red);
				dm_manual_set_attack_capsel(&st->attack_cap[1][2], 6, capsel_yellow);
				st->attack_cap_count[1] = 0;

				st->mode = MODE_WAIT_CAPSEL_FALL;
				st->capsel_flow++;
			}
			break;

		case 2:
			if(i == 10) {
				dm_manual_set_attack_capsel(&st->attack_cap[1][0], 1, capsel_red);
				dm_manual_set_attack_capsel(&st->attack_cap[1][1], 3, capsel_yellow);
				dm_manual_set_attack_capsel(&st->attack_cap[1][2], 5, capsel_red);
				dm_manual_set_attack_capsel(&st->attack_cap[1][3], 7, capsel_blue);
				st->attack_cap_count[1] = 0;

				st->mode = MODE_WAIT_CAPSEL_FALL;
				st->capsel_flow++;
			}
			break;
		}
		break;

	case MODE_WAIT_MESSAGE:
		st->mode_stop_flg = cap_flg_off;

		if(tutolWnd_isEnd(st->msgWnd)) {
			st->mode = st->next_mode;
		}
		break;

	case MODE_WAIT_CAPSEL_FALL:
		st->mode_stop_flg = cap_flg_off;

		if(!st->attack_cap[1][1].capsel_a_flg[cap_disp_flg]) {
			st->mode = st->next_mode;
		}
		break;

	case MODE_EXIT:
		res = false;
		break;
	}

	return res;
}

//////////////////////////////////////////////////////////////////////////////
//{### 操作説明その３処理メイン関数

static bool dm_manual_3_main()
{
	Manual *st = watchManual;
	DM_GAME_RET ret;
	int i, j;
	bool res = true;
	game_state *state;
	game_map *map;

	state = &game_state_data[0];
	map = game_map_data[0];

	if(st->mode_stop_flg) {
		ret = dm_manual_main_cnt(state, map, 0, 2);
		for(i = 1; i < 3; i++) {
			dm_manual_main_cnt(&game_state_data[i], game_map_data[i], i, 2);
		}

		// キー操作
		dm_manual_make_key(state, map);
	}

	// 連鎖等の特種表示の処理
	dm_manual_effect_cnt(state, map, 0);
	dm_manual_attack_capsel_down();

	// ウイルスアニメーション
	for(i = 0; i < 4; i++){
		dm_virus_anime(&game_state_data[i], game_map_data[i]);
	}

	// つみあがり警告音を再生
	dm_warning_h_line_se();

	switch(st->mode) {
	// 初期化
	case 0:
		tutolWnd_setPos(st->msgWnd, _posP4MsgWnd[1][0], _posP4MsgWnd[1][1]);
		st->pino_dir = 1;                    // キノピオの向き（左向き）
		st->pino_pos[0] = _posP4Mario[1][0]; // キノピオの座標
		st->pino_pos[1] = _posP4Mario[1][1];

		// メッセージウィンドウをフェードアウト
		tutolWnd_fadeOut(st->msgWnd);

		// 次の処理に
		st->mode = 1;

		// 落下カプセル作成
		for(i = 0; i < ARRAY_SIZE(capsel_3_1); i++) {
			CapsMagazine[i + 1] = capsel_3_1[i];
		}
		state->cap_magazine_cnt = 1; // マガジン残照を1にする
		dm_set_capsel(state);

		break;

	// ウイルス配置
	case 1:
		set_virus( map,
			virus_3_1[state->virus_number][1],
			virus_3_1[state->virus_number][2],
			virus_3_1[state->virus_number][0],
			virus_anime_table[virus_3_1[state->virus_number][0]][state->virus_anime]);

		state->virus_number++;

		if(state->virus_number >= ARRAY_SIZE(virus_3_1)) {
			// 次の処理に
			st->mode = 10;
		}
		break;

	case 10:
		tutolWnd_fadeIn(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_3_1);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 20;
		break;

	case 20:
		// メッセージウィンドウにメッセージを追加
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_3_2);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 30;
		break;

	case 30:
		// メッセージウィンドウにメッセージを追加
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_3_3);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 40;
		break;

	case 40:
		// メッセージウィンドウにメッセージを追加
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_3_4);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 50;
		break;

	case 50:
		// メッセージウィンドウにメッセージを追加
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_3_5);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 60;
		break;

	case 60:
		// メッセージウィンドウにメッセージを追加
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_3_6);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 61;
		break;

	case 61:
		state->mode_now = dm_mode_down;
		st->mode = MODE_CAPSEL_PROC;
		st->next_mode = 70;
		break;

	case 70:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_3_7);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 71;
		break;

	case 71:
		st->mode = MODE_CAPSEL_PROC;
		st->next_mode = 80;
		break;

	case 80:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_3_8);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 81;
		break;

	case 81:
		st->mode = MODE_CAPSEL_PROC;
		st->next_mode = 90;
		break;

	case 90:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_3_9);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 91;
		break;

	case 91:
		st->mode = MODE_CAPSEL_PROC;
		st->next_mode = 100;
		break;

	case 100:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_3_10);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 110;
		break;

	case 110:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_3_11);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 120;
		break;

	case 120:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_3_12);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 130;
		break;

	case 130:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_3_13);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 140;
		break;

	case 140:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_3_14);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 150;
		break;

	case 150:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_3_15);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = MODE_EXIT;
		break;

	case MODE_CAPSEL_PROC:
		st->mode_stop_flg = cap_flg_on;

		i = state->cap_magazine_cnt - 2;
		if(i < ARRAY_SIZE(position_3_1)) {
			aifMake2(state,
				position_3_1[i][0], position_3_1[i][1],
				position_3_1[i][2], position_3_1[i][3]);
		}

		switch(st->capsel_flow) {
		case 0:
			if(i == 2) {
				dm_manual_set_attack_capsel(&st->attack_cap[1][0], 2, capsel_blue);
				dm_manual_set_attack_capsel(&st->attack_cap[1][1], 6, capsel_yellow);
				st->attack_cap_count[1] = 0;

				st->mode = MODE_WAIT_CAPSEL_FALL;
				st->capsel_flow++;
			}
			break;

		case 1:
			if(i == 5) {
				dm_manual_set_attack_capsel(&st->attack_cap[2][0], 2, capsel_red);
				dm_manual_set_attack_capsel(&st->attack_cap[2][1], 6, capsel_yellow);
				st->attack_cap_count[2] = 0;

				st->mode = MODE_WAIT_CAPSEL_FALL;
				st->capsel_flow++;

				st->msgWnd->alphaStep = -st->msgWnd->alphaStep;
			}
			break;

		case 2:
			if(i == 8) {
				dm_manual_set_attack_capsel(&st->attack_cap[3][0], 2, capsel_red);
				dm_manual_set_attack_capsel(&st->attack_cap[3][1], 6, capsel_blue);
				st->attack_cap_count[3] = 0;

				st->mode = MODE_WAIT_CAPSEL_FALL;
				st->capsel_flow++;

				st->msgWnd->alphaStep = -st->msgWnd->alphaStep;
			}
			break;

		case 3:
			if(i == 10) {
				dm_manual_set_attack_capsel(&st->attack_cap[2][0], 2, capsel_red);
				dm_manual_set_attack_capsel(&st->attack_cap[2][1], 6, capsel_yellow);
				st->attack_cap_count[2] = 0;

				dm_manual_set_attack_capsel(&st->attack_cap[3][0], 2, capsel_red);
				dm_manual_set_attack_capsel(&st->attack_cap[3][1], 6, capsel_yellow);
				st->attack_cap_count[3] = 0;

				st->mode = MODE_WAIT_CAPSEL_FALL;
				st->capsel_flow++;

				st->msgWnd->alphaStep = -st->msgWnd->alphaStep;
			}
			break;
		}
		break;

	case MODE_WAIT_MESSAGE:
		st->mode_stop_flg = cap_flg_off;

		if(tutolWnd_isEnd(st->msgWnd)) {
			st->mode = st->next_mode;
		}
		break;

	case MODE_WAIT_CAPSEL_FALL:
		st->mode_stop_flg = cap_flg_off;

		if(!st->attack_cap[1][1].capsel_a_flg[cap_disp_flg]
		&& !st->attack_cap[2][1].capsel_a_flg[cap_disp_flg]
		&& !st->attack_cap[3][1].capsel_a_flg[cap_disp_flg])
		{
			if(st->pino_dir < 0) {
				if(st->msgWnd->alphaStep > 0) {
					st->msgWnd->alphaStep = -st->msgWnd->alphaStep;
				}
				else if(st->msgWnd->alpha == 0) {
					tutolWnd_setPos(st->msgWnd, _posP4MsgWnd[1][0], _posP4MsgWnd[1][1]);
					st->msgWnd->alphaStep = -st->msgWnd->alphaStep;
					st->pino_dir = -st->pino_dir;
					st->pino_pos[0] = _posP4Mario[1][0];
					st->pino_pos[1] = _posP4Mario[1][1];
				}
			}
			else {
				st->mode = st->next_mode;
			}
		}
		else {
			if(st->msgWnd->alphaStep < 0 && st->msgWnd->alpha == 0) {
				tutolWnd_setPos(st->msgWnd, _posP4MsgWnd[0][0], _posP4MsgWnd[0][1]);
				st->msgWnd->alphaStep = -st->msgWnd->alphaStep;
				st->pino_dir = -st->pino_dir;
				st->pino_pos[0] = _posP4Mario[0][0];
				st->pino_pos[1] = _posP4Mario[0][1];
			}
		}
		break;

	case MODE_EXIT:
		res = false;
		break;
	}

	return res;
}

//////////////////////////////////////////////////////////////////////////////
//{### 操作説明その４処理メイン関数

static bool dm_manual_4_main()
{
	Manual *st = watchManual;
	game_state *state;
	game_map *map;
	DM_GAME_RET ret;
	int i, j;
	bool res = true;

	state = &game_state_data[0];
	map = game_map_data[0];

	// キー点滅カウンター
	for(i = 0; i < ARRAY_SIZE(st->key_flash_count); i++) {
		if(st->key_flash_count[i] != KEY_FLASH_END) {
			st->key_flash_count[i] += KEY_FLASH_STEP;
		}
	}

	if(st->mode_stop_flg) {
		ret = dm_manual_main_cnt(state, map, 0, 0);

		// キー操作
		dm_manual_make_key(state, map);
	}
	dm_manual_effect_cnt(state, map, 0); // 連鎖等の特種表示の処理

	// ウイルスアニメーション
	dm_virus_anime(state, map);

	// 巨大ウイルスアニメーション
	dm_manual_update_virus_anime(state);

	// つみあがり警告音を再生
	dm_warning_h_line_se();

	switch(st->mode) {
	// 初期化
	case 0:
		tutolWnd_setPos(st->msgWnd, _posLvMsgWnd[0], _posLvMsgWnd[1]);
		st->pino_dir = 1;                 // キノピオの向き（左向き）
		st->pino_pos[0] = _posLvMario[0]; // キノピオの座標
		st->pino_pos[1] = _posLvMario[1];

		// メッセージウィンドウをフェードアウト
		tutolWnd_fadeOut(st->msgWnd);

		// 次の処理に
		st->mode = 1;

		// 落下カプセル作成
		for(i = 0; i < ARRAY_SIZE(capsel_4_1); i++) {
			CapsMagazine[i + 1] = capsel_4_1[i];
		}
		state->cap_magazine_cnt = 1; // マガジン残照を1にする
		dm_set_capsel(state);        // カプセル設定

		break;

	// ウイルス配置
	case 1:
		set_virus(map,
			virus_4_1[state->virus_number][1],
			virus_4_1[state->virus_number][2],
			virus_4_1[state->virus_number][0],
			virus_anime_table[virus_4_1[state->virus_number][0]][state->virus_anime]);

		state->virus_number++;
		if(state->virus_number >= ARRAY_SIZE(virus_4_1)) {
			// 次の処理に
			st->mode = 10;
		}
		break;

	case 10:
		tutolWnd_fadeIn(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_4_1);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 20;
		break;

	case 20:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_4_2);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 21;
		break;

	case 21:
		state->mode_now = dm_mode_down;
		st->mode = MODE_CAPSEL_PROC;
		st->next_mode = 30;
		break;

	case 30:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_4_3);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 40;
		break;

	case 40:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_4_4);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 41;
		break;

	case 41:
		st->mode = MODE_CAPSEL_PROC;
		st->next_mode = 50;
		break;

	case 50:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_4_5);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 51;
		break;

	case 51:
		st->mode = MODE_CAPSEL_PROC;
		st->next_mode = 60;
		break;

	case 60:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_4_6);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 61;
		break;

	case 61:
		st->mode = MODE_CAPSEL_PROC;
		st->next_mode = 70;
		break;

	case 70:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_4_7);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 71;
		break;

	case 71:
		st->mode = MODE_CAPSEL_PROC;
		st->next_mode = 80;
		break;

	case 80:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_4_8);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 81;
		break;

	case 81:
		st->mode = MODE_CAPSEL_PROC;
		st->next_mode = 90;
		break;

	case 90:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_4_9);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = 100;
		break;

	case 100:
		tutolWnd_clear(st->msgWnd);
		tutolWnd_addStr(st->msgWnd, mes_4_10);
		st->mode = MODE_WAIT_MESSAGE;
		st->next_mode = MODE_EXIT;
		break;

	case MODE_CAPSEL_PROC:
		st->mode_stop_flg = cap_flg_on;

		i = state->cap_magazine_cnt - 2;
		if(i < ARRAY_SIZE(position_4_1)) {
			aifMake2(state,
				position_4_1[i][0], position_4_1[i][1],
				position_4_1[i][2], position_4_1[i][3]);
		}

		switch(st->capsel_flow) {
			case 0: j = (i == 3); break;
			case 1: j = (i == 5); break;
			case 2: j = (i == 10); break;
			case 3: j = (i == 12); break;
			case 4: j = (i == 15); break;
			case 5: j = (i == 16); break;
			default: j = 0;
		}

		if(j) {
			st->mode = st->next_mode;
			st->capsel_flow++;
		}
		break;

	case MODE_WAIT_MESSAGE:
		st->mode_stop_flg = cap_flg_off;
		if(tutolWnd_isEnd(st->msgWnd)) {
			st->mode = st->next_mode;
		}
		break;

	case MODE_EXIT:
		res = false;
		break;
	}

	return res;
}

//////////////////////////////////////////////////////////////////////////////
//{### 描画物

//## AB を描画
static void draw_AB_guide(int x_pos, int y_pos)
{
	static const int _tex[][2] = {
		{ TEX_a_button, TEX_a_button_alpha },
		{ TEX_b_button, TEX_b_button_alpha },
	};
	static const int _pos[][2] = {
		{ 0, 0 }, { 0, 12 },
	};
	Manual *st = watchManual;
	STexInfo *texC, *texA;
	int width, alpha, i;

//	alpha = sins(st->animeCount << 10) * (127.f / 32768.f) + 191.f;
//	alpha = CLAMP(0, 255, alpha);
	alpha = 255;

	gSPDisplayList(gp++, alpha_texture_init_dl);
	gDPSetCombineMode(gp++, G_CC_ALPHATEXTURE_PRIM, G_CC_PASS2);
	gDPSetPrimColor(gp++, 0,0, 255,255,255, alpha);

	for(i = 0; i < ARRAY_SIZE(_tex); i++) {
		texC = _texAll + _tex[i][0];
		texA = _texAll + _tex[i][1];
		width = MIN(texC->size[0], texA->size[0]);

		StretchAlphaTexTile(&gp, width, texC->size[1],
			texC->addr[1], texC->size[0],
			texA->addr[1], texA->size[0],
			0, 0, width, texC->size[1],
			x_pos + _pos[i][0], y_pos + _pos[i][1], width, texC->size[1]);
	}
}

//////////////////////////////////////
//## 傘丸を描画
static void drawKaSaMaRu(Gfx **gpp, Mtx **mpp, Vtx **vpp, int x, int y, int dir, int alpha)
{
	static const int _pat[] = { 0, 1, 2, 3, 2, 1 };
	Manual *st = watchManual;

	Gfx *gp = *gpp;
	Mtx *mp = *mpp;
	Vtx *vp = *vpp;

	STexInfo *texC, *texA;
	int i, width, height;
	float mf[4][4], angle;

	// 下拵え
	guOrtho(mp, 0, SCREEN_WD, SCREEN_HT, 0, 1, 10, 1);
	gSPMatrix(gp++, mp, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
	mp++;

	guTranslate(mp, 0, 0, -5);
	gSPMatrix(gp++, mp, G_MTX_PROJECTION | G_MTX_MUL | G_MTX_NOPUSH);
	mp++;

	// 揺れ具合
	angle = WrapF(0, 1, st->animeCount * (1.0 / 128.0)) * M_PI * 2;
	guRotateRPYF(mf, 0, (1 - dir) * 90, sinf(angle) * 4 * dir);

	// 傘丸
	i = WrapI(0, ARRAY_SIZE(_pat), ((st->animeCount & 127) * ARRAY_SIZE(_pat) * 16) >> 7);
	if(!tutolWnd_isSpeaking(st->msgWnd)) i = 0;

	texC = _texKaSa + TEX_kasamaru01 + _pat[i];
	texA = _texKaSa + TEX_kasamaru_alpha;
	width = MIN(texC->size[0], texA->size[0]);
	height = MIN(texC->size[1], texA->size[1]);

	gSPDisplayList(gp++, alpha_texture_init_dl);
	gSPClearGeometryMode(gp++, 0|G_ZBUFFER|G_SHADE|G_SHADING_SMOOTH|G_CULL_FRONT|G_CULL_BACK|G_LIGHTING|G_FOG|G_TEXTURE_GEN|G_TEXTURE_GEN_LINEAR|G_CLIPPING|G_LOD|G_TEXTURE_ENABLE);
	gDPSetCombineMode(gp++, G_CC_ALPHATEXTURE_PRIM, G_CC_PASS2);
	gDPSetTexturePersp(gp++, G_TP_NONE);
	gDPSetPrimColor(gp++, 0,0, 255,255,255, alpha);

	mf[3][0] = (dir > 0) ? x : x + width;
	mf[3][1] = y;
	guMtxF2L(mf, mp);
	gSPMatrix(gp++, mp, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
	mp++;

	RectAlphaTexTile(&gp, &vp, width, height,
		texC->addr[1], texC->size[0],
		texA->addr[1], texA->size[0],
		0, 0, width, height,
		0, 0, width, height);

	// お口
//	if(tutolWnd_isSpeaking(st->msgWnd) && (st->animeCount & 4)) {
//		gSPDisplayList(gp++, normal_texture_init_dl);
//
//		texC = _texKaSa + TEX_kasamaru_mouth;
//		RectTexTile8(&gp, &vp,
//			texC->size[0], texC->size[1],
//			texC->addr[0], texC->addr[1],
//			0, 0, texC->size[0], texC->size[1],
//			24, 61, texC->size[0], texC->size[1]);
//	}

	// 〆
	*vpp = vp;
	*mpp = mp;
	*gpp = gp;
}

//////////////////////////////////////
//## 操作説明用攻撃カプセル描画関数
void dm_manual_at_cap_draw(game_state *state, game_a_cap *cap, int size_flg)
{
//	static u8 cap_size[] = {220,128};
	int i, j;

	// 色の分だけループをまわす
	for(i = 0;i < 3;i++) {
		// パレットロード
//		load_TexPal( cap_pal[size_flg][i] );
		load_TexPal( dm_game_get_capsel_pal(size_flg, i)->addr[0] );

		for(j = 0;j < 4;j++) {
			// 表示フラグをチェック
			if(!(cap + j)->capsel_a_flg[cap_disp_flg]) {
				continue;
			}
			// 同じ色かチェック
			if((cap + j)->capsel_a_p != i) {
				continue;
			}
			// ボール描画
			draw_Tex(
				state->map_x + ( cap + j )->pos_a_x *  state->map_item_size,
				state->map_y + ( cap + j )->pos_a_y * state->map_item_size,
				state->map_item_size,state->map_item_size,0,capsel_b * state->map_item_size );
		}
	}
}

//////////////////////////////////////
//## コントローラ描画関数

static const int _posContPanel[] = { 10, 14 };

static void disp_cont()
{
#if LOCAL==CHINA
	static const int _posCircle[][2] = {
		{ 22, 46 }, // left
		{ 31, 42 }, // right
		{ 52, 27 }, // a
		{ 43, 25 }, // b
	};
	static const int _posFinger[][2] = {
		{ 22, 32 }, // left
		{ 31, 28 }, // right
		{ 52, 13 }, // a
		{ 43, 11 }, // b
	};
#else
	static const int _posCircle[][2] = {
		{ 13, 20 }, // left
		{ 28, 21 }, // right
		{ 70, 32 }, // a
		{ 62, 26 }, // b
	};
	static const int _posFinger[][2] = {
		{ 13,  6 }, // left
		{ 28,  7 }, // right
		{ 70, 18 }, // a
		{ 62, 12 }, // b
	};
#endif
	Manual *st = watchManual;
	STexInfo *texC, *texA;
	const int *pos;
	int i, x, y, width;

	for(i = 0; i < ARRAY_SIZE(st->key_flash_count); i++) {
		if(st->key_flash_count[i] == KEY_FLASH_END) {
			continue;
		}

		gSPDisplayList(gp++, normal_texture_init_dl);
		gDPSetCombineLERP(gp++,
			0,0,0,PRIMITIVE, TEXEL0,0,PRIMITIVE,0,
			0,0,0,PRIMITIVE, TEXEL0,0,PRIMITIVE,0);
		gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
		gDPSetPrimColor(gp++, 0,0, 255,0,0,255);
		gDPSetTextureLUT(gp++, G_TT_NONE);

		// カーソル
		pos = _posCircle[i];
		texC = _texAll + TEX_account_circle;
		StretchTexBlock4i(&gp,
			texC->size[0], texC->size[1], texC->addr[1],
			_posContPanel[0] + pos[0],
			_posContPanel[1] + pos[1],
			texC->size[0], texC->size[1]);

		gSPDisplayList(gp++, alpha_texture_init_dl);

		// 指
		pos = _posFinger[i];
		texC = _texAll + TEX_account_finger;
		texA = _texAll + TEX_account_finger_alpha;
		width = MIN(texC->size[0], texA->size[0]);
		StretchAlphaTexBlock(&gp, width, texC->size[1],
			texC->addr[1], texC->size[0],
			texA->addr[1], texA->size[0],
			_posContPanel[0] + pos[0],
			_posContPanel[1] + pos[1] - MIN(4, abs(st->key_flash_count[i])),
			width, texC->size[1]);
	}
}

//////////////////////////////////////
//## 前景を描画

static void dm_manual_draw_fg(Mtx **mpp, Vtx **vpp)
{
	Manual *st = watchManual;
	STexInfo *texC, *texA;
	int i, width;

	// 特殊背景描画
	switch(evs_manual_no) {
	case 0:
	case 3:
		gSPDisplayList(gp++, normal_texture_init_dl);
		texC = _texAll + TEX_account_controller;
		tiStretchTexBlock(&gp, texC, false,
			_posContPanel[0], _posContPanel[1], texC->size[0], texC->size[1]);

		// 巨大ウイルス
		dm_draw_big_virus(&gp);
		break;
	}

	// 瓶の中の情報描画
	switch(evs_manual_no) {
	// 操作説明その１
	case 0:
		// カプセル描画
		dm_game_graphic_p(&game_state_data[0], 0, game_map_data[0]);

		// 演出描画
//		dm_game_graphic_effect(&game_state_data[0], 0, 0);

		// ○描画
		disp_cont();
		break;

	// 操作説明その２
	case 1:
		// 瓶を描画
		dm_calc_bottle_2p();
		dm_draw_bottle_2p(&gp);

		for(i = 0; i < 2; i++) {
			// カプセル描画
			dm_game_graphic_p(&game_state_data[i], i, game_map_data[i]);

			// 攻撃カプセル描画
			dm_manual_at_cap_draw(&game_state_data[i], st->attack_cap[i], 0);

			// 演出描画
//			dm_game_graphic_effect(&game_state_data[i], i, 0);
		}
		break;

	// 操作説明その３
	case 2:
		for(i = 0; i < 4; i++) {
			// カプセル描画
			dm_game_graphic_p(&game_state_data[i], i, game_map_data[i]);

			// 攻撃カプセル描画
			dm_manual_at_cap_draw(&game_state_data[i], st->attack_cap[i], 1);

			// 演出描画
//			dm_game_graphic_effect(&game_state_data[i], i, 1);
		}
		break;

	// 操作説明その４
	case 3:
		// カプセル描画
		dm_game_graphic_p(&game_state_data[0], 0, game_map_data[0]);

		// 演出描画
//		dm_game_graphic_effect(&game_state_data[0], 0, 0);

		// ○描画
		disp_cont();
		break;
	}

	// 傘丸を描画
	dm_draw_KaSaMaRu(&gp, mpp, vpp, tutolWnd_isSpeaking(st->msgWnd),
		st->pino_pos[0], st->pino_pos[1], st->pino_dir, st->msgWnd->alpha * 255);

	// PUSH ANY KEY描画
	switch(evs_manual_no) {
	case 0:
	case 3:
		if(main_old == MAIN_TITLE) {
			push_any_key_draw(220, 210);
		}
		else {
			draw_AB_guide(200, 202);
		}
		break;

	case 1:
		if(main_old == MAIN_TITLE) {
			push_any_key_draw(128, 210);
		}
		else {
			draw_AB_guide(110, 202);
		}
		break;

	case 2:
		if(main_old == MAIN_TITLE) {
			push_any_key_draw(230, 210);
		}
		else {
			draw_AB_guide(110, 202);
		}
		break;
	}

	// メッセージウィンドウを描画
	tutolWnd_draw(st->msgWnd, &gp);
}

//////////////////////////////////////////////////////////////////////////////
//{### 初期化

#include "boot_data.h"

//////////////////////////////////////
//## 初期化
void dm_manual_all_init()
{
	static const s16 map_x_table[][4] = {
		{ dm_wold_x,       dm_wold_x,       dm_wold_x,       dm_wold_x       }, // 1P
		{ dm_wold_x_vs_1p, dm_wold_x_vs_2p, dm_wold_x_vs_1p, dm_wold_x_vs_2p }, // 2P
		{ dm_wold_x_4p_1p, dm_wold_x_4p_2p, dm_wold_x_4p_3p, dm_wold_x_4p_4p }, // 4P
	};
	static const u8 _seqTbl[] = {
		SEQ_Game_C >> 1, SEQ_Game_D >> 1,
	};
	static const u8 map_y_table[] = { dm_wold_y, dm_wold_y_4p };
	static const u8 size_table[] = { cap_size_10, cap_size_8 };
	Manual *st;
	int i, j, k;

	// BGM
	evs_seqence = TRUE;
	evs_seqnumb = _seqTbl[evs_manual_no % ARRAY_SIZE(_seqTbl)];

	// dm_game_init を呼ぶ前の下ごしらえ
	switch(evs_manual_no) {
	case 0: case 3:
		// SE の設定のため
		evs_gamesel = GSL_1PLAY;
		evs_playcnt = 1;
		break;

	case 1: case 4:
		// SE の設定のため
		evs_gamesel = GSL_2PLAY;
		evs_playcnt = 2;
		break;

	case 2: case 5:
		// SE の設定のため
		evs_gamesel = GSL_4PLAY;
		evs_playcnt = 4;
		break;
	}
	evs_gamemode = GMD_NORMAL;
	story_proc_no = STORY_M_ST1;

	// ヒープを初期化
	dm_game_init_heap();
	{
		void **heap;
		void *(*tbl)[2] = _romDataTbl;

		// ゲームメインからヒープを取得
		heap = dm_game_heap_top();

		// メイン構造体をヒープに割り当てる
		st = watchManual = (Manual *)ALIGN_16(*heap);
		bzero(st, sizeof(Manual));
		*heap = watchManual + 1;

		// テクスチャを読み込む
		_texAll = tiLoadTexData(heap, tbl[_tutorial_data][0], tbl[_tutorial_data][1]);
		_texKaSa = tiLoadTexData(heap, tbl[_menu_kasa][0], tbl[_menu_kasa][1]);

		// メッセージウィンドウを初期化
		tutolWnd_init(st->msgWnd, heap);
		tutolWnd_fadeIn(st->msgWnd);

		// グラフィックデータの読込み(背景等)
		dm_game_init_static();

		// 背景取り込みの準備(※)
		dm_game_init_snap_bg();

		// ゲーム用変数初期化
		dm_game_init(0);
	}

	// Mtx, Vtx buffer
//	for(i = 0; i < GFX_GTASK_NUM; i++) {
//		st->mtxPtr[i] = st->mtxBuf[i];
//		st->vtxPtr[i] = st->vtxBuf[i];
//	}

	// フェードイン・アウト制御
	st->fade_count = 255;
	st->fade_step = -8;

	// 背景の取り込みを行うフラグ
//	st->bg_snap_flg = 1;

	// リーチSEフラグをＯＦＦにする
	st->last3_flg = cap_flg_off;

	// キー点滅時間ＯＦＦにする
	for(i = 0; i < ARRAY_SIZE(st->key_flash_count); i++) {
		st->key_flash_count[i] = KEY_FLASH_END;
	}

	// win, lose, game over などの timer
	st->logo_timer = 0;

	// BGM再生制御
	st->bgm_timer = 0;
	st->bgm_timeout = 0;

	// アニメーションカウンタ
	st->animeCount = 0;

	for(i = j = 0; i < 3; i++) {
		st->big_virus_flg[i][0] = 0;
		st->big_virus_flg[i][1] = 0;

		// 巨大ウイルスの座標を計算する
		st->big_virus_pos[i][2] = j;	// sin,cos用数値
		st->big_virus_pos[i][0] = sinf(DEGREE(st->big_virus_pos[i][2])) *  20 +  45;
		st->big_virus_pos[i][1] = cosf(DEGREE(st->big_virus_pos[i][2])) * -20 + 155;
		j += 120;
	}

	switch(evs_manual_no) {
	case 0: // その１(１人用)
	case 3: // その３(１人用)
		k = 0; // アイテムの大きさ
		j = 0; // 基準Ｘ座標
		break;
	case 1: // その２(２人用)
	case 4: // その４(２人用)
		k = 0; // アイテムの大きさ
		j = 1; // 基準Ｘ座標
		break;
	case 2: // その３(４人用)
	case 5: // その３(４人用)
		k = 1; // アイテムの大きさ
		j = 2; // 基準Ｘ座標
		break;
	}

	for(i = 0; i < 4; i++) {
		game_state *state = &game_state_data[i];
		// 座標系の設定
		state->map_x = map_x_table[j][i];     // 基準Ｘ座標
		state->map_y = map_y_table[k];        // 基準Ｙ座標
		state->map_item_size = size_table[k]; // 基準アイテムサイズ
	}

	for(i = 0; i < 4; i++) {
		game_state *state = &game_state_data[i];

		// マニュアルは SPEED_MID に固定
		state->cap_def_speed = SPEED_MID;

		// 落下速度の設定
		state->cap_speed = GameSpeed[state->cap_def_speed];

		// 
		state->think_type = CHR_MARIO;
		state->think_level = 2;

		switch(evs_manual_no) {
		case 2:
		case 5:
			// ４Ｐの場合
			state->virus_anime_spead = v_anime_def_speed_4p; // アニメーション間隔の設定
			state->virus_anime_max = 0;                      // ウイルスアニメーション最大コマ数の設定
			break;
		default:
			// それ以外の場合
			state->virus_anime_spead = v_anime_def_speed; // アニメーション間隔の設定
			state->virus_anime_max = 2;                   // ウイルスアニメーション最大コマ数の設定
			break;
		}

		// 攻撃カプセルの設定
		st->attack_cap_count[i] = 0;

		for(j = 0; j < ARRAY_SIZE(st->attack_cap[i]); j++) {
			game_a_cap *acap = &st->attack_cap[i][j];

			acap->pos_a_x = 0;
			acap->pos_a_y = 0;
			acap->capsel_a_p = 0;

			for(k = 0; k < ARRAY_SIZE(acap->capsel_a_flg); k++) {
				acap->capsel_a_flg[k] = 0;
			}
		}
	}

	st->mode = 0;
	st->next_mode = 0;
	st->mode_stop_flg = 0;

	st->capsel_flow = 0;
}

//////////////////////////////////////////////////////////////////////////////
//{### スケジューラから呼ばれる関数

//////////////////////////////////////
//## メイン
int dm_manual_main(NNSched*	sched)
{
	Manual *st;
	OSMesgQueue msgQ;
	OSMesg msgbuf[MAX_MESGS];
	NNScClient client;
	int i, key;
	bool loop_flg = true;
	bool fade_out = false;

	// メッセージキューの作成
	osCreateMesgQueue(&msgQ, msgbuf, MAX_MESGS);

	// クライアントに登録
	nnScAddClient(sched, &client, &msgQ);

	// 初期化
	dm_manual_all_init();
	st = watchManual;

	// 思考フラグを初期化
	for(i = 0; i < 4; i++) {
		aifMakeFlagSet(&game_state_data[i]);
	}

	// メインループ
	while(!fade_out) {
		joyProcCore();

		// リトレースメッセージ
		osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);

		if ( GameHalt ) {
			graphic_no = GFX_NULL;
			dm_audio_update();
			continue;
		}

		// 文字列進行処理
		tutolWnd_update(st->msgWnd);

		// PUSH ANY KEY の揺れ処理
		dm_effect_make();

		// アニメーションカウンタ
		st->animeCount++;

		// フェードイン･アウト制御
		fade_out = (st->fade_step > 0 && st->fade_count == 255);
		st->fade_count = CLAMP(0, 255, st->fade_count + st->fade_step);

		switch(evs_manual_no) {
		case 0:
			loop_flg = dm_manual_1_main();
			break;
		case 1:
			loop_flg = dm_manual_2_main();
			break;
		case 2:
			loop_flg = dm_manual_3_main();
			break;
		case 3:
			loop_flg = dm_manual_4_main();
			break;
		}

		// ボリュームを下げる
		dm_seq_set_volume(0x60);

		// ＳＥ再生
		dm_audio_update();

		// 音楽再生中
		if(st->bgm_timeout) {
			if(++st->bgm_timer >= st->bgm_timeout){
				// 再生時間終了
				st->bgm_timer = 0;
				st->bgm_timeout = 0;
				dm_seq_play_in_game(evs_seqnumb << 1);
			}
		}

		// キャンセル処理
		if(st->fade_count == 0) {
			// デモ画面の操作説明は A_BUTTON を any key に含めない
			switch(main_old) {
			case MAIN_TITLE:
				key = DM_ANY_KEY;
				break;

			default:
				key = B_BUTTON;
				break;
			}

			if(joyupd[main_joy[0]] & key) {
				// 終了
				loop_flg = false;
			}
		}

		if(!loop_flg && st->fade_step < 0) {
			// フェードアウトを開始
			st->fade_step = -st->fade_step;
		}

		// グラフィックの設定
		graphic_no = GFX_MANUAL;
	}

	// 音楽停止
	dm_audio_stop();

	// 表示停止
	graphic_no = GFX_NULL;

	// グラフィック・オーディオタスク終了待ち
	while(!dm_audio_is_stopped() || (pendingGFX != 0)) {
		osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
		dm_audio_update();
	}

	// クライアントの抹消
	nnScRemoveClient(sched, &client);

	if(main_old == MAIN_TITLE) {
		return MAIN_TITLE;
	}
	else if(main_old == MAIN_MENU) {
		return MAIN_MENU;
	}
}

//////////////////////////////////////
//## グラフィック

void dm_manual_graphic()
{
	Manual *st = watchManual;
	NNScTask *gt;
	Mtx *mp;
	Vtx *vp;

	// ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄﾊﾞｯﾌｧ･ﾀｽｸﾊﾞｯﾌｧの指定
	gp = &gfx_glist[gfx_gtask_no][0];
	gt = &gfx_task[gfx_gtask_no];
	mp = dm_get_mtx_buf();
	vp = dm_get_vtx_buf();

	// RSP 初期設定
	gSPSegment(gp++, 0, 0x0);

	// フレームバッファのクリア
	F3RCPinitRtn();
//	F3ClearFZRtn(st->bg_snap_flg);
	F3ClearFZRtn(false);

	// 描画範囲の指定
	gDPSetScissor(gp++,G_SC_NON_INTERLACE, 0, 0, SCREEN_WD-1, SCREEN_HT-1);

	// 背景を描画
	dm_game_draw_snap_bg(&gp, &mp, &vp, FALSE);
//	if(st->bg_snap_flg) {
//		dm_game_draw_snap_bg(&gp, &mp, &vp, FALSE);
//	}
//	else {
//		dm_game_draw_snapped_bg(&gp);
//	}

	// 前景を描画
	dm_manual_draw_fg(&mp, &vp);
//	if(!st->bg_snap_flg) {
//		dm_manual_draw_fg(&mp, &vp);
//	}

	// フェードフィルタ
	{
		int c, a;
		c = 255;
		a = CLAMP(0, 255, (st->fade_count - 127) * 1.2 + 127);
		if(a > 0) {
			FillRectRGBA(&gp, 0, 0, SCREEN_WD, SCREEN_HT, c, c, c, a);
		}
	}

	// ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄ作成終了, ｸﾞﾗﾌｨｯｸﾀｽｸの開始
	gDPFullSync(gp++);       // ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄ最終処理
	gSPEndDisplayList(gp++); // ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄ終端
	osWritebackDCacheAll();  // dinamicｾｸﾞﾒﾝﾄのﾌﾗｯｼｭ
	gfxTaskStart(gt, gfx_glist[gfx_gtask_no],
		(s32)(gp - gfx_glist[gfx_gtask_no]) * sizeof(Gfx),
		GFX_GSPCODE_F3DEX,
//		st->bg_snap_flg ? 0 : NN_SC_SWAPBUFFER);
		NN_SC_SWAPBUFFER);

//	st->bg_snap_flg = 0;
}

//////////////////////////////////////////////////////////////////////////////
//{### EOF
