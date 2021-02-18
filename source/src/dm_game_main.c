//////////////////////////////////////////////////////////////////////////////
//	filename	:	dm_game_main.c
//	create		:	1999/08/02
//	modify		:	1999/12/06
//	created		:	Hiroyuki Watanabe
//////////////////////////////////////////////////////////////////////////////

#include <ultra64.h>
#include <PR/ramrom.h>
#include <PR/gs2dex.h>
#include <assert.h>
#ifndef LINUX_BUILD
#include <math.h>
#endif

#include "calc.h"
#include "def.h"
#include "segment.h"
#include "message.h"
#include "nnsched.h"
#include "main.h"
#include "audio.h"
#include "graphic.h"
#include "joy.h"
#include "static.h"
#include "musdrv.h"

#include "aiset.h"
#include "aidata.h"

#include "evsworks.h"
#include "dm_game_main.h"
#include "record.h"
#include "cpu_task.h"

#include "vr_init.h"
#include "dm_virus_init.h"

#include "util.h"
#include "tex_func.h"
#include "char_anime.h"
#include "game_etc.h"
#include "msgwnd.h"
#include "main_story.h"

#ifdef DEBUG
#include "aidebug.h"
#endif

#include "debug.h"
#include "replay.h"
#include "passwd.h"
#include "local.h"

// 最終提出時に無効になる定義
#if !defined(NINTENDO)
#define _ENABLE_CONTROL // 強制操作
#endif // NINTENDO

//////////////////////////////////////////////////////////////////////////////
//{### マクロ

#define _getKeyTrg(playerNo) ((int)joyupd[main_joy[(playerNo)]])
#define _getKeyRep(playerNo) ((int)joycur[main_joy[(playerNo)]])
#define _getKeyLvl(playerNo) ((int)joynew[main_joy[(playerNo)]])

//## ヒープの先頭
#define HEAP_START _codeSegmentEnd

//## リトライコインの最大数
#define COINMAX 3

//## デバッグモードに入る時に押すボタン
#define CONT_DEBUG (DM_KEY_CU | DM_KEY_CD)
#define CONT_CONFIG (DM_KEY_CL | DM_KEY_CR)

/*
//## 指定したステータスがＣＰＵかどうかをチェック
#define PLAYER_IS_CPU(state, playerNo) \
	((state)->player_type == PUF_PlayerCPU || \
	 PLAYER_IS_DEBUG((state), (playerNo)))

//## 指定したステータスがデバッグ用ＣＰＵかどうかチェック
#define PLAYER_IS_DEBUG(state, playerNo) \
	((state)->player_type != PUF_PlayerCPU && \
	 (playerNo) == 0 && aiDebugP1 >= 0)
*/
/* XXXblythe stupid gcc can't parse the above macros properly */
#define PLAYER_IS_CPU(state, playerNo) ((state)->player_type == PUF_PlayerCPU || PLAYER_IS_DEBUG((state), (playerNo)))

#define PLAYER_IS_DEBUG(state, playerNo) ((state)->player_type != PUF_PlayerCPU && (playerNo) == 0 && aiDebugP1 >= 0)


#define MARIO_STAND_X (218 + 32)
#define MARIO_STAND_Y ( 20 + 64)
#define MARIO_THROW_X (MARIO_STAND_X - 32 +  0)
#define MARIO_THROW_Y (MARIO_STAND_Y - 64 + 32)

#define _setDarkCapPrim(gp) gDPSetPrimColor((gp), 0,0, 96,96,96,150)

#define JOYrTIMEvm 13500 // DrMｹﾞｰﾑ時ｺﾝﾄﾛｰﾗ読みとりﾀｲﾐﾝｸﾞ( vsMEN )
//#define JOYrTIMEvc 10500 //             ..               ( vsCPU )

//## パネル影のアルファ値
#define PANEL_SHADOW_ALPHA 160

//## 瓶の透明度
#define BOTTLE_ALPHA 192

//## 最終ステージ番号
#define LAST_STAGE_NO 8

//## EXステージ番号
#define EX_STAGE_NO 9

//## シーケンスの音量
#define SEQ_VOL_LOW 0x40
#define SEQ_VOL_HI  0x80

//## スコアの最大値
#define SCORE_MAX_VAL 9999900

//## スコアアタックのタイムリミット
#define SCORE_ATTACK_TIME_LIMIT (3 * 60 * FRAME_PAR_SEC)

//## ウィルスレベルマックス
#define VIRUS_LEVEL_MAX 99

//## シザリングフラグ
#define SCISSOR_FLAG 2

//////////////////////////////////////////////////////////////////////////////
//{### 共通データ

//## ウイルスのアニメーションパターン番号
u8 virus_anime_table[][4] = {
	{ virus_a1, virus_a2, virus_a3, virus_a4 }, // 赤用
	{ virus_b1, virus_b2, virus_b3, virus_b4 }, // 黄用
	{ virus_c1, virus_c2, virus_c3, virus_c4 }, // 青用
};

//////////////////////////////////////////////////////////////////////////////
//{### 共通ルーチン

//## 得点を算出
static u32 dm_calc_score(game_state *state, int level)
{
	u32 score = 0;
	int i;

	for(i = state->erase_virus_count_old; i < state->erase_virus_count; i++) {
		score += Score1p[level][MIN(i, SCORE_MAX - 1)] * 100;
	}

	state->erase_virus_count_old = state->erase_virus_count;

	return score;
}

//## 得点を加算
static void dm_add_score(game_state *state, u32 score)
{
	state->game_score += score;

	// カンスト
	if(state->game_score > SCORE_MAX_VAL) {
		state->game_score = SCORE_MAX_VAL;
	}

	// ハイスコアの更新
	if(evs_high_score < state->game_score) {
		evs_high_score = state->game_score;
	}
}

//## 得点関数
u32 dm_make_score(game_state *state)
{
	int level;
	u32 score;

	if(evs_story_flg) {
		level = MIN(2, evs_story_level);
	}
	else {
		switch(evs_gamesel) {
		case GSL_1PLAY:
		case GSL_1DEMO:
			level = state->cap_def_speed;
			break;

		case GSL_2PLAY:
		case GSL_VSCPU:
		case GSL_2DEMO:
			level = state->cap_def_speed;
			break;

		case GSL_4PLAY:
		case GSL_4DEMO:
			level = state->cap_def_speed;
			break;

		default:
			DBG0(true, "不正なゲームモード\n");
			break;
		}
	}

	score = dm_calc_score(state, level);
	dm_add_score(state, score);

	return score;
}

//## 攻撃音発生関数
void dm_attack_se(game_state *state, int player_no)
{
	// 2P時の攻撃ＳＥテーブル
	static s8 dm_chaine_se_table_vs[] = {
		SE_gCombo1P, SE_gCombo2P
	};
	// 4P時の攻撃ＳＥテーブル
	static s8 dm_chaine_se_table_4p[4][3] = {
		{ SE_gCombo4P, SE_gCombo3P, SE_gCombo2P },
		{ SE_gCombo1P, SE_gCombo4P, SE_gCombo3P },
		{ SE_gCombo2P, SE_gCombo1P, SE_gCombo4P },
		{ SE_gCombo3P, SE_gCombo2P, SE_gCombo1P },
	};
//	static s8 dm_chaine_se_table_4p[4] = {
//		SE_gCombo1P, SE_gCombo2P, SE_gCombo3P, SE_gCombo4P,
//	};
	int i;

	if(state->chain_line < 2) {
		return;
	}

	switch(evs_gamesel) {
	case GSL_1PLAY:
		dm_snd_play_in_game(SE_gCombo1P);
		break;

	case GSL_2PLAY:
	case GSL_VSCPU:
		dm_snd_play_in_game(dm_chaine_se_table_vs[player_no]);
		break;

	case GSL_4PLAY:
		for(i = 0; i < 3; i++) {
			if(state->chain_color[3] & (1 << i)) {
				dm_snd_play_in_game(dm_chaine_se_table_4p[player_no][i]);
			}
		}
//		dm_snd_play_in_game(dm_chaine_se_table_4p[player_no]);
		break;
	}
}

//## 積み上げ警告音発生関数
void dm_warning_h_line(game_state *state, game_map *map)
{
	int i, j, flg = 0;

	// 上の３段しか調べない
	for(i = 1; i < 4; i++) {
		for(j = 0; j < GAME_MAP_W; j++) {
			if(get_map_info(map, j, i) == cap_flg_on) {
				// 上の３段に何か物があった
				flg = 1;
				break;
			}
		}
		if(flg) {
			break;
		}
	}

	if(flg) {
		if(state->warning_se_flag == 0) {
			// 鳴らしていないか再度積みあがった
//			dm_snd_play_in_game(SE_gWarning); // 警告 SE 再生
			state->warning_se_flag++;
		}
	}
	else {
		// ビットを消す
		state->warning_se_flag = 0;
	}
}

//## 落下情報設定関数
static int set_down_flg(game_map *map)
{
	int i, j, ret;
	unsigned int k, l, flg[2];

	for(i = 0;i < 120;i++) {	// 落下フラグの設定
		map[i].capsel_m_flg[cap_down_flg] = cap_flg_on;	// 落下フラグの設定
	}

	for(j = 14;j >= 0;j--)	// 最下段の１つ上の段から始める
	{
		for(i = 0;i < 8;i++)
		{
			k = (j << 3) + i;	// 配列番号の計算
			if((map + k)->capsel_m_flg[cap_disp_flg])
			{
				// 表示されている
				if((map + k)->capsel_m_flg[cap_virus_flg] < 0)
				{
					// ウイルスでない
					if((map + k)->capsel_m_g > capsel_d && (map + k)->capsel_m_g < capsel_b)
					{
						// 横向きカプセルの場合
						if((map + k)->capsel_m_g == capsel_l)
						{
							// 最下段ではない
							for(l = 0;l < 2;l++) {
								flg[l] = cap_flg_on;
								if(get_map_info(map,(map + k+ l)->pos_m_x,(map + k+ l)->pos_m_y + 1) == cap_flg_on)
								{
									// 障害物があった場合
									if((map + k + l + 8)->capsel_m_flg[cap_down_flg] != cap_flg_on) {
										// その障害物は落下出来ない
										flg[l] = cap_flg_off;
									}
								}
							}

							if(!flg[0] || !flg[1])
							{
								// 両方とも下に落ちることが出来ない
								for(l = 0;l < 2;l++)
								{
									(map + k + l)->capsel_m_flg[cap_down_flg] = cap_flg_off;	// 落下フラグを立消す
								}
							}
						} else if((map + k)->capsel_m_g == capsel_r) {
							for(l = 0;l < 2;l++) {
								flg[l] = cap_flg_on;
								if(get_map_info(map,(map + k - l)->pos_m_x,(map + k - l)->pos_m_y + 1) == cap_flg_on)
								{
									// 障害物があった場合
									if((map + k - l + 8)->capsel_m_flg[cap_down_flg] != cap_flg_on) {
										// その障害物は落下出来ない
										flg[l] = cap_flg_off;
									}
								}
							}
							if(!flg[0] || !flg[1])
							{
								// 両方とも下に落ちることが出来ない
								for(l = 0;l < 2;l++)
								{
									(map + k - l)->capsel_m_flg[cap_down_flg] = cap_flg_off;	// 落下フラグを消す
								}
							}
						}
					} else {
						// ボールの場合
						if(get_map_info(map,(map + k)->pos_m_x, (map + k)->pos_m_y + 1) == cap_flg_on)
						{
							// 物体の下に何かあった
							if((map + k + 8)->capsel_m_flg[cap_down_flg] != cap_flg_on) {
								(map + k)->capsel_m_flg[cap_down_flg] = cap_flg_off;	// 落下フラグの設定
							}
						}
					}
				} else {
					(map + k)->capsel_m_flg[cap_down_flg] = cap_flg_off;	// 落下フラグの設定
				}
			} else {
				(map + k)->capsel_m_flg[cap_down_flg] = cap_flg_off;	// 落下フラグの設定
			}
		}
	}

	// 落下フラグの状況を返す
	for(i = ret = 0;i < 128;i++) {
		if(map[i].capsel_m_flg[cap_disp_flg]) {
			// 表示フラグが立っている
			if(map[i].capsel_m_flg[cap_down_flg]) {
				// 落下フラグが立っている
				ret++;
			}
		}
	}

	return ret;
}

//## 粒落下関数
void go_down(game_state *state, game_map *map , int cout)
{
	int i, j, p;
	unsigned int k, se_flg = 0;

	state->erase_anime_count++;

	// ウェイト時間経過?
	if(state->erase_anime_count < cout) {
		return;
	}

	state->erase_anime_count = 0;

	// 最下段の１つ上の段から始める
	for(j = GAME_MAP_H; j >= 0; j--) {

		for(i = 0; i < GAME_MAP_W; i++) {

			// 配列番号の計算
			k = j * GAME_MAP_W + i;

			// 落下フラグが立っている?
			if((map + k)->capsel_m_flg[cap_down_flg]) {

				// 落下先に情報をコピー
				set_map(map ,(map + k)->pos_m_x,(map + k)->pos_m_y + 1,(map + k)->capsel_m_g,(map + k)->capsel_m_p);

				// コピー元情報の削除
				clear_map(map,(map + k)->pos_m_x, (map + k)->pos_m_y);

				// 落下音を鳴らす
				se_flg = 1;
			}
		}
	}

	if(se_flg) {
		// 落下ＳＥ再生
		dm_snd_play_in_game(SE_gFreeFall);
	}

	if(state->flg_game_over) {
		// リタイア状態
		if(state->cnd_training == dm_cnd_training) {
			// 練習中
			for(i = 0; i < GAME_MAP_W * GAME_MAP_H; i++) {
				if(map[i].capsel_m_flg[cap_disp_flg])
				{
					// 表示フラグが立っている
					if(!map[i].capsel_m_flg[cap_down_flg])
					{
						// 落下フラグが立っていない
						if(map[i].capsel_m_p < 3) {
							// 暗くない場合
							map[i].capsel_m_p += 3;
						}
					}
				}
			}
		}
	}

	j = set_down_flg(map);	// 落下フラグの再設定

	// 落下終了?
	if(j == 0) {
		if(dm_h_erase_chack(map) != cap_flg_off || dm_w_erase_chack(map) != cap_flg_off) {
			if(!state->flg_game_over) {	// リタイアしていない
				state->mode_now = dm_mode_erase_chack;	// 消滅判定
			}
			else if(state->flg_game_over) {	// リタイアしている(練習中)
				state->mode_now = dm_mode_tr_erase_chack;	// 消滅判定
			}
			state->cap_speed_count = 0;
		}
		else {
			if(!state->flg_game_over) {	// リタイアしていない
				state->mode_now = dm_mode_cap_set;	// カプセルセット
			}
			else if(state->flg_game_over) {	// リタイアしている(練習中)
				state->mode_now = dm_mode_tr_cap_set;	// カプセルセット
			}
		}
	}
}

//## マップ上消滅アニメーション関数
void erase_anime(game_map *map)
{
	int i;

	for(i = 0; i < 128; i++) {
		// 表示フラグが立っている
		if(!map[i].capsel_m_flg[cap_disp_flg]) {
			continue;
		}

		// 消滅アニメーション状態だった
		if(!map[i].capsel_m_flg[cap_condition_flg]) {
			continue;
		}

		// 消滅アニメーションの進行
		map[i].capsel_m_g++;

		// ウイルスの場合
		if(map[i].capsel_m_flg[cap_virus_flg] >= 0) {
			if(map[i].capsel_m_g > erase_virus_b) {
				// マップ情報の削除
				clear_map(map,map[i].pos_m_x,map[i].pos_m_y);
			}
		}
		// ウイルス以外の場合
		else {
			if(map[i].capsel_m_g > erase_cap_b) {
				// マップ情報の削除
				clear_map(map,map[i].pos_m_x,map[i].pos_m_y);
			}
		}
	}
}

//## 投げ状態カプセル左回転関数
static void throw_rotate_capsel(game_cap *cap)
{
	static const int rotate_table[] = { 1, 3, 4, 2 };
	static const int rotate_mtx[] = {
		capsel_l, capsel_u, capsel_r, capsel_d, capsel_l, capsel_u
	};
	int i, vec, save;

	// カプセルが縦になっていた場合
	if(cap->pos_x[0] == cap->pos_x[1]) {
		cap->pos_x[1]++;
		cap->pos_y[1]++;
		save = cap->capsel_p[0];
		cap->capsel_p[0] = cap->capsel_p[1];
		cap->capsel_p[1] = save;
		vec = 1;
	}
	// カプセルが横になっていた場合
	else {
		// 座標変更
		cap->pos_x[1]--;
		cap->pos_y[1]--;
		vec = -1;
	}

	// グラフィックの入れ替え
	for(i = 0;i < 2;i++) {
		save = rotate_table[cap->capsel_g[i]];
		save += vec;
		cap->capsel_g[i] = rotate_mtx[save];
	}
}

//## カプセル左右移動関数
void translate_capsel(game_map *map, game_state *state, int move_vec, int joy_no)
{
	int vec = 0;
	game_cap *cap;

	cap = &state->now_cap;

	// 落下していない || 非表示
	if(cap->pos_y[0] <= 0 || !cap->capsel_flg[cap_disp_flg]) {
		return;
	}

	if(move_vec == cap_turn_r) {
		// 右移動の場合
		if(cap->pos_x[0] == cap->pos_x[1]) {
			// カプセルが縦並びの場合
			if(cap->pos_x[0] < 7) {
				if(get_map_info(map,cap->pos_x[0] + 1,cap->pos_y[0]) != cap_flg_on) {
					if(cap->pos_y[1] == 0) {
						// 移動先に障害物が無い場合
						vec = 1;
					} else {
						if(get_map_info(map,cap->pos_x[0] + 1,cap->pos_y[1]) != cap_flg_on) {
							// 移動先に障害物が無い場合
							vec = 1;
						}
					}
				}
			}
		} else {
			// カプセルが横並びの場合
			if(cap->pos_x[1] < 7 && (get_map_info(map,cap->pos_x[1] + 1,cap->pos_y[0]) != cap_flg_on)) {
				// 移動先に障害物が無い場合
				vec = 1;
			}
		}
	} else if(move_vec == cap_turn_l) {
		// 左移動の場合
		if(cap->pos_x[0] == cap->pos_x[1]) {
			// カプセルが縦並びの場合
			if(cap->pos_x[0] > 0) {
				if(get_map_info(map,cap->pos_x[0] - 1,cap->pos_y[0]) != cap_flg_on) {
					if(cap->pos_y[1] == 0) {
						// 移動先に障害物が無い場合
						vec = -1;
					} else {
						if(get_map_info(map,cap->pos_x[0] - 1,cap->pos_y[1]) != cap_flg_on) {
							// 移動先に障害物が無い場合
							vec = -1;
						}
					}
				}
			}
		} else {
			if(cap->pos_x[0] > 0) {
				if(get_map_info(map,cap->pos_x[0] - 1,cap->pos_y[0]) != cap_flg_on) {
					// 移動方向に障害物が無い場合
					vec = -1;
				}
			}
		}
	}

	if(vec != 0) {
		dm_snd_play_in_game(SE_gLeftRight);	// 移動SE再生
		state->cap_move_se_flg = 0;
		cap->pos_x[0] += vec;
		cap->pos_x[1] += vec;
	} else {
		if(state->cap_move_se_flg == 0) {
			// 移動できなくても一回は鳴らす
			state->cap_move_se_flg = 1;
			dm_snd_play_in_game(SE_gLeftRight);	// 移動SE再生
		}
		if(move_vec == cap_turn_r) {
			joyCursorFastSet(R_JPAD, joy_no);
		} else if(move_vec == cap_turn_l) {
			joyCursorFastSet(L_JPAD, joy_no);
		}
	}
}

//## カプセル左右回転関数
void rotate_capsel(game_map *map, game_cap *cap, int move_vec)
{
	static unsigned int rotate_table[] = {1,3,4,2};
	static unsigned int rotate_mtx[] = {capsel_l,capsel_u,capsel_r,capsel_d,capsel_l,capsel_u};
	int vec = 0;
	unsigned int i, save;

	if(cap->pos_y[0] > 0)	// ＮＥＸＴの位置では回転させない
	{
		if(cap->capsel_flg[cap_disp_flg]) {	// 表示フラグ
			if(cap->pos_x[0] == cap->pos_x[1]) {	// カプセルが縦になっていた場合
				if(cap->pos_x[0] == 7 || (get_map_info(map,cap->pos_x[0] + 1,cap->pos_y[0]) == cap_flg_on)) {	// 右壁に張りついていたか横に障害物があった場合
					if(cap->pos_x[0] != 0 && (get_map_info(map,cap->pos_x[0] - 1,cap->pos_y[0]) != cap_flg_on)) {	// 左壁に張りついていないでずれたところに障害物が無かった場合
						cap->pos_x[0] --;
						vec = 1;
					}
				} else {
					cap->pos_x[1] ++;
					vec = 1;
				}
				if(vec != 0) {
					cap->pos_y[1] ++;
					if(move_vec == cap_turn_l) {
						// パレットの入れ替え
						save = cap->capsel_p[0];
						cap->capsel_p[0] = cap->capsel_p[1];
						cap->capsel_p[1] = save;
					}
				}
			} else {	// カプセルが横になっていた場合
				if(cap->pos_y[0] == 1) {	// 最上段だった場合
					cap->pos_x[1] --;
					vec = -1;
				} else {
					if(get_map_info(map,cap->pos_x[0],cap->pos_y[0] - 1) == cap_flg_on) {	// 回転軸の上に何かあった場合
						if(get_map_info(map,cap->pos_x[0] + 1,cap->pos_y[0] - 1) != cap_flg_on) {	// 移動先に何も無かった場合
							cap->pos_x[0] ++;
							vec = -1;
						}
					} else {	// 回転軸の上に何も無かった場合
						// 座標変更
						cap->pos_x[1] --;
						vec = -1;
					}
				}
				if(vec != 0) {
					cap->pos_y[1] --;
					// パレットの入れ替え
					if(move_vec == cap_turn_r) {
						save = cap->capsel_p[0];
						cap->capsel_p[0] = cap->capsel_p[1];
						cap->capsel_p[1] = save;
					}
				}
			}
			if(vec != 0) {
				dm_snd_play_in_game(SE_gCapRoll);	// 回転SE再生
				// グラフィックの入れ替え
				for(i = 0;i < 2;i++) {
					save = rotate_table[cap->capsel_g[i]];
					save += vec;
					cap->capsel_g[i] = rotate_mtx[save];
				}
			}
		}
	}
}

//## カプセルマガジン作成関数
static void dm_make_magazine()
{
	u8 old[2], now[2];
	int i;

	CapsMagazine[0] = random(0xffff) % MAGA_MAX;
	old[0] = (CapsMagazine[0] >> 4) % 3;
	old[1] = (CapsMagazine[0] >> 0) % 3;
	i = 1;

	while(i < MAGA_MAX) {
		CapsMagazine[i] = random(0xffff) % MAGA_MAX;
		now[0] = (CapsMagazine[i] >> 4) % 3;
		now[1] = (CapsMagazine[i] >> 0) % 3;

		if(now[0] != old[0] || now[1] != old[1]) {
			old[0] = now[0];
			old[1] = now[1];
			i++;
		}
	}
}

//## カプセル初期化設定関数(落下フラグを立てない)
static void dm_init_capsel(game_cap *cap, unsigned int left_cap_col, unsigned int right_cap_col)
{
	cap->pos_x[0] = 3;	// 左カプセルＸ座標
	cap->pos_x[1] = 4;	// 右カプセルＸ座標
	cap->pos_y[0] = cap->pos_y[1] = 0;	// カプセルＹ座標
	cap->capsel_g[0] = capsel_l;		// グラフィック番号
	cap->capsel_g[1] = capsel_r;		// グラフィック番号
	cap->capsel_p[0] = left_cap_col;	// 色（左）
	cap->capsel_p[1] = right_cap_col;	// 色（右）
	cap->capsel_flg[cap_disp_flg] = cap_flg_on;	// 表示フラグをＯＮに
	cap->capsel_flg[1] = cap->capsel_flg[2] = cap_flg_off;	// 今のところ未使用
}

//## カプセル初期化設定関数(落下フラグを立てる)
static void dm_init_capsel_go(game_cap *cap, int left_cap_col, int right_cap_col)
{
	dm_init_capsel(cap, left_cap_col, right_cap_col);
	cap->capsel_flg[cap_down_flg] = 1;
}

//## カプセル設定関数
void dm_set_capsel(game_state *state)
{
	state->cap_move_se_flg = 0;
	dm_init_capsel_go(&state->now_cap,(CapsMagazine[state->cap_magazine_cnt] >> 4) % 3,CapsMagazine[state->cap_magazine_cnt] % 3);
	state->cap_magazine_save = state->cap_magazine_cnt;	// 前のマガジン位置の保存
	state->cap_magazine_cnt ++;
	if(state->cap_magazine_cnt >= 0xfe)
		state->cap_magazine_cnt = 1;

	dm_init_capsel(&state->next_cap,(CapsMagazine[state->cap_magazine_cnt] >> 4) % 3,CapsMagazine[state->cap_magazine_cnt] % 3);
}

//## カプセル速度上昇設定関数
void dm_capsel_speed_up(game_state *state)
{
	static const u8 _speed[] = { 15, 20, 25 };

	state->cap_count ++;
	if(state->cap_count >= 10) {	// スピードアップ
		dm_snd_play_in_game(SE_gSpeedUp);	// スピードアップＳＥ再生
		state->cap_count = 0;
		state->cap_speed ++;
		if(state->cap_speed > GameSpeed[3]) {	// 速度限界
			state->cap_speed = GameSpeed[3];
		}
	}

	// 耐久モードでの速度制限
	if(evs_gamemode == GMD_TaiQ) {
		state->cap_speed = MIN(_speed[ state->game_level ], state->cap_speed);
	}
}

//## ゲームオーバー(積みあがり)判定関数
int dm_check_game_over(game_state *state, game_map *map)
{
	if(state->cnd_static == dm_cnd_game_over) {
		return cap_flg_on; // ゲームオーバー
	}
	else {
		return cap_flg_off; // ゲームオーバーじゃない
	}
}

//## フラッシュウィルスの残り数を数える
static int update_flash_virus_count(game_state *state, game_map *map, bool flag)
{
	int count = 0;
	int i, j;

	for(i = 0; i < state->flash_virus_count; i++) {
		j = state->flash_virus_pos[i][0] + state->flash_virus_pos[i][1] * GAME_MAP_W;

		// (非表示 || 消滅状態 || ウィルスではない) の場合
		if(!map[j].capsel_m_flg[cap_disp_flg]
		||  map[j].capsel_m_flg[cap_condition_flg]
		||  map[j].capsel_m_flg[cap_virus_flg] < 0)
		{
			if(flag) {
				state->flash_virus_pos[i][2] = -1;
			}
			continue;
		}

		count++;
	}

	return count;
}

// ウイルス数を更新
static int dm_update_virus_count(game_state *state, game_map *map, bool flag)
{
	switch(evs_gamemode) {
	case GMD_FLASH:
		state->virus_number = update_flash_virus_count(state, map, flag);
		break;
	default:
		state->virus_number = get_virus_count(map);
		break;
	}
	return state->virus_number;
}

//## ウイルス転送関数
static void dm_set_virus(game_state *state, game_map *map, virus_map *v_map, u8 *order)
{
	unsigned int x_pos, y_pos, p, s;
	int virus_max;

	if(state->cnd_now != dm_cnd_init) {
		return;
	}

	// ウイルス数の割り出し
	virus_max = dm_get_first_virus_count(evs_gamemode, state);

#if 0
	while(1) {
		s = random(virus_max);
		if(*(order + s) != 0xff) {
			p = *(order + s);
			*(order + s) = 0xff;
			break;
		}
	}
	state->virus_order_number++;
	set_virus(map,
		(v_map + p)->x_pos, (v_map + p)->y_pos, (v_map + p)->virus_type,
		virus_anime_table[(v_map + p)->virus_type][state->virus_anime]);
#else
	while(state->virus_order_number < virus_max && (order[state->virus_order_number] & 0x80)) {
		state->virus_order_number++;
	}
	if(state->virus_order_number < virus_max) {
		p = order[state->virus_order_number];
		order[state->virus_order_number] |= 0x80;

		set_virus(map,
			v_map[p].x_pos, v_map[p].y_pos, v_map[p].virus_type,
			virus_anime_table[v_map[p].virus_type][state->virus_anime]);

		// トレーニング時は暗い色に設定
		if(state->cnd_training == dm_cnd_training) {
			map[p].capsel_m_p += 3;
		}
	}
#endif

	if(state->virus_order_number >= virus_max) {
		// ウイルス出現終了
		state->cnd_now = dm_cnd_wait; // 待機状態へ
		state->mode_now = dm_mode_wait; // 待機状態へ
	}

	dm_update_virus_count(state, map, false);
}

//## ウイルスアニメ関数
void dm_virus_anime(game_state *state, game_map *map)
{
	int i;

	// カウントの増加
	state->virus_anime_count++;

	// アニメーション間隔を取った
	if(state->virus_anime_count <= state->virus_anime_spead) {
		return;
	}

	// カウントのクリア
	state->virus_anime_count = 0;

	// アニメーションの進行
	state->virus_anime += state->virus_anime_vec;

	// アニメーションの最大コマ数を越えた
	if(state->virus_anime > state->virus_anime_max) {
		// アニメーション進行方向の反転
		state->virus_anime_vec = -1;
	}
	// アニメーションの最小コマ数を越えた
	else if(state->virus_anime < 1) {
		// アニメーション進行方向の反転
		state->virus_anime_vec = 1;
	}

	// マップ上のウイルスをアニメーションさせる
	for(i = 0; i < 128; i++) {
		// 表示されているか？
		if(!map[i].capsel_m_flg[cap_disp_flg]) {
			continue;
		}

		// 消滅状態か？
		if(map[i].capsel_m_flg[cap_condition_flg]) {
			continue;
		}

		// ウイルスか？
		if(map[i].capsel_m_flg[cap_virus_flg] < 0) {
			continue;
		}

		// グラフィック変更
		map[i].capsel_m_g = virus_anime_table[ map[i].capsel_m_flg[cap_virus_flg] ][ state->virus_anime ];
	}
}

//## ウイルス(カプセル)消滅関数
void dm_capsel_erase_anime(game_state *state, game_map *map)
{
	int i;

	state->erase_anime_count++;

	// 第一段階
	if(state->erase_anime_count == dm_erace_speed_1) {
		erase_anime(map); // 消滅アニメーション進行
		state->erase_anime++;
	}
	// 第二段階
	else if(state->erase_anime_count >= dm_erace_speed_2) {
		erase_anime(map); // 消滅アニメーション進行
		set_down_flg(map);// 落下設定
		state->erase_anime = 0;
		state->mode_now = dm_mode_ball_down;

		// 消滅直後はすぐに落ちる
		state->erase_anime_count = dm_down_speed;
	}
}

//## 縦消し化処理関数
static void dm_make_erase_h_line(game_state *state, game_map *map, int start, int chain, int pos_x)
{
	int i, p;

	for(i = start; i < start + chain + 1; i++) {
		p = (i << 3) + pos_x;

		if(map[p].capsel_m_flg[cap_condition_flg] == cap_flg_on) {
			continue;
		}

		// 消滅状態にする
		map[p].capsel_m_flg[cap_condition_flg] = cap_flg_on;

		// カプセルの場合
		if(map[p].capsel_m_flg[cap_virus_flg] < 0) {
			map[p].capsel_m_g = erase_cap_a;
		}
		// ウイルスの場合
		else {
			map[p].capsel_m_g = erase_virus_a;
			state->erase_virus_count++;    // 消滅ウイルス数の増加
			state->chain_color[3] |= 0x08; // ウイルスを含む連鎖
			state->chain_color[3] |= 0x10 << map[p].capsel_m_flg[cap_col_flg];
		}
	}
}

//## 縦消し判定処理関数
int dm_h_erase_chack(game_map *map)
{
	int i, j, p, chain, chain_start, chain_col;

	for(i = 0;i < 8;i++) {
		chain = 0;
		chain_col = chain_start = -1;
		for(j = 0;j < 16;j++) {
			p = (j << 3) + i;
			if((map + p)->capsel_m_flg[cap_disp_flg]) {	// 何か表示されている
				if((map + p)->capsel_m_flg[cap_col_flg] != chain_col) {
					// 同じ色でなかった
					if(chain >= 3) {
						// ４個以上だった
						return cap_flg_on;
					}
					if(j > 12) {	// 消滅判定が出来ないため(４個以上無い)
						break;
					} else {
						chain_start = j;									// 連結開始位置の変更
						chain_col = (map + p)->capsel_m_flg[cap_col_flg];	// 色の入れ替え
						chain = 0;
					}
				} else if((map + p)->capsel_m_flg[cap_col_flg] == chain_col) {
					chain++;	// 連結追加
					if(j == 15) {	// 最下段に達した場合
						if(chain >= 3) {
							// ４個以上だった
							return cap_flg_on;
						}
					}
				}
			} else {
				if(chain >= 3) {
					// ４個以上だった
					return cap_flg_on;
				}
				if(j > 12) {	// 現在位置がした３段以内の場合中止
					break;
				} else {
					chain = 0;
					chain_col = chain_start = -1;
				}
			}
		}
	}
	return cap_flg_off;
}

//## 縦消し判定と消滅設定関数
void dm_h_erase_chack_set(game_state *state, game_map *map)
{
	int i, j, p, end, chain, chain_start, chain_col;

	for(i = 0; i < GAME_MAP_W; i++) {
		chain = 0;
		chain_col = chain_start = -1;

		for(j = end = 0; j < GAME_MAP_H; j++) {
			p = j * GAME_MAP_W + i;

			// 何か表示されている
			if((map + p)->capsel_m_flg[cap_disp_flg]) {

				// 同じ色でなかった
				if((map + p)->capsel_m_flg[cap_col_flg] != chain_col) {
					// ４個以上だった
					if(chain >= 3) {
						// 消滅状態にする
						dm_make_erase_h_line(state,map,chain_start,chain,i);

						if(state->chain_count == 0) {
							// １回目の消した色のビットを立てる
							state->chain_color[3] |= 1 << chain_col;
						}
						state->chain_color[chain_col]++; // 消滅色のカウント
						state->chain_line++;             // 消滅列数のカウント
					}

					// 消滅判定が出来ないため(４個以上無い)
					if(j > 12) {
						end = 1;
					}
					else {
						// 連結開始位置の変更
						chain_start = j;

						// 色の入れ替え
						chain_col = (map + p)->capsel_m_flg[cap_col_flg];

						chain = 0;
					}
				}
				else if((map + p)->capsel_m_flg[cap_col_flg] == chain_col) {
					// 連結追加
					chain++;

					// 最下段に達した場合
					if(j == 15) {
						// ４個以上だった
						if(chain >= 3) {
							// 消滅状態にする
							dm_make_erase_h_line(state,map,chain_start,chain,i);

							if(state->chain_count == 0) {
								// １回目の消した色のビットを立てる
								state->chain_color[3] |= 1 << chain_col;
							}
							state->chain_color[chain_col]++; // 消滅色のカウント
							state->chain_line++;             // 消滅列数のカウント
						}
					}
				}
			}
			else {
				// ４個以上だった
				if(chain >= 3) {
					// 消滅状態にする
					dm_make_erase_h_line(state,map,chain_start,chain,i);

					if(state->chain_count == 0) {
						// １回目の消した色のビットを立てる
						state->chain_color[3] |= 1 << chain_col;
					}
					state->chain_color[chain_col]++; // 消滅色のカウント
					state->chain_line++;             // 消滅列数のカウント
				}

				// 消滅判定が出来ないため(４個以上無い)
				if(j > 12) {
					end = 1;
				}
				else {
					chain = 0;
					chain_col = chain_start = -1;
				}
			}

			if(end) {
				break;
			}
		}
	}
}

//## 横消し化処理関数
static void dm_make_erase_w_line(game_state *state, game_map *map, unsigned int start, unsigned int chain, unsigned int pos_y)
{
	int i, p;

	for(i = start; i < start + chain + 1; i++) {
		p = (pos_y << 3) + i;

		if(map[p].capsel_m_flg[cap_condition_flg] == cap_flg_on) {
			continue;
		}

		// 消滅状態にする
		map[p].capsel_m_flg[cap_condition_flg] = cap_flg_on;

		// カプセルの場合
		if(map[p].capsel_m_flg[cap_virus_flg] < 0) {
			map[p].capsel_m_g = erase_cap_a;
		}
		// ウイルスの場合
		else {
			map[p].capsel_m_g = erase_virus_a;
			state->erase_virus_count++;    // 消滅ウイルス数の増加
			state->chain_color[3] |= 0x08; // ウイルスを含む連鎖
			state->chain_color[3] |= 0x10 << map[p].capsel_m_flg[cap_col_flg];
		}
	}
}

//## 横消し判定処理関数
int dm_w_erase_chack(game_map *map)
{
	int i, j, p, chain, chain_start, chain_col;

	// 横消し
	for(j = 0;j < 16;j++) {
		chain = 0;
		chain_col = chain_start = -1;
		for(i = 0;i < 8;i++) {
			p = (j << 3) + i;
			if((map + p)->capsel_m_flg[cap_disp_flg]) {
				// 同じ色でなかった
				if((map + p)->capsel_m_flg[cap_col_flg] != chain_col) {
					if(chain >= 3) {
						// ４個以上だった
						return cap_flg_on;
					}
					if(i > 4) {
						break;
					} else {
						chain_start = i;									// 連結開始位置の変更
						chain_col = (map + p)->capsel_m_flg[cap_col_flg];	// 色の入れ替え
						chain = 0;											// 連鎖数クリア
					}
				} else if((map + p)->capsel_m_flg[cap_col_flg] == chain_col) {
					chain++;
					if(i == 7) {
						if(chain >= 3) {
							return cap_flg_on;
						}
					}
				}
			} else {
				// ４個以上だった
				if(chain >= 3) {
					// ４個以上だった
					return cap_flg_on;
				}
				if(i > 4) {
					break;
				} else {
					chain = 0;
					chain_col = chain_start = -1;
				}
			}
		}
	}
	return cap_flg_off;
}

//## 横消し判定と消滅設定関数
void dm_w_erase_chack_set(game_state *state, game_map *map)
{
	unsigned int p, end;
	int i, j, chain, chain_start, chain_col;

	// 横消し
	for(j = 0;j < 16;j++) {
		chain = 0;
		chain_col = chain_start = -1;

		for(i = end = 0;i < 8;i++) {
			p = (j << 3) + i;

			if((map + p)->capsel_m_flg[cap_disp_flg]) {

				// 同じ色でなかった
				if((map + p)->capsel_m_flg[cap_col_flg] != chain_col) {

					// ４個以上だった
					if(chain >= 3) {
						// 消滅状態にする
						dm_make_erase_w_line(state,map,chain_start,chain,j);

						if(state->chain_count == 0) {
							// １回目の消した色のビットを立てる
							state->chain_color[3] |= 1 << chain_col;
						}
						state->chain_color[chain_col]++; // 消滅色のカウント
						state->chain_line++;             // 消滅列数のカウント
					}

					if(i > 4) {
						end = 1;
					}
					else {
						// 連結開始位置の変更
						chain_start = i;

						// 色の入れ替え
						chain_col = (map + p)->capsel_m_flg[cap_col_flg];

						// 連鎖数クリア
						chain = 0;
					}
				}
				else if((map + p)->capsel_m_flg[cap_col_flg] == chain_col) {
					chain++;
					if(i == 7) {
						// ４個以上だった
						if(chain >= 3) {
							// 消滅状態にする
							dm_make_erase_w_line(state,map,chain_start,chain,j);

							if(state->chain_count == 0) {
								// １回目の消した色のビットを立てる
								state->chain_color[3] |= 1 << chain_col;
							}
							state->chain_color[chain_col]++; // 消滅色のカウント
							state->chain_line++;             // 消滅列数のカウント
						}
					}
				}
			}
			else {
				// ４個以上だった
				if(chain >= 3) {
					// 消滅状態にする
					dm_make_erase_w_line(state,map,chain_start,chain,j);

					if(state->chain_count == 0) {
						// １回目の消した色のビットを立てる
						state->chain_color[3] |= 1 << chain_col;
					}
					state->chain_color[chain_col]++; // 消滅色のカウント
					state->chain_line++;             // 消滅列数のカウント
				}

				if(i > 4) {
					end = 1;
				}
				else {
					chain = 0;
					chain_col = chain_start = -1;
				}
			}

			if(end) {
				break;
			}
		}
	}
}

//## 縦粒判定処理関数
void dm_h_ball_chack(game_map *map)
{
	unsigned int i, j, p, p2;

	for(i = 0;i < 8;i++) {
		for(j = 0;j < 16;j++) {
			p = (j << 3) + i;
			if((map + p)->capsel_m_flg[cap_disp_flg]) {
				// 表示されている
				if((map + p)->capsel_m_g == capsel_u) {
					// 上側のカプセルだったら
					p2 = ((j + 1) << 3) + i;
					if((map + p2)->capsel_m_g != capsel_d) {	// １段下を調べる
						(map + p)->capsel_m_g = capsel_b;
					}
				} else if((map + p)->capsel_m_g == capsel_d) {
					// 下側のカプセルだったら
					if((map + p)->pos_m_y == 1) {	// 最上段で下側のカプセルは
						// 強制的に粒にする
						(map + p)->capsel_m_g = capsel_b;
					} else {
						p2 = ((j - 1) << 3) + i;
						if((map + p2)->capsel_m_g != capsel_u) {	// １段上を調べる
							(map + p)->capsel_m_g = capsel_b;
						}
					}
				}
			}
		}
	}
}

//## 横粒判定処理関数
void dm_w_ball_chack(game_map *map)
{
	unsigned int i, j, p;

	for(j = 0;j < 16;j++) {
		for(i = 0;i < 8;i++) {
			p = (j << 3) + i;
			if((map + p)->capsel_m_flg[cap_disp_flg]) {
				// 表示されている
				if((map + p)->capsel_m_g == capsel_l) {
					// 左側のカプセルの場合
					if((map + p + 1)->capsel_m_g != capsel_r) {	// 一つ右を調べる
						(map + p)->capsel_m_g = capsel_b;
					}
				} else if((map + p)->capsel_m_g == capsel_r) {
					// 右側のカプセルの場合
					if((map + p - 1)->capsel_m_g != capsel_l) {	// 一つ左を調べる
						(map + p)->capsel_m_g = capsel_b;
					}
				}
			}
		}
	}
}

//## ゲームオーバーの時の暗くなる関数
int dm_black_up(game_state *state, game_map *map)
{
	u8	i,p;

	if(state->flg_retire && !state->flg_game_over) {
		// リタイアフラグが立っているが、ゲームオーバーフラグは立っていない状態

		state->erase_anime_count ++;
		if(state->erase_anime_count >= dm_black_up_speed) {	// 一定間隔を取る
			state->erase_anime_count = 0;

			p = (state->black_up_count - 1) << 3;
			for(i = 0;i < 8;i++) {
				if((map + p + i)->capsel_m_flg[cap_disp_flg]) {
					// カプセル(ウイルス)が表示されていたら
					(map + p + i)->capsel_m_p += 3;	// 暗い色に設定
				}
			}
			state->black_up_count --;
			if(state->black_up_count == 0) {	// 最上段まで処理し終わったら終了
				state->flg_game_over = 1;
				return cap_flg_on;	// 終了
			}
		}
	}
	return cap_flg_off;	// まだ作業中
}

//## やられ設定
int dm_broken_set(game_state *state, game_map *map)
{
	unsigned int i, j, chack;
	u16 work[DAMAGE_MAX][2];
	u8 work_b[DAMAGE_MAX][6];
	int ret = 0;

	// やられ判定
	if(state->cap_attack_work[0][0] != 0x0000) {
		// 被害あり

		if(state->chain_line_max < state->chain_line) {	// 最大消滅列数の保管
			state->chain_line_max = state->chain_line;
		}

		state->chain_line = 0;		// 消滅ライン数のリセット
		state->chain_count = 0;		// 連鎖数のリセット
		state->erase_virus_count = 0;	// 消滅ウイルス数のリセット
		state->erase_virus_count_old = 0;

		// 消滅色のリセット
		for(i = 0; i < 4; i++) {
			state->chain_color[i] = 0;
		}

		for(i = 0, j = 7; i < 16; i += 2, j--) {
			chack = state->cap_attack_work[0][0] & (0x0003 << i);
			if(chack != 0) {
				// カプセルセット
				set_map(map, j, 1, capsel_b, (chack >> i) - 1);
			}
		}

		// 粒化処理
		dm_h_ball_chack(map);
		dm_w_ball_chack(map);

		// 落下設定
		set_down_flg(map);

		// やられデータのシフト
		for(i = 0; i < DAMAGE_MAX; i++) {
			work[i][0] = state->cap_attack_work[i][0]; // データの保存
			work[i][1] = state->cap_attack_work[i][1]; // データの保存
			state->cap_attack_work[i][0] = 0x0000; // データクリア
			state->cap_attack_work[i][1] = 0x0000; // データクリア
		}
		for(i = 0, j = 1; i < DAMAGE_MAX - 1; i++, j++) {
			state->cap_attack_work[i][0] = work[j][0];
			state->cap_attack_work[i][1] = work[j][1];
		}

		// やられ
		ret = 1;
	}

	return ret;
}

//## ウィルス消去時の得点表示座標を求める
static void dm_calc_erase_score_pos(game_state *state, game_map *map, int pos[2])
{
	int count, x, y;

	// 座標地をクリア
	pos[0] = pos[1] = 0;

	count = 0;

	for(y = 0; y < GAME_MAP_H; y++) {
		for(x = 0; x < GAME_MAP_W; x++) {
			game_map *mp = &map[x + y * GAME_MAP_W];

			// 消滅アニメーション中でなければ中断
			if(!mp->capsel_m_flg[cap_condition_flg]) {
				continue;
			}

			pos[0] += mp->pos_m_x;
			pos[1] += mp->pos_m_y;
			count++;
		}
	}

	if(count > 0) {
		pos[0] = pos[0] * state->map_item_size / count;
		pos[1] = pos[1] * state->map_item_size / count;
	}
}

//## 操作カプセルの描画位置を求める
static int dm_calc_capsel_pos(game_state *state, int *xx, int *yy)
{
	game_cap *n_cap = &state->now_cap;
	float aa, bb, cc, dd, ee, ff, ii;
	int gg, hh;
	int i, j, k, m, n, q;
	int curve = 36;

	// 表示フラグが立っていなければ中断
	if(!n_cap->capsel_flg[cap_disp_flg]) {
		return FALSE;
	}

	// 投げ中
	if(state->mode_now == dm_mode_throw) {

		if(state->cap_def_speed == SPEED_LOW) {
			gg = (state->cap_speed_count >> 2);
		}
		else {
			gg = (state->cap_speed_count >> 1);
		}

		gg = gg % 4;
		gg = ONES_ooAB + gg;
		hh = 0;
		if(ONES_8_AB == gg || ONES_8_BA == gg) {
			hh = 5;
		}

		// もしカプセルが縦だった場合
		if(state->now_cap.pos_x[0] == state->now_cap.pos_x[1]) {
			if(gg == ONES_ooAB || gg == ONES_ooBA) {
				throw_rotate_capsel(&state->now_cap);
			}
		}
		// もしカプセルが横だった場合
		else {
			if(gg == ONES_8_AB || gg == ONES_8_BA) {
				throw_rotate_capsel(&state->now_cap);
			}
		}

		// 弧を描く
		if(state->cap_speed_count < (FlyingCnt[state->cap_def_speed] / 3)) {
			// 1/3 出だし
			ii = curve / (FlyingCnt[state->cap_def_speed] / 3) * state->cap_speed_count;
		}
		else if(state->cap_speed_count < (FlyingCnt[state->cap_def_speed] / 3 << 1)) {
			// 2/3 中間位置
			ii = curve;
		}
		else {
			ii = (FlyingCnt[state->cap_def_speed] << 1) / 3;
			ii = state->cap_speed_count - ii;
			ii = curve - (ii * 2);
		}

		// 着弾地点 x,y
		aa = (state->map_x + (state->map_item_size * n_cap->pos_x[0]));
		bb = (state->map_y + (state->map_item_size * (n_cap->pos_y[0] - 1)));

		// 開始地点 x,y
		cc = MARIO_THROW_X; // state->map_x + 101;
		dd = MARIO_THROW_Y; // state->map_y + 16;

		// 左上位置 x,y
		ee = aa+(((cc-aa)/(FlyingCnt[state->cap_def_speed]-1))*(FlyingCnt[state->cap_def_speed] - state->cap_speed_count));
		ff = bb+(((dd-bb)/(FlyingCnt[state->cap_def_speed]-1))*(FlyingCnt[state->cap_def_speed] - state->cap_speed_count));

		for(i = 0; i < 2; i++) {
			xx[i] = (u32)ee + hh + (state->map_item_size * (n_cap->pos_x[i] - 3));
			yy[i] = ((u32)ff + hh - (u16)ii) + 1 + (state->map_item_size * (n_cap->pos_y[i] + 1));
		}
	}
	// 落下待ち
	else if(n_cap->pos_y[0] < 1) {
		// カーソル位置の割り出し
		for(i = 0; i < 2; i++) {
			xx[i] = state->map_x + (state->map_item_size * n_cap->pos_x[i]);
			yy[i] = state->map_y + (state->map_item_size * n_cap->pos_y[i]) - 10;
		}
	}
	// 落下中
	else if(n_cap->pos_y[0] > 0) {
		// カーソル位置の割り出し
		for(i = 0; i < 2; i++) {
			xx[i] = state->map_x + (state->map_item_size * n_cap->pos_x[i]);
			yy[i] = state->map_y + (state->map_item_size * n_cap->pos_y[i]) + 1;
		}
	}
	// 描画せずに終了
	else {
		return FALSE;
	}

	return TRUE;
}

//## 操作カプセルを描画
static void dm_draw_capsel_by_gfx(game_state *state, int *xx, int *yy)
{
	game_cap *cap = &state->now_cap;
	STexInfo *tex, *pal;
	int i, type;

	gSPDisplayList(gp++, normal_texture_init_dl);

	// カプセルの大きさを判別
	if(state->map_item_size == cap_size_10) {
		type = 0;
	}
	else {
		type = 1;
	}

	// カプセルのテクスチャを読み込む
	tex = dm_game_get_capsel_tex(type);
	load_TexBlock_4b(tex->addr[1], tex->size[0], tex->size[1]);

	for(i = 0; i < 2; i++) {

		// カプセルのパレットを読み込む
		pal = dm_game_get_capsel_pal(type, cap->capsel_p[i]);
		load_TexPal(pal->addr[0]);

		// カプセルを描画
		draw_Tex(xx[i], yy[i], state->map_item_size, state->map_item_size,
			0, cap->capsel_g[i] * state->map_item_size);
	}
}

//## 操作カプセルをCPUで描画
static void dm_draw_capsel_by_cpu(game_state *state, int *xx, int *yy)
{
	game_cap *cap = &state->now_cap;
	STexInfo *inf;
	int i, x, y, type, bits, texel;
	int texStep, cfbStep;
	u16 *pal, *cfb;
	u8 *tex;

	// カプセルの大きさを判別
	if(state->map_item_size == cap_size_10) {
		type = 0;
	}
	else {
		type = 1;
	}

	for(i = 0; i < 2; i++) {

		// 画面端からはみ出る場合は中断
		if(yy[i] < 0 || yy[i] + state->map_item_size > SCREEN_HT) {
			continue;
		}
		if(xx[i] < 0 || xx[i] + state->map_item_size > SCREEN_WD) {
			continue;
		}

		// パレットのアドレス取得
		inf = dm_game_get_capsel_pal(type, cap->capsel_p[i]);
		pal = (u16 *)inf->addr[0];

		// テクセルのアドレス取得
		inf = dm_game_get_capsel_tex(type);
		tex = (u8 *)inf->addr[1];
		tex += (cap->capsel_g[i] * state->map_item_size * inf->size[0]) >> 1;
		texStep = (inf->size[0] - state->map_item_size) >> 1;

		// フレームバッファのアドレスを取得
		cfb = &fbuffer[wb_flag ^ 1][yy[i] * SCREEN_WD + xx[i]];
		cfbStep = SCREEN_WD - state->map_item_size;

		// 縦ループ
		for(y = 0; y < state->map_item_size; y++) {

			// 横ループ
			for(x = 0; x < state->map_item_size; x += 2) {

				// テクスチャデータより１バイト取得
				bits = *tex;

				// 偶数ドットを描画
				if(texel = bits >> 4) {
					cfb[0] = pal[texel];
				}

				// 奇数ドットを描画
				if(texel = bits & 0x0f) {
					cfb[1] = pal[texel];
				}

				tex++;
				cfb += 2;
			}

			tex += texStep;
			cfb += cfbStep;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//{### ウィルス増加判定
#if 0
/*
typedef struct {
	int	level;		// ゲームレベル 0:easy, 1:normal, 2:hard
	int	cup_cnt;	// カプセルを落とすごとにカウントする
	int	virus;		// ウイルスを消した数をカウントする
	int	time;		// １フレーム単位の時間（チェック内でカウントしている）
	int	bottom_up;
} TAIKYU_PARAM;

//## 初期化
static void init_taikyu_param(TAIKYU_PARAM *st, int level)
{
	st->level = level;
	st->cup_cnt = 0;
	st->virus = 0;
	st->time = 0;
	st->bottom_up = FALSE;
}

//## 判定
//
//	ウイルスが増加する条件
//	前回増加したときより、
//		１．ウイルスがｎ個以上消えた（ easy:10   normal:7   hard:5 ）
//		２．時間がｎ秒経過した（ easy:40   normal:30   hard:20 ）
//		３．カプセルをｎ個落とした（ easy:20   normal:15   hard:10 ）
//
// return:
//		TRUE	ウイルスを一段アップ
static int	virus_add_check(TAIKYU_PARAM *st)
{
	static	int	virus_chk[] = { 10, 7 ,5 };
	static	int	time_chk[] = { 40*FRAME_PAR_SEC, 30*FRAME_PAR_SEC ,20*FRAME_PAR_SEC };
	static	int	cup_chk[] = { 20, 15 ,10 };

	if(evs_gamemode != GMD_TaiQ) {
		return FALSE;
	}

	if ( virus_chk[st->level] <= st->virus ) {
		st->cup_cnt = 0;
		st->virus = 0;
		st->time = 0;
		return st->bottom_up = TRUE;
	}

	if ( time_chk[st->level] <= st->time ) {
		st->cup_cnt = 0;
		st->virus = 0;
		st->time = 0;
		return st->bottom_up = TRUE;
	}

	if ( cup_chk[st->level] <= st->cup_cnt ) {
		st->cup_cnt = 0;
		st->virus = 0;
		st->time = 0;
		return st->bottom_up = TRUE;
	}

	st->time++;
	return FALSE;
}
*/
#endif

//////////////////////////////////////////////////////////////////////////////
//{### 構造体

//## タイムアタックの結果表示
typedef struct {
	int level;
	bool bonus;
	int flow;
	int frame;
	int cnt;

	u32 time;
	int combo;
	int virus;
	u32 score, scoreDelta;
	u32 result;
} _TimeAttackResult;

//## スコアの一桁
typedef struct {
	int pos[2];
	int number;
	int color;
	float time;
} _ScoreNum;

//## スコア
typedef struct {
	_ScoreNum numbers[8];
	int index;
} _ScoreNums;

//## 星構造体
typedef struct {
	int *xtbl, *ytbl;
	int frame[16];
} _StarForce;

typedef struct {
	// リプレイ再生中フラグ
	int replayFlag;

	// ランダムのシード値
	int randSeed;
	int randSeed2;




	// スター描画位置の格納場所
	int star_pos_x[12];
	int star_pos_y[12];
	int star_count;
	_StarForce starForce;

	// スコアエフェクト
	_ScoreNums scoreNums[4];

	int retry_type[4];   // リトライメニューの種類
	int retry_select[4]; // リトライのカーソル位置
	int retry_result[4]; // リトライの選択結果
	int retry_coin;      // リトライ用のコインの枚数

	// 強制的に操作カプセルを描画するカウンタ
	int force_draw_capsel_count[4];

	// カーテンを上下させるためのカウンタ
	int curtain_count;
	int curtain_step;

	// 瓶、パネルを移動させるときのカウンタ
	#define FRAME_MOVE_MAX 20
	int frame_move_count;
	int frame_move_step;

	// 勝敗時の顔アニメーションカウンタ
	int face_anime_count[4];

	// カウントダウン処理の制御
	int count_down_ctrl;

	int eep_rom_flg;    // EEP 書き込み用フラグ
	int bgm_bpm_flg[2]; // BGMの速さを変えるフラグ
	int last_3_se_cnt;  // 
	int last_3_se_flg;  // ウイルスが３個以下になったときにSEを鳴らすためのフラグ
	int started_game_flg; // ゲームが開始されたことを示すフラグ

	// 巨大ウィルス
	float big_virus_wait;
	float big_virus_timer;
	float big_virus_pos[3][2]; // 巨大ウイルスの座標 ( x, y, degree )
	float big_virus_rot[3];
	float big_virus_scale[3];  // スケール
	int   big_virus_flg[3];    // 消滅アニメーションを一回だけ設定するフラグ
	bool  big_virus_no_wait;   // ノーウェイトフラグ
	int   big_virus_stop_count;// 停止カウント
	int   big_virus_blink_count;// 点滅カウント
	u8    big_virus_count[3];  // ウイルス数の各色別の個数の変数

	// デモのタイマー, デモフラグ
	int demo_timer;
	int demo_flag;

	// 表示物点滅処理カウンタ
	int blink_count;

	// カプセル積み上がり時の SE 再生カウンタ
	int warning_se_count;

	// オブジェクトDL
	void *objSeg;

	// 各種テクスチャ
	STexInfo *texAL; // 共通
	STexInfo *texLS; // レベルセレクト
	STexInfo *texP1; // 一人用
	STexInfo *texP2; // 二人用
	STexInfo *texP4; // 四人用
	STexInfo *texItem; // カプセル
	STexInfo *texKaSa; // カサマル

	// ウィルスキャラアニメーション
	SAnimeState virus_anime_state[3];

	// ウィルス消滅時の煙アニメーション
	SAnimeSmog virus_smog_state[3];

	// グラフィックスレッドのプライオリティ
	int graphic_thread_pri;

	// 初期化を行ったときに、1ｲﾝﾄ 背景のみの描画を行うためのフラグ
	u8  *bg_snap_buf;
	bool bg_snapping;

	// エフェクトデータのアドレス
	u8 *effect_data_buf;
	int effect_timer[4];

	int touch_down_wait; // ｶｰｿﾙｶﾌﾟｾﾙの下に何かあるときのｳｴｲﾄを加算

	int win_count[4];    // 勝利数カウンタ
	int vs_win_count[2]; // 2PLAY & VSCOM 勝利数カウンタ(３勝で１増える)
	int vs_win_total[2]; // 2PLAY & VSCOM 勝利数カウンタ(表示用)

	int vs_4p_player_count; // プレイヤーの参加数
	int vs_4p_team_flg;     // チーム対抗戦のフラグ
	int vs_4p_team_bits[2]; // チーム対抗戦の判定用フラグ

	int story_4p_name_num[4];     // ストーリーの 4P 時の チームのメンバー数
	int story_4p_stock_cap[4][4]; // ストーリーの 4P 時の ストックカプセル用配列(何も無いときは -1 が入る)

	// スタッフロール用
	SMsgWnd msgWnd;

	// コイン
	int coin_count;
	int coin_time[COINMAX];

	// コーヒーブレーク制御
	int coffee_break_flow;
	int coffee_break_timer;
	int coffee_break_level;
	int coffee_break_shard;

	// 耐久モード時、一段シフトするフラグ
	bool bottom_up_flag;

	// ポーズ、デバッグ、コンフィグ 要求者
	int query_play_pause_se;
	int query_pause_player_no;
	int query_debug_player_no;
	int query_config_player_no;

	// タイムアタックの結果表示
	_TimeAttackResult timeAttackResult[2];

	// パスワード
	SMsgWnd passWnd;
	int passAlphaStep;
	#define PASSWORD_LENGTH 20
	u8 passBuf[PASSWORD_LENGTH * 2 + 2];

	// EEPROM書き込み中の警告メッセージ
	RecWritingMsg writingMsg;
} Game;

typedef struct {
	Game game;
	game_state state[4];
	game_map map[4][GAME_MAP_W * (GAME_MAP_H + 1)];
	u32 highScore;
	u32 timer;
} GameBackup;

typedef struct {
	Mtx mtxBuf[GFX_GTASK_NUM][32];
	Vtx vtxBuf[GFX_GTASK_NUM][128];
} Geometry;

//////////////////////////////////////////////////////////////////////////////
//{### 変数

// ヒープトップ
static void *heapTop;

// ウォッチ
Game *watchGame;

// リプレイ時のバックアップバッファ
GameBackup *gameBackup[2];

// ジオメトリー
Geometry *gameGeom;

// 思考開始フラグ
int dm_think_flg = 0;

// 落下地点にカプセルを表示するフラグ
static int visible_fall_point[4] = { 0,0,0,0 };

#ifdef NN_SC_PERF
static int _disp_meter_flag = 0;
#endif

// 瓶の中の情報
game_map game_map_data[4][GAME_MAP_W * (GAME_MAP_H + 1)];

// ウイルス設定用配列
virus_map virus_map_data[4][GAME_MAP_W * GAME_MAP_H];

// 各プレイヤーの状態及び制御
game_state game_state_data[4];

// ウイルス設定順用配列
u8 virus_map_disp_order[4][96];

// ポーズ時、ゲーム終了時のパネルの項目数
#define PAUSE_NEX_END        0
#define PAUSE_NEX_TRY_END    1
#define CONTINUE_REP_END     2
#define CONTINUE_REP_TRY_END 3
#define CONTINUE_REP_NEX     4
#define CONTINUE_REP_NEX_END 5
static const u8 _retryMenu_itemCount[] = {
	2, // ポーズ時     [続ける] [終わる]
	3, //              [続ける] [もう一度] [終わる]
	2, // ゲーム終了時 [リプレイ] [終わる]
	3, //              [リプレイ] [もう一度] [終わる]
	2, //              [リプレイ] [続ける]
	3, //              [リプレイ] [続ける] [終わる]
};

// 耐久モード : ウィルス回転ウェイトの範囲
static const float _big_virus_def_wait[] = { 12.5, 10, 7.5 };
static const float _big_virus_min_wait[] = { 5, 4.5, 4 };
static const float _big_virus_max_wait[] = { 12.5, 10, 7.5 };

// スコアの色
static const u8 _scoreNumsColor[SCORE_MAX][3] = {
	{ 255, 255, 255 },
	{ 255, 255, 255 },
	{ 255, 255, 255 },
	{ 255, 255, 255 },
	{ 255, 255, 255 },
	{ 255, 255, 255 },
};

// 裏技
#if LOCAL==JAPAN
#include "tech.mes"
#elif LOCAL==AMERICA
#include "tech_am.mes"
#elif LOCAL==CHINA
#include "tech_zh.mes"
#endif

#if (LOCAL == CHINA)
#include "record_zh.mes"
#else
#include "record.mes"
#endif

//////////////////////////////////////////////////////////////////////////////
//{### 描画座標

//////////////////////////////////////
//## ストーリーモード

// スター (ストーリーモード)
#define _posStP4StarY 13
static const int _posStP4StarX[] = { 205, 230, 255, 280 };

// スター
static const int _posStStar[][2] = {
	{ 128, 72 }, { 172, 72 },
};

//////////////////////////////////////
//## レベルセレクトモード

// タイム
static const int _posLsTime[] = { 39, 95 };

//////////////////////////////////////
//## 二人用

// スター
static const int _posP2StarX[2] = { 128, 172 };
static const int _posP2StarY[][3] = {
	{ 72, -1, -1 }, { 80, 64, -1 }, { 88, 72, 56 },
};

//////////////////////////////////////
//## 四人用

// ボトル
static const int _posP4Bottle[][2] = {
	{ 16, 37 }, { 88, 37 }, { 160, 37 }, { 232, 37 },
};

// ボトルの中身 (ボトルからの相対座標)
static const int _posP4BottleIn[] = { 4, 3 };

// キャラパネル
static const int _posP4CharBase[][2] = {
	{ 16, 186 }, { 88, 186 }, { 160, 186 }, { 232, 186 },
};

// スター (団体戦)
#define _posP4TeamStarY 13
static const int _posP4TeamStarX[][2][3] = {
	{ {  45,  -1,  -1, },
	  { 191,  -1,  -1, } },
	{ {  34,  57,  -1, },
	  { 179, 202,  -1, } },
	{ {  22,  45,  68, },
	  { 167, 190, 214, } },
};

// スター (個人戦)
#define _posP4CharStarY 13
static const int _posP4CharStarX[][4][3] = {
	{ {  41,  -1,  -1, },
	  { 113,  -1,  -1, },
	  { 185,  -1,  -1, },
	  { 257,  -1,  -1, } },
	{ {  30,  53,  -1, },
	  { 102, 125,  -1, },
	  { 174, 197,  -1, },
	  { 246, 269,  -1, } },
	{ {  18,  42,  65, },
	  {  90, 114, 137, },
	  { 162, 186, 209, },
	  { 234, 258, 281, } },
};

// ストックカプセル
static const int _posP4StockCap[][2] = {
	{ 95, 24 }, { 240, 24 },
};

//////////////////////////////////////
//## ストーリーモードと二人用で共通するの描画物位置データ

// ウィルス数
static const int _posP2VirusNum[][2] = { { 138, 210 }, { 182, 210 } };

// キャラクタの枠
static const int _posP2CharFrm[][2] = { { 138, 149 }, { 181, 149 } };

//////////////////////////////////////////////////////////////////////////////
//{### 外部公開用関数

// ヒープ
void **dm_game_heap_top()
{
	return &heapTop;
}

// ウィルスキャラアニメーション
SAnimeState *get_virus_anime_state(int index)
{
	Game *st = watchGame;
	return &st->virus_anime_state[index];
}

// ウィルス消滅時の煙アニメーション
SAnimeSmog *get_virus_smog_state(int index)
{
	Game *st = watchGame;
	return &st->virus_smog_state[index];
}

// マトリックスバッファ
Mtx *dm_get_mtx_buf()
{
	return gameGeom->mtxBuf[gfx_gtask_no];
}

// バーテックスバッファ
Vtx *dm_get_vtx_buf()
{
	return gameGeom->vtxBuf[gfx_gtask_no];
}

//////////////////////////////////////////////////////////////////////////////
//{### エフェクト

#define EFFECT_WIN_WAIT  180
#define EFFECT_LOSE_WAIT 180
#define EFFECT_DRAW_WAIT 180
#define EFFECT_PAUSE_WAIT 180
#define EFFECT_GAMEOVER_WAIT 180
#define EFFECT_NEXTSTAGE_WAIT 180
#define EFFECT_RETIRE_WAIT 180

//## エフェクトを初期化
static void effectAll_init()
{
	Game *st = watchGame;
	int i;

	for(i = 0; i < evs_playcnt; i++) {
		st->effect_timer[i] = 0;
	}

	initEtcWork((u32)st->effect_data_buf, evs_playcnt);
}

//## エフェクトの初期化
static void _effectX_init(int playerNo, int timer)
{
	Game *st = watchGame;

	st->effect_timer[playerNo] = timer;
}

//## 勝利エフェクトの初期化
static void effectWin_init(int playerNo)
{
	_effectX_init(playerNo, EFFECT_WIN_WAIT);
}

//## 敗北エフェクトの初期化
static void effectLose_init(int playerNo)
{
	_effectX_init(playerNo, EFFECT_LOSE_WAIT);
}

//## 引分エフェクトの初期化
static void effectDraw_init(int playerNo)
{
	_effectX_init(playerNo, EFFECT_DRAW_WAIT);
}

//## ボーズエフェクトの初期化
static void effectPause_init(int playerNo)
{
	_effectX_init(playerNo, EFFECT_PAUSE_WAIT);
	init_pause_disp();
}

//## ゲームオーバーエフェクトの初期化
static void effectGameOver_init(int playerNo)
{
	_effectX_init(playerNo, EFFECT_GAMEOVER_WAIT);
}

//## ネクストステージエフェクトの初期化
static void effectNextStage_init(int playerNo)
{
	_effectX_init(playerNo, EFFECT_NEXTSTAGE_WAIT);
}

//## リタイアエフェクトの初期化
static void effectRetire_init(int playerNo)
{
	_effectX_init(playerNo, EFFECT_RETIRE_WAIT);
}

//## エフェクトの計算処理
static bool _effectX_calc(int playerNo)
{
	Game *st = watchGame;

	if(st->effect_timer[playerNo]) {
		st->effect_timer[playerNo]--;
	}

	return st->effect_timer[playerNo] != 0;
}

//## 勝利エフェクトの計算処理
static bool effectWin_calc(int playerNo)
{
	return _effectX_calc(playerNo);
}

//## 敗北エフェクトの計算処理
static bool effectLose_calc(int playerNo)
{
	return _effectX_calc(playerNo);
}

//## 引分エフェクトの計算処理
static bool effectDraw_calc(int playerNo)
{
	return _effectX_calc(playerNo);
}

//## ポーズエフェクトの計算処理
static bool effectPause_calc(int playerNo)
{
	return _effectX_calc(playerNo);
}

//## ゲームオーバーエフェクトの計算処理
static bool effectGameOver_calc(int playerNo)
{
	return _effectX_calc(playerNo);
}

//## ネクストステージエフェクトの計算処理
static bool effectNextStage_calc(int playerNo)
{
	return _effectX_calc(playerNo);
}

//## リタイアエフェクトの計算処理
static bool effectRetire_calc(int playerNo)
{
	return _effectX_calc(playerNo);
}

//////////////////////////////////////////////////////////////////////////////
//{### ルーチン

//## タイムアタックの結果表示 : 残り時間、連鎖数、消したウィルス、スコア を設定
static void timeAttackResult_set(_TimeAttackResult *st,
	int level, bool bonus,
	u32 time, int combo, int virus, u32 score)
{
	// ゲームレベル
	st->level = level;

	// ボーナスの有無
	st->bonus = bonus;

	// 処理の流れ
	st->flow = 0;

	// フレームカウンタ
	st->frame = 0;

	// 残り時間、連鎖数、消したウィルス
	st->time  = time;
	st->combo = combo;
	st->virus = virus;

	// スコア
	st->score = score;
	st->scoreDelta = 0;
}

//## タイムアタックの結果表示 : 初期化
static void timeAttackResult_init(_TimeAttackResult *st)
{
	timeAttackResult_set(st, 0, false, 0, 0, 0, 0);
	st->result = 0;
}

//## タイムアタックの結果表示 : 終了確認
static bool timeAttackResult_isEnd(_TimeAttackResult *st)
{
	return st->flow == 99;
}

//## タイムアタックの結果表示 : 更新
static bool timeAttackResult_update(_TimeAttackResult *st, bool se)
{
	bool res = true;
	int i, j;

	switch(st->flow) {
	case 0:
		if(st->frame < 120) {
			st->frame++;
		}
		else {
			st->flow = 10;
			st->frame = 0;
			st->cnt = 0;
		}
		break;

	case 10:
//		if(se && (st->frame & 7) == 0) {
//			dm_snd_play_in_game(SE_mLeftRight);
//		}

		if(st->time == 0) {
			st->flow = 20;
			st->frame = 0;
		}
		else {
			static int _n[] = { 10, 20, 30 };
			i = MIN(FRAME_PAR_MSEC, st->time);

			st->time -= i;
			if(se) {
				if(st->time == 0) {
					dm_snd_play_in_game(SE_gCoin);
				} else {
					if ( st->cnt % 10 == 0 )
						dm_snd_play_in_game(SE_mLeftRight);
				}
			}

			j = _n[st->level];
			if ( st->cnt >= 200 ) {
				j *= 3;
			} else if ( st->cnt >= 100 ) {
				j *= 2;
			}
			if(st->bonus) {
				st->score = MIN(SCORE_MAX_VAL, st->score + j);
				st->scoreDelta += j;
			}
			st->cnt++;
		}

		st->frame++;
		break;

	case 20:
		if(st->frame < 60) {
			st->frame++;
		}
		else {
			st->flow = 99;
			st->frame = 0;
		}
		break;

	case 99:
		res = !timeAttackResult_isEnd(st);
		break;
	}

	return res;
}

//## タイムアタックの結果表示 : スキップ
static void timeAttackResult_skip(_TimeAttackResult *st)
{
	while(timeAttackResult_update(st, false));
}

//## タイムアタックの結果表示 : 値を設定 & 結果を取得
static u32 timeAttackResult_result(_TimeAttackResult *st,
	int level, bool bonus,
	u32 time, int combo, int virus, u32 score)
{
	timeAttackResult_set(st, level, bonus, time, combo, virus, score);

	timeAttackResult_skip(st);
	st->result = st->score;

	timeAttackResult_set(st, level, bonus, time, combo, virus, score);

	return st->result;
}

//## スコア表示 : 初期化
static void scoreNums_init(_ScoreNums *st)
{
	_ScoreNum *num;
	int i;

	for(i = 0; i < ARRAY_SIZE(st->numbers); i++) {
		num = &st->numbers[i];
		num->pos[0] = 0;
		num->pos[1] = 0;
		num->number = 0;
		num->color = 0;
		num->time = 1;
	}

	st->index = 0;
}

//## スコア表示 : 更新
static void scoreNums_update(_ScoreNums *st)
{
	_ScoreNum *num, *pre;
	float speed = 1.0 / 40.0;
	int i, j;

	for(i = 0, j = st->index; i < ARRAY_SIZE(st->numbers); i++) {
		num = &st->numbers[j];
		pre = &st->numbers[WrapI(0, ARRAY_SIZE(st->numbers), j - 1)];

		if(pre->time > 0.20) {
			num->time = MIN(1, num->time + speed);
		}

		j = (j + 1) % ARRAY_SIZE(st->numbers);
	}
}

//## スコア表示 : 設定
static void scoreNums_set(_ScoreNums *st, u32 score, int erase, int x, int y)
{
	int i, color, columns, column[16];

	color = CLAMP(0, SCORE_MAX - 1, erase - 1);
	columns = 0;

	while(score != 0) {
		column[columns] = score % 10;
		score /= 10;
		columns++;
	}

	x -= (7 * columns + 1) / 2;
	y -= 6;

	for(i = columns - 1; i >= 0; i--) {
		_ScoreNum *num = &st->numbers[st->index];

		num->pos[0] = x;
		num->pos[1] = y;
		num->number = column[i];
		num->color = color;
		num->time = 0;

		x += 7;
		st->index = WrapI(0, ARRAY_SIZE(st->numbers), st->index + 1);
	}
}

//## ゲームステータスをバックアップ
static void backup_game_state(int bufNo)
{
	Game *st = watchGame;
	GameBackup *bak = gameBackup[bufNo];
	int i, j;

	// Game 構造体
	bak->game = *st;

	// プレイヤー毎のステータス
	assert(sizeof(bak->state) == sizeof(game_state_data));
	for(i = 0; i < ARRAY_SIZE(bak->state); i++) {
		bak->state[i] = game_state_data[i];
	}

	// マップ情報
	assert(sizeof(bak->map) == sizeof(game_map_data));
	for(i = 0; i < ARRAY_SIZE(bak->map); i++) {
		for(j = 0; j < ARRAY_SIZE(bak->map[i]); j++) {
			bak->map[i][j] = game_map_data[i][j];
		}
	}

	// ハイスコア
	bak->highScore = evs_high_score;

	// プレイ時間
	bak->timer = evs_game_time;
}

//## ゲームステータスを修復
static void resume_game_state(int bufNo)
{
	Game *st = watchGame;
	GameBackup *bak = gameBackup[bufNo];
	int i, j;

	// Game 構造体
	*st = bak->game;

	// プレイヤー毎のステータス
	for(i = 0; i < ARRAY_SIZE(bak->state); i++) {
		game_state_data[i] = bak->state[i];
	}

	// マップ情報
	for(i = 0; i < ARRAY_SIZE(bak->map); i++) {
		for(j = 0; j < ARRAY_SIZE(bak->map[i]); j++) {
			game_map_data[i][j] = bak->map[i][j];
		}
	}

	// ハイスコア
	evs_high_score = bak->highScore;

	// プレイ時間
	evs_game_time = bak->timer;
}

//## リプレイ状態へ
static void set_replay_state()
{
	Game *st = watchGame;

	if(!st->replayFlag) {
		backup_game_state(1);
	}

	resume_game_state(0);
	st->replayFlag = 1;

//	replay_play_init();
}

//## リプレイ状態から帰還
static void reset_replay_state()
{
	Game *st = watchGame;

	if(st->replayFlag) {
		resume_game_state(1);
		st->replayFlag = 0;
	}

//	replay_record_init(evs_playcnt);
}

//## 録画・再生 を実行
static void start_replay_proc()
{
	Game *st = watchGame;

	if(st->replayFlag) {
		replay_play_init();
	}
	else {
		replay_record_init(evs_playcnt);
	}
}

//## 積み上げ警告音を鳴らす
void dm_warning_h_line_se()
{
	Game *st = watchGame;
	int i, warning;

	for(i = warning = 0; i < evs_playcnt; i++) {
		if(game_state_data[i].cnd_static == dm_cnd_wait
		&& game_state_data[i].warning_se_flag)
		{
			warning++;
		}
	}

	if(warning) {
		st->warning_se_count++;

		// 警告 SE 再生
		if(st->warning_se_count == 1) {
			dm_snd_play_in_game(SE_gWarning);
		}
		else if(st->warning_se_count >= FRAME_PAR_SEC * 5) {
			st->warning_se_count = 0;
		}
	}
	else {
		st->warning_se_count = 0;
	}
}

//## カウントダウンＳＥを再生
static void dm_play_count_down_se()
{
	Game *st = watchGame;
	int i, j;

	if(evs_gamemode != GMD_TIME_ATTACK) {
		return;
	}

	for(i = 0; i < evs_playcnt; i++) {
		if(game_state_data[i].cnd_static == dm_cnd_wait) {
			break;
		}
	}

	if(i == evs_playcnt) {
		return;
	}

	i = SCORE_ATTACK_TIME_LIMIT - evs_game_time;
	j = i / FRAME_PAR_SEC;
	i %= FRAME_PAR_SEC;

	if(i == 0 && j >= 1 && j <= 10) {
		dm_snd_play_in_game(SE_gCount1);
	}
}

//## 操作カプセル落下処理関数
void dm_capsel_down(game_state *state, game_map *map)
{
	static const int black_color[] = { 0, 3 };
	Game *st = watchGame;
	game_cap *cap;
	unsigned int i, j;

	cap = &state->now_cap;

	if(cap->pos_y[0] > 0) {	// 落下中の速度設定
		i  = FallSpeed[state->cap_speed];	// カプセル落下速度の設定
		if(cap->pos_y[0] >= 1 && cap->pos_y[0] <= 3) {
			i += BonusWait[cap->pos_y[0] - 1][state->cap_def_speed];	// ｶﾌﾟｾﾙの高さによるﾎﾞｰﾅｽｳｴｲﾄを加算
		}
		j = 0;
		if(cap->pos_y[0] > 0) {
			if(get_map_info(map,cap->pos_x[0],cap->pos_y[0] + 1)) {
				// カプセルの下にアイテムがあった場合
//				j = TouchDownWait[evs_playcnt-1];
				j = st->touch_down_wait;
			}
		}
		state->cap_speed_max = i + j;
	}
	else {	// 落下前の速度設定(一律30)
		state->cap_speed_max = 30;
	}

	// 落下カウンタの増加
	state->cap_speed_count += state->cap_speed_vec;

	// 一定時間過ぎた？
	if(state->cap_speed_count < state->cap_speed_max) {
		return;
	}

	// カウンタクリア
	state->cap_speed_count = 0;

	// 非表示の場合は処理を中断
	if(!cap->capsel_flg[cap_disp_flg]) {
		return;
	}

	// １段落下
	if(cap->pos_y[0] > 0) {	// 一段目は強制的に落とす

		// 落下カプセルとマップ情報の判定
		if(cap->pos_x[0] == cap->pos_x[1]) {	// 落下カプセルは縦か？
			// カプセルが縦の場合
			if(get_map_info(map,cap->pos_x[0],cap->pos_y[0] + 1)) {
				// カプセルの下にアイテムがあった場合

				cap->capsel_flg[cap_down_flg] = cap_flg_off;	// 落下フラグのクリア
			}
		}
		else {
			// カプセルが横の場合
			for(j = 0;j < 2;j++) {
				if(get_map_info(map,cap->pos_x[j],cap->pos_y[j] + 1)) {
					// カプセルの下にアイテムがあった場合
					cap->capsel_flg[cap_down_flg] = cap_flg_off;	// 落下フラグのクリア
					break;
				}
			}
		}
	}

	// 最下段判定
	for(i = 0;i < 2;i++) {
		if(cap->pos_y[i] == 16) {
			cap->capsel_flg[cap_down_flg] = cap_flg_off;	// 落下フラグのクリア
			break;
		}
	}

	// 落下処理
	if(cap->capsel_flg[cap_down_flg]) {	// 落下フラグが立っているか？
		// 落下する
		for(i = 0;i < 2;i++) {
			if(cap->pos_y[i] < 16) {	// 念のため
				cap->pos_y[i] ++;
			}
		}
		for(i = 0;i < 2;i++) {
			// 落下した場所にアイテムがある
			if(get_map_info(map,cap->pos_x[i], cap->pos_y[i])) {
				// ゲームオーバー設定
				state->cnd_static = dm_cnd_game_over;

				// ネクストカプセルの表示中止 // 落下フラグのクリア
				state->next_cap.capsel_flg[cap_disp_flg] = cap_flg_off;
				cap->capsel_flg[cap_down_flg] = cap_flg_off;
				break;
			}
		}
	}

	// 落下できない
	if(!cap->capsel_flg[cap_down_flg]) {
		dm_snd_play_in_game(SE_gTouchDown); // 着地ＳＥ再生
		state->mode_now = dm_mode_down_wait; // バウンド
		cap->capsel_flg[cap_disp_flg] = cap_flg_off; // 表示フラグのクリア

#if 0 && defined(DEBUG)
	if(PLAYER_IS_CPU(state, state->player_no) &&
		(cap->pos_x[0] != state->ai.aiEX-1 || cap->pos_y[0] != state->ai.aiEY))
	{
		PRTFL();
		osSyncPrintf("目的地に落下できなかった(player(%d), cap(%d,%d), ai(%d,%d)\n",
			state->player_no, cap->pos_x[0], cap->pos_y[0], state->ai.aiEX-1, state->ai.aiEY);
	}
#endif

		for(i = 0;i < 2;i++) {
			if(cap->pos_y[i] != 0) {
				// マップデータに変更
				set_map(map,cap->pos_x[i], cap->pos_y[i], cap->capsel_g[i],
					cap->capsel_p[i] + black_color[state->flg_game_over]);
			}
		}
	}
}

//## 攻撃位置を計算する関数
static int dm_make_attack_pattern(unsigned int max)
{
	Game *st = watchGame;
	int pattern = 0;

	// ４個の場合
	if(max >= 4) {
		pattern = 0x55 << random(2);
	}
	// ３個の場合
	else if(max >= 3) {
		pattern = 0x15 << random(4);
	}
	// ２個の場合
	else if(max >= 2) {
		pattern = 0x11 << random(4);
	}

	return pattern;
}

//## カプセルを落とす処理
static int dm_set_attack_2p(game_state *state)
{
	Game *st = watchGame;
	game_state *enemy;
	int i, x, chainCount, pattern;

	if(state->chain_line < 2) {
		return FALSE;
	}

	// 敵のステータスを取得
	enemy = &game_state_data[state->player_no ^ 1];

	// 連鎖数を取得
	chainCount = MIN(GAME_MAP_W / 2, state->chain_line);

	for(i = 0; i < 1; i++) { // <= 2pの場合は,ストックバッファは1個だけ
		// 落下待ちカプセルがあったら、その隙間を探す
		if(enemy->cap_attack_work[i][0] != 0) {
			int flag;

			pattern = 0;

			for(x = 0; x < GAME_MAP_W; x++) {
				if(enemy->cap_attack_work[i][0] & (3 << (x << 1))) {
					pattern |=  1 << x;
					flag = x & 1;
				}
			}

			for(x = 0; x < GAME_MAP_W; x++) {
				if((x & 1) == flag) {
					pattern ^= 1 << x;
				}
			}

			if(pattern == 0) {
				continue;
			}
		}
		// カプセルの落とす場所を取得
		else {
			pattern = dm_make_attack_pattern(chainCount);
		}

		enemy->cap_attack_work[i][1] = state->player_no;

		for(x = 0; x < GAME_MAP_W; x++) {
			int c;

			// 落とす場所でなければ中断
			if((pattern & (1 << x)) == 0) {
				continue;
			}

			// 落としてないカプセルを探す
			while(state->chain_color[0] ||
				state->chain_color[1] || state->chain_color[2])
			{
				c = random(3);

				if(state->chain_color[c]) {
					// カプセルを落とす
					state->chain_color[c]--;
					enemy->cap_attack_work[i][0] |= (c + 1) << (x << 1);
					break;
				}
			}
		}

		break;
	}

	return TRUE;
}

//## 攻撃設定 (4P用)
static int dm_set_attack_4p(game_state *state)
{
	static const char attack_table[][3] = {
		{ 3, 2, 1 }, { 0, 3, 2 }, { 1, 0, 3 }, { 2, 1, 0 },
	};
	Game *st = watchGame;
	game_state *enemy;
	int c, i, j, chainCount, work[3];
	int attackWork[3];
	int attackFlag, stockFlag;

	if(state->chain_line < 2) {
		return FALSE;
	}

	for(i = 0; i < ARRAY_SIZE(attackWork); i++) {
		attackWork[i] = 0;
	}
	attackFlag = stockFlag = 0;
	chainCount = 0;

	// 攻撃先、攻撃カプセル色、を求める
	for(i = 0; i < 3; i++) {

		// 連鎖時、最初に消した色によって攻撃相手を選ぶ
		if(!(state->chain_color[3] & (1 << i))) {
			continue;
		}

		// 敵のステータスを取得
		enemy = &game_state_data[attack_table[state->player_no][i]];

		// 同じチームの場合
		if(enemy->team_no == state->team_no) {
			// リタイアしていたら中断
			if(enemy->flg_retire) {
				continue;
			}
			stockFlag |= (1 << attack_table[state->player_no][i]);
		}
		else {
			// リタイアした && (!トレーニング中 || !攻撃ありトレーニング)
			if(enemy->flg_retire &&
				(enemy->cnd_training != dm_cnd_training || !enemy->flg_training))
			{
				continue;
			}
			attackFlag |= (1 << attack_table[state->player_no][i]);
		}

		// すでに攻撃カプセル色を求めた場合は中断
		if(chainCount > 0) {
			continue;
		}

		// 連鎖数を取得
		chainCount = MIN(GAME_MAP_W / 2, state->chain_line);

		// ストックを使用する
		for(j = 0; j < ARRAY_SIZE(st->story_4p_stock_cap[state->team_no]); j++) {

			// ストックを使用できるか?
			if(chainCount >= GAME_MAP_W / 2) {
				break;
			}

			// ストックがあった場合使用
			if(st->story_4p_stock_cap[state->team_no][j] == -1) {
				continue;
			}

			// ストック色の追加
			state->chain_color[ st->story_4p_stock_cap[state->team_no][j] ]++;

			// ストック色の消去
			st->story_4p_stock_cap[state->team_no][j] = -1;

			chainCount++;
		}

		// 攻撃カプセル色を決定
		j = 0;
		while(j < chainCount &&
			state->chain_color[0] + state->chain_color[1] + state->chain_color[2] > 0)
		{
			c = random(3);

			if(state->chain_color[c] > 0) {
				state->chain_color[c]--;
				attackWork[c]++;
				j++;
			}
		}
	}

	// ターゲットがすべてリタイアしていた
	if(attackFlag + stockFlag == 0) {
		return FALSE;
	}

	// 攻撃する
	for(i = 0; i < 4; i++) {
		int x, pattern;

		// 攻撃相手でなければ中断
		if((attackFlag & (1 << i)) == 0) {
			continue;
		}

		// 敵のステータスを取得
		enemy = &game_state_data[i];

		// 攻撃エフェクト
		add_attack_effect(state->player_no,
			_posP4CharBase[state->player_no][0], _posP4CharBase[state->player_no][1],
			_posP4CharBase[enemy->player_no][0], _posP4CharBase[enemy->player_no][1]);

		// 連鎖カプセル数を保存
		for(j = 0; j < ARRAY_SIZE(work); j++) {
			work[j] = attackWork[j];
		}

		for(j = 0; j < DAMAGE_MAX; j++) {

			// 空きが無ければ中断
			if(enemy->cap_attack_work[j][0] != 0x0000) {
				continue;
			}
			// カプセルの落とす場所を取得
			else {
				pattern = dm_make_attack_pattern(chainCount);
			}

			// 攻撃したプレイヤー番号
			enemy->cap_attack_work[j][1] = state->player_no;

			for(x = 0; x < GAME_MAP_W; x++) {
				if((pattern & (1 << x)) == 0) {
					continue;
				}

				// 座標決定
				while(work[0] + work[1] + work[2] > 0) {
					c = random(3);

					if(work[c] > 0) {
						work[c]--;
						enemy->cap_attack_work[j][0] |= (c + 1) << (x << 1);
						break;
					}
				}
			}
		}
	}

	// ストックのシフト
	for(i = j = 0; i < 4; i++) {
		if(st->story_4p_stock_cap[state->team_no][i] == -1) continue;
		st->story_4p_stock_cap[state->team_no][j] = st->story_4p_stock_cap[state->team_no][i];
		j++;
	}
	for(; j < 4; j++) {
		st->story_4p_stock_cap[state->team_no][j] = -1;
	}

	// ストック計算
	for(i = 0; i < 4; i++) {

		// ストック相手でなければ中断
		if((stockFlag & (1 << i)) == 0) {
			continue;
		}

		// 味方のステータスを取得
		enemy = &game_state_data[attack_table[state->player_no][i]];

		// 連鎖カプセル数を保存
		for(j = 0; j < ARRAY_SIZE(work); j++) {
			work[j] = attackWork[j];
		}

		for(j = c = 0; j < 4; j++) {
			// もしストックが在ったら中断
			if(st->story_4p_stock_cap[state->team_no][j] != -1) {
				continue;
			}

			for(; c < 3; c++) {
				if(work[c] > 0) {
					work[c]--;
					st->story_4p_stock_cap[state->team_no][j] = c;
					break;
				}
			}
		}
	}

	return TRUE;
}

//## プレイヤー毎のカプセル落下位置ON/OFF をゲームのワークに反映
static void load_visible_fall_point_flag()
{
	int i, j;

	switch(evs_gamesel) {
	case GSL_1PLAY:
	case GSL_VSCPU:
		j = 1;
		break;

	case GSL_2PLAY:
		j = 2;
		break;

	default:
		j = 0;
		break;
	}

	for(i = 0; i < j; i++) {
		mem_char *mc = &evs_mem_data[ evs_select_name_no[i] ];
		visible_fall_point[i] = (mc->mem_use_flg & DM_MEM_CAP) ? 1 : 0;
	}
}

//## 落下位置ON/OFF をセーブデータに反映
static void save_visible_fall_point_flag()
{
	int i, j;

	switch(evs_gamesel) {
	case GSL_1PLAY:
	case GSL_VSCPU:
		j = 1;
		break;

	case GSL_2PLAY:
		j = 2;
		break;

	default:
		j = 0;
		break;
	}

	for(i = 0; i < j; i++) {
		mem_char *mc = &evs_mem_data[ evs_select_name_no[i] ];
		mc->mem_use_flg &= ~DM_MEM_CAP;
		if(visible_fall_point[i]) mc->mem_use_flg |=  DM_MEM_CAP;
	}
}

//## リトライメニュー: カーソル位置などを初期化
static void retryMenu_init(int playerNo, int type)
{
	Game *st = watchGame;

	st->retry_type[playerNo] = type;
	st->retry_select[playerNo] = 0;
	st->retry_result[playerNo] = -1;

	switch(st->retry_type[playerNo]) {
	case CONTINUE_REP_END:
	case CONTINUE_REP_TRY_END:
	case CONTINUE_REP_NEX:
	case CONTINUE_REP_NEX_END:
		st->retry_select[playerNo]++;
		break;
	}
}

//## リトライメニュー: ポーズロゴを初期化
static void retryMenu_initPauseLogo(int playerNo)
{
	retryMenu_init(playerNo, PAUSE_NEX_END);
}

//## リトライメニュー: 入力処理
static int retryMenu_input(int playerNo)
{
	Game *st = watchGame;
	int rep = _getKeyRep(playerNo);
	int trg = _getKeyTrg(playerNo);
	int select, vy = 0;
	int sound = -1, result = -1;

	select = st->retry_select[playerNo] =
		WrapI(0, _retryMenu_itemCount[st->retry_type[playerNo]], st->retry_select[playerNo]);

	if(rep & DM_KEY_UP) vy--;
	if(rep & DM_KEY_DOWN) vy++;
	select =
		WrapI(0, _retryMenu_itemCount[st->retry_type[playerNo]], select + vy);

	if(select != st->retry_select[playerNo]) {
		st->retry_select[playerNo] = select;
		sound = SE_mUpDown;
	}
	else if(trg & DM_KEY_OK) {
		result = st->retry_result[playerNo];
	}

	if(sound >= 0) {
		dm_snd_play_in_game(sound);
	}

	return result;
}

//## リトライメニュー: 描画処理
static void retryMenu_drawPause(int playerNo, Gfx **gpp, bool dispMenu)
{
	Game *st = watchGame;
	Gfx *gp = *gpp;

	switch(st->retry_type[playerNo]) {
	case PAUSE_NEX_END:
	case PAUSE_NEX_TRY_END:
		st->retry_result[playerNo] = disp_pause_logo(&gp, playerNo, 0,
			dispMenu ? st->retry_select[playerNo] : -1, st->retry_type[playerNo]);
		break;
	}

	*gpp = gp;
}

//## リトライメニュー: 描画処理
static void retryMenu_drawContinue(int playerNo, Gfx **gpp)
{
	Game *st = watchGame;
	Gfx *gp = *gpp;

	switch(st->retry_type[playerNo]) {
	case CONTINUE_REP_END:
	case CONTINUE_REP_TRY_END:
	case CONTINUE_REP_NEX:
	case CONTINUE_REP_NEX_END:
		if(evs_gamesel == GSL_2PLAY && evs_gamemode == GMD_TIME_ATTACK) {
			st->retry_result[playerNo] = disp_continue_logo_score(&gp, playerNo,
				st->retry_select[playerNo], st->retry_type[playerNo] - 2);
		}
		else {
			st->retry_result[playerNo] = disp_continue_logo(&gp, playerNo,
				st->retry_select[playerNo], st->retry_type[playerNo] - 2);
		}
		break;
	}

	*gpp = gp;
}

//## 瓶の中身を一段上げる時、新たに湧き出るウィルスを設定
static void set_bottom_up_virus(game_state *state, game_map *map)
{
	int i, j, x, endI, color;
	u8	work[GAME_MAP_W];

	for( x = 0; x < GAME_MAP_W; x++ ) work[x] = 0;

	endI = GAME_MAP_W * GAME_MAP_H;
	i = endI - GAME_MAP_W;
	x = 0;

	// 一段上の列のカプセルの下には、必ずウイルスを配置
	for(; i < endI; i++, x++) {
		if(!map[i].capsel_m_flg[cap_disp_flg]) {
			// 非表示だった
			continue;
		}
		if(map[i].capsel_m_flg[cap_virus_flg] >= 0) {
			// ウィルスだった
			continue;
		}
		// ここにウイルスを置く
		work[x] = 1;
	}

	// 75%の確率で配置
	j = 0;
	for( x = 0; x < GAME_MAP_W; x++ ) {
		if ( random(100) < 75 ) work[x] = 1;
		if ( work[x] != 0 ) j++;
	}
	// もし１個も配置されなかったら、強制的に１個定義
	if ( j == 0 ) work[random(GAME_MAP_W)] = 1;

	endI += GAME_MAP_W;
	x = 0;

	// ウイルスを配置する
	for(; i < endI; i++, x++) {
		int mask[3], flag[3], pos[3];

		// 置かない
		if( work[x] == 0 ) continue;

		mask[0] = mask[1] = mask[2] = 0;
		flag[0] = 1;
		flag[1] = (x > 1);
//		flag[2] = (x < GAME_MAP_W - 2);
		pos[0] = i - GAME_MAP_W*2;
		pos[1] = i - 2;
//		pos[2] = i + 2;

		// 周囲の色
		for(j = 0; j < 2; j++) {
			if(!flag[j]) {
				// 枠外
				continue;
			}
			if(map[ pos[j] ].capsel_m_flg[cap_disp_flg]) {
				// 色を記録
				mask[ map[ pos[j] ].capsel_m_flg[cap_col_flg] ]++;
			}
		}

		// 三色とも使われている
		if( mask[0] && mask[1] && mask[2] ) continue;

		// 色を決定
		do {
			color = random(3);
		} while ( mask[color] );

		// ウィルスを設定
		set_virus(map, x, GAME_MAP_H + 1, color, virus_anime_table[color][state->virus_anime]);
	}
}

//## 瓶の中身を一段上げる & 天井を貫いたか判定
static bool bottom_up_bottle_items(game_map *map)
{
	int i, j, size;
	bool flow = false;

	for(i = 0; i < GAME_MAP_W; i++) {
		if(map[i].capsel_m_flg[cap_disp_flg]) {
			// 天井を貫いた
			flow = true;
		}
	}
	size = GAME_MAP_W * GAME_MAP_H;

	for(i = 0, j = GAME_MAP_W; i < size; i++, j++) {
		map[i] = map[j];
		map[i].pos_m_y--;
	}

	size += GAME_MAP_W;

	for(j = 0; i < size; i++, j++) {
		bzero(&map[i], sizeof(game_map));
		map[i].pos_m_x = j;
		map[i].pos_m_y = GAME_MAP_H + 1;
	}

	return flow;
}

//## 耐久モードの、連鎖時ボーナスウェイトを加算
static void add_taiQ_bonus_wait(game_state *state)
{
	static const int _bonus[] = {
		 30, // ウィルス、消したライン
		120, // 2連鎖
		150, // 3連鎖
		180, // 4連鎖
		210, // 5連鎖
		240, // 6連鎖以上
	};
	Game *st = watchGame;
	int i, count;

	if(evs_gamemode != GMD_TaiQ || state->chain_line < 2) {
		return;
	}

	// ボーナスウェイトを加算
	st->big_virus_wait += 0.25 * (state->chain_line - 1);
	st->big_virus_wait = MIN(st->big_virus_wait, _big_virus_max_wait[state->game_level]);

	count = 0;
	count += _bonus[0] * (state->erase_virus_count + state->chain_line);
	for(i = 1; i < state->chain_count; i++) {
		count += _bonus[ MIN(i, ARRAY_SIZE(_bonus) - 1) ];
	}

PRT1("停止時間 += %d\n", count);

	// ウィルスの回転停止時間を設定
	st->big_virus_stop_count += count;
}

//## 決着が付いたか判別
static bool dm_check_one_game_finish()
{
	int i;

	for(i = 0; i < evs_playcnt; i++) {
		DM_GAME_CONDITION cnd = game_state_data[i].cnd_static;
		if(cnd != dm_cnd_win && cnd != dm_cnd_lose && cnd != dm_cnd_draw) {
			break;
		}
	}

	return i == evs_playcnt;
}

//////////////////////////////////////////////////////////////////////////////
//{### セーブ

//## ゲーム用 EEP 書き込み関数コールバック
static void dm_game_eep_write_callback(void *args)
{
	Game *st = (Game *)args;

	// EEPROM書き込み中の警告メッセージ
	RecWritingMsg_setStr(&st->writingMsg, _mesWriting);
	RecWritingMsg_setPos(&st->writingMsg,
		(SCREEN_WD - msgWnd_getWidth(&st->writingMsg.msgWnd)) / 2,
		(SCREEN_HT - msgWnd_getHeight(&st->writingMsg.msgWnd)) - 32);
	RecWritingMsg_start(&st->writingMsg);

	// スリープタイマー
	setSleepTimer(500);
}

//## ゲーム用 EEP 書き込み関数
static void dm_game_eep_write(void *arg)
{
	Game *st = watchGame;
	EepRomErr err;

	if(st->eep_rom_flg) {
		err = EepRom_WriteAll(dm_game_eep_write_callback, st);
		st->eep_rom_flg = 0;

		PRTFL();
		EepRom_DumpErrMes(err);
	}
}

//## ストーリーモードでの勝利時、次の面に進むかどうかを判別
static bool dm_query_next_stage(int level, int stage, int retry)
{
	bool next = false;

	if(stage < LAST_STAGE_NO) {
		next = true;
	}
	else if(stage == LAST_STAGE_NO) {
		if(level > 2 || (level > 0 && retry == 0)) {
			next = true;
		}
	}

	return next;
}

//## セーブ＿オール
static void dm_save_all()
{
	Game *st = watchGame;
	game_state *state = &game_state_data[0];
	int i;

	if(st->replayFlag || st->eep_rom_flg) {
		return;
	}

	switch(evs_gamesel) {
	case GSL_4PLAY: {
		int stage;

		// ストーリーモード
		if(!evs_story_flg) break;

		stage = evs_story_no;
		if(state->cnd_static == dm_cnd_win) {
			stage++;
		}

		// セーブデータセット
		dm_story_sort_set(evs_select_name_no[0],
			(story_proc_no >= STORY_W_OPEN ? 1 : 0),
			evs_story_level, state->game_score,
			evs_game_time, stage, evs_one_game_flg);
		st->eep_rom_flg = 1;
		} break;

	case GSL_2PLAY:
		// 記録の保存
		for(i = 0; i < 2; i++) {
			switch(evs_gamemode) {
			case GMD_NORMAL:
				dm_vsman_set(evs_select_name_no[i], st->vs_win_count[i], st->vs_win_count[i ^ 1]);
				break;
			case GMD_FLASH:
				dm_vm_fl_set(evs_select_name_no[i], st->vs_win_count[i], st->vs_win_count[i ^ 1]);
				break;
			case GMD_TIME_ATTACK:
				dm_vm_ta_set(evs_select_name_no[i], st->vs_win_count[i], st->vs_win_count[i ^ 1]);
				break;
			}
		}
		st->eep_rom_flg = 1;
		st->vs_win_count[0] = st->vs_win_count[1] = 0;
		break;

	case GSL_VSCPU:
		if(!evs_story_flg) {
			// 記録の保存
			switch(evs_gamemode) {
			case GMD_NORMAL:
				dm_vscom_set(evs_select_name_no[0], st->vs_win_count[0], st->vs_win_count[1]);
				break;
			case GMD_FLASH:
				dm_vc_fl_set(evs_select_name_no[0], st->vs_win_count[0], st->vs_win_count[1]);
				break;
			}
			st->eep_rom_flg = 1;
			st->vs_win_count[0] = st->vs_win_count[1] = 0;
		}
		// ストーリーモード
		else {
			mem_char *mc = &evs_mem_data[evs_select_name_no[0]];
			game_config *cfg = &mc->config;
			int charNo = (story_proc_no < STORY_W_OPEN ? 0 : 1);
			int stage = evs_story_no;

			// 到達ステージ数を、メニューのカーソル位置に反映
			cfg->st_st = CLAMP(0, LAST_STAGE_NO - 1, stage - 1);

			if(state->cnd_static == dm_cnd_win) {
				if(stage == EX_STAGE_NO && state->game_retry == 0) {
					evs_secret_flg[charNo] = 1;
				}

				if(dm_query_next_stage(evs_story_level, stage, state->game_retry)) {
					stage++;
				}
				else if(stage == EX_STAGE_NO) {
					// 記録でＡＬＬを表示するためのインクリメント
					stage++;
				}
			}

			// セーブデータセット
			dm_story_sort_set(evs_select_name_no[0], charNo,
				evs_story_level, state->game_score,
				evs_game_time, stage, evs_one_game_flg);
			st->eep_rom_flg = 1;
		}
		break;

	case GSL_1PLAY:
		// セーブデータセット
		state = &game_state_data[0];
		switch(evs_gamemode) {
		case GMD_NORMAL: {
			int level = state->virus_level;

			if(state->cnd_static == dm_cnd_win) {
				if(level >= 21) {
					evs_level_21 = 1;
				}
				if(level < VIRUS_LEVEL_MAX) {
					level++;
				}
			}
			dm_level_sort_set(evs_select_name_no[0],
				state->cap_def_speed, state->game_score, level);

			// 到達したレベルをメニューのカーソル位置に反映させる
			evs_mem_data[evs_select_name_no[0]].config.p1_lv = MIN(21, level);
			} break;

		case GMD_TaiQ:
			dm_taiQ_sort_set(evs_select_name_no[0],
				state->game_level,
				state->game_score,
				evs_game_time);
			break;

		case GMD_TIME_ATTACK:
			dm_timeAt_sort_set(evs_select_name_no[0],
				state->game_level,
				state->game_score,
				evs_game_time,
				state->total_erase_count);
			break;
		}
		st->eep_rom_flg = 1;
		break;
	}

	// セーブ
	cpuTask_sendTask(dm_game_eep_write, 0);
}

//////////////////////////////////////////////////////////////////////////////
//{### ゲーム進行

#ifdef _ENABLE_CONTROL
//## デバッグ操作処理
static void dm_query_debug_player(game_state *state)
{
	Game *st = watchGame;
	int trg = _getKeyTrg(state->player_no);
	int i;

	if(st->query_debug_player_no >= 0 || st->query_config_player_no >= 0) {
		return;
	}

	// デバック用操作
	if((trg & CONT_DEBUG) == CONT_DEBUG) {
		st->query_debug_player_no = state->player_no;
	}
	else if((trg & CONT_CONFIG) == CONT_CONFIG) {
		st->query_config_player_no = state->player_no;
	}
}
#endif // _ENABLE_CONTROL

//## ポーズ操作処理
static void dm_query_pause_player(game_state *state)
{
	Game *st = watchGame;
	int trg = _getKeyTrg(state->player_no);
	int i;

	if(st->query_pause_player_no >= 0) {
		return;
	}

	switch(state->cnd_static) {
	case dm_cnd_wait:
	case dm_cnd_pause:
		break;

	case dm_cnd_lose:
		if(!st->replayFlag || state->cnd_now != dm_cnd_tr_chack) {
			return;
		}
		break;

	default:
		return;
	}

	if(state->cnd_now == dm_cnd_init) return;
	if(state->player_type != PUF_PlayerMAN) return;

	if(!st->replayFlag) {
		if(trg & DM_KEY_START) {
			st->query_play_pause_se = 1;
			st->query_pause_player_no = state->player_no;
		}
	}
	else {
		if(trg & DM_ANY_KEY) {
			st->query_pause_player_no = state->player_no;
		}
	}
}

//## ゲーム操作処理(１Ｐ用)
static DM_GAME_RET dm_game_main_cnt_1P(game_state *state, game_map *map, int player_no)
{
	Game *st = watchGame;
	int i, j;

	// ポーズ要求
	dm_query_pause_player(state);

#ifdef _ENABLE_CONTROL
	// デバッグ操作要求
	dm_query_debug_player(state);
#endif // _ENABLE_CONTROL

	// キャラアニメーションを更新
	animeState_update(&state->anime);

	// スコアエフェクトを更新
	scoreNums_update(&st->scoreNums[player_no]);

	// タイムアタックの場合、制限時間が過ぎているかチェック
	if(evs_gamemode == GMD_TIME_ATTACK &&
		evs_game_time >= SCORE_ATTACK_TIME_LIMIT)
	{
		if(state->cnd_static == dm_cnd_wait) {
			// 制限時間が過ぎていたのでゲームオーバー
			return dm_ret_game_over;
		}
	}

#ifdef _ENABLE_CONTROL
	// 次のステージへ
	if(_getKeyTrg(0) & DM_KEY_Z) {
		if(state->cnd_static == dm_cnd_wait) {
			return dm_ret_clear;
		}
	}

//	if(_getKeyTrg(0) & DM_KEY_L) {
//		// コイン追加
//		if(st->retry_coin < COINMAX) {
//			st->retry_coin++;
//		}
//	}
#endif // _ENABLE_CONTROL

	// 内部処理部分
	switch(state->mode_now) {
	// ウイルス配置
	case dm_mode_init:
		dm_set_virus(state, map, virus_map_data[player_no], virus_map_disp_order[player_no]);
		return dm_ret_virus_wait;

	// ただの待ち
	case dm_mode_wait:
		return dm_ret_virus_wait;

	// 投げ中
	case dm_mode_throw:
		state->cap_speed_count++;

		// カプセルが瓶内部に入ったら
		if(state->cap_speed_count == FlyingCnt[state->cap_def_speed]) {
			dm_init_capsel_go(&state->now_cap,
				(CapsMagazine[state->cap_magazine_save] >> 4) % 3,
				CapsMagazine[state->cap_magazine_save] % 3);

			// カプセル落下開始
			state->mode_now = dm_mode_down;
			state->cap_speed_count = 30;
			dm_capsel_down(state,map);
		}
		break;

	// 着地ウェイト
	case dm_mode_down_wait:
		// ゲームオーバーの場合
		if(dm_check_game_over(state,map)) {
			for(i = 0; i < 3; i++) {
				// 消滅していなかったら巨大ウイルス笑いアニメーションセット
				if(st->virus_anime_state[i].animeSeq->animeNo != ASEQ_LOSE) {
					animeState_set(&st->virus_anime_state[i], ASEQ_WIN);
				}
			}
			// 巨大ウィルス停止カウントをクリア
			if(evs_gamemode == GMD_TaiQ) {
				st->big_virus_stop_count = 0;
			}
			// 積みあがり(ゲームオーバー)
			return dm_ret_game_over;
		}
		else {
			// 消滅があった場合
			if(dm_h_erase_chack(map) != cap_flg_off || dm_w_erase_chack(map) != cap_flg_off) {
				// 消滅開始
				state->mode_now = dm_mode_erase_chack;
				state->cap_speed_count = 0;
			}
			else {
				// カプセルセットを指定しておく
				state->mode_now = dm_mode_cap_set;
			}
		}
		break;

	// 消滅判定
	case dm_mode_erase_chack:
		state->cap_speed_count++;

		// 着地ウェイトを取った
		if(state->cap_speed_count < dm_bound_wait) {
			break;
		}
		state->cap_speed_count = 0;

		// 消滅開始
		state->mode_now = dm_mode_erase_anime;

		// 縦消し判定 // 横消し判定
		dm_h_erase_chack_set(state,map);
		dm_w_erase_chack_set(state,map);

		// 縦粒化処理 // 横粒化処理
		dm_h_ball_chack(map);
		dm_w_ball_chack(map);

		// 各色のウイルス数の取得
		i = state->virus_number;
		i -= get_virus_color_count(map, &st->big_virus_count[0],
			&st->big_virus_count[1] ,&st->big_virus_count[2]);
		state->virus_number -= i;

		// ウィルス消し数を取得
		state->total_erase_count += i;

		for(i = 0; i < 3; i++) {
			// 各色のウイルスが全滅した場合
			if(st->big_virus_count[i] == 0) {
				// まだ消滅フラグが立っていない場合
				if(!st->big_virus_flg[i]) {
					// 消滅フラグ立をたてる
					st->big_virus_flg[i] = 1;

					// 巨大ウイルス消滅アニメーションセット
					animeState_set(&st->virus_anime_state[i], ASEQ_LOSE);
					animeSmog_start(&st->virus_smog_state[i]);

					if(state->virus_number != 0) {
						// 巨大ウイルスダメージ
						dm_snd_play_in_game(SE_gVirusStruggle);

						// 巨大ウイルス消滅 SE セット
						dm_snd_play_in_game(SE_gVrsErsAl);
					}
				}
			}
			else if(state->chain_color[3] & (0x10 << i)) {
				if(st->big_virus_stop_count == 0) {
					// 巨大ウイルスやられアニメーションセット
					animeState_set(&st->virus_anime_state[i], ASEQ_DAMAGE);

					// 巨大ウイルスダメージ
					dm_snd_play_in_game(SE_gVirusStruggle);
				}
			}
		}

		// 消滅色のリセット
		state->chain_color[3] &= 0x0f;

		// 得点計算 & スコアエフェクト
		{
			int pos[2];
			dm_calc_erase_score_pos(state, map, pos);
			scoreNums_set(&st->scoreNums[player_no],
				dm_make_score(state), state->erase_virus_count,
				state->map_x + state->map_item_size / 2 + pos[0],
				state->map_y + state->map_item_size / 2 + pos[1]);
		}

		// ウイルス全滅
		if(state->virus_number == 0 && evs_gamemode != GMD_TaiQ) {
			// ステージクリア
			state->cnd_now = dm_cnd_stage_clear;
			state->mode_now = dm_mode_stage_clear;

			// 最大消滅列数の保管
			if(state->chain_line_max < state->chain_line) {
				state->chain_line_max = state->chain_line;
			}

			return dm_ret_clear;
		}
		// ウイルスが残り３個以下のとき警告音を鳴らす
		else if(state->virus_number <= 3 && evs_gamemode != GMD_TaiQ) {
			// １回ならしたら以後鳴らさない
			if(!st->last_3_se_flg) {
				st->last_3_se_flg = 1;

				// ウイルス残り３個ＳＥ再生
				dm_snd_play_in_game(SE_gReach);
			}

			// 音楽速度アップのフラグを立てる
			if(st->bgm_bpm_flg[0] == 0) {
				st->bgm_bpm_flg[0] = 1; // ２回処理しないためのフラグ
				st->bgm_bpm_flg[1] = 1; // 処理を行うフラグを立てる
			}
		}

		state->chain_count++;

		if(state->chain_line < 2) {
			if(state->chain_color[3] & 0x08) {
				// 消滅色のリセット
				state->chain_color[3] &= 0xF7;

				// ウイルスを含む消滅ＳＥ再生
				dm_snd_play_in_game(SE_gVrsErase);
			}
			else {
				// カプセルのみの消滅ＳＥ再生
				dm_snd_play_in_game(SE_gCapErase);
			}
		}
		else {
			if(state->chain_color[3] & 0x08) {
				// 消滅色のリセット
				state->chain_color[3] &= 0xF7;
			}
			i = MIN(2, state->chain_line - 2);
			dm_snd_play_in_game(SE_getAttack(state->charNo, i));
		}
		break;

	// 消滅アニメーション
	case dm_mode_erase_anime:
		dm_capsel_erase_anime(state,map);
		break;

	// カプセル(粒)落下
	case dm_mode_ball_down:
		go_down(state,map,dm_down_speed);
		break;

	// 新カプセルセット
	case dm_mode_cap_set:

		// 耐久モードの、連鎖時ボーナスウェイトを加算
		add_taiQ_bonus_wait(state);

		// 耐久モードの処理
		if(st->bottom_up_flag && st->big_virus_stop_count == 0) {
			state->bottom_up_scroll = 0;
			state->mode_now = dm_mode_bottom_up;
			set_bottom_up_virus(state, map);
			dm_snd_play_in_game(SE_gBottomUp);
			break;
		}

		// 合計連鎖回数を更新
		if(state->chain_count > 1) {
			state->total_chain_count = MIN(99,
				state->total_chain_count + state->chain_count - 1);
		}

		// 積みあがりチェック
		dm_warning_h_line(state, map);

		// 新カプセルセット
		dm_set_capsel(state);

		// 落下カプセル速度の計算
		dm_capsel_speed_up(state);

		// 攻撃ＳＥ再生
		dm_attack_se(state, player_no);

		// mario throw
		animeState_set(&state->anime, ASEQ_ATTACK);

		// 落下前思考追加位置
		if(PLAYER_IS_CPU(state, player_no)) {
			aifMakeFlagSet(state);
		}

		// 最大消滅列数の保管
		if(state->chain_line_max < state->chain_line) {
			state->chain_line_max = state->chain_line;
		}

		// カプセル落下処理へ
		state->mode_now = dm_mode_throw;
//		state->mode_now = dm_mode_down;

		// カウントのリセット // 落下速度のリセット
		state->cap_speed_count = 0;
		state->cap_speed_max = 0;

		// 消滅ライン数のリセット // 連鎖数のリセット
		state->chain_line = 0;
		state->chain_count = 0;

		// 消滅ウイルス数のリセット
		state->erase_virus_count = 0;
		state->erase_virus_count_old = 0;

		// 消滅色のリセット
		for(i = 0; i < 4; i++) {
			state->chain_color[i] = 0;
		}
		break;

	// 瓶の中身を一段上げる
	case dm_mode_bottom_up:
		// スクロール値を更新
		state->bottom_up_scroll++;
		if(state->bottom_up_scroll < state->map_item_size) {
			break;
		}
		state->bottom_up_scroll = 0;

		st->bottom_up_flag = false;
		st->big_virus_wait = MAX(_big_virus_min_wait[state->game_level], st->big_virus_wait - 0.5);

		// ハーフカプセル落下処理
		state->mode_now = dm_mode_ball_down;

		if(bottom_up_bottle_items(map)) {
			// 天井を貫いたので、ゲームオーバー
			return dm_ret_game_over;
		}

		// 各色のウイルス数の取得
		state->virus_number = get_virus_color_count(map,
			&st->big_virus_count[0], &st->big_virus_count[1] ,&st->big_virus_count[2]);

		// 消滅していた色のウィルスが復活した場合、煙と共に登場させる。
		for(i = 0; i < 3; i++) {
			if(st->big_virus_count[i] != 0 && st->big_virus_flg[i]) {
				// 巨大ウイルスアニメーションセット
				animeState_set(&st->virus_anime_state[i], ASEQ_NORMAL);
				animeSmog_start(&st->virus_smog_state[i]);
				st->big_virus_flg[i] = 0;
			}
			else if(st->virus_anime_state[i].animeSeq->animeNo == ASEQ_WIN) {
				animeState_set(&st->virus_anime_state[i], ASEQ_NORMAL);
			}
		}
		break;

//	// コイン獲得
//	case dm_mode_get_coin:
//		state->mode_now = dm_mode_stage_clear;
//		break;

	case dm_mode_clear_wait:
	case dm_mode_gover_wait:
		if(state->mode_now == dm_mode_clear_wait) {
			if(effectNextStage_calc(player_no)) break;
		}
		else {
			if(effectGameOver_calc(player_no)) break;
		}

		if(_getKeyTrg(player_no) & DM_ANY_KEY) {
			switch(state->mode_now) {
			case dm_mode_clear_wait:
				state->cnd_now = dm_cnd_clear_result;
				state->mode_now = dm_mode_clear_result;
				break;
			case dm_mode_gover_wait:
				state->cnd_now = dm_cnd_gover_result;
				state->mode_now = dm_mode_gover_result;
				break;
			}
		}
		break;

	case dm_mode_clear_result:
	case dm_mode_gover_result:
		if(_getKeyTrg(player_no) & DM_ANY_KEY) {
			timeAttackResult_skip(&st->timeAttackResult[player_no]);
		}

		timeAttackResult_update(&st->timeAttackResult[player_no], true);
		dm_add_score(state, st->timeAttackResult[player_no].scoreDelta);
		st->timeAttackResult[player_no].scoreDelta = 0;

		if(timeAttackResult_isEnd(&st->timeAttackResult[player_no])) {
			switch(state->mode_now) {
			case dm_mode_clear_result:
				state->cnd_now = dm_cnd_stage_clear;
				state->mode_now = dm_mode_stage_clear;
				break;
			case dm_mode_gover_result:
				state->cnd_now = dm_cnd_game_over;
				state->mode_now = dm_mode_game_over;
				break;
			}
		}
		break;

	// ステージクリア演出
	// ゲームオーバー演出
	case dm_mode_stage_clear:
	case dm_mode_game_over:
		if(state->mode_now == dm_mode_stage_clear) {
			if(effectNextStage_calc(player_no)) break;
		}
		else {
			if(effectGameOver_calc(player_no)) break;
		}

		// パスワードを表示
		i = DM_KEY_L | DM_KEY_R | DM_KEY_LEFT | DM_KEY_Z;
		if(st->passAlphaStep < 0 && (_getKeyLvl(0) & i) == i) {
			static char _guest[] = { 0, 0, 0, 0 };
			u32 mode, level, time;
			char *name;

			switch(evs_gamemode) {
			case GMD_NORMAL:
				mode = 0;
				level = state->virus_level;
				break;
			case GMD_TIME_ATTACK:
				mode = 1;
				level = state->game_level;
				break;
			case GMD_TaiQ:
				mode = 2;
				level = state->game_level;
				break;
			}

			if(evs_select_name_no[0] == DM_MEM_GUEST) {
				name = _guest;
			}
			else {
				name = evs_mem_data[evs_select_name_no[0]].mem_name;
			}

			if(st->passBuf[0] == 0) {
				make_passwd((u16 *)st->passBuf, mode, level, state->cap_def_speed,
					state->game_score / 10, evs_game_time / FRAME_PAR_MSEC, name);
				st->passBuf[PASSWORD_LENGTH * 2 + 0] = '~';
				st->passBuf[PASSWORD_LENGTH * 2 + 1] = 'z';
			}

			msgWnd_clear(&st->passWnd);
			msgWnd_addStr(&st->passWnd, _mesPassword);
			msgWnd_addStr(&st->passWnd, st->passBuf);
			msgWnd_skip(&st->passWnd);

			st->passAlphaStep = -st->passAlphaStep;
			break;
		}
		else if(_getKeyTrg(0) != 0 && st->passAlphaStep > 0) {
			st->passAlphaStep = -st->passAlphaStep;
		}

		switch(retryMenu_input(player_no)) {
		case CONTINUE_RET_REPLAY:
			// リプレイの準備
			set_replay_state();

			dm_snd_play_in_game(SE_mDecide);
			return dm_ret_replay;

		case CONTINUE_RET_CONTINUE:
			// リプレイ状態をリセット
			reset_replay_state();

			dm_snd_play_in_game(SE_mDecide);
			return dm_ret_next_stage;

		case CONTINUE_RET_RETRY:
			// リプレイ状態をリセット
			reset_replay_state();

			if(state->game_retry < 999) {
				// リトライ数の追加
				state->game_retry++;
			}

			if(state->virus_level >= 22 && st->retry_coin > 0) {
				// コイン数減少
				st->retry_coin--;

				// コイン使用音
				dm_snd_play_in_game(SE_gCoin);
			}
			else {
				// リトライ
				dm_snd_play_in_game(SE_mDecide);
			}
			return dm_ret_retry;

		case CONTINUE_RET_EXIT:
			// 終了
			dm_snd_play_in_game(SE_mDecide);
			return dm_ret_end;
		}
		break;

	// ポーズ処理
	case dm_mode_pause:
		break;

	case dm_mode_pause_retry:
		switch(retryMenu_input(player_no)) {
		case PAUSE_RET_CONTINUE:
			// ポーズ解除
			if(st->query_pause_player_no < 0) {
				st->query_pause_player_no = player_no;
			}
			dm_snd_play_in_game(SE_mDecide);
//			return dm_ret_pause;
			break;

		case PAUSE_RET_RETRY:
			// リトライ数の追加
			if(state->game_retry < 999) {
				state->game_retry++;
			}

			if(state->virus_level >= 22 && st->retry_coin > 0) {
				// コイン数減少
				st->retry_coin--;

				// コイン使用音
				dm_snd_play_in_game(SE_gCoin);
			}
			else {
				// リトライ
				dm_snd_play_in_game(SE_mDecide);
			}
			return dm_ret_retry;

		case PAUSE_RET_EXIT:
			// 終了
			dm_snd_play_in_game(SE_mDecide);
			return dm_ret_end;
		}
		break;

	// 何もしない
	case dm_mode_no_action:
		break;
	}

	return dm_ret_null;
}

//## ゲーム操作処理(多人数用)
static DM_GAME_RET dm_game_main_cnt(game_state *state, game_map *map, int player_no)
{
	Game *st = watchGame;
	int i, j, out, trg;

	// キー情報の取得
	trg = _getKeyTrg(player_no);

	// PAUSE 要求 or PAUSE 解除要求処理
	dm_query_pause_player(state);

#ifdef _ENABLE_CONTROL
	// デバッグ操作要求
	dm_query_debug_player(state);
#endif // _ENABLE_CONTROL

	// キャラアニメーションを更新
	animeState_update(&state->anime);

	// スコアエフェクトを更新
	scoreNums_update(&st->scoreNums[player_no]);

	// 黒上がり処理
	if(state->cnd_static != dm_cnd_wait && state->cnd_static != dm_cnd_pause) {
		dm_black_up(state, map);
	}

	// タイムアタックの場合、制限時間が過ぎているかチェック
	if(evs_gamemode == GMD_TIME_ATTACK &&
		evs_game_time >= SCORE_ATTACK_TIME_LIMIT)
	{
		if(state->cnd_static == dm_cnd_wait) {
			// 制限時間が過ぎていたのでゲームオーバー
			return dm_ret_game_over;
		}
	}

#ifdef _ENABLE_CONTROL
	// 強制勝利
	if(trg & DM_KEY_Z) {
		if(state->cnd_static == dm_cnd_wait) {
			return dm_ret_clear;
		}
	}
#endif // _ENABLE_CONTROL

	switch(state->mode_now) {
	// ウイルス配置
	case dm_mode_init:
		dm_set_virus(state, map, virus_map_data[player_no], virus_map_disp_order[player_no]);
		return dm_ret_virus_wait;

	// ウェイト
	case dm_mode_wait:
		return dm_ret_virus_wait;

	// 着地ウェイト
	case dm_mode_down_wait:

		// 積みあがり判定
		if(dm_check_game_over(state, map)) {
			// 積みあがり(ゲームオーバー)
			return dm_ret_game_over;
		}
		else {
			if(dm_h_erase_chack(map) != cap_flg_off || dm_w_erase_chack(map) != cap_flg_off) {
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
	case dm_mode_erase_chack: {
		int erase;

		state->cap_speed_count++;
		if(state->cap_speed_count < dm_bound_wait) {
			break;
		}

		// 消滅開始
		state->mode_now = dm_mode_erase_anime;
		state->cap_speed_count = 0;

		// 消し判定 // 粒化処理
		dm_h_erase_chack_set(state, map);
		dm_w_erase_chack_set(state, map);
		dm_h_ball_chack(map);
		dm_w_ball_chack(map);

		// ウイルス数の取得
		erase = state->virus_number;
		erase -= dm_update_virus_count(state, map, true);

		// ウィルス消し数を取得
		state->total_erase_count += erase;

		// 得点計算
		{
			int pos[2];
			dm_calc_erase_score_pos(state, map, pos);
			scoreNums_set(&st->scoreNums[player_no],
				dm_make_score(state), state->erase_virus_count,
				state->map_x + state->map_item_size / 2 + pos[0],
				state->map_y + state->map_item_size / 2 + pos[1]);
		}

		// ウイルス消滅(フラッシュウィルス全滅)
		if(state->virus_number == 0) {
			// 最大消滅列数の保管
			if(state->chain_line_max < state->chain_line) {
				state->chain_line_max = state->chain_line;
			}

			return dm_ret_clear;
		}
		else if(state->virus_number <= st->last_3_se_cnt) {
			// ウイルスが残り３個以下のとき警告音を鳴らす
			if(!st->last_3_se_flg) {
				// １回ならしたら以後鳴らさない
				st->last_3_se_flg = 1;

				// ウイルス残り３個ＳＥ再生
				dm_snd_play_in_game(SE_gReach);
			}

			// 音楽速度アップのフラグを立てる
			if(st->bgm_bpm_flg[0] == 0) {
				st->bgm_bpm_flg[0] = 1; // ２回行わないためのフラグ
				st->bgm_bpm_flg[1] = 1; // 処理を行うフラグを立てる
			}
		}

		state->chain_count++;

		switch(evs_gamesel) {
		case GSL_2PLAY: // 2Ｐの時
		case GSL_2DEMO: // 2Ｐデモ
		case GSL_VSCPU: // VSCPUのとき
			if(state->chain_line < 2) {

				if(state->chain_color[3] & 0x08) {
					// ウイルス消滅のリセット
					state->chain_color[3] &= 0xF7;

					// ウイルスを含む消滅ＳＥ再生
					if(evs_gamemode == GMD_FLASH && erase) {
						dm_snd_play_in_game(SE_gEraseFlash);
					}
					else {
						dm_snd_play_in_game(SE_gVrsErase);
					}
				}
				else {
					// カプセルのみの消滅ＳＥ再生
					dm_snd_play_in_game(SE_gCapErase);
				}
			}
			else {
				j = MIN(2, state->chain_line - 2);
				dm_snd_play_in_game(SE_getAttack(state->charNo, j));

				if(state->chain_color[3] & 0x08) {
					// ウイルス消滅のリセット
					state->chain_color[3] &= 0xF7;
				}
			}
			break;

		// 4Ｐ or 4Ｐデモ の時
		case GSL_4PLAY:
		case GSL_4DEMO:
			if(state->chain_color[3] & 0x08) {
				// ウイルスを含む消滅のリセット
				state->chain_color[3] &= 0xF7;

				// ウイルスを含む消滅ＳＥ再生
				if(evs_gamemode == GMD_FLASH && erase) {
					dm_snd_play_in_game(SE_gEraseFlash);
				}
				else {
					dm_snd_play_in_game(SE_gVrsErase);
				}
			}
			else {
				// カプセルのみの消滅ＳＥ再生
				dm_snd_play_in_game(SE_gCapErase);
			}
		}
		} break;

	// 消滅アニメーション
	case dm_mode_erase_anime:
		dm_capsel_erase_anime(state, map);
		break;

		// カプセル(粒)落下
	case dm_mode_ball_down:
		go_down(state,map,dm_down_speed);
		break;

	// 新カプセルセット
	case dm_mode_cap_set:
		out = 1;

		// 合計連鎖回数を更新
		if(state->chain_count > 1) {
			state->total_chain_count = MIN(99,
				state->total_chain_count + state->chain_count - 1);
		}

		// 攻撃ＳＥ再生
		dm_attack_se(state, player_no);

		// 積みあがりチェック
		dm_warning_h_line(state, map);

		switch(evs_gamesel) {
		// 2Ｐ or 2Ｐデモ or VSCPU の時
		case GSL_2DEMO:
		case GSL_2PLAY:
		case GSL_VSCPU:
			// タイムアタック時は攻撃を行わない
			if(evs_gamemode == GMD_TIME_ATTACK) {
				break;
			}

			// 攻撃判定
			i = dm_set_attack_2p(state);

			// 攻撃アニメーションセット
			if(i) {
				animeState_set(&state->anime, ASEQ_ATTACK);
			}

			// やられ判定
			if(dm_broken_set(state, map)) {
				// やられアニメーションセット(強制)
				animeState_set(&state->anime, ASEQ_DAMAGE);

				// ウイルスを含む消滅ＳＥ再生
				dm_snd_play_in_game(SE_getDamage(state->charNo));

				// 粒落下処理へ
				state->mode_now = dm_mode_ball_down;
				out = 0;
				if(state->ai.aiState & AIF_DAMAGE) {
					state->ai.aiState |= AIF_DAMAGE2;
				}
				else {
					state->ai.aiState |= AIF_DAMAGE;
				}
			}
			break;

		// 4Ｐ or 4Ｐデモ の時
		case GSL_4PLAY:
		case GSL_4DEMO:
			// 攻撃判定
			i = dm_set_attack_4p(state);

			// 攻撃アニメーション
			if(i) {
				animeState_set(&state->anime, ASEQ_ATTACK);
			}

			// やられ判定
			if(dm_broken_set(state, map)) {
				// やられアニメーションを設定
				animeState_set(&state->anime, ASEQ_DAMAGE);

				// 粒落下処理へ
				state->mode_now = dm_mode_ball_down;
				out = 0;

				if(state->ai.aiState & AIF_DAMAGE) {
					state->ai.aiState |= AIF_DAMAGE2;
				}
				else {
					state->ai.aiState |= AIF_DAMAGE;
				}
			}
			break;
		}

		if(out) {
			dm_set_capsel(state);

			// 落下カプセル速度の計算
			dm_capsel_speed_up(state);

			// 最大消滅列数の保管
			if(state->chain_line_max < state->chain_line) {
				state->chain_line_max = state->chain_line;
			}

			// 消滅ライン数, 連鎖数, 消滅ウイルス数, をリセット
			state->chain_line = 0;
			state->chain_count = 0;
			state->erase_virus_count = 0;
			state->erase_virus_count_old = 0;

			// 消滅色のリセット
			for(i = 0; i < 4; i++) {
				state->chain_color[i] = 0;
			}

			// カプセル落下処理へ
			state->mode_now = dm_mode_down;

			// 落下前思考追加位置
			if(PLAYER_IS_CPU(state, player_no)) {
				aifMakeFlagSet(state);
			}
		}
		break;

	case dm_mode_clear_wait:
		effectNextStage_calc(player_no);
		break;

	case dm_mode_gover_wait:
		effectGameOver_calc(player_no);
		break;

	case dm_mode_retire_wait:
		effectRetire_calc(player_no);
		break;

	case dm_mode_clear_result:
	case dm_mode_gover_result:
	case dm_mode_retire_result:
		if(_getKeyTrg(player_no) & DM_ANY_KEY) {
			timeAttackResult_skip(&st->timeAttackResult[player_no]);
		}

		timeAttackResult_update(&st->timeAttackResult[player_no], true);
		dm_add_score(state, st->timeAttackResult[player_no].scoreDelta);
		st->timeAttackResult[player_no].scoreDelta = 0;
		break;

	// WIN & RETRY
	// LOSE & RETRY
	// DRAW & RETRY
	case dm_mode_win_retry:
	case dm_mode_lose_retry:
	case dm_mode_draw_retry:
		if(state->mode_now == dm_mode_win_retry) {
			if(effectWin_calc(player_no)) break;
		}
		else if(state->mode_now == dm_mode_lose_retry) {
			if(effectLose_calc(player_no)) break;
		}
		else if(state->mode_now == dm_mode_draw_retry) {
			if(effectDraw_calc(player_no)) break;
		}

		switch(retryMenu_input(player_no)) {
		case CONTINUE_RET_REPLAY:
			// リプレイの準備
			set_replay_state();

			dm_snd_play_in_game(SE_mDecide);
			return dm_ret_replay;

		case CONTINUE_RET_CONTINUE:
			// リプレイ状態をリセット
			reset_replay_state();

			dm_snd_play_in_game(SE_mDecide);
			return dm_ret_end;

		case CONTINUE_RET_RETRY:
			// リプレイ状態をリセット
			reset_replay_state();

			// リトライ数の追加
			if(state->game_retry < 999) {
				state->game_retry++;
			}
			dm_snd_play_in_game(SE_mDecide);
			return dm_ret_retry;

		case CONTINUE_RET_EXIT:
			// 終了
			dm_snd_play_in_game(SE_mDecide);
			return dm_ret_game_end;
		}
		break;

	// WIN演出
	case dm_mode_win:
		if(!effectWin_calc(player_no)) {
//			if(trg & DM_ANY_KEY) {
//				return dm_ret_end;
//			}
		}
		break;

	// LOSE演出
	case dm_mode_lose:
		if(!effectLose_calc(player_no)) {
//			if(trg & DM_ANY_KEY) {
//				return dm_ret_end;
//			}
		}
		break;

	// DRAW演出
	case dm_mode_draw:
		if(!effectDraw_calc(player_no)) {
//			if(trg & DM_ANY_KEY) {
//				return dm_ret_end;
//			}
		}
		break;

	// 練習確認
	case dm_mode_tr_chaeck:
		// 攻撃無し版
		if(trg & DM_KEY_START) {
			if(!st->replayFlag) {
				return dm_ret_tr_a;
			}
		}
//		// 攻撃あり版
//		else if(trg & DM_KEY_Z) {
//			return dm_ret_tr_b;
//		}
		break;

	// トレーニング初期化
	case dm_mode_training:
		// ゲームオーバーフラグを立てる(念のため)
		state->flg_game_over = 1;

		clear_map_all(map); // マップ情報削除
		state->virus_number = 0; // ウイルス数のクリア
		state->virus_order_number = 0; // 配置中のウィルス番号
		state->virus_anime_spead = v_anime_def_speed_4p;
		state->warning_se_flag = 0; // 警告音フラグをクリア

		// カプセル落下速度の設定
		state->cap_speed = GameSpeed[state->cap_def_speed];
		state->cap_speed_max = 0; // 随時設定される
		state->cap_speed_vec = 1; // 落下カウンタ増加値
		state->cap_magazine_cnt = 1; // マガジン残照を1にする
		state->cap_count = state->cap_speed_count = 0; // カウンタの初期化

		// カプセル情報の初期設定
		dm_set_capsel(state);

		// ウイルスアニメーションの設定
		state->erase_anime = 0; // 消滅アニメーションコマ数の初期化
		state->erase_anime_count = 0; // 消滅アニメーションカウンタの初期化
		state->erase_virus_count = 0; // 消滅ウイルスカウンタの初期化
		state->erase_virus_count_old = 0;

		// 連鎖数の初期化
		state->chain_count = state->chain_line = 0;

		// 攻撃カプセルデータの初期化
		for(i = 0; i < 4; i++) {
			state->chain_color[i] = 0;
		}

		// 被爆カプセルデータの初期化
		for(i = 0; i < DAMAGE_MAX; i++) {
			state->cap_attack_work[i][0] = 0; // 攻撃無し
			state->cap_attack_work[i][1] = 0; // 攻撃無し
		}

//		state->mode_now = dm_mode_down; // 通常処理に
//		state->cnd_now = dm_cnd_wait; // 通常状態に

		state->mode_now = dm_mode_init;
		state->cnd_now = dm_cnd_init;

		// ウィルス配置情報を初期化
		for(i = 0; i < ARRAY_SIZE(virus_map_disp_order[player_no]); i++) {
			virus_map_disp_order[player_no][i] &= ~0x80;
		}

		// フラッシュウィルス色情報を初期化
		for(i = 0; i < state->flash_virus_count; i++) {
			state->flash_virus_pos[i][2] = state->flash_virus_bak[i];
		}

		break;

	case dm_mode_tr_erase_chack:
		state->cap_speed_count++;
		if(state->cap_speed_count < dm_bound_wait) {
			break;
		}

		state->cap_speed_count = 0;
		state->mode_now = dm_mode_erase_anime;

		dm_h_erase_chack_set(state,map); // 縦消し判定
		dm_w_erase_chack_set(state,map); // 横消し判定
		dm_h_ball_chack(map); // 縦粒化現象
		dm_w_ball_chack(map); // 横粒化現象

		state->chain_count++;

		// ウイルス数の取得
		dm_update_virus_count(state, map, true);

		if(state->chain_color[3] & 0x08) {
			// ウイルスフラグ
			state->chain_color[3] &= 0xF7;

			// ウイルスを含む消滅ＳＥ再生
			dm_snd_play_in_game(SE_gVrsErase);
		}
		else {
			// カプセルのみの消滅ＳＥ再生
			dm_snd_play_in_game(SE_gCapErase);
		}
		break;

	case dm_mode_tr_cap_set:
		out = 1;

		// 積みあがりチェック
		dm_warning_h_line(state, map);

		// 攻撃あり
		if(state->flg_training) {
			// 攻撃ＳＥ再生
			dm_attack_se(state, player_no);

			// 攻撃判定
			dm_set_attack_4p(state);

			// 攻撃アニメーションを設定
			animeState_set(&state->anime, ASEQ_ATTACK);

			// やられ判定
			if(dm_broken_set(state, map)) {
				// 粒落下処理へ
				state->mode_now = dm_mode_ball_down;
				out = 0;
			}
		}
		else {
			for(j = 0; j < DAMAGE_MAX; j++) {
				state->cap_attack_work[j][0] = 0; // 攻撃無し
				state->cap_attack_work[j][1] = 0; // 攻撃無し
			}
		}

		if(out) {
			dm_set_capsel(state);

			// 落下カプセル速度の計算
			dm_capsel_speed_up(state);

			// 消滅ライン数, 連鎖数, 消滅ウイルス数, をリセット
			state->chain_line = 0;
			state->chain_count = 0;
			state->erase_virus_count = 0;
			state->erase_virus_count_old = 0;

			// 消滅色のリセット
			for(i = 0; i < 4; i++) {
				state->chain_color[i] = 0;
			}

			// カプセル落下処理へ
			state->mode_now = dm_mode_down;
		}
		break;

	// ポーズ処理
	case dm_mode_pause:
		break;

	case dm_mode_pause_retry:
		switch(retryMenu_input(player_no)) {
		case PAUSE_RET_CONTINUE:
			// ポーズ解除
			if(st->query_pause_player_no < 0) {
				st->query_pause_player_no = player_no;
			}
			dm_snd_play_in_game(SE_mDecide);
//			return dm_ret_pause;
			break;

		case PAUSE_RET_RETRY:
			// リトライ数の追加
			if(state->game_retry < 999) {
				state->game_retry++;
			}
			dm_snd_play_in_game(SE_mDecide);
			return dm_ret_retry;

		case PAUSE_RET_EXIT:
			// 終了
			dm_snd_play_in_game(SE_mDecide);
			return dm_ret_game_end;
		}
		break;

	// 何もしない
	case dm_mode_no_action:
		break;
	}

	return dm_ret_null;
}

//## PAUSE ON の設定
static void dm_set_pause_on(
	game_state *state, // ゲーム情報
	int master // ポーズONした人
	)
{
	Game *st = watchGame;
	int p = state->player_no;

	// PAUSE 処理初期化
	effectPause_init(p);

	state->cnd_static = dm_cnd_pause;    // 絶対的コンディションを PAUSE に設定
	state->cnd_old    = state->cnd_now;  // 現在の状態の保存
	state->mode_old   = state->mode_now; // 現在の処理の保存

	// レベルセレクト
	if(evs_gamesel == GSL_1PLAY) {
		state->mode_now = dm_mode_pause_retry; // 処理を PAUSE & RETRY に設定
		state->cnd_now  = dm_cnd_pause_re;     // 状態を PAUSE & RETRY に設定

		// LEVEL 21以下 か RETRY 用コインがある場合
		if(state->virus_level < 22 || st->retry_coin > 0) {
			// RETRY 可能に設定
			retryMenu_init(0, PAUSE_NEX_TRY_END);
		}
		// お金がなくて、リトライ不可の場合
		else {
			retryMenu_init(0, PAUSE_NEX_END);
		}
	}
	// ポーズを要求した人がリトライ・中断選択権を得る
	else if(p == master) {
		state->mode_now = dm_mode_pause_retry; // PAUSE & RETRY 処理に設定
		state->cnd_now  = dm_cnd_pause_re;     // PAUSE & RETRY 状態に設定

		// ストーリーモードのEXTRA以外の場合
//		if(evs_story_flg && !evs_one_game_flg) {
		if(evs_story_flg) {
			// PAUSE & RETRY & SCORE 状態に設定
			state->cnd_now = dm_cnd_pause_re_sc;

			// ピーチ姫戦
			if(evs_story_no == EX_STAGE_NO) {
				// リトライ不可
				retryMenu_init(p, PAUSE_NEX_TRY_END);
			}
			else {
				// リトライ可能
				retryMenu_init(p, PAUSE_NEX_TRY_END);
			}
		}
		else {
			// リトライ可能
			retryMenu_init(p, PAUSE_NEX_TRY_END);
		}
	}
	else {
		retryMenu_initPauseLogo(p);
		state->cnd_now  = dm_cnd_pause;  // PAUSE 状態に設定
		state->mode_now = dm_mode_pause; // PAUSE 処理に設定
	}
}

//## PAUSE OFF の設定
static void dm_set_pause_off(
	game_state *state // ゲーム情報
	)
{
	state->cnd_static = dm_cnd_wait; // 絶対的コンディションを通常状態に設定
	state->cnd_now    = state->cnd_old; // 前の状態に戻す
	state->mode_now   = state->mode_old; // 前の処理に戻す

	// 練習確認中だった場合
	// 絶対的コンディションを敗北に設定
	if(state->mode_now == dm_mode_tr_chaeck) {
		state->cnd_static = dm_cnd_lose;
	}
}

//## PAUSE 処理 & 音量の設定
static void dm_set_pause_and_volume(game_state **state, int count)
{
	Game *st = watchGame;
	int pause = st->query_pause_player_no;
	int sound = st->query_play_pause_se ? SE_gPause : -1;
	int i;

	st->query_pause_player_no = -1;
	st->query_play_pause_se = 0;

	// ポーズを要求した人がいない場合は中断
	if(pause < 0) {
		return;
	}

	if(state[pause]->cnd_static == dm_cnd_wait) {
		// 音量減少
		dm_seq_set_volume(SEQ_VOL_LOW);

		// PAUSE ON!!!
		if(!st->replayFlag) {
			for(i = 0; i < count; i++) {
				dm_set_pause_on(state[i], pause);
			}
		}
		// リプレイから復帰
		else {
			resume_game_state(1);
		}

		pause = -1;
	}
	// PAUSE 解除要求
	else if(state[pause]->cnd_static == dm_cnd_pause) {
		// 音量回復
		dm_seq_set_volume(SEQ_VOL_HI);

		// PAUSE OFF!!!
		for(i = 0; i < count; i++) {
			dm_set_pause_off(state[i]);
		}

		pause = -1;
	}
	else if(st->replayFlag && state[pause]->cnd_now == dm_cnd_tr_chack) {
		// 音量減少
		dm_seq_set_volume(SEQ_VOL_LOW);

		// リプレイから復帰
		resume_game_state(1);
	}

	// ポーズSEを再生
	if(pause < 0 && sound >= 0) {
		dm_snd_play_in_game(sound);
	}
}
static void dm_set_pause_and_volume_1p(game_state *state)
{
	game_state *stateArray[1] = { state };
	dm_set_pause_and_volume(stateArray, 1);
}

#ifdef _ENABLE_CONTROL
//## デバッグモードの設定
static void dm_set_debug_mode()
{
	Game *st = watchGame;
	game_state *state = &game_state_data[0];
	bool dbg = (st->query_debug_player_no >= 0);
	bool cfg = (st->query_config_player_no >= 0);

	st->query_debug_player_no = -1;
	st->query_config_player_no = -1;

	switch(state->mode_now) {
	// ポーズ処理
	case dm_mode_pause_retry:
	case dm_mode_pause:
		// デバック用操作
		if(dbg) {
			joyflg[0] |= DM_KEY_L | DM_KEY_R;
			state->cnd_now = dm_cnd_debug;
			state->mode_now = dm_mode_debug;
		}
		else if(cfg) {
			state->cnd_now = dm_cnd_debug_config;
			state->mode_now = dm_mode_debug_config;
		}
		break;

	// デバック用操作
	case dm_mode_debug:
		if(dbg) {
			joyflg[0] &= ~(DM_KEY_L | DM_KEY_R);
			state->cnd_now = dm_cnd_pause;
			state->mode_now = dm_mode_pause;
		}
		break;

	case dm_mode_debug_config:
		if(cfg) {
			state->cnd_now = dm_cnd_pause;
			state->mode_now = dm_mode_pause;
		}
		break;
	}
}
#endif // _ENABLE_CONTROL

//## 巨大ウィルスの座標計算
void dm_calc_big_virus_pos(game_state *state)
{
	Game *st = watchGame;
	int i, stopFlag;

	// 拡大・縮小
	for(i = 0; i < 3; i++) {
		SAnimeState *ani = &st->virus_anime_state[i];
		float vec = 1;

		switch(ani->animeSeq->animeNo) {
		case ASEQ_LOSE:
			if(ani->frameCount >= 90) {
				vec = -1;
			}
			break;
		}
		st->big_virus_scale[i] = CLAMP(0, 1, st->big_virus_scale[i] + (vec / 60.0));
	}

	// ウィルス回転停止中の点滅処理
#if 1
	do {
		enum { DARK = 127 };
		static const struct {
			short limit, blink;
		} _tbl[] = {
			{ FRAME_PAR_SEC * 0.5, FRAME_PAR_SEC / 15 },
			{ FRAME_PAR_SEC * 1.0, FRAME_PAR_SEC / 10 },
			{ FRAME_PAR_SEC * 2.0, FRAME_PAR_SEC /  5 },
		};
		int color = DARK;

		for(i = 0; i < ARRAY_SIZE(_tbl); i++) {
			if(st->big_virus_stop_count < _tbl[i].limit) {
				static const short _clr[] = { 255, DARK };
				color = _clr[(st->big_virus_stop_count / _tbl[i].blink) & 1];
				break;
			}
		}

		for(i = 0; i < 3; i++) {
			SAnimeState *anim = &st->virus_anime_state[i];
			anim->color[0] = anim->color[1] = anim->color[2] = color;
		}

		st->big_virus_blink_count++;
	} while(0);
#else
/*
	do {
		int range = MIN(160, st->big_virus_stop_count + 1);
		int count = (st->big_virus_blink_count %= range);
		int color = count * 511 / range;

		if(color > 255) {
			color = 511 - color;
		}
		color = 255 - color / 2;

		for(i = 0; i < 3; i++) {
			SAnimeState *anim = &st->virus_anime_state[i];
			anim->color[0] = anim->color[1] = anim->color[2] = color;
		}

		st->big_virus_blink_count++;
	} while(0);
*/
#endif

	// 通常状態だった
	if(state->cnd_static != dm_cnd_wait) {
		return;
	}

	// ウィルス停止カウンタをデクリメント
	if(st->big_virus_stop_count > 0) {
		if(st->big_virus_no_wait) {
			st->big_virus_stop_count = 0;
			st->big_virus_no_wait = false;
		}
		else {
			st->big_virus_stop_count--;
		}
	}

	// 回転を行うか判定
	for(i = stopFlag = 0; i < 3; i++) {
		SAnimeState *ani = &st->virus_anime_state[i];

		switch(ani->animeSeq->animeNo) {
		case ASEQ_LOSE:
			switch(evs_gamemode) {
			case GMD_TaiQ:
				break;
			default:
				if(!animeSeq_isEnd(ani->animeSeq)) stopFlag++;
				break;
			}
			break;

		case ASEQ_DAMAGE:
			switch(evs_gamemode) {
			case GMD_TaiQ:
				break;
			default:
				stopFlag++;
				break;
			}
			break;

		case ASEQ_NORMAL:
			break;

		case ASEQ_WIN:
			if(state->cnd_static != dm_cnd_wait) {
				stopFlag++;
			}
			break;
		}
	}

	// 停止カウンターが有効?
	if(st->big_virus_stop_count > 0) {
		stopFlag++;
	}

	// ウィルス回転処理
	if(!stopFlag && st->started_game_flg) {
		int top = -1, lost = 0;

		for(i = 0; i < 3; i++) {
			SAnimeState *ani = &st->virus_anime_state[i];

			if(!st->big_virus_flg[i]) {
				if(top < st->big_virus_rot[i]) {
					top = st->big_virus_rot[i];
				}
			}
			else {
				if(animeSeq_isEnd(ani->animeSeq)) {
					lost++;
				}
			}
		}

		if(evs_gamemode == GMD_TaiQ && lost == 3) {
			for(i = 0; i < 3; i++) {
				st->big_virus_rot[i] = i * 120 + 1;
			}
			st->bottom_up_flag = true;
		}
		else if(st->big_virus_no_wait) {
			st->big_virus_timer = st->big_virus_wait * (360 - top);
		}
		else if(st->big_virus_timer < st->big_virus_wait) {
			st->big_virus_timer += 1;
		}
	}

	if(st->big_virus_timer >= st->big_virus_wait) {
		int step;

		step = st->big_virus_timer / st->big_virus_wait;
		step = MIN(step, 4);

		st->big_virus_timer -= st->big_virus_wait * step;

		for(i = 0; i < 3; i++) {
			st->big_virus_rot[i] += step;

			if(st->big_virus_rot[i] >= 360) {
				st->big_virus_rot[i] -= 360;

				// 瓶の中身をボトムアップ
				if(!st->big_virus_flg[i] && evs_gamemode == GMD_TaiQ) {
					st->bottom_up_flag = true;
					st->big_virus_timer = 0;
					animeState_set(&st->virus_anime_state[i], ASEQ_WIN);
				}
			}
		}
	}

	// ウィルスの座標を計算
	for(i = 0; i < 3; i++) {
		st->big_virus_pos[i][0] = sinf(DEGREE(st->big_virus_rot[i])) *  20 +  61;
		st->big_virus_pos[i][1] = cosf(DEGREE(st->big_virus_rot[i])) * -20 + 171;
	}
}

//## ゲーム処理関数(1P用)
static DM_GAME_RET dm_game_main_1p()
{
	Game *st = watchGame;
	game_state *state = &game_state_data[0];
	game_map *map = game_map_data[0];
	DM_GAME_RET ret;
	int i, j;

	// ポーズ処理
	dm_set_pause_and_volume_1p(state);

#ifdef _ENABLE_CONTROL
	// デバッグ処理
	dm_set_debug_mode();
#endif // _ENABLE_CONTROL

	// メイン処理
	ret = dm_game_main_cnt_1P(state, map, 0);

	// つみあがり警告音を再生
	dm_warning_h_line_se();

	// ウィルスキャラアニメーションを更新
	if(st->big_virus_stop_count == 0) {
		for(i = 0; i < 3; i++) {
			animeState_update(&st->virus_anime_state[i]);
			animeSmog_update(&st->virus_smog_state[i]);
		}
	}

	// 巨大ウィルスの座標計算
	dm_calc_big_virus_pos(state);

	// カウントダウンSEを再生
	dm_play_count_down_se();

	// ウイルス配置完了
	if(ret == dm_ret_virus_wait) {
		if(st->count_down_ctrl < 0) {
			if(state->mode_now == dm_mode_wait) {
				// タイマー作動 // カプセル落下開始
				st->started_game_flg = 1;
				state->mode_now = dm_mode_throw;
//				state->mode_now = dm_mode_down;

				// mario throw
				animeState_set(&state->anime, ASEQ_ATTACK);

				// リプレイの 再生・録画 を開始
				start_replay_proc();
			}
		}
	}
	// クリア
	else if(ret == dm_ret_clear) {
		u32 score;

		effectNextStage_init(state->player_no);

		// クリア BGM の設定
		dm_seq_play_in_game(SEQ_End_A);

		// タイマーカウント停止
		st->started_game_flg = 0;

		// 絶対的コンディションを勝利状態にする
		state->cnd_static = dm_cnd_win;

		// 次の処理を決定
		switch(evs_gamemode) {
		case GMD_TIME_ATTACK:
			state->cnd_now  = dm_cnd_clear_wait;
			state->mode_now = dm_mode_clear_wait;

			timeAttackResult_result(&st->timeAttackResult[0], state->game_level, true,
				MAX(0, (int)SCORE_ATTACK_TIME_LIMIT - (int)evs_game_time),
				state->total_chain_count, state->total_erase_count, state->game_score);
			break;

		default:
			state->cnd_now  = dm_cnd_stage_clear;  // 状態を STAGE CLEAR に設定
			state->mode_now = dm_mode_stage_clear; // 処理を STAGE CLEAR に設定
			break;
		}

		// メニューを設定
		switch(evs_gamemode) {
		case GMD_TIME_ATTACK:
			// [リプレイ] [もう一度] [終わる]
			retryMenu_init(0, CONTINUE_REP_TRY_END);
			break;

		case GMD_NORMAL:
			// [リプレイ] [続ける]
			retryMenu_init(0, CONTINUE_REP_NEX_END);
			break;
		}

//		// コイン計算
//		if(state->virus_level >= 21 && (state->virus_level - 21) % 5 == 0) {
//			// (通常モード && 現在のコインが COINMAX 枚以下) の場合
//			if(evs_gamemode == GMD_NORMAL && st->retry_coin < COINMAX) {
//				// コイン獲得
//				st->retry_coin++;
//
//				// コイン獲得音
//				dm_snd_play_in_game(SE_gCoin);
//
//				// 処理をコイン獲得演出に設定
////				state->mode_now = dm_mode_get_coin;
//			}
//		}

		// 勝利アニメーションセット
		animeState_set(&state->anime, ASEQ_WIN);

		// セーブ
		switch(evs_gamemode) {
		case GMD_NORMAL:
			dm_save_all();
			break;
		case GMD_TIME_ATTACK:
			score = game_state_data[0].game_score;
			game_state_data[0].game_score = st->timeAttackResult[0].result;
			dm_save_all();
			game_state_data[0].game_score = score;
			break;
		}
	}
	// ゲームオーバー
	else if(ret == dm_ret_game_over) {
		effectGameOver_init(state->player_no);

		// 引き分けアニメーションセット
		animeState_set(&state->anime, ASEQ_LOSE);

		// GAME OVER 音設定
		dm_seq_play_in_game(SEQ_End_C);

		state->virus_anime_spead = v_anime_speed; // アニメーション速度を早くする
		state->cnd_static        = dm_cnd_lose;   // 絶対的コンディションを負け状態にする

		switch(evs_gamemode) {
		case GMD_TIME_ATTACK:
			state->cnd_now  = dm_cnd_gover_wait;
			state->mode_now = dm_mode_gover_wait;

			timeAttackResult_result(&st->timeAttackResult[0], state->game_level, false,
				0, state->total_chain_count, state->total_erase_count, state->game_score);
			break;
		default:
			state->cnd_now  = dm_cnd_game_over;  // 状態を GAME OVER に設定する
			state->mode_now = dm_mode_game_over; // 処理を GAME OVER に設定する
			break;
		}

		// タイマーカウント停止
		st->started_game_flg = 0;

		// LEVEL 21 以下の場合
		// RETRY コインを持っていた場合
		if(state->virus_level < 22 || st->retry_coin > 0) {
			// RETRY 可能に設定
			retryMenu_init(0, CONTINUE_REP_TRY_END);
		}
		// LEVEL 22 以上の場合
		else {
			// RETRY 不可に設定
			retryMenu_init(0, CONTINUE_REP_END);
		}

		// セーブ
		dm_save_all();
	}
	// NEXT STAGE の場合
	else if(ret == dm_ret_next_stage) {
		return dm_ret_next_stage;
	}
	// RETRY の場合
	else if(ret == dm_ret_retry) {
		return dm_ret_retry;
	}
	// REPLAY の場合
	else if(ret == dm_ret_replay) {
		return dm_ret_replay;
	}
	// ゲーム終了
	else if(ret == dm_ret_end) {
		return dm_ret_game_over;
	}
	// 完全終了
	else if(ret == dm_ret_game_end) {
		return dm_ret_game_end;
	}

	return dm_ret_null;
}

//## 勝利数を加算(2P)
static bool dm_add_win_2p(
	game_state *state // プレイヤー情報
	)
{
	Game *st = watchGame;
	bool finish = false;
	int sound = SEQ_End_C;
	int x, y, p = state->player_no;

	// 勝ちポイントの増加
	st->win_count[p]++;

	// ストーリーモードの場合
	if(evs_story_flg) {
		// スターグラフィックを追加
		x = _posStStar[p][0];
		y = _posStStar[p][1];

		// 決着が付いた
		finish = true;

		// プレイヤーが勝った場合
		if(state->player_type == PUF_PlayerMAN) {
			sound = SEQ_End_A;
		}
	}
	// 対戦の場合
	else {
		// スターグラフィックを追加
		x = _posP2StarX[p];
		y = _posP2StarY[evs_vs_count - 1][st->win_count[p] - 1];

		// 決着が付いた
		finish = (st->win_count[p] == evs_vs_count);

		// 勝利数を追加
		if(finish) {
			st->vs_win_total[p] = MIN( 99, st->vs_win_total[p] + 1);
			st->vs_win_count[p] = MIN(999, st->vs_win_count[p] + 1);
		}

		if(evs_gamesel != GSL_VSCPU ||
			state->player_type == PUF_PlayerMAN)
		{
			// ３本取得したか？
			if(finish) {
				// ３本取得の場合
				sound = SEQ_End_B;
			}
			else {
				// １本取得の場合
				sound = SEQ_End_A;
			}
		}
	}

	st->star_pos_x[st->star_count] = x;
	st->star_pos_y[st->star_count] = y;
	st->star_count++;

	// サウンドを再生
	dm_seq_play_in_game(sound);

	return finish;
}

//## 勝利者の設定(2P,4P)
static bool dm_set_win_2p(
	game_state *state, // ゲーム情報
	bool finish, // 決着が付いたか?
	bool menu    // メニューが設定されているか?
	)
{
	int p = state->player_no;

	// 絶対的コンディションを勝利状態にする
	state->cnd_static = dm_cnd_win;

	// 勝利演出の初期化
	effectWin_init(p);

	// 処理アニメーションを設定
	animeState_set(&state->anime, ASEQ_WIN);

	// 勝利の雄叫び
	dm_snd_play_in_game(SE_getWin(state->charNo));

	// MAN の場合
	if(!menu && state->player_type == PUF_PlayerMAN) {
		menu = true;

		state->cnd_now  = dm_cnd_win_retry;  // 状態を WIN & RETRY に設定
		state->mode_now = dm_mode_win_retry; // 処理を WIN & RETRY に設定

		// ストーリーモード以外の場合
		if(!evs_story_flg) {
			// 勝敗が付いた場合リトライ受付付きにする
			if(finish) {
				retryMenu_init(p, CONTINUE_REP_TRY_END);
			}
			else {
				retryMenu_init(p, CONTINUE_REP_NEX_END);
			}
		}
		// ストーリーモードの場合
		else {
			// ストーリーEXTRA
//			if(evs_one_game_flg) {
//				retryMenu_init(p, CONTINUE_REP_END);
//			}
//			else
			{
				state->cnd_now = dm_cnd_win_retry_sc;
				retryMenu_init(p, CONTINUE_REP_NEX_END);
			}
		}
	}
	// CPU の場合
	else {
		state->cnd_now  = dm_cnd_win;  // 状態を WIN に設定する
		state->mode_now = dm_mode_win; // 処理を WIN に設定する
	}

	return menu;
}

//## 敗北者の設定(2P,4P)
static bool dm_set_lose_2p(
	game_state *state, // ゲーム情報
	bool finish, // 決着が付いたか?
	bool menu    // メニューが設定されているか?
	)
{
	int p = state->player_no;

	// 絶対的コンディションを LOSE に設定する
	state->cnd_static = dm_cnd_lose;

	// 負けアニメーションを設定
	animeState_set(&state->anime, ASEQ_LOSE);

	// 負け演出の初期化
	effectLose_init(p);

	// MAN の場合
	if(!menu && state->player_type == PUF_PlayerMAN) {
		menu = true;

		state->cnd_now  = dm_cnd_lose_retry; // 状態を LOSE & RETRY に設定する
		state->mode_now = dm_mode_lose_retry; // 処理を LOSE & RETRY に設定する

		// ストーリーモードの場合
		if(evs_story_flg) {
			// ストーリーEXTRA
//			if(evs_one_game_flg) {
//				retryMenu_init(p, CONTINUE_REP_END);
//			}
//			else
			{
				// 状態を LOSE & RETRY & SCORE に設定する
				state->cnd_now = dm_cnd_lose_retry_sc;

				// ピーチ姫戦
				if(evs_story_no == EX_STAGE_NO) {
					// リトライ不能に設定する
					retryMenu_init(p, CONTINUE_REP_TRY_END);
				}
				else {
					// リトライ可能に設定する
					retryMenu_init(p, CONTINUE_REP_TRY_END);
				}
			}
		}
		// ストーリーモード以外の場合
		else {
			// 勝敗が付いた場合リトライ受付付きにする
			if(finish) {
				retryMenu_init(p, CONTINUE_REP_TRY_END);
			}
			else {
				retryMenu_init(p, CONTINUE_REP_NEX_END);
			}
		}
	}
	// CPU の場合
	else {
		state->cnd_now = dm_cnd_lose;  // 状態を LOSE に設定する
		state->mode_now = dm_mode_lose; // 処理を LOSE に設定する
	}

	return menu;
}

//## 引分の設定(2P,4P)
static bool dm_set_draw_2p(
	game_state *state, // ゲーム情報
	bool menu // メニューが設定されているか?
	)
{
	int p = state->player_no;

	// 絶対的コンディションを LOSE に設定する
	state->cnd_static = dm_cnd_draw;

	// DRAW 演出処理の初期化
	effectDraw_init(p);

	// 負けアニメーションを設定
	animeState_set(&state->anime, ASEQ_LOSE);

	// MAN の場合
	if(!menu && state->player_type == PUF_PlayerMAN) {
		menu = true;

		// DRAW & ANY KEY
		state->cnd_now = dm_cnd_draw_retry;
		state->mode_now = dm_mode_draw_retry;
		retryMenu_init(p, CONTINUE_REP_NEX_END);
	}
	// CPU の場合
	else {
		state->cnd_now = dm_cnd_draw; // DRAW
		state->mode_now = dm_mode_draw; // 処理を DRAW に設定
	}

	return menu;
}

//## タイムアタックの決着設定
static bool dm_set_time_attack_result_2p(game_state **state)
{
	Game *st = watchGame;
	bool menu, finish;
	u32 result[2];
	int i;

	menu = finish = false;

	result[0] = st->timeAttackResult[0].result;
	result[1] = st->timeAttackResult[1].result;

	for(i = 0; i < 2; i++) {
		// 勝利
		if(result[i] > result[i^1]) {
			finish = dm_add_win_2p(state[i]);
		}
	}

	// 勝利
	for(i = 0; i < 2; i++) {
		if(result[i] > result[i^1]) {
			menu = dm_set_win_2p(state[i], finish, menu);
		}
	}

	// 敗北
	for(i = 0; i < 2; i++) {
		if(result[i] < result[i^1]) {
			menu = dm_set_lose_2p(state[i], finish, menu);
		}
	}

	// 分け
	for(i = 0; i < 2; i++) {
		if(result[i] == result[i^1]) {
			menu = dm_set_draw_2p(state[i], menu);
			dm_seq_play_in_game(SEQ_End_C);
		}
	}

	return finish;
}

//## ゲーム処理関数(2P用)
static DM_GAME_RET dm_game_main_2p()
{
	Game *st = watchGame;
	game_state *state[2];
	game_map *map[2];
	bool finish, menu;
	int i, x, y;
	int clear, gover;
	int ret[2];

	finish = menu = false;
	clear = gover = 0;

	// テーブルを作成
	for(i = 0; i < 2; i++) {
		state[i] = &game_state_data[i];
		map[i] = game_map_data[i];
	}

	// ポーズ処理
	dm_set_pause_and_volume(state, 2);

#ifdef _ENABLE_CONTROL
	// デバッグ処理
	dm_set_debug_mode();
#endif // _ENABLE_CONTROL

	// メイン処理
	for(i = 0; i < 2; i++) {
		ret[i] = dm_game_main_cnt(state[i], map[i], i);
	}

	// つみあがり警告音を再生
	dm_warning_h_line_se();

	// カウントダウンSEを再生
	dm_play_count_down_se();

	// ウイルス設定完了
	if(ret[0] == dm_ret_virus_wait && ret[1] == dm_ret_virus_wait)
	{
		if(st->count_down_ctrl < 0) {
			for(i = 0; i < 2; i++) {
				if(state[i]->mode_now != dm_mode_wait) {
					// まだ配置が終わっていない
					break;
				}
			}

			// 配置完了?
			if(i == 2) {
				// タイマーカウント開始
				st->started_game_flg = 1;

				for(i = 0; i < 2; i++) {
					//カプセル落下開始
					state[i]->mode_now = dm_mode_down;

					// 落下前思考追加位置
					if(PLAYER_IS_CPU(state[i], i)) {
						aifMakeFlagSet(state[i]);
					}
				}

				// リプレイの 再生・録画 を開始
				start_replay_proc();
			}
		}
	}
	else {
		int waitTA, resultTA;

		for(i = 0; i < 2; i++) {
			// クリア
			if(ret[i] == dm_ret_clear) {
				if(evs_gamemode == GMD_TIME_ATTACK) {
					// ステージクリアロゴエフェクトを初期化
					effectNextStage_init(i);

					// 相方の決着がつくのを待つ設定
					state[i]->mode_now = dm_mode_clear_wait;
					state[i]->cnd_now = dm_cnd_clear_wait;
					state[i]->cnd_static = dm_cnd_clear_wait;

					// タイムアタックのスコア設定
					timeAttackResult_result(&st->timeAttackResult[i],
						state[i]->game_level, true,
						MAX(0, (int)SCORE_ATTACK_TIME_LIMIT - (int)evs_game_time),
						state[i]->total_chain_count,
						state[i]->total_erase_count,
						state[i]->game_score);

					// 勝利ＢＧＭ(short ver)
					_dm_seq_play_in_game(1, SEQ_End_BS);
				}
				else {
					clear++;

					// 絶対的コンディションを WIN に設定
					state[i]->cnd_static = dm_cnd_win;
				}
			}
			// ゲームオーバー側の設定
			else if(ret[i] == dm_ret_game_over) {

				if(evs_gamemode == GMD_TIME_ATTACK) {
					if(evs_game_time >= SCORE_ATTACK_TIME_LIMIT) {
						// ゲームオーバーロゴエフェクトを初期化
						effectGameOver_init(i);

						// 相方の決着がつくのを待つ設定
						state[i]->mode_now = dm_mode_gover_wait;
						state[i]->cnd_now = dm_cnd_gover_wait;
						state[i]->cnd_static = dm_cnd_gover_wait;
					}
					else {
						// リタイアロゴエフェクトを初期化
						effectRetire_init(i);

						// 相方の決着がつくのを待つ設定
						state[i]->mode_now = dm_mode_retire_wait;
						state[i]->cnd_now = dm_cnd_retire_wait;
						state[i]->cnd_static = dm_cnd_retire_wait;
					}

					// タイムアタックのスコア設定
					timeAttackResult_result(&st->timeAttackResult[i],
						state[i]->game_level, false,
						0,
						state[i]->total_chain_count,
						state[i]->total_erase_count,
						state[i]->game_score);

					// 勝利ＢＧＭ(short ver)
					_dm_seq_play_in_game(1, SEQ_End_CS);
				}
				else {
					gover++;

					// 絶対的コンディションを LOSE に設定
					state[i]->cnd_static = dm_cnd_lose;
				}

				// アニメーション速度を早くする
				state[i]->virus_anime_spead = v_anime_speed;

				state[i]->black_up_count = 16; // 黒上がり処理のカウンタの設定
				state[i]->flg_retire     = 1;  // リタイアフラグを立てる
			}
			// RETRY
			else if(ret[i] == dm_ret_retry) {
				return dm_ret_retry;
			}
			// REPLAY
			else if(ret[i] == dm_ret_replay) {
				return dm_ret_replay;
			}
			// 終了
			else if(ret[i] == dm_ret_end) {
				return dm_ret_game_over;
			}
			// 完全終了
			else if(ret[i] == dm_ret_game_end) {
				return dm_ret_game_end;
			}
		}

		// タイムアタックの決着した人数を求める
		waitTA = resultTA = 0;
		for(i = 0; i < 2; i++) {
			switch(state[i]->cnd_now) {
			case dm_cnd_clear_wait:
			case dm_cnd_gover_wait:
			case dm_cnd_retire_wait:
				if(st->effect_timer[i] == 0) {
					waitTA++;
				}
				break;
			case dm_cnd_clear_result:
			case dm_cnd_gover_result:
			case dm_cnd_retire_result:
				if(timeAttackResult_isEnd(&st->timeAttackResult[i])) {
					resultTA++;
				}
				break;
			}
		}

		// タイムアタックの決着が付いた
		if(waitTA == 2) {
			for(i = 0; i < 2; i++) {
				// タイムアタックのスコア表示
				switch(state[i]->cnd_now) {
				case dm_cnd_clear_wait:
					state[i]->cnd_static = dm_cnd_clear_result;
					state[i]->cnd_now    = dm_cnd_clear_result;
					state[i]->mode_now   = dm_mode_clear_result;
					break;
				case dm_cnd_gover_wait:
					state[i]->cnd_static = dm_cnd_gover_result;
					state[i]->cnd_now    = dm_cnd_gover_result;
					state[i]->mode_now   = dm_mode_gover_result;
					break;
				case dm_cnd_retire_wait:
					state[i]->cnd_static = dm_cnd_retire_result;
					state[i]->cnd_now    = dm_cnd_retire_result;
					state[i]->mode_now   = dm_mode_retire_result;
					break;
				}
			}
		}
		// タイムアタックのスコア表示が完了した
		else if(resultTA == 2) {
			finish = dm_set_time_attack_result_2p(state);
		}
		else if(clear == 2 || gover == 2) {
			for(i = 0; i < 2; i++) {
				// 引き分けの設定
				menu = dm_set_draw_2p(state[i], menu);
			}
			// サウンドを再生
			dm_seq_play_in_game(SEQ_End_C);
		}
		else if(clear) {
			// サウンドの設定とゲーム終了判定の前設定
			for(i = 0; i < 2; i++) {
				if(state[i]->cnd_static == dm_cnd_win) {
					finish = dm_add_win_2p(state[i]);
				}
			}

			// 勝利者の設定
			for(i = 0; i < 2; i++) {
				if(state[i]->cnd_static == dm_cnd_win) {
					menu = dm_set_win_2p(state[i], finish, menu);
				}
			}

			// 敗北者の設定
			for(i = 0; i < 2; i++) {
				if(state[i]->cnd_static != dm_cnd_win) {
					menu = dm_set_lose_2p(state[i], finish, menu);
				}
			}

		}
		else if(gover) {
			// サウンドの設定とゲーム終了判定の前設定
			for(i = 0; i < 2; i++) {
				if(state[i]->cnd_static != dm_cnd_lose) {
					finish = dm_add_win_2p(state[i]);
				}
			}

			// 勝利者の設定
			for(i = 0; i < 2; i++) {
				if(state[i]->cnd_static != dm_cnd_lose) {
					menu = dm_set_win_2p(state[i], finish, menu);
				}
			}

			// 敗北者の設定
			for(i = 0; i < 2; i++) {
				if(state[i]->cnd_static == dm_cnd_lose) {
					menu = dm_set_lose_2p(state[i], finish, menu);
				}
			}
		}

		// セーブ
		if(finish) {
			dm_save_all();
		}
	}

	return dm_ret_null;
}

//## ゲーム処理関数(4P用)
static DM_GAME_RET dm_game_main_4p()
{
	Game *st = watchGame;
	game_state *state[4];
	game_map *map[4];
	DM_GAME_RET ret[4];
	int i;

	// テーブルを作成
	for(i = 0; i < 4; i++) {
		state[i] = &game_state_data[i];
		map[i] = game_map_data[i];
	}

	// ポーズ処理
	dm_set_pause_and_volume(state, 4);

#ifdef _ENABLE_CONTROL
	// デバッグ処理
	dm_set_debug_mode();
#endif // _ENABLE_CONTROL

	// メイン処理
	for(i = 0; i < 4; i++) {
		ret[i] = dm_game_main_cnt(state[i], map[i], i);
	}

	// つみあがり警告音を再生
	dm_warning_h_line_se();

	if(ret[0] == dm_ret_virus_wait && ret[1] == dm_ret_virus_wait &&
		ret[2] == dm_ret_virus_wait && ret[3] == dm_ret_virus_wait)
	{
		if(st->count_down_ctrl < 0) {
			for(i = 0; i < 4; i++) {
				if(state[i]->mode_now != dm_mode_wait) {
					// まだ配置が終わっていない
					break;
				}
			}

			// ウイルス設定完了?
			if(i == 4) {
				// タイマーカウント開始
				st->started_game_flg = 1;

				for(i = 0; i < 4; i++) {
					//カプセル落下開始
					state[i]->mode_now = dm_mode_down;

					// 落下前思考追加位置
					if(PLAYER_IS_CPU(state[i], i)) {
						aifMakeFlagSet(state[i]);
					}
				}

				// リプレイの 再生・録画 を開始
				start_replay_proc();
			}
		}
	}
	else {
		bool finish, menu;
		int clear, gover, sound, win_team;

		finish = menu = false;
		clear = gover = 0;

		for(i = 0; i < 4; i++) {
			// ウィルスを再配置
			if(ret[i] == dm_ret_virus_wait &&
				state[i]->cnd_training == dm_cnd_training)
			{
				if(state[i]->mode_now == dm_mode_wait) {
					state[i]->mode_now = dm_mode_down;
				}
			}
			// クリア
			else if(ret[i] == dm_ret_clear) {
				clear++;

				// 絶対的コンディションを WIN に設定する
				state[i]->cnd_static = dm_cnd_win;
			}
			// ゲームオーバー側の設定
			else if(ret[i] == dm_ret_game_over) {
				// リタイア状態じゃない場合
				if(!state[i]->flg_retire) {
					gover++;

					state[i]->cnd_now        = dm_cnd_retire;    // リタイア状態にする
					state[i]->cnd_training   = dm_cnd_retire;    // リタイア状態にする
					state[i]->cnd_static     = dm_cnd_lose;      // 絶対的コンディションを LOSE に設定する
					state[i]->black_up_count = 16;               // 黒上がり用カウンタのセット
					state[i]->flg_retire     = 1;                // リタイアフラグを立てる

					// ＣＰＵなら練習しない
					if(state[i]->player_type == PUF_PlayerCPU) {
						state[i]->mode_now = dm_mode_no_action; // 処理を NO ACTION (何もしない) に設定する
					}
					else {
						state[i]->cnd_now  = dm_cnd_tr_chack;   // 状態を練習確認に設定する
						state[i]->mode_now = dm_mode_tr_chaeck; // 処理を練習確認に設定する
					}
				}
				// 練習中の場合
				else {
					state[i]->cnd_now  = dm_cnd_tr_chack;   // 状態を練習確認に設定する
					state[i]->mode_now = dm_mode_tr_chaeck; // 処理を練習確認に設定する
				}

				// アニメーション速度を早くする
				state[i]->virus_anime_spead = v_anime_speed_4p;
			}
			// リトライ
			else if(ret[i] == dm_ret_retry) {
				return dm_ret_retry;
			}
			// REPLAY
			else if(ret[i] == dm_ret_replay) {
				return dm_ret_replay;
			}
			// 終了
			else if(ret[i] == dm_ret_end) {
				return dm_ret_game_over;
			}
			// 完全終了
			else if(ret[i] == dm_ret_game_end) {
				return dm_ret_game_end;
			}
			// 練習攻撃無し
			else if(ret[i] == dm_ret_tr_a) {
				state[i]->cnd_static   = dm_cnd_wait;      // 絶対的コンディションを 通常 に設定する
				state[i]->flg_training = 0;                // 攻撃（被弾）無しに設定する
				state[i]->cnd_training = dm_cnd_training;  // 状態を練習に設定する
				state[i]->mode_now     = dm_mode_training; // 処理を練習に設定する
			}
			// 練習攻撃あり
			else if(ret[i] == dm_ret_tr_b) {
				state[i]->cnd_static   = dm_cnd_wait;      // 絶対的コンディションを 通常 に設定する
				state[i]->flg_training = 1;                // 攻撃（被弾）有りに設定する
				state[i]->cnd_training = dm_cnd_training;  // 状態を練習に設定する
				state[i]->mode_now     = dm_mode_training; // 処理を練習に設定する
			}
		}

		// 誰かがクリアした
		if(clear) {
			int clearBit = 0;

			for(i = 0; i < 4; i++) {
				if(state[i]->cnd_static == dm_cnd_win) {
					clearBit |= 1 << i;
					win_team = state[i]->team_no;
				}
			}

			// バトルロイヤルで、クリア人数が２人以上の場合 DRAW
			if(!st->vs_4p_team_flg && clear > 1) {
				for(i = 0; i < 4; i++) {
					menu = dm_set_draw_2p(state[i], menu);
				}
				// サウンドを設定
				sound = SEQ_End_C;
			}
			// チームバトルで、両方のチームにクリアしたものがい場合 DRAW
			else if(st->vs_4p_team_flg &&
				(clearBit & st->vs_4p_team_bits[TEAM_MARIO]) &&
				(clearBit & st->vs_4p_team_bits[TEAM_ENEMY]))
			{
				for(i = 0; i < 4; i++) {
					menu = dm_set_draw_2p(state[i], menu);
				}
				// サウンドを設定
				sound = SEQ_End_C;
			}
			else {
				sound = SEQ_End_A;

				for(i = 0; i < 4; i++) {
					// 勝利チームのカウントを追加する
					// (チーム対抗戦でない場合はチーム番号が別々なので問題なし)
					if(state[i]->cnd_static == dm_cnd_win) {
						int team = state[i]->team_no;
						int win  = st->win_count[team];

						if(evs_story_flg) {
							st->star_pos_x[st->star_count] = _posStP4StarX[i];
							st->star_pos_y[st->star_count] = _posStP4StarY;
						}
						else if(st->vs_4p_team_flg) {
							st->star_pos_x[st->star_count] = _posP4TeamStarX[evs_vs_count - 1][team][win];
							st->star_pos_y[st->star_count] = _posP4TeamStarY;
						}
						else {
							st->star_pos_x[st->star_count] = _posP4CharStarX[evs_vs_count - 1][team][win];
							st->star_pos_y[st->star_count] = _posP4CharStarY;
						}

						st->star_count++;
						st->win_count[team]++;

						if(evs_story_flg) {
							finish = true;
							// SEはあとで決めます
						}
						// 勝負が付いた(３本先取)
						else if(st->win_count[team] == evs_vs_count) {
							// 勝負が付いたフラグを立てる
							finish = true;
							sound = SEQ_End_B;
						}
					}
				}

				// 勝者の設定
				for(i = 0; i < 4; i++) {
					if(state[i]->team_no == win_team) {
						if(evs_story_flg && state[i]->player_type == PUF_PlayerMAN) {
							sound = SEQ_End_A;
						}
						menu = dm_set_win_2p(state[i], finish, menu);
					}
				}

				// 敗者の設定
				for(i = 0; i < 4; i++) {
					if(state[i]->team_no != win_team) {
						if(evs_story_flg && state[i]->player_type == PUF_PlayerMAN) {
							sound = SEQ_End_C;
						}
						menu = dm_set_lose_2p(state[i], finish, menu);
					}
				}
			}

			// BGM の設定
			dm_seq_play_in_game(sound);
		}
		else if(gover) {
			bool end = false;
			int retire = 0, retireBit = 0;

			for(i = 0; i < 4; i++) {
				// リタイアしている
				if(state[i]->flg_retire) {
					retire++;
					retireBit |= 1 << i;
				}
			}

			// 全員リタイア(DRAW)
			if(retire == 4) {
				for(i = 0; i < 4; i++) {
					menu = dm_set_draw_2p(state[i], menu);
				}
				// サウンドを再生
				dm_seq_play_in_game(SEQ_End_C);
			}
			// ストーリーでプレイヤーが敗北
			else if(evs_story_flg && (retireBit & 1)) {
				finish = true;
				menu = dm_set_lose_2p(state[0], finish, menu);
				for(i = 1; i < 4; i++) {
					menu = dm_set_win_2p(state[i], finish, menu);
				}
				// サウンドを再生
				dm_seq_play_in_game(SEQ_End_C);
			}
			// バトルロイヤル(1P vs 2P vs 3P vs 4P)の場合
			// リタイアが ３人のとき
			else if(!st->vs_4p_team_flg && retire == 3) {
				int team, win;

				end = true;
				sound = SEQ_End_A;

				for(i = 0; i < 4; i++) {
					if(state[i]->flg_retire) continue;

					team = state[i]->team_no;
					win  = st->win_count[team];

					if(evs_story_flg) {
						st->star_pos_x[st->star_count] = _posStP4StarX[i];
						st->star_pos_y[st->star_count] = _posStP4StarY;
					}
					else {
						st->star_pos_x[st->star_count] = _posP4CharStarX[evs_vs_count - 1][team][win];
						st->star_pos_y[st->star_count] = _posP4CharStarY;
					}
					st->star_count++;
					st->win_count[team]++;
					win_team = team;

					if(evs_story_flg) {
						if(state[i]->player_type != PUF_PlayerMAN) {
							sound = SEQ_End_C;
						}
						finish = true;
					}
					else if(st->win_count[team] == evs_vs_count) {
						// 勝負が決まったに設定する
						finish = true;
						sound = SEQ_End_B;
					}

					break;
				}
			}
			// チーム対抗戦の場合
			else if(st->vs_4p_team_flg) {
				int team, win;

				if((retireBit & st->vs_4p_team_bits[TEAM_MARIO]) == st->vs_4p_team_bits[TEAM_MARIO]) {
					team = TEAM_ENEMY;
					win  = st->win_count[team];

					st->win_count[team]++; // ENEMY チームの取得ポイント加算
					win_team = team;       // 勝利チームの設定をする
					end = true;            // １試合終了の設定をする
				}
				else if((retireBit & st->vs_4p_team_bits[TEAM_ENEMY]) == st->vs_4p_team_bits[TEAM_ENEMY]) {
					team = TEAM_MARIO;
					win  = st->win_count[team];

					st->win_count[team]++; // MARIO チームの取得ポイント加算
					win_team = team;       // 勝利チームの設定をする
					end = true;            // １試合終了の設定をする
				}

				// スターグラフィックを追加
				if(end) {
					st->star_pos_x[st->star_count] = _posP4TeamStarX[evs_vs_count - 1][team][win];
					st->star_pos_y[st->star_count] = _posP4TeamStarY;
					st->star_count++;

					// 勝敗が決まった場合、試合終了の設定をする
					sound = SEQ_End_A;
					for(i = 0; i < 4; i++) {
						if(st->win_count[i] == evs_vs_count) {
							finish = true;
							sound = SEQ_End_B;
							break;
						}
					}
				}
			}

			if(end) {
				// 勝者の設定
				for(i = 0; i < 4; i++) {
					if(state[i]->team_no == win_team) {
						menu = dm_set_win_2p(state[i], finish, menu);
					}
				}

				// 敗者の設定
				for(i = 0; i < 4; i++) {
					if(state[i]->team_no != win_team) {
						menu = dm_set_lose_2p(state[i], finish, menu);
					}
				}

				dm_seq_play_in_game(sound);
			}
		}

		// セーブ
		if(finish) {
			dm_save_all();
		}
	}

	return dm_ret_null;
}

//## ゲームデモ処理関数(1P用)
static DM_GAME_RET dm_game_demo_1p()
{
	Game *st = watchGame;
	DM_GAME_RET ret;
	int i;

	ret = dm_game_main_cnt_1P(&game_state_data[0], game_map_data[0], 0);

	// つみあがり警告音を再生
	dm_warning_h_line_se();

	// 巨大ウイルスアニメーション
	for(i = 0; i < 3; i++) {
		animeState_update(&st->virus_anime_state[i]);
		animeSmog_update(&st->virus_smog_state[i]);
	}

	// 巨大ウィルスの座標計算
	dm_calc_big_virus_pos( &game_state_data[0] );

	// ウイルス配置完了
	if(ret == dm_ret_virus_wait) {
		if(st->count_down_ctrl < 0) {
			//カプセル落下開始
			game_state_data[0].mode_now = dm_mode_throw;
//			game_state_data[0].mode_now = dm_mode_down;

			// mario throw
			animeState_set(&game_state_data[0].anime, ASEQ_ATTACK);

			if(PLAYER_IS_CPU(&game_state_data[0], 0)) {
				aifMakeFlagSet(&game_state_data[0]);
			}

			// タイマーカウント開始
			st->started_game_flg = 1;
		}
	}

	if(st->demo_timer) {
		st->demo_timer--;

		if(_getKeyTrg(0) & DM_ANY_KEY) {
			st->demo_timer = 0;
		}

		// デモ中断
		if(st->demo_timer == 0) {
			return dm_ret_next_stage;
		}
	}

	return dm_ret_null;
}

//## ゲームデモ処理関数(2P用)
static DM_GAME_RET dm_game_demo_2p()
{
	Game *st = watchGame;
	DM_GAME_RET ret[2];
	int i;

	for(i = 0; i < 2; i++) {
		ret[i] = dm_game_main_cnt(&game_state_data[i], game_map_data[i], i);
	}

	// つみあがり警告音を再生
	dm_warning_h_line_se();


	// ウイルス設定完了
	if(ret[0] == dm_ret_virus_wait && ret[1] == dm_ret_virus_wait) {
		if(st->count_down_ctrl < 0) {
			for(i = 0; i < 2; i++) {
				//カプセル落下開始
				game_state_data[i].mode_now = dm_mode_down;

				// 落下前思考追加位置
				if(PLAYER_IS_CPU(&game_state_data[i], i)) {
					aifMakeFlagSet(&game_state_data[i]);
				}
			}
			// タイマーカウント開始
			st->started_game_flg = 1;
		}
	}

	if(st->demo_timer) {
		st->demo_timer--;

		if(_getKeyTrg(0) & DM_ANY_KEY) {
			st->demo_timer = 0;
		}

		// デモ中断
		if(st->demo_timer == 0) {
			return dm_ret_next_stage;
		}
	}

	return dm_ret_null;
}

//## ゲームデモ処理関数(4P用)
static DM_GAME_RET dm_game_demo_4p()
{
	Game *st = watchGame;
	DM_GAME_RET ret[4];
	int i;

	for(i = 0; i < 4; i++) {
		ret[i] = dm_game_main_cnt(&game_state_data[i], game_map_data[i], i);
	}

	// つみあがり警告音を再生
	dm_warning_h_line_se();

	// ウイルス設定完了
	if(ret[0] == dm_ret_virus_wait && ret[1] == dm_ret_virus_wait &&
		ret[2] == dm_ret_virus_wait && ret[3] == dm_ret_virus_wait)
	{
		if(st->count_down_ctrl < 0) {
			for(i = 0; i < 4; i++) {
				//カプセル落下開始
				game_state_data[i].mode_now = dm_mode_down;

				// 落下前思考追加位置
				if(PLAYER_IS_CPU(&game_state_data[i], i)) {
					aifMakeFlagSet(&game_state_data[i]);
				}
			}
			// タイマーカウント開始
			st->started_game_flg = 1;
		}
	}

	if(st->demo_timer) {
		st->demo_timer--;

		if(_getKeyTrg(0) & DM_ANY_KEY) {
			st->demo_timer = 0;
		}

		// デモ中断
		if(st->demo_timer == 0) {
			return dm_ret_next_stage;
		}
	}

	return dm_ret_null;
}

//////////////////////////////////////////////////////////////////////////////
//{### グラフィックデータ

//## 各種テクスチャのラベル
#include "texture/game/game_al.h"
#include "texture/game/game_p1.h"
#include "texture/game/game_p2.h"
#include "texture/game/game_p4.h"
#include "texture/game/game_ls.h"
#include "texture/game/game_item.h"
#include "texture/menu/menu_kasa.h"

//## カプセルのグラフィックを取得
STexInfo *dm_game_get_capsel_tex(int sizeIndex)
{
	static const int cap_tex[] = { TEX_MITEM10R, TEX_MITEM08R };
	Game *st = watchGame;
	return st->texItem + cap_tex[sizeIndex];
}

//## カプセルのパレットを取得
STexInfo *dm_game_get_capsel_pal(int sizeIndex, int colorIndex)
{
	static const int cap_pal[][6] = {
		{ TEX_MITEM10R, TEX_MITEM10Y, TEX_MITEM10B,
		  TEX_MITEM10RX,TEX_MITEM10YX,TEX_MITEM10BX },
		{ TEX_MITEM08R, TEX_MITEM08Y, TEX_MITEM08B,
		  TEX_MITEM08RX,TEX_MITEM08YX,TEX_MITEM08BX },
	};
	Game *st = watchGame;
	return st->texItem + cap_pal[sizeIndex][colorIndex];
}

//////////////////////////////////////////////////////////////////////////////
//{### 描画物

//## スコア表示 : スコアを描画
static void scoreNums_draw(_ScoreNums *st, Gfx **gpp)
{
	Game *g = watchGame;
	Gfx *gp = *gpp;
	_ScoreNum *num;
	STexInfo *texC = g->texAL + TEX_rank_figure;
	STexInfo *texA = g->texAL + TEX_rank_figure_alpha;
	int width = MIN(texC->size[0], texA->size[0]);
	int height = texC->size[1] / 12;
	int i, x, y, alpha;
	float time, bound;

	gSPDisplayList(gp++, alpha_texture_init_dl);
	gDPSetCombineMode(gp++, G_CC_ALPHATEXTURE_PRIM, G_CC_PASS2);

	for(i = 0; i < ARRAY_SIZE(st->numbers); i++) {
		num = &st->numbers[WrapI(0, ARRAY_SIZE(st->numbers), st->index + i)];
		time = num->time;

		if(time == 1.0) {
			continue;
		}

		if(time < 0.2) {
			alpha = time * (255 * 5);
		}
		else if(time < 0.8) {
			alpha = 255;
		}
		else {
			alpha = (1 - time) * (255 * 5);
		}

		bound = MIN(0.5, time) * (1.0 / 0.25) - 1.0;
		bound = (1.0 - bound * bound) * 8.0;

		gDPSetPrimColor(gp++, 0,0,
			_scoreNumsColor[num->color][0], _scoreNumsColor[num->color][1],
			_scoreNumsColor[num->color][2], alpha);

		StretchAlphaTexBlock(&gp, width, height,
			(u8 *)texC->addr[1] + texC->size[0] * height * num->number * 2, texC->size[0],
			(u8 *)texA->addr[1] + texA->size[0] * height * num->number / 2, texA->size[0],
			num->pos[0], num->pos[1] - bound, width, height);
	}

	*gpp = gp;
}

//## 星を初期化
static void starForce_init(_StarForce *star, int *xx, int *yy)
{
	int i;

	// 座標テーブル
	star->xtbl = xx;
	star->ytbl = yy;

	// フレームカウンタ
	for(i = 0; i < ARRAY_SIZE(star->frame); i++) {
		star->frame[i] = 0;
	}
}

//## 星を計算
static void starForce_calc(_StarForce *star, int count)
{
	int i;

	for(i = 0; i < count; i++) {
		if(star->frame[i] < 48) {
			star->frame[i]++;
		}
		else {
			star->frame[i] = WrapI(48, 48 + 60, star->frame[i] + 1);
		}
	}
}

//## 星を描画
static void starForce_draw(_StarForce *star, Gfx **gpp, int count)
{
	Game *st = watchGame;
	Gfx *gp = *gpp;
	STexInfo *texC, *texA;
	int i, j;

	gSPDisplayList(gp++, alpha_texture_init_dl);
	gDPSetCombineMode(gp++, G_CC_ALPHATEXTURE_PRIM, G_CC_PASS2);

	texC = st->texAL + TEX_new_star;
	texA = st->texAL + TEX_new_star_alpha;

	// 星を描画
	for(i = 0; i < count; i++) {
		if(star->frame[i] < 48) {
			gDPSetPrimColor(gp++, 0,0, 255,255,255, 255);
			tiStretchAlphaTexItem(&gp, texC, texA, false, 16, 0,
				star->xtbl[i], star->ytbl[i], texC->size[0], texC->size[1] / 16);
		}

		j = MIN(255, star->frame[i] << 3);
		gDPSetPrimColor(gp++, 0,0, 255,255,255, j);

		j = MAX(0, star->frame[i] - 48) >> 2;
		tiStretchAlphaTexItem(&gp, texC, texA, false, 16, j + 1,
			star->xtbl[i], star->ytbl[i], texC->size[0], texC->size[1] / 16);
	}

	// 星屑を描画
	gSPDisplayList(gp++, normal_texture_init_dl);
	gDPSetCombineMode(gp++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
	gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
	gDPSetPrimColor(gp++, 0,0, 255,255,200, 255);
	gDPSetTextureLUT(gp++, G_TT_NONE);

	for(i = 0; i < count; i++) {
		j = star->frame[i] >> 2;
		if(j >= 12) continue;

		texC = st->texAL + TEX_STARDUST01 + j;
		StretchTexTile4i(&gp,
			texC->size[0], texC->size[1], texC->addr[1],
			0, 0, texC->size[0], texC->size[1],
			star->xtbl[i], star->ytbl[i], texC->size[0], texC->size[1]);
	}

	*gpp = gp;
}

//## 星の下地を描画
static void draw_star_base(Gfx **gpp, int x, int y, int cached)
{
	Game *st = watchGame;
	Gfx *gp = *gpp;
	STexInfo *texC, *texA;

	texC = st->texAL + TEX_new_star;
	texA = st->texAL + TEX_new_star_alpha;

	if(!cached) {
		gSPDisplayList(gp++, alpha_texture_init_dl);
	}

	tiStretchAlphaTexItem(&gp, texC, texA, cached, 16, 0,
		x, y, texC->size[0], texC->size[1] / 16);

	*gpp = gp;
}

//## 4P,PAUSE 時の攻撃対象パネル
static void draw_4p_attack_guide_panel(Gfx **gpp, int playerCount, int playerNo, int x, int y)
{
	static const u8 _tbl[][4] = {
		{ 0, 5, 8, 10 },
		{ 0, 1, 6,  9 },
		{ 0, 1, 2,  4 },
		{ 0, 1, 2,  3 },
	};

	Game *st = watchGame;
	Gfx *gp = *gpp;
	STexInfo *texC, *texA;
	int i, j;

	gSPDisplayList(gp++, normal_texture_init_dl);

	// 下地
	texC = st->texP4 + TEX_P4_CP_PANEL;
	tiStretchTexItem(&gp, texC, false,
		4, playerNo, x, y, texC->size[0], texC->size[1] / 4);

	gSPDisplayList(gp++, alpha_texture_init_dl);

	// プレイヤー番号
	for(i = 0; i < 3; i++) {
		float xx, yy;
		int a, b;
		xx = x + 36;
		yy = y + 5 + i * 13;

		a = game_state_data[playerNo].team_no;
		b = game_state_data[(playerNo + 1 + i) % 4].team_no;

		if(a == b) {
			texC = st->texP4 + TEX_stock_a + a;
			texA = st->texP4 + TEX_stock_alpha;
			StretchAlphaTexBlock(&gp, texC->size[0], texC->size[1],
				texC->addr[1], texC->size[0], texA->addr[1], texA->size[0],
				xx, yy+1, texC->size[0], texC->size[1]);
		}
		else {
			texC = st->texP4 + TEX_ATTACK_P;
			texA = st->texP4 + TEX_ATTACK_P_ALPHA;
			j = _tbl[playerCount - 1][(playerNo + i + 1) % 4];
			tiStretchAlphaTexItem(&gp, texC, texA, false,
				11, j, xx, yy, texC->size[0], texC->size[1] / 11);
		}
	}

	*gpp = gp;
}

//## ゲーム描画関数マップ描画
static void dm_map_draw(game_map *map, u8 col_no, s16 x_p, s16 y_p, s8 size)
{
	int i;

	for(i = 0; i < GAME_MAP_W * (GAME_MAP_H + 1); i++) {
		// ウイルス(カプセル)の表示フラグがＯＮのとき
		// 色（Ｒ・Ｂ・Ｙ）が等しいとき
		if(map[i].capsel_m_flg[cap_disp_flg]
		&& map[i].capsel_m_p == col_no)
		{
			gSPTextureRectangle(gp++,
				map[i].pos_m_x * size + x_p << 2,
				map[i].pos_m_y * size + y_p << 2,
				map[i].pos_m_x * size + x_p + size << 2,
				map[i].pos_m_y * size + y_p + size << 2,
				G_TX_RENDERTILE,
				0, map[i].capsel_m_g * size << 5,
				1 << 10, 1 << 10);
		}
	}
}

//## カプセルの落下地点を検索
static void dm_find_fall_point(
	game_map *map,
	game_cap *cap,
	int       fallPosY[2])
{
	int i, y, minY = 16;

	for(i = 0; i < 2; i++) {
		for(y = MAX(1, cap->pos_y[i]); y <= 16; y++) {
			if(get_map_info(map, cap->pos_x[i], y)) {
				minY = MIN(minY, y - 1);
				break;
			}
		}
	}

	fallPosY[0] = minY - (cap->pos_y[0] < cap->pos_y[1] ? 1 : 0);
	fallPosY[1] = minY - (cap->pos_y[0] > cap->pos_y[1] ? 1 : 0);
}

//## ウィルス数を描画
static void draw_virus_number(Gfx **gpp, u32 number, int x, int y, float sx, float sy)
{
	static const char _tbl[] = { 9,0,1,2,3,4,5,6,7,8 };
	Game *st = watchGame;
	STexInfo *texC = st->texAL + TEX_AL_VIRUS_NUMBER;
	STexInfo *texA = st->texAL + TEX_AL_VIRUS_NUMBER_ALPHA;
	int i, xx, yy, tmp[16], column = 0;
	int width = MIN(texC->size[0], texA->size[0]);
	int height = texC->size[1] / 10;

	do {
		tmp[column++] = number % 10;
		number /= 10;
	} while(number);

	xx = column * -7;
	yy = height / -2;

	for(i = column - 1; i >= 0; i--) {
		StretchAlphaTexBlock(gpp,
			width, height,
			(u8 *)texC->addr[1] + texC->size[0] * height * _tbl[tmp[i]] * 2,
			texC->size[0],
			(u8 *)texA->addr[1] + texA->size[0] * height * _tbl[tmp[i]] / 2,
			texA->size[0],
			x + xx * sx, y + yy * sx, width * sx, height * sx);
		xx += 14;
	}
}

//## スコア表示用の数字を描画
static void draw_count_number(Gfx **gpp, int color, int column, u32 number, int x, int y)
{
	static const int _tex[] = { TEX_count_num, TEX_count_1p_num, TEX_count_2p_num };
	static const int _row[] = { 13, 12, 12 };
	Game *st = watchGame;
	STexInfo *texC = st->texP1 + _tex[color];
	STexInfo *texA = st->texP1 + TEX_count_num_alpha;
	int i, tmp[16];
	int width = MIN(texC->size[0], texA->size[0]);
	int height = texC->size[1] / _row[color];

	for(i = 0; i < column; i++) {
		tmp[i] = number % 10;
		number /= 10;
	}

	switch(column) {
	case -1:
		tmp[0] = 10;
		column = 1;
		break;
	case -2:
		tmp[0] = 11;
		column = 1;
		break;
	case -3:
		tmp[0] = 12;
		column = 1;
		break;
	}

	for(i = column - 1; i >= 0; i--) {
		StretchAlphaTexBlock(gpp,
			width, height,
			(u8 *)texC->addr[1] + texC->size[0] * height * tmp[i] * 2, texC->size[0],
			(u8 *)texA->addr[1] + texA->size[0] * height * tmp[i] / 2, texA->size[0],
			x, y, width, height);
		x += 9;
	}
}

#if 0
/*
//## 時間表示用の数字を描画
static void draw_time_number(Gfx **gpp, u32 number, int column, int x, int y)
{
	Game *st = watchGame;
	STexInfo *texC = st->texP1 + TEX_P1_VS_TIME_NUMBER;
	STexInfo *texA = st->texP1 + TEX_P1_VS_TIME_NUMBER_ALPHA;
	int i, tmp[16];
	int width = MIN(texC->size[0], texA->size[0]);
	int height = texC->size[1] / 12;

	for(i = 0; i < column; i++) {
		tmp[i] = number % 10 + 2;
		number /= 10;
	}

	switch(column) {
	case -1:
		tmp[0] = 0;
		column = 1;
		break;
	case -2:
		tmp[0] = 1;
		column = 1;
		break;
	}

	for(i = column - 1; i >= 0; i--) {
		StretchAlphaTexBlock(gpp,
			width, height,
			(u8 *)texC->addr[1] + texC->size[0] * height * tmp[i] * 2, texC->size[0],
			(u8 *)texA->addr[1] + texA->size[0] * height * tmp[i] / 2, texA->size[0],
			x, y, width, height);
		x += 10;
	}
}
*/
#endif

//## 時間を表示
static void draw_time(Gfx **gpp, u32 time, int x, int y)
{
	static const u8 _pos[] = { 0, 17, 26 };
	static const s8 _col[] = { 2, -3, 2 };
	int i, t[3];
	u32 m, s;

	time /= FRAME_PAR_SEC;
	t[2] = time % 60;

	time /= 60;
	t[0] =  time % 100;

	t[1] = 0;

	for(i = 0; i < 3; i++) {
		draw_count_number(gpp, 0, _col[i], t[i], x + _pos[i], y);
	}
}

//## 時間を表示(ミリ秒単位)
static void draw_time2(Gfx **gpp, u32 time, int x, int y)
{
	static const u8 _pos[] = { 0, 6, 13, 28, 35 };
	static const s8 _col[] = { 1, -3, 2, -3, 1 };
	int i, t[5];

	t[1] = t[3] = 0;

	time /= FRAME_PAR_MSEC;
	t[4] = time % 10;

	time /= 10;
	t[2] = time % 60;

	time /= 60;
	t[0] = time % 60;

	for(i = 0; i < 5; i++) {
		draw_count_number(gpp, 0, _col[i], t[i], x + _pos[i], y);
	}
}

//## push_any_key を描画
void push_any_key_draw(int x_pos, int y_pos)
{
	Game *st = watchGame;
	STexInfo *texC, *texA;
	int width, alpha;

	alpha = sins(st->blink_count << 10) * (255.f / 32768.f) + 127.f;
	alpha = CLAMP(0, 255, alpha);

	gSPDisplayList(gp++, alpha_texture_init_dl);
	gDPSetCombineMode(gp++, G_CC_ALPHATEXTURE_PRIM, G_CC_PASS2);
	gDPSetPrimColor(gp++, 0,0, 255,255,255, alpha);

	texC = st->texAL + TEX_AL_INFO;
	texA = st->texAL + TEX_AL_INFO_ALPHA;
	width = MIN(texC->size[0], texA->size[0]);

	StretchAlphaTexTile(&gp, width, texC->size[1],
		texC->addr[1], texC->size[0],
		texA->addr[1], texA->size[0],
		0, 0, width, texC->size[1],
		x_pos, y_pos, width, texC->size[1]);
}

//## デモロゴを表示
static void draw_demo_logo(Gfx **gpp, int x, int y)
{
	static const int _tex[][2] = {
		{ TEX_AL_TITLE_01, TEX_AL_TITLE_01_ALPHA },
		{ TEX_AL_TITLE_02, TEX_AL_TITLE_02_ALPHA },
		{ TEX_AL_LOGO, TEX_AL_LOGO_ALPHA },
	};
	Game *st = watchGame;
	Gfx *gp = *gpp;
	STexInfo *texC, *texA;
	int i, width, height, alpha[3];

	alpha[0] = sins(st->blink_count << 10) * (255.f / 32768.f) + 127.f;
	alpha[0] = CLAMP(0, 255, alpha[0]);
	alpha[1] = 255 - alpha[0];
	alpha[2] = 255;

	gSPDisplayList(gp++, alpha_texture_init_dl);
	gDPSetCombineMode(gp++, G_CC_ALPHATEXTURE_PRIM, G_CC_PASS2);

	for(i = 0; i < ARRAY_SIZE(_tex); i++) {
		if(i == 2) {
			x = 14;
			y = 18;
		}
		texC = st->texAL + _tex[i][0];
		texA = st->texAL + _tex[i][1];
		width = MIN(texC->size[0], texA->size[0]);
		height = MIN(texC->size[1], texA->size[1]);
		gDPSetPrimColor(gp++, 0,0, 255,255,255, alpha[i]);
		StretchAlphaTexTile(&gp, width, height,
			texC->addr[1], texC->size[0], texA->addr[1], texA->size[0],
			0, 0, width, height, x, y, width, height);
	}

	*gpp = gp;
}

//## リプレイロゴを表示
static void draw_replay_logo(Gfx **gpp, int x, int y)
{
	static const int _tex[][2] = {
		{ TEX_AL_INFO, TEX_AL_INFO_ALPHA },
		{ TEX_AL_REPLAY, TEX_AL_REPLAY_ALPHA },
	};
	Game *st = watchGame;
	Gfx *gp = *gpp;
	STexInfo *texC, *texA;
	int i, width, height, alpha[2];

	alpha[0] = sins(st->blink_count << 10) * (255.f / 32768.f) + 127.f;
	alpha[0] = CLAMP(0, 255, alpha[0]);
	alpha[1] = 255 - alpha[0];

	gSPDisplayList(gp++, alpha_texture_init_dl);
	gDPSetCombineMode(gp++, G_CC_ALPHATEXTURE_PRIM, G_CC_PASS2);

	for(i = 0; i < ARRAY_SIZE(_tex); i++) {
		texC = st->texAL + _tex[i][0];
		texA = st->texAL + _tex[i][1];
		width = MIN(texC->size[0], texA->size[0]);
		height = MIN(texC->size[1], texA->size[1]);
		gDPSetPrimColor(gp++, 0,0, 255,255,255, alpha[i]);
		StretchAlphaTexTile(&gp, width, height,
			texC->addr[1], texC->size[0], texA->addr[1], texA->size[0],
			0, 0, width, height, x, y, width, height);
	}

	*gpp = gp;
}

//-------------------------------------------------------------------------
//## コインの初期化
//		int count	初めに持っているコインの数
static void _init_coin_logo(int count)
{
	Game *st = watchGame;
	int		i;

	st->coin_count = count;
	for ( i = 0; i < 4; i++ ) {
		if ( i < count ) {
			st->coin_time[i] = 0;		// 有る
		} else {
			st->coin_time[i] = -1;		// 無い
		}
	}
}

//-------------------------------------------------------------------------
//## コインの表示
//		int count	表示するコイン数
static void _disp_coin_logo(Gfx **glp, int count)
{
	enum {
		COIN_XPOS = 237,
		COIN_YPOS =  81,
		COIN_XINC =  16,
		COIN_TIME =  30,
	};

	Game *st = watchGame;
	Gfx			*pgfx;
	STexInfo	*texC;
	int			i, a, tileW;
	float		fx, fy;
	static u32	texofs[] = { 0, 17, 34, 51 };

	pgfx = *glp;

	if ( st->coin_count < count ) {
		// コインが増えた
		for ( i = st->coin_count; i < count; i++ ) {
			if ( st->coin_time[i] == 0 ) {
				st->coin_time[i] = 0;
				st->coin_count++;
			} else {
				if ( st->coin_time[i] < 0 ) {
					st->coin_time[i] = COIN_TIME;
				} else {
					st->coin_time[i]--;
				}
			}
		}
	} else if ( st->coin_count > count ) {
		// コインが減った
		for ( i = st->coin_count - 1; i >= count; i-- ) {
			if ( st->coin_time[i] >= COIN_TIME ) {
				st->coin_count--;
				st->coin_time[i] = -1;
			} else {
				st->coin_time[i]++;
			}
		}
		count = st->coin_count;
	}

	// 設定
	gSPDisplayList(pgfx++, normal_texture_init_dl);
	gDPSetRenderMode(pgfx++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
	gDPSetCombineLERP(pgfx++,
		0,0,0,TEXEL0,   PRIMITIVE,0,TEXEL0,0,
		0,0,0,TEXEL0,   PRIMITIVE,0,TEXEL0,0);

	// コインの表示
	texC = st->texLS + TEX_coin_00;
	tileW = texC->size[0] / 4;
	for ( i = 0; i < count; i++ ) {
		fx = (float)( COIN_XPOS + COIN_XINC * i );
		fy = (float)( st->coin_time[i] - COIN_TIME/2 );
		fy = COIN_YPOS + ( fy * fy ) * 0.125 - ( (COIN_TIME/2) * (COIN_TIME/2) ) * 0.125;
		a = 255 - ( ( 255 * st->coin_time[i] ) / COIN_TIME );
		gDPSetPrimColor(pgfx++, 0, 0, 255, 255, 255, a);
		StretchTexTile4(&pgfx,
			texC->size[0], texC->size[1],
			texC->addr[0], texC->addr[1],
			tileW * ((st->coin_time[i] >> 1) & 0x03), 0, tileW, texC->size[1],
			fx, fy, tileW, texC->size[1]);
	}

	*glp = pgfx;
}

//## フラッシュウィルスの光を１個描画
static void draw_flash_virus_light(Gfx **gpp, bool cached, int x, int y, int color)
{
	static u8 tbl[] = { 0, 1, 2, 1 };
	static u8 col[][4] = {
		{255,   0,  80, 255},// Red
		{255, 200,   0, 255},// Yellow
		{100, 100, 255, 255},// Blue
	};
	Gfx *gp = *gpp;
	Game *st = watchGame;
	STexInfo *tex = st->texItem + TEX_F1 + tbl[(st->blink_count >> 1) & 3];

	if(!cached) {
		gSPDisplayList(gp++, normal_texture_init_dl);
		gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
		gDPSetCombineLERP(gp++,
			ENVIRONMENT, PRIMITIVE, TEXEL0, PRIMITIVE,
			ENVIRONMENT, 0, TEXEL0, 0, 
			ENVIRONMENT, PRIMITIVE, TEXEL0, PRIMITIVE,
			ENVIRONMENT, 0, TEXEL0, 0);
		gDPSetTextureLUT(gp++, G_TT_NONE);
		gDPSetEnvColor(gp++, 255, 255, 255, 255);
	}

	gDPSetPrimColor(gp++, 0, 0, col[color][0], col[color][1], col[color][2], col[color][3]);
	tiStretchTexBlock(&gp, tex, cached, x, y, 20, 20);

	*gpp = gp;
}

//## フラッシュウィルスの光を描画
static void draw_flash_virus_lights(Gfx **gpp, game_state *state, game_map *map)
{
	Game *st = watchGame;
	bool cached = false;
	int i, dx, dy;

	switch(state->map_item_size) {
	case cap_size_8:
		dx = dy = -6;
		break;
	case cap_size_10:
		dx = dy = -5;
		break;
	}

	for(i = 0; i < state->flash_virus_count; i++) {

		// (非表示 || 消滅状態 || ウィルスではない) の場合
		if(state->flash_virus_pos[i][2] < 0) {
			continue;
		}

		draw_flash_virus_light(gpp, cached,
			dx + state->map_x + state->map_item_size *  state->flash_virus_pos[i][0],
			dy + state->map_y + state->map_item_size * (state->flash_virus_pos[i][1] + 1),
			state->flash_virus_pos[i][2]);

		cached = true;
	}
}

//## タイムアタックの結果表示 : 描画
static void timeAttackResult_draw(_TimeAttackResult *st, Gfx **gpp, int x, int y)
{
	Game *g = watchGame;
	Gfx *gp = *gpp;
	STexInfo *texC, *texA;
	int i;

	gSPDisplayList(gp++, alpha_texture_init_dl);

	// 下地
	texC = g->texP1 + TEX_totalscore_panel;
	texA = g->texP1 + TEX_totalscore_panel_alpha;
	StretchAlphaTexTile(&gp, texC->size[0], texC->size[1],
		texC->addr[1], texC->size[0], texA->addr[1], texA->size[0],
		0, 0, texC->size[0], texC->size[1],
		x, y, texC->size[0], texC->size[1]);

	// 残り時間
	draw_time2(&gp, st->time + FRAME_PAR_MSEC - 1, x + 18, y + 14);

	// 連鎖数
	draw_count_number(&gp, 0, 2, st->combo, x + 31, y + 35);

	// 消したウィルス
	draw_count_number(&gp, 0, 2, st->virus, x + 31, y + 56);

	// トータルスコア
	draw_count_number(&gp, 0, 7, st->score, x + 9, y + 80);

	*gpp = gp;
}

//## ストーリーモードのパネルを描画
static void draw_story_board(Gfx **gpp, int x, int y, bool visBoard, bool visScore)
{
	Game *st = watchGame;
	Gfx *gp = *gpp;
	Gfx *obj;
	STexInfo *texC, *texA;

	gSPDisplayList(gp++, alpha_texture_init_dl);

	if(visBoard) {
		// パネル
		texC = st->texP2 + TEX_top_ohanasi_panel;
		texA = st->texP2 + TEX_top_ohanasi_panel_alpha;
		StretchAlphaTexBlock(&gp, texC->size[0], texA->size[1],
			texC->addr[1], texC->size[0], texA->addr[1], texA->size[0],
			x + 120, y + 11, texC->size[0], texC->size[1]);

		// NORMAL or HARD
		texC = st->texP2 + TEX_top_gamelv_contents;
		texA = st->texP2 + TEX_top_gamelv_contents_alpha;
		tiStretchAlphaTexItem(&gp, texC, texA, false, 4, evs_story_level,
			x + 142, y + 54, texC->size[0], texC->size[1] / 4);
	}

	if(visScore) {
		// スコア
		draw_count_number(&gp, 0, 7, game_state_data[0].game_score, x + 129, y + 25);
	}

	*gpp = gp;
}

//## 対戦画面のパネルを描画
static void draw_vsmode_board(Gfx **gpp, int x, int y, bool visBoard, bool visScore)
{
	Game *st = watchGame;
	Gfx *gp = *gpp;
	STexInfo *texC, *texA;
	int i, pos[2][2];

	gSPDisplayList(gp++, alpha_texture_init_dl);

	if(visBoard) {
		switch(evs_gamemode) {
		// GAME LV
		case GMD_FLASH:
		case GMD_TIME_ATTACK:
			texC = st->texP2 + TEX_top_gamelv_panel;
			texA = st->texP2 + TEX_top_gamelv_panel_alpha;
			StretchAlphaTexBlock(&gp, texC->size[0], texC->size[1],
				texC->addr[1], texC->size[0], texA->addr[1], texA->size[0],
				x + 95, y + 9, texC->size[0], texC->size[1]);

			texC = st->texP2 + TEX_top_gamelv_contents;
			texA = st->texP2 + TEX_top_gamelv_contents_alpha;
			for(i = 0; i < 2; i++) {
				static const int _x[] = { 99, 184 };
				tiStretchAlphaTexItem(&gp, texC, texA, false, 4,
					game_state_data[i].game_level,
					x + _x[i], y + 11, texC->size[0], texC->size[1] / 4);
			}
			break;

		// VIRUS LV
		default:
			texC = st->texP2 + TEX_top_viruslv_panel;
			texA = st->texP2 + TEX_top_viruslv_panel_alpha;
			StretchAlphaTexBlock(&gp, texC->size[0], texC->size[1],
				texC->addr[1], texC->size[0], texA->addr[1], texA->size[0],
				x + 104, y + 9, texC->size[0], texC->size[1]);

			for(i = 0; i < 2; i++) {
				static const int _x[] = { 113, 189 };
				draw_count_number(&gp, 0, 2, game_state_data[i].virus_level, x + _x[i], y + 11);
			}
			break;
		}

		// SPEED
		texC = st->texP2 + TEX_top_speed_panel;
		texA = st->texP2 + TEX_top_speed_panel_alpha;
		StretchAlphaTexBlock(&gp, texC->size[0], texC->size[1],
			texC->addr[1], texC->size[0], texA->addr[1], texA->size[0],
			x + 104, y + 25, texC->size[0], texC->size[1]);

		texC = st->texP2 + TEX_top_speed_contents;
		texA = st->texP2 + TEX_top_speed_contents_alpha;
		for(i = 0; i < 2; i++) {
			static const int _x[] = { 108, 184 };
			tiStretchAlphaTexItem(&gp, texC, texA, false, 3,
				2 - game_state_data[i].cap_def_speed,
				x + _x[i], y + 27, texC->size[0], texC->size[1] / 3);
		}
	}

	switch(evs_gamesel) {
	// SUCORE xxxxxxx
	case GSL_VSCPU:
		if(visBoard) {
			texC = st->texP2 + TEX_top_score_panel_1p;
			texA = st->texP2 + TEX_top_score_panel_1p_alpha;
			StretchAlphaTexBlock(&gp, texC->size[0], texC->size[1],
				texC->addr[1], texC->size[0], texA->addr[1], texA->size[0],
				x + 112, y + 41, texC->size[0], texC->size[1]);
		}
		if(visScore) {
			draw_count_number(&gp, 0, 7, game_state_data[0].game_score, x + 144, y + 42);
		}
		break;

	// xxxxxxx SCORE xxxxxxx
	default:
		if(visBoard) {
			texC = st->texP2 + TEX_top_score_panel_2p;
			texA = st->texP2 + TEX_top_score_panel_2p_alpha;
			StretchAlphaTexBlock(&gp, texC->size[0], texC->size[1],
				texC->addr[1], texC->size[0], texA->addr[1], texA->size[0],
				x + 88, y + 41, texC->size[0], texC->size[1]);
		}
		if(visScore) {
			for(i = 0; i < 2; i++) {
				static const int _x[] = { 94, 162 };
				draw_count_number(&gp, i + 1, 7, game_state_data[i].game_score, x + _x[i], y + 42);
			}
		}
		break;
	}

	*gpp = gp;
}

//## 瓶を描画
static void _draw_bottle_10(Gfx **gpp, const int *xx, const int *yy, int count)
{
	static const int _rect[][4] = {
		{ 0,   0, 96,  20 },
		{ 0,  20, 96,  20 },
		{ 0,  40,  8, 160 }, { 88, 40, 8, 160 },
		{ 0, 200, 96,   8 },
	};
	Game *st = watchGame;
	Gfx *gp = *gpp;
	STexInfo *texC, *texA;
	int h, i, j;

	// 半透明描画の準備
	gSPDisplayList(gp++, normal_texture_init_dl);
	gDPSetCombineMode(gp++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
	gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
	gDPSetPrimColor(gp++, 0,0, 255,255,255, BOTTLE_ALPHA);

	// 瓶の影を描画
	texC = st->texP1 + TEX_P1_BOTTLE_SHADOW;
	for(i = 0; i < texC->size[1]; i += 42) {
		for(j = 0; j < count; j++) {
			h = MIN(42, texC->size[1] - i);
			tiStretchTexTile(&gp, texC, j,
				0, i, texC->size[0], h,
				xx[j], yy[j] + i, texC->size[0], h);
		}
	}

	gSPDisplayList(gp++, normal_texture_init_dl);

	// 瓶を描画
	texC = st->texP1 + TEX_P1_BOTTLE;
	for(i = 0; i < ARRAY_SIZE(_rect); i++) {
		for(j = 0; j < count; j++) {
			tiStretchTexTile(&gp, texC, j,
				_rect[i][0], _rect[i][1], _rect[i][2], _rect[i][3],
				xx[j] + _rect[i][0], yy[j] + _rect[i][1], _rect[i][2], _rect[i][3]);
		}
	}

	*gpp = gp;
}
void dm_calc_bottle_2p() {
	Game *st = watchGame;
	int i = (FRAME_MOVE_MAX - st->frame_move_count) * SCREEN_WD / 2 / FRAME_MOVE_MAX;
	int mx[] = { dm_wold_x_vs_1p-i, dm_wold_x_vs_2p+i };

	for(i = 0; i < 2; i++) {
		game_state *state = &game_state_data[i];
		state->map_x = mx[i];     // 基準Ｘ座標
		state->map_y = dm_wold_y; // 基準Ｙ座標
	}
}
void dm_draw_bottle_2p(Gfx **gpp)
{
	Game *st = watchGame;
	Gfx *gp = *gpp;
	int i = (FRAME_MOVE_MAX - st->frame_move_count) * SCREEN_WD / 2 / FRAME_MOVE_MAX;
	int pos[] = { -i, i };
	int x[2], y[2];

	for(i = 0; i < 2; i++) {
		game_state *state = &game_state_data[i];
		x[i] = state->map_x - 8;
		y[i] = state->map_y - 30;
	}

	_draw_bottle_10(&gp, x, y, 2);

	*gpp = gp;
}

//## ウィルスキャラを描画
void dm_draw_big_virus(Gfx **gpp)
{
	Game *st = watchGame;
	Gfx *gp = *gpp;
	int i;

	for(i = 0; i < 3; i++) {
		animeState_initDL2(&st->virus_anime_state[i], &gp);

		animeState_draw(&st->virus_anime_state[i], &gp,
			st->big_virus_pos[i][0], st->big_virus_pos[i][1],
			st->big_virus_scale[i], st->big_virus_scale[i]);

		animeSmog_draw(&st->virus_smog_state[i], &gp,
			st->big_virus_pos[i][0], st->big_virus_pos[i][1],
			st->big_virus_scale[i], st->big_virus_scale[i]);
	}

	*gpp = gp;
}

//## 傘丸を描画
void dm_draw_KaSaMaRu(Gfx **gpp, Mtx **mpp, Vtx **vpp,
	int speaking, int x, int y, int dir, int alpha)
{
	static const int _pat[] = { 0, 1, 2, 3, 2, 1 };
	Game *st = watchGame;

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
	angle = WrapF(0, 1, st->blink_count * (1.0 / 128.0)) * M_PI * 2;
	guRotateRPYF(mf, 0, (1 - dir) * 90, sinf(angle) * 4 * dir);

	// 傘丸
	i = WrapI(0, ARRAY_SIZE(_pat), ((st->blink_count & 127) * ARRAY_SIZE(_pat) * 12) >> 7);
	if(!speaking) i = 0;

	texC = st->texKaSa + TEX_kasamaru01 + _pat[i];
	texA = st->texKaSa + TEX_kasamaru_alpha;
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
//		texC = st->texKaSa + TEX_kasamaru_mouth;
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

//////////////////////////////////////////////////////////////////////////////
//{### 描画メイン

//## ゲーム描画関数(1P, 多人数 共通)
static void dm_game_graphic_common(game_state *state, int player_no, game_map *map)
{
	Game *st = watchGame;
	STexInfo *tex;
	int i, size_flg = 0;

	if(state->map_item_size == cap_size_8) {
		size_flg = 1;
	}

	gSPDisplayList(gp++, normal_texture_init_dl);

	// ウイルスグラフィックデータの読込み
	tex = dm_game_get_capsel_tex(size_flg);
	load_TexTile_4b(tex->addr[1],
		tex->size[0], tex->size[1], 0, 0,
		tex->size[0] - 1, tex->size[1] - 1);

	// シザー
	gfxSetScissor(&gp, SCISSOR_FLAG,
		state->map_x, state->map_y + state->map_item_size,
		state->map_item_size * GAME_MAP_W,
		state->map_item_size * GAME_MAP_H);

	// ウイルス描画
	for(i = 0; i < 6; i++) {
		// ウイルスパレットデータの読込み
		tex = dm_game_get_capsel_pal(size_flg, i);
		load_TexPal(tex->addr[0]);
		dm_map_draw(map, i, state->map_x, state->map_y - state->bottom_up_scroll, state->map_item_size);
	}
	gDPPipeSync(gp++);

	// シザー解除
	gfxSetScissor(&gp, SCISSOR_FLAG, 0, 0, SCREEN_WD, SCREEN_HT);

	// 落下地点にカプセルを描画
	if(!PLAYER_IS_CPU(state, player_no) &&
		visible_fall_point[player_no] &&
		state->mode_now == dm_mode_down &&
		state->now_cap.pos_y[0] > 0 &&
		state->now_cap.capsel_flg[cap_disp_flg])
	{
		game_cap *cap = &state->now_cap;
		int fallPosY[2], color, x, y, size;

		dm_find_fall_point(map, cap, fallPosY);
		color = (fallPosY[0] - cap->pos_y[0]) << 3;

		gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
		gDPSetCombineMode(gp++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
		_setDarkCapPrim(gp++);

		for(i = 0; i < 2; i++) {
			tex = dm_game_get_capsel_pal(size_flg, cap->capsel_p[i]);
			load_TexPal(tex->addr[0]);

			size = state->map_item_size;
			x = cap->pos_x[i] * size + state->map_x;
			y = fallPosY  [i] * size + state->map_y;

			gSPTextureRectangle(gp++, x<<2, y<<2,
				(x + size)<<2, (y + size)<<2,
				G_TX_RENDERTILE,
				0, cap->capsel_g[i] * size << 5,
				1 << 10, 1 << 10);
		}
		gDPSetPrimColor(gp++, 0, 0, 255, 255, 255, 255);
		gDPSetRenderMode(gp++, G_RM_TEX_EDGE, G_RM_TEX_EDGE2);
	}
}

//## ゲーム描画関数
void dm_game_graphic_p(game_state *state, int player_no, game_map *map)
{
	Game *st = watchGame;
	int i, xx[2], yy[2];
	int size_flg = 0;
	STexInfo *tex;

	// ポーズ中は描画しない
	if(state->cnd_static == dm_cnd_pause) {
		return;
	}

	if(state->map_item_size == cap_size_8) {
		size_flg = 1;
	}

	// 
	dm_game_graphic_common(state, player_no, map);
	gSPDisplayList(gp++, normal_texture_init_dl);

	// 操作カプセル描画
	if(dm_calc_capsel_pos(state, xx, yy)) {
		// デモ中 || カプセル落下待ち || ゲームの決着が付いた
		if(st->demo_flag || state->now_cap.pos_y[0] < 1 ||
			state->cnd_now != dm_cnd_pause && state->cnd_now != dm_cnd_wait)
		{
			// カプセルを描画
			dm_draw_capsel_by_gfx(state, xx, yy);
		}
	}

	// NEXTカプセル描画
	if(state->next_cap.capsel_flg[cap_disp_flg]) {
		// 表示フラグが立っていた場合
		if(state->now_cap.pos_y[0] > 0) {
			// 落下していた
			for(i = 0; i < 2; i++) {
				// パレットロード
				tex = dm_game_get_capsel_pal(size_flg, state->next_cap.capsel_p[i]);
				load_TexPal(tex->addr[0]);

				// カプセル描画
				draw_Tex(
					(state->next_cap.pos_x[i] * state->map_item_size) + state->map_x,
					(state->next_cap.pos_y[i] * state->map_item_size) + state->map_y - 10,
					state->map_item_size, state->map_item_size,
					0, state->next_cap.capsel_g[i] * state->map_item_size);
			}
		}
	}
}

//## ゲーム描画関数(1P用)
static void dm_game_graphic_1p(game_state *state, int player_no, game_map *map)
{
	Game *st = watchGame;
	int i, xx[2], yy[2];
	STexInfo *tex;

	// ポーズ中は描画しない
	if(state->cnd_static == dm_cnd_pause) {
		return;
	}

	// 
	dm_game_graphic_common(state, player_no, map);
	gSPDisplayList(gp++, normal_texture_init_dl);

	// 操作カプセル描画
	if(dm_calc_capsel_pos(state, xx, yy)) {
		i = 0;

		// カプセル配置中 || 一回目のカプセル投入待ち
		if(state->mode_now == dm_mode_init || state->mode_now == dm_mode_wait) {
			xx[1] -= xx[0];
			yy[1] -= yy[0];
			xx[1] += (xx[0] = MARIO_THROW_X);
			yy[1] += (yy[0] = MARIO_THROW_Y);
			i++;
		}
		// デモ中
		else if(st->demo_flag) {
			i++;
		}

		// カプセルを描画
		if(i) {
			dm_draw_capsel_by_gfx(state, xx, yy);
		}
	}

	// NEXTカプセル描画
		// 表示フラグが立っていた場合
			// 落下していた
	if(state->next_cap.capsel_flg[cap_disp_flg] &&
		state->now_cap.pos_y[0] > 0 && state->cnd_static == dm_cnd_wait)
	{
		for(i = 0; i < 2; i++) {
			// パレットロード
			tex = dm_game_get_capsel_pal(0, state->next_cap.capsel_p[i]);
			load_TexPal(tex->addr[0]);

			// カプセル描画
			draw_Tex(i * cap_size_10 + MARIO_THROW_X, MARIO_THROW_Y,
				cap_size_10, cap_size_10, 0,
				state->next_cap.capsel_g[i] * cap_size_10);
		}
	}
}

//## ゲーム描画関数(演出関係用)
void dm_game_graphic_effect(game_state *state, int player_no, int type)
{
	Game *st = watchGame;
	STexInfo *texC, *texA;
	int i;

	// フラッシュウィルスを描画
	switch(state->cnd_now) {
	case dm_cnd_wait:
	case dm_cnd_win:			// WIN
	case dm_cnd_win_retry:		// WIN & RETRY
	case dm_cnd_win_retry_sc:	// WIN & RETRY & SCORE
	case dm_cnd_lose:			// LOSE
	case dm_cnd_lose_retry:		// LOSE & RETRY
	case dm_cnd_lose_retry_sc:	// LOSE & RETRY & SCORE
	case dm_cnd_draw:			// DRAW
	case dm_cnd_draw_retry:		// DRAW & RETRY
		if(evs_gamemode == GMD_FLASH) {
			draw_flash_virus_lights(&gp, state, game_map_data[player_no]);
		}
		break;
	}

	// スコアエフェクトを描画
	if(evs_score_flag) {
		scoreNums_draw(&st->scoreNums[player_no], &gp);
	}

	switch(state->cnd_training) {
	case dm_cnd_training:		// 練習中
		if(state->cnd_static != dm_cnd_wait) {
			break;
		}
		texC = st->texP4 + TEX_PRACTICES;
		texA = st->texP4 + TEX_PRACTICES_ALPHA;
		gSPDisplayList(gp++, alpha_texture_init_dl);
		gDPSetCombineMode(gp++, G_CC_ALPHATEXTURE_PRIM, G_CC_PASS2);
		i = sins(st->blink_count << 9) * (127.f / 32768.f) + 127.f;
		gDPSetPrimColor(gp++, 0,0, 255,255,255, i);
		i = sins(st->blink_count << 10) * (5.f / 32768.f);
		StretchAlphaTexBlock(&gp, texC->size[0], texC->size[1],
			texC->addr[1], texC->size[0],
			texA->addr[1], texA->size[0],
			state->map_x, state->map_y + i + 160,
			texC->size[0], texC->size[1]);
		break;
	}

	// TRY NEXT STATE を描画
	switch(state->cnd_now) {
	case dm_cnd_stage_clear:	// TRY NEXT STAGE
		switch(evs_gamemode) {
		case GMD_TIME_ATTACK:
			break;
		case GMD_TaiQ:
			disp_clear_logo(&gp, player_no, 0);
			break;
		default:
			disp_clear_logo(&gp, player_no, 1);
			break;
		}
		break;
	}

	// ALL CLEAR を描画
	switch(state->cnd_now) {
	case dm_cnd_clear_wait:
		disp_allclear_logo(&gp, player_no, 0);
		break;
	}

	// GAME OVER を描画
	switch(state->cnd_now) {
	case dm_cnd_game_over:		// GAME OVER
	case dm_cnd_gover_wait:
		if(state->cnd_now == dm_cnd_game_over &&
			evs_gamemode == GMD_TIME_ATTACK)
		{
			break;
		}

		if(evs_gamemode == GMD_TIME_ATTACK &&
			evs_game_time >= SCORE_ATTACK_TIME_LIMIT)
		{
			disp_timeover_logo(&gp, player_no);
		}
		else {
			disp_gameover_logo(&gp, player_no);
		}
		break;
	}

	// リタイアを描画
	switch(state->cnd_now) {
	case dm_cnd_retire:   // リタイア
	case dm_cnd_tr_chack: // 練習確認中
		// [START BUTTON で練習開始] を描画
		if(!st->replayFlag &&
			state->player_type == PUF_PlayerMAN)
		{
			i = sins(st->blink_count << 10) * (127.f / 32768.f) + 127.f;
			texC = st->texP4 + TEX_PRACTICE;
			texA = st->texP4 + TEX_PRACTICE_ALPHA;
			gSPDisplayList(gp++, alpha_texture_init_dl);
			gDPSetCombineMode(gp++, G_CC_ALPHATEXTURE_PRIM, G_CC_PASS2);
			gDPSetPrimColor(gp++, 0,0, 255,255,255, i);
			StretchAlphaTexBlock(&gp, texC->size[0], texC->size[1],
				texC->addr[1], texC->size[0],
				texA->addr[1], texA->size[0],
				state->map_x, state->map_y + 92,
				texC->size[0], texC->size[1]);
		}

		// リタイアロゴ
		disp_retire_logo(&gp, player_no);
		break;

	case dm_cnd_retire_wait:
		disp_retire_logo(&gp, player_no);
		break;
	}

	// タイムアタックの結果を表示
	switch(state->cnd_now) {
	case dm_cnd_stage_clear:	// TRY NEXT STAGE
	case dm_cnd_game_over:		// GAME OVER
	case dm_cnd_win:			// WIN
	case dm_cnd_win_retry:		// WIN & RETRY
	case dm_cnd_win_retry_sc:	// WIN & RETRY & SCORE
	case dm_cnd_lose:			// LOSE
	case dm_cnd_lose_retry:		// LOSE & RETRY
	case dm_cnd_lose_retry_sc:	// LOSE & RETRY & SCORE
	case dm_cnd_draw:			// DRAW
	case dm_cnd_draw_retry:		// DRAW & RETRY
	case dm_cnd_clear_result:
	case dm_cnd_gover_result:
	case dm_cnd_retire_result:
		if(evs_gamemode == GMD_TIME_ATTACK) {
			int y = (evs_gamesel == GSL_1PLAY) ? 10 : 74;
			timeAttackResult_draw(&st->timeAttackResult[player_no], &gp,
				state->map_x, state->map_y + y);
		}
		break;
	}

	// WIN を描画
	switch(state->cnd_now) {
	case dm_cnd_win:			// WIN
	case dm_cnd_win_retry:		// WIN & RETRY
	case dm_cnd_win_retry_sc:	// WIN & RETRY & SCORE
		disp_win_logo(&gp, player_no);
		break;
	}

	// LOSE を描画
	switch(state->cnd_now) {
	case dm_cnd_lose:			// LOSE
	case dm_cnd_lose_retry:		// LOSE & RETRY
	case dm_cnd_lose_retry_sc:	// LOSE & RETRY & SCORE
		disp_lose_logo(&gp, player_no);
		break;
	}

	// DRAW を描画
	switch(state->cnd_now) {
	case dm_cnd_draw:			// DRAW
	case dm_cnd_draw_retry:		// DRAW & RETRY
		disp_draw_logo(&gp, player_no);
		break;
	}

	// カウントダウンを描画
	switch(state->cnd_now) {
	case dm_cnd_init:
	case dm_cnd_wait: {
		int frame = st->count_down_ctrl / evs_playcnt;
		int sound = frame / 48;

		if(st->count_down_ctrl >= 0) {
			if(disp_count_logo(&gp, player_no, frame)) {
				st->count_down_ctrl = -1;

				// カウントダウン終了SEを再生
				if(player_no == 0) {
					dm_snd_play_in_game(SE_gCountEnd);
				}
			}
			else {
				if(player_no == 0 && frame % 48 == 20) {
					if(sound < 3) {
						// カウントダウンSEを再生
						dm_snd_play_in_game(SE_gCount1);
					}
				}
				st->count_down_ctrl++;
			}
		}
		} break;
	}

	// PUSH ANY KEY を描画
	switch(state->cnd_now) {
	case dm_cnd_clear_wait:
	case dm_cnd_gover_wait:
		if(evs_gamesel == GSL_1PLAY && evs_gamemode == GMD_TIME_ATTACK) {
			if(st->effect_timer[player_no] == 0) {
				push_any_key_draw(state->map_x + 8, state->map_y + 160);
			}
		}
		break;
	}

	// PAUSE を描画
	switch(state->cnd_now) {
	case dm_cnd_pause:			// PAUSE
		retryMenu_drawPause(player_no, &gp, false);
		break;

	case dm_cnd_pause_re:		// PAUSE & RETRRY
	case dm_cnd_pause_re_sc:	// PAUSE & RETRRY & SCORE
		retryMenu_drawPause(player_no, &gp, true);
		break;
	}

	// 4P,PAUSE 時の攻撃対象パネル
	switch(state->cnd_now) {
	case dm_cnd_pause:			// PAUSE
	case dm_cnd_pause_re:		// PAUSE & RETRRY
	case dm_cnd_pause_re_sc:	// PAUSE & RETRRY & SCORE
		if(evs_gamesel == GSL_4PLAY) {
			draw_4p_attack_guide_panel(&gp,
				st->vs_4p_player_count, player_no,
				state->map_x, state->map_y - 36);
		}
		break;
	}

	// [続ける][リトライ][終わる] を描画
	switch(state->cnd_now) {
	case dm_cnd_stage_clear:	// TRY NEXT STAGE
	case dm_cnd_game_over:		// GAME OVER
	case dm_cnd_win_retry:		// WIN & RETRY
	case dm_cnd_win_retry_sc:	// WIN & RETRY & SCORE
	case dm_cnd_lose_retry:		// LOSE & RETRY
	case dm_cnd_lose_retry_sc:	// LOSE & RETRY & SCORE
	case dm_cnd_draw_retry:		// DRAW & RETRY
		if(st->effect_timer[player_no] == 0) {
			retryMenu_drawContinue(player_no, &gp);
		}
		break;
	}

	// 描画範囲を戻す
	gfxSetScissor(&gp, SCISSOR_FLAG, 0, 0, SCREEN_WD, SCREEN_HT);
}

//////////////////////////////////////////////////////////////////////////////
//{### 思考

//## ゲーム用キー情報作成初期設定関数
static void key_cntrol_init(void)
{
	int i;

	for(i = 0; i < 4; i++) {
		joyflg[i] =(0x0000 | DM_KEY_UP | DM_KEY_DOWN | DM_KEY_LEFT | DM_KEY_RIGHT | DM_KEY_A | DM_KEY_B | DM_KEY_CU | DM_KEY_CD | DM_KEY_CL | DM_KEY_CR);
		joygmf[i] = TRUE;
		joygam[i] = 0x0000;
	}
	joycur1 = evs_keyrept[0];
	joycur2 = evs_keyrept[1];
}

//## ゲーム用キー情報作成関数
void dm_make_key(void)
{
	Game *st = watchGame;
	int i, count;

	// joygam[x] に ｹﾞｰﾑ用ｷｰ を作成
	switch(evs_gamesel) {
	case GSL_1PLAY:
	case GSL_VSCPU:
		count = 1;
		break;

	case GSL_2PLAY:
		count = 2;
		break;

	// 4PLAY & DEMO
	default:
		count = 4;
		break;
	}

	for(i = 0; i < count; i++) {
		if(game_state_data[i].player_type != PUF_PlayerMAN) {
			continue;
		}

		if(!joygmf[main_joy[i]]) {
			continue;
		}

		joygam[i] =
			(_getKeyTrg(i) & (DM_KEY_A | DM_KEY_B)) |
			(_getKeyRep(i) & (DM_KEY_LEFT | DM_KEY_RIGHT)) |
//			(_getKeyLvl(i) & (DM_KEY_L | DM_KEY_R));
			(_getKeyTrg(i) & (DM_KEY_L | DM_KEY_R));

		if(!(_getKeyLvl(i) & (DM_KEY_LEFT | DM_KEY_RIGHT))) {
			joygam[i] = joygam[i] | (_getKeyLvl(i) & DM_KEY_DOWN);
		}
	}
}

//## カプセル操作関数
void key_control_main(game_state *state, game_map *map, int player_no, int joy_no)
{
	Game *st = watchGame;
	game_cap *cap;
	u32 joybak;
	int i, xx[2], yy[2];

	// プレイヤー毎のカプセル落下位置ON/OFF をゲームのワークに反映
	load_visible_fall_point_flag();

	// 「落下地点にカプセルを描画」の ON/OFF
	if(!PLAYER_IS_CPU(state, player_no) && (joyupd[joy_no] & (DM_KEY_CU|DM_KEY_CD|DM_KEY_CL|DM_KEY_CR))) {
		visible_fall_point[player_no] = !visible_fall_point[player_no];
	}

	// 落下位置ON/OFF をセーブデータに反映
	save_visible_fall_point_flag();

	// 投げ中
	if(state->mode_now == dm_mode_throw) {

		// カプセルを描画
		if(!st->demo_flag && dm_calc_capsel_pos(state, xx, yy)) {
			dm_draw_capsel_by_cpu(state, xx, yy);
		}

		// ｺﾝﾄﾛｰﾗが効かない時の処理 :ｷｰﾘﾋﾟｰﾄを直前のﾃﾞｰﾀにする
		if(state->player_type == PUF_PlayerMAN) {
			if(joynew[joy_no] & DM_KEY_RIGHT) {
				joyCursorFastSet(DM_KEY_RIGHT,joy_no);
			}
			if(joynew[joy_no] & DM_KEY_LEFT) {
				joyCursorFastSet(DM_KEY_LEFT, joy_no);
			}
		}
	}
	// 操作カプセル落下
	else if(state->mode_now == dm_mode_down) {

		// 通常状態
		if(state->cnd_static == dm_cnd_wait) {

			// 落下中思考追加位置
			if(PLAYER_IS_CPU(state, player_no)) {
				joybak = joygam[player_no];
				aifKeyOut(state);

				// リプレイ中
				if(st->replayFlag) {
					joygam[player_no] = joybak;
				}
			}

			cap = &state->now_cap;

			// 左回転
			if(joygam[player_no] & DM_ROTATION_L) {
				rotate_capsel(map,cap,cap_turn_l);
			}
			// 右回転
			else if(joygam[player_no] & DM_ROTATION_R) {
				rotate_capsel(map,cap,cap_turn_r);
			}

			// 左移動
			if(joygam[player_no] & DM_KEY_LEFT) {
				translate_capsel(map,state,cap_turn_l,joy_no);
			}
			// 右移動
			else if(joygam[player_no] & DM_KEY_RIGHT) {
				translate_capsel(map,state,cap_turn_r,joy_no);
			}

			state->cap_speed_vec = 1;

			// 高速落下
			if((joygam[player_no] & DM_KEY_DOWN) && cap->pos_y[0] > 0) {
				i = FallSpeed[state->cap_speed];
				i = (i >> 1) + (i & 1);
				state->cap_speed_vec = i;
			}
		}
		else {
			// ｺﾝﾄﾛｰﾗが効かない時の処理 :ｷｰﾘﾋﾟｰﾄを直前のﾃﾞｰﾀにする
			if(state->player_type == PUF_PlayerMAN) {
				if(joynew[joy_no] & DM_KEY_RIGHT) {
					joyCursorFastSet(DM_KEY_RIGHT,joy_no);
				}
				if(joynew[joy_no] & DM_KEY_LEFT) {
					joyCursorFastSet(DM_KEY_LEFT, joy_no);
				}
			}
		}

		// カプセルを描画
		if(!st->demo_flag && dm_calc_capsel_pos(state, xx, yy)) {
			dm_draw_capsel_by_cpu(state, xx, yy);
		}

		dm_capsel_down(state, map);

		st->force_draw_capsel_count[player_no] = 2;
	}
	else {
		if(st->force_draw_capsel_count[player_no]) {
			int bak = state->now_cap.capsel_flg[cap_disp_flg];
			state->now_cap.capsel_flg[cap_disp_flg] = cap_flg_on;

			// カプセルを描画
			if(!st->demo_flag && dm_calc_capsel_pos(state, xx, yy)) {
				dm_draw_capsel_by_cpu(state, xx, yy);
			}

			state->now_cap.capsel_flg[cap_disp_flg] = bak;
			st->force_draw_capsel_count[player_no]--;
		}

		if(state->player_type == PUF_PlayerMAN) {
			joyCursorFastSet(DM_KEY_RIGHT,joy_no);
			joyCursorFastSet(DM_KEY_LEFT, joy_no);
		}
	}
}

//## 思考錯誤関数
void make_ai_main()
{
	int i;

	if(!dm_think_flg) {
		return;
	}

	switch(evs_gamesel) {
	case GSL_2PLAY:
	case GSL_VSCPU:
	case GSL_2DEMO:
		for(i = 0; i < 2; i++) {
			if(PLAYER_IS_CPU(&game_state_data[i], i)) {
				if(game_state_data[i].cnd_static == dm_cnd_wait) {
					aifMake(&game_state_data[i]);
				}
			}
		}
		break;
	case GSL_4PLAY:
	case GSL_4DEMO:
		for(i = 0; i < 4; i++) {
			if(PLAYER_IS_CPU(&game_state_data[i], i)) {
				if(game_state_data[i].cnd_static == dm_cnd_wait) {
					aifMake(&game_state_data[i]);
				}
			}
		}
		break;
	case GSL_1PLAY:
	case GSL_1DEMO:
		if(PLAYER_IS_CPU(&game_state_data[0], 0)) {
			aifMake(&game_state_data[0]);
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////
//{### 他

//## 共通計算
void dm_effect_make(void)
{
	Game *st = watchGame;
	int i;

	// 点滅
	st->blink_count++;

	// 瓶、パネルを移動させるときのカウンタ
	st->frame_move_count = CLAMP(0, FRAME_MOVE_MAX, st->frame_move_count +  st->frame_move_step);

	// ゲーム時間カウントを更新
	for(i = 0; i < evs_playcnt; i++) {
		if(game_state_data[i].cnd_static == dm_cnd_wait) {
			if(st->started_game_flg && evs_game_time < DM_MAX_TIME) {
				evs_game_time++;
			}
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//{### 初期化

#include "boot_data.h"

//## ヒープを初期化
void dm_game_init_heap()
{
	Game *st;
	int i;

	// ヒープの先頭を設定
	heapTop = (void *)HEAP_START;

	// ゲーム構造体を初期化
	st = watchGame = (Game *)ALIGN_16(heapTop);
	bzero(st, sizeof(Game));
	heapTop = watchGame + 1;

	// リプレイ時のバックアップバッファを確保
	for(i = 0; i < ARRAY_SIZE(gameBackup); i++) {
		gameBackup[i] = (GameBackup *)ALIGN_16(heapTop);
		heapTop = gameBackup[i] + 1;
	}

	// ジオメトリー
	gameGeom = (Geometry *)ALIGN_16(heapTop);
	heapTop = gameGeom + 1;
}

//## ゲーム初期化関数
void dm_game_init(int reinit)
{
	static const s16 map_x_table[][4] = {
		{dm_wold_x,       dm_wold_x,       dm_wold_x,       dm_wold_x      }, // 1P
		{dm_wold_x_vs_1p, dm_wold_x_vs_2p, dm_wold_x_vs_1p, dm_wold_x_vs_2p}, // 2P
		{dm_wold_x_4p_1p, dm_wold_x_4p_2p, dm_wold_x_4p_3p, dm_wold_x_4p_4p}, // 4P
	};
	static const u8 map_y_table[] = { dm_wold_y, dm_wold_y_4p };
	static const u8 size_table[] = { cap_size_10, cap_size_8 };
	Game *st = watchGame;
	game_state *state;
	int i, j, k;

	// ランダムシードを設定
	if(!reinit || !st->replayFlag) {
		st->replayFlag = 0;
		st->randSeed = (genrand(0xffff) + osGetTime()) * 0x00010001;
		st->randSeed2 = irandom() + osGetTime();
	}
	sgenrand(st->randSeed);
	randomseed(st->randSeed2);

	// スターを初期化
	if(!reinit) {
		// スター描画位置の格納場所
		for(i = 0; i < ARRAY_SIZE(st->star_pos_x); i++) {
			st->star_pos_x[i] = st->star_pos_y[i] = 0;
		}
		st->star_count = 0;

		// 星を初期化
		starForce_init(&st->starForce, st->star_pos_x, st->star_pos_y);
	}

	// リトライコイン
	if(!reinit) {
		st->retry_coin = 0;
	}

	// スコア
	for(i = 0; i < ARRAY_SIZE(st->scoreNums); i++) {
		scoreNums_init(&st->scoreNums[i]);
	}

	// 操作カプセル強制描画カウンタを初期化
	for(i = 0; i < ARRAY_SIZE(st->force_draw_capsel_count); i++) {
		st->force_draw_capsel_count[i] = 0;
	}

	// カーテンの位置を設定
	st->curtain_count = CURTAIN_DOWN;
	st->curtain_step = -1;

	// 勝敗時の顔アニメーションカウンタ
	for(i = 0; i < ARRAY_SIZE(st->face_anime_count); i++) {
		st->face_anime_count[i] = 1;
	}

	// カウントダウン処理の制御
	st->count_down_ctrl = 0;

	// BGMの速さを１回変えるためのフラグ
	st->bgm_bpm_flg[0] = st->bgm_bpm_flg[1] = 0;

	// リーチ音を鳴らすフラグのクリア
	st->last_3_se_flg = 0;
	st->last_3_se_cnt = (evs_gamemode == GMD_FLASH) ? 1 : 3;

	// タイム加算を開始するフラグ
	st->started_game_flg = (main_no == MAIN_GAME) ? 0 : 1;

	// 巨大ウィルスの描画位置
	for(i = 0; i < ARRAY_SIZE(st->big_virus_flg); i++) {
		// 消滅アニメーションフラグ
		st->big_virus_flg[i] = 0;

		// 位置を設定
		st->big_virus_rot[i] = i * 120 + 1; // sin,cos用数値
		st->big_virus_scale[i] = 1;
	}

	// ウィルス移動のウェイト&タイマー
	st->big_virus_wait = 10;
	st->big_virus_timer = 0;
	st->big_virus_no_wait = false;
	st->big_virus_stop_count = 0;
	st->big_virus_blink_count = 0;
	if(evs_gamemode == GMD_TaiQ) {
		st->big_virus_wait = _big_virus_def_wait[game_state_data[0].game_level];
	}

	// 各色のウイルス数
	for(i = 0; i < ARRAY_SIZE(st->big_virus_count); i++) {
		st->big_virus_count[i] = 0;
	}

	// デモタイマーを設定
	switch(evs_gamesel) {
	case GSL_1DEMO: case GSL_2DEMO: case GSL_4DEMO:
		st->demo_timer = 1800;
		st->demo_flag = 1;
		break;
	default:
		st->demo_timer = 0;
		st->demo_flag = 0;
		break;
	}

	// 表示物点滅処理カウンタ
	st->blink_count = 0;

	// カプセル積み上がり時の SE 再生カウンタ
	st->warning_se_count = 0;

	// 勝利数カウントの初期化
	if(!reinit) {
		for(i = 0; i < 4; i++) {
			st->win_count[i] = 0;
		}
	}

	// 耐久 or タイムアタックの場合タイムをクリア
	switch(evs_gamemode) {
	case GMD_TIME_ATTACK:
	case GMD_TaiQ:
		evs_game_time = 0;
		break;
	}

	// 音楽再生
	dm_seq_play_in_game(evs_seqnumb << 1);

	// カプセル作成
	dm_make_magazine();

	// ウイルス数表示サイズの初期化
	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			// 4Pチーム対抗戦の攻撃カプセルストックのクリア
			st->story_4p_stock_cap[i][j] = -1;
		}
	}

	// 耐久モード時、一段シフトするフラグ
	st->bottom_up_flag = false;

	// ポーズ、デバッグ、コンフィグ 要求者
	st->query_play_pause_se = 0;
	st->query_pause_player_no = -1;
	st->query_debug_player_no = -1;
	st->query_config_player_no = -1;

	// パスワード
	st->passWnd.alpha = 0;
	st->passAlphaStep = -16;
	bzero(st->passBuf, sizeof(st->passBuf));

	switch(evs_gamesel) {
	case GSL_1PLAY:
	case GSL_1DEMO:
		k = 0; // アイテムの大きさ
		j = 0; // 基準Ｘ座標
		break;
	case GSL_2PLAY:
	case GSL_VSCPU:
	case GSL_2DEMO:
		k = 0; // アイテムの大きさ
		j = 1; // 基準Ｘ座標
		break;
	case GSL_4PLAY:
	case GSL_4DEMO:
		k = 1; // アイテムの大きさ
		j = 2; // 基準Ｘ座標
		break;
	}

	for(i = 0; i < 4; i++) {
		state = &game_state_data[i];

		// 座標系の設定
		state->map_x = map_x_table[j][i]; // 基準Ｘ座標
		state->map_y = map_y_table[k];    // 基準Ｙ座標
		state->map_item_size = size_table[k]; // 基準アイテムサイズ
	}

	for(i = 0; i < 4; i++) {
		state = &game_state_data[i];

		// 内部制御変数の設定
		state->player_no    = i;            // プレイヤー番号
		state->mode_now     = dm_mode_init; // 内部制御を初期化モードに設定
		state->cnd_now      = dm_cnd_init;  // 状態を初期化状態に設定
		state->cnd_training = dm_cnd_wait;  // トレーニング状態を通常状態(トレーニング無し)に設定
		state->cnd_static   = dm_cnd_wait;  // 絶対的状態を通常状態に設定
		state->virus_number = 0;            // ウイルス数のクリア
		state->virus_order_number = 0;      // 配置中のウィルス番号

		// 警告音フラグをクリア
		state->warning_se_flag = 0;

		state->flg_retire    = 0; // リタイアフラグのクリア
		state->flg_game_over = 0; // GameOverフラグのクリア
		state->flg_training  = 0; // Trainingフラグのクリア

		// カプセル落下速度の設定
		state->cap_speed        = GameSpeed[state->cap_def_speed]; // 初期落下カウントの設定
		state->cap_speed_max    = 0; // 随時設定される
		state->cap_speed_vec    = 1; // 落下カウンタ増加値
		state->cap_magazine_cnt = 1; // マガジン残照を1にする
		state->cap_count        = 0; // カウンタの初期化
		state->cap_speed_count  = 0;
		state->cap_move_se_flg  = 0; // 移動時のＳＥ再生用フラグのクリア

		// カプセル情報の初期設定
		dm_set_capsel(state);

		// ウイルスアニメーションの設定
		state->virus_anime       = 0; // カウンタとアニメション番号の初期化
		state->virus_anime_count = 0;
		state->virus_anime_vec   = 1; // アニメーション進行方向の指定
		state->erase_anime       = 0; // 消滅アニメーションコマ数の初期化
		state->erase_anime_count = 0; // 消滅アニメーションカウンタの初期化
		state->erase_virus_count = 0; // 消滅ウイルスカウンタの初期化
		state->erase_virus_count_old = 0;

		// 連鎖数の初期化
		state->chain_count = state->chain_line = 0;

		// 瓶の中身を一段上げる時のカウンタ
		state->bottom_up_scroll = 0;

		// ウィルス消し数
		state->total_erase_count = 0;

		// 合計連鎖回数
		state->total_chain_count = 0;

		switch(evs_gamesel)
		{
		case GSL_4PLAY:
		case GSL_4DEMO:
			// ４Ｐの場合
			state->virus_anime_spead = v_anime_def_speed_4p;// アニメーション間隔の設定
			state->virus_anime_max = 0;                     // ウイルスアニメーション最大コマ数の設定
			break;

		default:
			// それ以外の場合
			state->virus_anime_spead = v_anime_def_speed;   // アニメーション間隔の設定
			state->virus_anime_max = 2;                     // ウイルスアニメーション最大コマ数の設定
			break;
		}

		// 攻撃カプセルデータの初期化
		for(j = 0; j < 4; j++) {
			state->chain_color[j] = 0;
		}

		// 被爆カプセルデータの初期化
		for(j = 0; j < DAMAGE_MAX; j++) {
			state->cap_attack_work[j][0] = 0; // 攻撃無し
			state->cap_attack_work[j][1] = 0; // 攻撃無し
		}

		// マップ情報（瓶の中）の初期化
		init_map_all(game_map_data[i]);
	}

	// ウイルスデータの作成
	for(i = 0; i < evs_playcnt; i++) {
		dm_virus_init(evs_gamemode, &game_state_data[i], virus_map_data[i], virus_map_disp_order[i]);

		// フラッシュウィルスの位置を決める
		if(evs_gamemode == GMD_FLASH) {
			make_flash_virus_pos(&game_state_data[i], virus_map_data[i], virus_map_disp_order[i]);
		}
		else {
			game_state_data[i].flash_virus_count = 0;
		}
	}

	// レベルが同じだった場合ウイルス情報をコピーする
	for(i = 0; i < evs_playcnt - 1; i++) {
	for(j = i + 1; j < evs_playcnt; j++) {
		if(game_state_data[i].virus_level == game_state_data[j].virus_level) {
			// ウィルスの位置をコピーする
			dm_virus_map_copy(virus_map_data[i], virus_map_data[j], virus_map_disp_order[i], virus_map_disp_order[j]);

			// フラッシュウィルスの位置をコピーする
			if(evs_gamemode == GMD_FLASH) {
				game_state_data[j].flash_virus_count = game_state_data[i].flash_virus_count;
				bcopy(game_state_data[i].flash_virus_pos, game_state_data[j].flash_virus_pos, sizeof(game_state_data[1].flash_virus_pos));
				bcopy(game_state_data[i].flash_virus_bak, game_state_data[j].flash_virus_bak, sizeof(game_state_data[1].flash_virus_bak));
			}
			break;
		}
	}}

	// エフェクトを初期化
	effectAll_init();

	aifGameInit();

	fool_mode = s_hard_mode = 0;
	if(evs_story_level == 3) {
		if(evs_story_no == LAST_STAGE_NO - 1) {
			fool_mode = TRUE;
		}
		else {
			s_hard_mode = TRUE;
		}
	}
}

//## 背景テクスチャを読み込む
void dm_game_init_static()
{
	Game *st = watchGame;
	void *(*tbl)[2] = _romDataTbl;
	mem_char *mc;
	int i, j, score;

	// EEPROM の書き込み処理フラグ
	st->eep_rom_flg = 0;

	// グラフィックスレッドのプライオリティを設定
	st->graphic_thread_pri = OS_PRIORITY_APPMAX;

	// 共通テクスチャデータを読み込む
	st->texAL = tiLoadTexData(&heapTop, tbl[_game_al][0], tbl[_game_al][1]);
	st->texItem = tiLoadTexData(&heapTop, tbl[_game_item][0], tbl[_game_item][1]);

	// カサマル
	if(main_no != MAIN_GAME) {
		st->texKaSa = tiLoadTexData(&heapTop, tbl[_menu_kasa][0], tbl[_menu_kasa][1]);
	}

	// カーテンの位置を設定
//	st->curtain_count = CURTAIN_DOWN;
//	st->curtain_step = -1;

	// エフェクトデータをロード
	st->effect_data_buf = heapTop;
	heapTop = (void *)ExpandGZip(
		tbl[_game_etc][0], st->effect_data_buf,
		(u8 *)tbl[_game_etc][1] - (u8 *)tbl[_game_etc][0]);

	// 変数の初期化
	for(i = 0; i < 2; i++) {
		st->vs_win_count[i] = 0;
	}

	// 瓶、パネルを移動させるときのカウンタ
	st->frame_move_count = 0;
	st->frame_move_step = 1;

	// カプセル着地時のウェイト
	st->touch_down_wait = 2;

	// コイングラフィックアニメーションを初期化
	_init_coin_logo(0);

	// コーヒーブレーク中
	st->coffee_break_flow = 0;
	st->coffee_break_timer = 0;
	st->coffee_break_level = 0;
	st->coffee_break_shard = 0;

	// リプレイデータバッファを初期化
	replay_record_init_buffer(&heapTop);
	replay_record_init(evs_playcnt);

	// タイムアタックの結果表示
	for(i = 0; i < ARRAY_SIZE(st->timeAttackResult); i++) {
		timeAttackResult_init(&st->timeAttackResult[i]);
	}

	// パスワード
	msgWnd_init2(&st->passWnd, &heapTop, 256, 10, 5, 0, 0);
	st->passWnd.posX = (SCREEN_WD - st->passWnd.colStep * PASSWORD_LENGTH) / 2;
	st->passWnd.posY = (SCREEN_HT - st->passWnd.rowStep * 2) / 2;

	// EEPROM書き込み中の警告メッセージ
	RecWritingMsg_init(&st->writingMsg, &heapTop);

	// ハイスコアを取得
	switch(evs_gamesel) {
	case GSL_1DEMO:
		evs_high_score = DEFAULT_HIGH_SCORE;
		break;

	case GSL_1PLAY: {
		mem_char *mc = &evs_mem_data[evs_select_name_no[0]];

		if(evs_select_name_no[0] == DM_MEM_GUEST) {
			evs_high_score = DEFAULT_HIGH_SCORE;
			break;
		}

		switch(evs_gamemode) {
		case GMD_NORMAL:
			evs_high_score = mc->level_data[game_state_data[0].cap_def_speed].score;
			break;
		case GMD_TaiQ:
			evs_high_score = mc->taiQ_data[game_state_data[0].game_level].score;
			break;
		case GMD_TIME_ATTACK:
			evs_high_score = mc->timeAt_data[game_state_data[0].game_level].score;
			break;
		}
		} break;

	case GSL_2PLAY:
	case GSL_VSCPU:
	case GSL_2DEMO:
	case GSL_4PLAY:
	case GSL_4DEMO:
		if(!evs_story_flg) {
			evs_high_score = 0;
		}
		break;
	}

	// グラフィックデータの読込み(背景等)
	switch(evs_gamesel) {
	case GSL_1DEMO:
	case GSL_1PLAY:
		st->touch_down_wait = 1;

		// マリオアニメーションを初期化
		animeState_load(&game_state_data[0].anime, &heapTop, 15);
		animeState_set(&game_state_data[0].anime, ASEQ_DAMAGE);

		// テクスチャデータを読み込む(一人用)
		st->texP1 = tiLoadTexData(&heapTop, tbl[_game_p1][0], tbl[_game_p1][1]);

		// テクスチャデータを読み込む(レベルセレクト用)
		st->texLS = tiLoadTexData(&heapTop, tbl[_game_ls][0], tbl[_game_ls][1]);

		// ウイルスアニメーションを初期化
		for(i = 0; i < 3; i++) {
			animeState_load(&st->virus_anime_state[i], &heapTop, 16+i);
		}

		// 煙アニメーションを初期化
		animeSmog_load(&st->virus_smog_state[0], &heapTop);
		for(i = 1; i < 3; i++) {
			animeSmog_init(&st->virus_smog_state[i], &st->virus_smog_state[0].animeState[0]);
		}

		// コーヒーブレーク
		mc = &evs_mem_data[ evs_select_name_no[0] ];
#if LOCAL==JAPAN
		msgWnd_init2(&st->msgWnd, &heapTop, 0x1000, 20, 15, 64, 15);
#elif LOCAL==AMERICA
		msgWnd_init2(&st->msgWnd, &heapTop, 0x1000, 20, 15, 40, 15);
		st->msgWnd.centering = 1;
#elif LOCAL==CHINA
		msgWnd_init2(&st->msgWnd, &heapTop, 0x1000, 20, 15, 40, 15);
		st->msgWnd.centering = 1;
#endif
		msgWnd_addStr(&st->msgWnd, st_staffroll_txt);
		msgWnd_skip(&st->msgWnd);
		st->msgWnd.fontType = 1;
		st->msgWnd.contFlags = 0;
		heapTop = (void *)init_coffee_break((u32)heapTop, game_state_data[0].cap_def_speed);

		break;

	case GSL_2PLAY:
	case GSL_VSCPU:
	case GSL_2DEMO:
		// キャラアニメーションを初期化
		for(i = 0; i < 2; i++) {
			animeState_load(&game_state_data[i].anime, &heapTop, game_state_data[i].charNo);
		}

		// 勝利数を初期化
		if(!evs_story_flg) {
			// ポイント表示の数値をあらかじめセットしておく
			for(i = 0;i < 2;i++) {
				st->vs_win_total[i] = 0;
			}

			switch(evs_gamesel) {
			case GSL_2PLAY:
				for(i = 0; i < 2; i++) {
					st->vs_win_total[i] = evs_mem_data[evs_select_name_no[i]].vsman_data[0];
				}
				break;

			case GSL_VSCPU:
//				st->vs_win_total[0] = evs_mem_data[evs_select_name_no[0]].vscom_data[0];
//				st->vs_win_total[1] = evs_mem_data[evs_select_name_no[0]].vscom_data[1];
				break;
			}
		}

		// テクスチャデータを読み込む(一人用)
		st->texP1 = tiLoadTexData(&heapTop, tbl[_game_p1][0], tbl[_game_p1][1]);

		// テクスチャデータを読み込む(二人用)
		st->texP2 = tiLoadTexData(&heapTop, tbl[_game_p2][0], tbl[_game_p2][1]);

		break;

	case GSL_4PLAY:
	case GSL_4DEMO:
		// テクスチャデータを読み込む(一人用)
		st->texP1 = tiLoadTexData(&heapTop, tbl[_game_p1][0], tbl[_game_p1][1]);

		// テクスチャデータを読み込む(四人用)
		st->texP4 = tiLoadTexData(&heapTop, tbl[_game_p4][0], tbl[_game_p4][1]);

		// キャラアニメーションを初期化
		for(i = 0; i < 4; i++) {
			animeState_load(&game_state_data[i].anime, &heapTop, game_state_data[i].charNo);
		}

		// チームのメンバー数計算
		for(i = 0; i < 4; i++) {
			st->story_4p_name_num[i] = 0;
		}
		for(i = 0; i < 4; i++) {
			st->story_4p_name_num[ game_state_data[i].team_no ] ++;
		}

		// プレイヤーの参加数を求める
		for(i = st->vs_4p_player_count = 0; i < 4; i++) {
			if(game_state_data[i].player_type == PUF_PlayerMAN) {
				st->vs_4p_player_count++;
			}
		}

//		if(evs_story_flg) {	// ストーリーモードの場合
//			// チーム対抗戦
//			st->vs_4p_team_flg = 1;
//
//			// 1 VS 3	(MARIO : ENEMY)
//			st->vs_4p_team_bits[0] = 0x01;
//			st->vs_4p_team_bits[1] = 0x0e;
//		}
//		else {
		if(st->story_4p_name_num[0] == 1 && st->story_4p_name_num[1] == 1
		&&  st->story_4p_name_num[2] == 1 && st->story_4p_name_num[3] == 1)
		{
			// バトルロイヤル
			st->vs_4p_team_flg = 0;
		}
		else {
			// チーム対抗戦
			st->vs_4p_team_flg = 1;

			// 判定用ビットの作成
			st->vs_4p_team_bits[0] = st->vs_4p_team_bits[1] = 0;
			for(i = 0; i < 4; i++) {
				if(game_state_data[i].team_no == TEAM_MARIO) {
					st->vs_4p_team_bits[TEAM_MARIO] |= 1 << i;
				}
			}

			for(i = 0; i < 4; i++) {
				if(game_state_data[i].team_no == TEAM_ENEMY) {
					st->vs_4p_team_bits[TEAM_ENEMY] |= 1 << i;
				}
			}
		}
//		}
		break;
	}
}

//## 背景スナップの準備
void dm_game_init_snap_bg()
{
	Game *st = watchGame;
	Gfx *gp;

	st->bg_snap_buf = (u8 *)ALIGN_64(heapTop);
	heapTop = st->bg_snap_buf + 328 * 240 * 2;

	st->bg_snapping = false;

	switch(evs_gamesel) {
	case GSL_1PLAY:
	case GSL_1DEMO:
		break;
	default:
		heapTop = story_bg_init(story_proc_no, (u32)heapTop);
		break;
	}
}

//## 背景をスナップ
void dm_game_draw_snap_bg(Gfx **gpp, Mtx **mpp, Vtx **vpp, int drawPanelFlag)
{
	Game *st = watchGame;
	Gfx *gp = *gpp;
	STexInfo *texC, *texA;
	int i, width;

	if(st->bg_snapping) {
		gDPSetColorImage(gp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 328,
			osVirtualToPhysical(st->bg_snap_buf));
	}

	switch(evs_gamesel) {
	case GSL_1PLAY:
	case GSL_1DEMO: {
		static const short _bgPos[][2] = {
			{ 0, 0 }, { 80, 0 }, { 160, 0 }, { 240, 0 },
			{ 120, 120 }, { 200, 120 }, { 280, 120 },
		};
		static const short _panelPos[][2] = {
			{ 212, 103 }, { 212, 145 }, { 212, 187 },
		};
		static const char _bgTex[] = {
			TEX_bg_endless, TEX_bg_taikyu, TEX_bg_timeattack,
		};
		static const char _magTex[] = {
			TEX_magnifier_endless, TEX_magnifier_taikyu,
			TEX_magnifier_timeattack,
		};
		static const char _scrTex[] = {
			TEX_l_panel_endless, TEX_l_panel_endless, TEX_l_panel_timeattack,
		};
		static const char _panelTex[][3] = {
			{ TEX_r_panel_viruslv, TEX_r_panel_speed, TEX_r_panel_virus, },
			{ TEX_r_panel_level, TEX_r_panel_speed, TEX_r_panel_virus, },
			{ TEX_r_panel_level, TEX_r_panel_speed, TEX_r_panel_virus, },
		};
		enum { bottleX = 110, bottleY = 16 };
		int type;

		switch(evs_gamemode) {
		case GMD_NORMAL:
			type = 0;
			break;
		case GMD_TaiQ:
			type = 1;
			break;
		case GMD_TIME_ATTACK:
			type = 2;
			break;
		}

		gSPDisplayList(gp++, normal_texture_init_dl);

		// 背景を描画
		texC = st->texLS + _bgTex[type];
		for(i = 0; i < ARRAY_SIZE(_bgPos); i++) {
			tiStretchTexTile(&gp, texC, false,
				0, 0, texC->size[0], texC->size[1],
				_bgPos[i][0], _bgPos[i][1],
				texC->size[0], texC->size[1]);
		}

		// レンズ
		texC = st->texLS + _magTex[type];
		tiStretchTexTile(&gp, texC, false,
			0, 0, texC->size[0], texC->size[1],
			0, 120, texC->size[0], texC->size[1]);

		if(drawPanelFlag) {
			gSPDisplayList(gp++, alpha_texture_init_dl);

			// お立ち台
			texC = st->texLS + TEX_mario_platform;
			texA = st->texLS + TEX_mario_platform_alpha;
			width = MIN(texC->size[0], texA->size[0]);
			StretchAlphaTexTile(&gp, width, texC->size[1],
				texC->addr[1], texC->size[0],
				texA->addr[1], texA->size[0],
				0, 0, width, texC->size[1],
				202, 12, width, texC->size[1]);

			// スコアパネル
			texC = st->texLS + _scrTex[type];
			texA = st->texLS + TEX_l_panel_alpha;
			width = MIN(texC->size[0], texA->size[0]);
			StretchAlphaTexTile(&gp, width, texC->size[1],
				texC->addr[1], texC->size[0],
				texA->addr[1], texA->size[0],
				0, 0, width, texC->size[1],
				11, 17, width, texC->size[1]);

			// パネル群
			texA = st->texLS + TEX_r_panel_alpha;
			for(i = 0; i < ARRAY_SIZE(_panelPos); i++) {
				texC = st->texLS + _panelTex[type][i];
				width = MIN(texC->size[0], texA->size[0]);
				StretchAlphaTexTile(&gp, width, texC->size[1],
					texC->addr[1], texC->size[0],
					texA->addr[1], texA->size[0],
					0, 0, width, texC->size[1],
					_panelPos[i][0], _panelPos[i][1],
					width, texC->size[1]);
			}

			// 耐久とタイアタのゲームレベル
			switch(evs_gamemode) {
			case GMD_TaiQ:
			case GMD_TIME_ATTACK:
				texC = st->texLS + TEX_newmode_level;
				texA = st->texLS + TEX_newmode_level_alpha;
				tiStretchAlphaTexItem(&gp, texC, texA, false,
					3, game_state_data[0].game_level, 232, 120, 48, 16);
				break;
			}

			// ゲームスピード
			texC = st->texLS + TEX_endless_speed;
			texA = st->texLS + TEX_endless_speed_alpha;
			tiStretchAlphaTexItem(&gp, texC, texA, false,
				3, game_state_data[0].cap_def_speed, 232, 162, 48, 16);
		}

		// 半透明描画の準備
		gSPDisplayList(gp++, normal_texture_init_dl);
		gDPSetCombineMode(gp++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
		gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
		gDPSetPrimColor(gp++, 0,0, 255,255,255, BOTTLE_ALPHA / 2);

		// 瓶の影を描画
		texC = st->texP1 + TEX_P1_BOTTLE_SHADOW;
		for(i = 0; i < 2; i++) {
			tiStretchTexBlock(&gp, texC, false,
				bottleX, bottleY, texC->size[0], texC->size[1]);
		}

		// 瓶を描画
		texC = st->texP1 + TEX_P1_BOTTLE;
		for(i = 0; i < 2; i++) {
			tiCopyTexBlock(&gp, texC, false, bottleX, bottleY);
		}

		} break;

	case GSL_4PLAY:
	case GSL_4DEMO:
		// 背景を描画
		story_bg_proc(&gp);

		// 半透明描画の準備
		gSPDisplayList(gp++, normal_texture_init_dl);
		gDPSetCombineMode(gp++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
		gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
		gDPSetPrimColor(gp++, 0,0, 255,255,255, BOTTLE_ALPHA);

		// 瓶の影を描画
		texC = st->texP4 + TEX_P4_BOTTLE_SHADOW;
		for(i = 0; i < 4; i++) {
			tiStretchTexBlock(&gp, texC, false,
				_posP4Bottle[i][0] + _posP4BottleIn[0],
				_posP4Bottle[i][1] + _posP4BottleIn[1],
				texC->size[0], texC->size[1]);
		}

		// 瓶を描画
		texC = st->texP4 + TEX_P4_BOTTLE;
		for(i = 0; i < 4; i++) {
			tiCopyTexBlock(&gp, texC, false, _posP4Bottle[i][0], _posP4Bottle[i][1]);
		}

		if(!drawPanelFlag) {
			break;
		}

		gSPDisplayList(gp++, normal_texture_init_dl);

		// キャラパネルを描画
		texC = st->texP4 + TEX_P4_BOTTOMPANEL;
		for(i = 0; i < 4; i++) {
			tiStretchTexBlock(&gp, texC, false,
				_posP4CharBase[i][0], _posP4CharBase[i][1],
				texC->size[0], texC->size[1]);
		}

		gSPDisplayList(gp++, alpha_texture_init_dl);

		// Virusの文字を描画
		if(evs_story_flg || !st->vs_4p_team_flg) {
			if(evs_gamemode == GMD_FLASH) {
				texC = st->texP4 + TEX_p4_panel_flash;
				texA = st->texP4 + TEX_p4_panel_flash_alpha;
			}
			else {
				texC = st->texP4 + TEX_P4_PANEL_VIRUS;
				texA = st->texP4 + TEX_P4_PANEL_VIRUS_ALPHA;
			}
			width = MIN(texC->size[0], texA->size[0]);
			for(i = 0; i < 4; i++) {
				StretchAlphaTexBlock(&gp, width, texC->size[1],
					texC->addr[1], texC->size[0],
					texA->addr[1], texA->size[0],
					_posP4CharBase[i][0] + 25,
					_posP4CharBase[i][1],
					width, texC->size[1]);
			}
		}

		// ストーリーモーの場合
		if(evs_story_flg) {
			// タイム, ステージ数, 星マーク, の下地
			texC = st->texP4 + TEX_P4_TOPPANEL_STORY;
			texA = st->texP4 + TEX_P4_TOPPANEL_STORY_ALPHA;
			width = MIN(texC->size[0], texA->size[0]);
			StretchAlphaTexBlock(&gp,
				width, texC->size[1],
				texC->addr[1], texC->size[0],
				texA->addr[1], texC->size[0],
				16, 11, width, texC->size[1]);
		}
		// チーム対抗戦
		else if(st->vs_4p_team_flg) {
			// チームABを描画
			for(i = 0; i < 4; i++) {
				static const int tbl[][2] = {
					{ TEX_P4_TEAM_A, TEX_P4_TEAM_A_ALPHA },
					{ TEX_P4_TEAM_B, TEX_P4_TEAM_B_ALPHA },
				};
				int id = (game_state_data[i].team_no == TEAM_MARIO) ? 0 : 1;
				texC = st->texP4 + tbl[id][0];
				texA = st->texP4 + tbl[id][1];
				width = MIN(texC->size[0], texA->size[0]);
				StretchAlphaTexBlock(&gp,
					width, texC->size[1],
					texC->addr[1], texC->size[0],
					texA->addr[1], texC->size[0],
					_posP4CharBase[i][0] + 25,
					_posP4CharBase[i][1],
					width, texC->size[1]);
			}

			// チームパネル
			texC = st->texP4 + TEX_p4_toppanel_team_1 + evs_vs_count - 1;
			CopyTexBlock8(&gp,
				texC->addr[0], texC->addr[1],
				16, 8, texC->size[0], texC->size[1]);
		}
		// バトルロイヤル
		else {
			// 星マーク, の下地
			texC = st->texP4 + TEX_p4_toppanel_vs_1 + evs_vs_count - 1;
			CopyTexBlock8(&gp,
				texC->addr[0], texC->addr[1],
				16, 11, texC->size[0], texC->size[1]);
		}
		break;

	case GSL_2PLAY:
	case GSL_VSCPU:
	case GSL_2DEMO:
		story_bg_proc(&gp);

		if(!drawPanelFlag) {
			break;
		}

		gSPDisplayList(gp++, alpha_texture_init_dl);

		// STATE, TIME
		if(evs_story_flg) {
			texC = st->texP2 + TEX_bottom_stage_panel;
		}
		// POINT, TIME
		else {
			texC = st->texP2 + TEX_bottom_point_panel;
		}
		texA = st->texP2 + TEX_bottom_stage_point_panel_alpha;
		StretchAlphaTexBlock(&gp, texC->size[0], texC->size[1],
			texC->addr[1], texC->size[0], texA->addr[1], texA->size[0],
			114, 150, texC->size[0], texC->size[1]);

		// ステージ番号
		if(evs_story_flg) {
			draw_count_number(&gp, 0, 1, evs_story_no, 176, 152);
		}

		break;
	}

	*gpp = gp;
}

//## スナップした背景を描画
void dm_game_draw_snapped_bg(Gfx **gpp)
{
	Game *st = watchGame;
	Gfx *gp = *gpp;

	CopyTexBlock16(&gp, st->bg_snap_buf, 0, 0, 328, 240);

	*gpp = gp;
}

//////////////////////////////////////////////////////////////////////////////
//{### スケジューラから呼ばれるメイン関数

//## ゲーム処理メイン関数
int dm_game_main(NNSched *sched)
{
	static bool dm_game_main2();
	static MAIN_NO dm_game_main3();

	Game *st;
	OSMesgQueue msgQ;
	OSMesg msgbuf[MAX_MESGS];
	NNScClient client;
	MAIN_NO ret;
	DM_GAME_RET gret;
	bool loop_flg = true;
	int i, j;

	// ゲーム用キー情報作成初期設
	key_cntrol_init();

	// メッセージキューの作成
	osCreateMesgQueue(&msgQ, msgbuf, MAX_MESGS);

	// クライアントに登録
	nnScAddClient(sched, &client, &msgQ);

	// CPUタスクスレッドを作成
	cpuTask_createThread();

	// ヒープを初期化
	dm_game_init_heap();
	st = watchGame;

	// グラフィックデータの読込み(背景等)
	dm_game_init_static();

	// 背景取り込みの準備
	dm_game_init_snap_bg();
	st->bg_snapping = true;

	// ゲームの異初期化
	dm_game_init(0);

	// ステータスをバックアップ
	backup_game_state(0);

	DBG3(1, "use heap %08x ( %08x - %08x )\n",
		(u32)heapTop - (u32)HEAP_START, HEAP_START, heapTop);

	// 思考開始
	dm_think_flg = 1;

	// ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄﾊﾞｯﾌｧの指定
	gp = gfx_glist[gfx_gtask_no];

	while(loop_flg || st->curtain_count != CURTAIN_DOWN)
	{
		s16 *msgtype;
		int gs, joybak;
		int debug_flag =
			(game_state_data[0].mode_now == dm_mode_debug) ||
			(game_state_data[0].mode_now == dm_mode_debug_config);

		// リトレースメッセージ
		osRecvMesg(&msgQ,(OSMesg*)&msgtype, OS_MESG_BLOCK);

		if ( GameHalt ) {
			joyProcCore();
			graphic_no = GFX_NULL;
			dm_audio_update();
			continue;
		}

		// ＣＰＵ処理落ち
		if(!MQ_IS_EMPTY(&msgQ)) {
			nnp_over_time = 1;
		}

		if(*msgtype != NN_SC_RETRACE_MSG) {
			continue;
		}

		// メインループ脱出時にカーテンを閉じる
		if(!loop_flg) {
			if(st->curtain_step < 0) {
				st->curtain_step = -st->curtain_step;
			}
			continue;
		}

#ifdef NN_SC_PERF
		// パフォーマンスメーター表示処理
		if(_getKeyTrg(0) & Z_TRIG) {
			nnp_over_time = 0;
			_disp_meter_flag ^= 1;
		}
		nnScPushPerfMeter(NN_SC_PERF_MAIN);
#endif

		joybak = joyupd[0];
		for(gs = 0; loop_flg && gs < evs_gamespeed; gs++) {
			if(gs) joyupd[0] = 0;

			// 真のメイン
			gret = dm_game_main2();
			loop_flg = (gret == dm_ret_null);
		}
		joyupd[0] = joybak;

#ifdef NN_SC_PERF
		nnScPopPerfMeter();	// NN_SC_PERF_MAIN
#endif
		// デモ中はボリュームを下げる
		if(st->demo_flag) {
			dm_seq_set_volume(SEQ_VOL_LOW);
		}

		// サウンド処理の更新
		dm_audio_update();

		// パッド入力
		dm_game_graphic_onDoneSawp();

		if(GameHalt) {
			graphic_no = GFX_NULL;
		}
		else {
			graphic_no = GFX_GAME;
		}
	}

	// 思考停止
	dm_think_flg = 0;

	// グラフィックスレッドのプライオリティが元に戻るまで待つ
	st->graphic_thread_pri = GRAPHIC_THREAD_PRI;
	while(st->graphic_thread_pri) {
		osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
		dm_audio_update();
	}

	// 表示停止
	graphic_no = GFX_NULL;

	// 白で塗りつぶす
	memset(&fbuffer[wb_flag ^ 1], 0xff, SCREEN_WD * SCREEN_HT * 2);

	// シーケンスを停止
	dm_audio_stop();

	// グラフィック・オーディオタスク終了待ち
	while(!dm_audio_is_stopped() || pendingGFX) {
		osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
		dm_audio_update();
	}

	// 次にどの処理に飛ぶかを決定 & セーブデータを準備
	ret = dm_game_main3(gret);

	// CPUタスクスレッドを破棄
	while(cpuTask_getTaskCount()) {
		osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
	}
	cpuTask_destroyThread();

	// クライアントの抹消
	nnScRemoveClient(sched, &client);

	return ret;
}

//## ゲーム処理メイン関数2
static DM_GAME_RET dm_game_main2()
{
	Game *st = watchGame;
	DM_GAME_RET ret;
	game_state *state;
	bool finish = false;
	int i;

	if(st->bgm_bpm_flg[1]) {
		// hi tempo version のシーケンスに切り替える
		dm_seq_play_in_game((evs_seqnumb << 1) + 1);
		st->bgm_bpm_flg[1] = 0;
	}

	// 共通計算
	dm_effect_make();

	// EEPROM書き込み中の警告メッセージ
	RecWritingMsg_calc(&st->writingMsg);

	switch(evs_gamesel) {
	case GSL_1PLAY:

		// コーヒーブレーク中?
		if(st->coffee_break_flow > 0) {
			if(_getKeyLvl(0) & (DM_KEY_A | DM_KEY_B)) {
				st->msgWnd.scrSpeed = 1.0 / 8.0;
			}
			else {
				st->msgWnd.scrSpeed = 1.0 / 60.0;
			}

			msgWnd_update(&st->msgWnd);

			switch(st->coffee_break_flow) {
			// メッセージスクロール中
			case 1:
				if(_getKeyTrg(0) & START_BUTTON) {
					st->coffee_break_flow = 0;
				}
				else if(!msgWnd_isScroll(&st->msgWnd)) {
					st->coffee_break_flow = 2;
				}
				break;

			// 
			case 2:
				if(st->coffee_break_timer < 360) {
					st->coffee_break_timer++;
				}
				else if(_getKeyTrg(0) & DM_ANY_KEY) {
					st->coffee_break_flow = 0;
				}
				break;
			}

			if(st->coffee_break_flow == 0) {
				dm_seq_play_in_game(evs_seqnumb << 1);
			}
		}

		// コーヒーブレーク中?
		if(st->coffee_break_flow > 0) {
			ret = dm_ret_null;
			break;
		}

		ret = dm_game_main_1p();

		switch(ret) {
		// ステージクリア
		case dm_ret_next_stage:

			// コーヒーブレーク?
			i = game_state_data[0].virus_level;
			if(i == 21 || i == 24 || (i >= 30 && i % 5 == 0)) {
				st->coffee_break_flow = 1;
				st->coffee_break_level = (i < 30) ? 0 : (i < 40) ? 1 : 2;
				st->coffee_break_shard = (i < 30) ? ((i - 21) / 3) : ((i / 5) & 1);
			}

			if(st->coffee_break_flow == 1) {
				st->coffee_break_timer = 0;
				init_coffee_break_cnt();
				msgWnd_clear(&st->msgWnd);
				msgWnd_addStr(&st->msgWnd, st_staffroll_txt);
				msgWnd_skip(&st->msgWnd);
			}

			// 次のウィルスレベルへ
			if(game_state_data[0].virus_level < VIRUS_LEVEL_MAX) {
				game_state_data[0].virus_level++;
			}
			break;

		case dm_ret_retry:
			// スコアをゼロにする
			game_state_data[0].game_score = 0;
			break;

		case dm_ret_replay:
			ret = dm_ret_null;
			dm_game_init(1);
			break;
		}

		switch(ret) {
		case dm_ret_next_stage:
		case dm_ret_retry:
			ret = dm_ret_null;

			dm_game_init(1);
			animeState_set(&game_state_data[0].anime, ASEQ_DAMAGE);

			for(i = 0; i < 3; i++) {
				animeState_set(&st->virus_anime_state[i], ASEQ_NORMAL);
				animeSmog_stop(&st->virus_smog_state[i]);
			}

			// ステータスをバックアップ
			i = st->coffee_break_flow;
			st->coffee_break_flow = 0;
			backup_game_state(0);
			st->coffee_break_flow = i;

			if(st->coffee_break_flow > 0) {
				dm_seq_play_in_game(SEQ_Coffee);
			}
			break;
		}
		break;

	case GSL_2PLAY:
	case GSL_VSCPU:
		ret = dm_game_main_2p();

		// 決着が付いたか？
		for(i = 0; i < 2; i++) {
			if(evs_story_flg) {
				if(st->win_count[i] > 0) {
					finish = true;
				}
			}
			else {
				if(st->win_count[i] == evs_vs_count) {
					finish = true;
				}
			}
		}

		switch(ret) {
		case dm_ret_game_over:
			if(finish) break;

			ret = dm_ret_null;

			// スコアをゼロにする
			if(evs_gamemode == GMD_TIME_ATTACK) {
				for(i = 0; i < 2; i++) {
					game_state_data[i].game_score = 0;
				}
			}

			// 再試合
			dm_game_init(1);
			for(i = 0; i < 2; i++) {
				animeState_set(&game_state_data[i].anime, ASEQ_NORMAL);
			}

			// ステータスをバックアップ
			backup_game_state(0);
			break;

		// リトライの場合
		case dm_ret_retry:
			ret = dm_ret_null;

			// スコアをゼロにする
			for(i = 0; i < 2; i++) {
				game_state_data[i].game_score = 0;
			}

			// 再試合
			dm_game_init(0);
			for(i = 0; i < 2; i++) {
				animeState_set(&game_state_data[i].anime, ASEQ_NORMAL);
			}

			// ステータスをバックアップ
			backup_game_state(0);
			break;

		// リプレイの場合
		case dm_ret_replay:
			ret = dm_ret_null;
			dm_game_init(1);
			break;
		}
		break;

	case GSL_4PLAY:
		ret = dm_game_main_4p();

		// 決着が付いたか？
		for(i = 0; i < 4; i++) {
			if(evs_story_flg) {
				if(st->win_count[i] > 0) {
					finish = true;
				}
			}
			else {
				if(st->win_count[i] == evs_vs_count) {
					finish = true;
				}
			}
		}

		switch(ret) {
		case dm_ret_game_over:
			if(finish) break;

			ret = dm_ret_null;

			// 再試合
			dm_game_init(1);
			for(i = 0; i < 4; i++) {
				animeState_set(&game_state_data[i].anime, ASEQ_NORMAL);
			}

			// ステータスをバックアップ
			backup_game_state(0);
			break;

		case dm_ret_retry:
			ret = dm_ret_null;

			// スコアをゼロにする
			for(i = 0; i < 2; i++) {
				game_state_data[i].game_score = 0;
			}

			// 再試合
			dm_game_init(0);
			for(i = 0; i < 4; i++) {
				animeState_set(&game_state_data[i].anime, ASEQ_NORMAL);
			}

			// ステータスをバックアップ
			backup_game_state(0);
			break;

		case dm_ret_replay:
			ret = dm_ret_null;
			dm_game_init(1);
			break;
		}
		break;

	case GSL_1DEMO:
		ret = dm_game_demo_1p();
		break;

	case GSL_2DEMO:
		ret = dm_game_demo_2p();
		break;

	case GSL_4DEMO:
		ret = dm_game_demo_4p();
		break;
	}

	return ret;
}

//## 次にどの処理へ飛ぶか設定する
static MAIN_NO dm_game_main3(DM_GAME_RET gret)
{
	Game *st = watchGame;
	MAIN_NO ret;
	int i;

	switch(evs_gamesel) {
	case GSL_4PLAY:
		// ストーリーモード
		if(evs_story_flg) {
			evs_gamesel = GSL_VSCPU;
//			if(evs_one_game_flg) {
//				ret = MAIN_MENU;
//			}
//			else
			{
				if(game_state_data[0].cnd_static == dm_cnd_win && gret != dm_ret_game_end) {
					story_proc_no++;
					ret = MAIN_STORY;
				}
				// ゲームオーバー
				else {
					ret = MAIN_MENU;
				}
			}
		}
		else {
			ret = MAIN_MENU;
		}
		break;

	case GSL_2PLAY:
		ret = MAIN_MENU;
		break;

	case GSL_VSCPU:
//		// ストーリーモード && 1試合のみフラグ
//		if(evs_story_flg && evs_one_game_flg) {
//			ret = MAIN_MENU;
//		}
//		// ストーリーモード && !1試合のみフラグ
//		else if(evs_story_flg && !evs_one_game_flg) {
		if(evs_story_flg) {
			// 
			if(game_state_data[0].cnd_static == dm_cnd_win && gret != dm_ret_game_end) {
				if(dm_query_next_stage(evs_story_level, evs_story_no, game_state_data[0].game_retry)) {
					// 次のステージへ
//#define SHOW_CREDIT
#ifdef SHOW_CREDIT
                                        story_proc_no=EX_STAGE_NO + 1;
#else
					story_proc_no++;
#endif
				}
				else {
					// エンディング（のはず）
					story_proc_no += 2;
				}
				ret = MAIN_STORY;
			}
			else {
				// EXステージ
				if(evs_story_no == EX_STAGE_NO) {
					// メニューへ
					ret = MAIN_MENU;
				}
				else {
					// メニューへ
					ret = MAIN_MENU;
				}
			}

			// 裏技告知メッセージを設定
			if(story_proc_no == STORY_M_END || story_proc_no == STORY_M_END2
			|| story_proc_no == STORY_W_END || story_proc_no == STORY_W_END2)
			{
				switch(evs_story_level) {
				case 0:
					if(game_state_data[0].game_retry != 0) {
						EndingLastMessage = _mesEasyCont;
					}
					else {
						EndingLastMessage = _mesEasyNoCont;
					}
					break;

				case 1:
					if(game_state_data[0].game_retry != 0) {
						EndingLastMessage = _mesNormalCont;
					}
					else {
						EndingLastMessage = _mesNormalNoCont;
					}
					break;

				case 2:
					if(game_state_data[0].game_retry != 0) {
						EndingLastMessage = _mesHardCont;
					}
					else {
						EndingLastMessage = _mesHardNoCont;
					}
					break;

				case 3:
					if(game_state_data[0].game_retry != 0) {
						EndingLastMessage = _mesSHardCont;
					}
					else {
						EndingLastMessage = _mesSHardNoCont;
					}
					break;
				}
			}
		}
		else {
			ret = MAIN_MENU;
		}
		break;

	case GSL_1PLAY:
		ret = MAIN_MENU;
		break;

	case GSL_1DEMO:
	case GSL_2DEMO:
	case GSL_4DEMO:
		ret = MAIN_TITLE;
		break;
	}

	return ret;
}

//////////////////////////////////////////////////////////////////////////////
//{### スケジューラから呼ばれるグラフィック関数

//## ゲーム描画メイン関数
void dm_game_graphic()
{
	static void dm_game_graphic2();
	Game *st = watchGame;
	int i;

	if(st->graphic_thread_pri != OS_PRIORITY_APPMAX) {
		// プライオリティを元に戻す
		osSetThreadPri(NULL, GRAPHIC_THREAD_PRI);
		st->graphic_thread_pri = 0;
		return;
	}

	// EEPROM書き込み中の警告メッセージ
	RecWritingMsg_draw(&st->writingMsg, &gp);

	// ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄ作成終了
	gDPFullSync(gp++);
	gSPEndDisplayList(gp++);

	// ｸﾞﾗﾌｨｯｸﾀｽｸの開始
	osWritebackDCacheAll();
	gfxTaskStart(
		&gfx_task[gfx_gtask_no], gfx_glist[gfx_gtask_no],
		(s32)(gp - gfx_glist[gfx_gtask_no]) * sizeof(Gfx),
		GFX_GSPCODE_F3DEX,
		st->bg_snapping ? 0 : NN_SC_SWAPBUFFER);
//		NN_SC_SWAPBUFFER);

	// プライオリティを元に戻す
	osSetThreadPri(NULL, GRAPHIC_THREAD_PRI);

	// ディスプレイリストを構築
	dm_game_graphic2();

	// オーディオより高いプライオリティーに
	osSetThreadPri(NULL, OS_PRIORITY_APPMAX);
}

//## ゲーム描画メイン関数2
static void dm_game_graphic2()
{
	Game *st = watchGame;
	STexInfo *texC, *texA;
	Mtx *mp;
	Vtx *vp;
	int i, j, cached;
	int debug_flag =
		(game_state_data[0].mode_now == dm_mode_debug) ||
		(game_state_data[0].mode_now == dm_mode_debug_config);

	// ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄﾊﾞｯﾌｧの指定
	gp = gfx_glist[gfx_gtask_no];
	mp = dm_get_mtx_buf();
	vp = dm_get_vtx_buf();

	// RSP 初期設定
	gSPSegment(gp++, 0, 0x0);

	// フレームバッファの設定
	F3RCPinitRtn();
	gfxSetScissor(&gp, SCISSOR_FLAG, 0, 0, SCREEN_WD, SCREEN_HT);

	// フレームバッファのクリア
	F3ClearFZRtn(debug_flag);

	// 背景描画
	if(!debug_flag) {
//		if(st->coffee_break_flow == 0) {
//			dm_game_draw_snap_bg(&gp, &mp, &vp, TRUE);
//		}
		if(st->frame_move_count < FRAME_MOVE_MAX) {
			dm_game_draw_snap_bg(&gp, &mp, &vp, TRUE);
		}
		else if(st->coffee_break_flow == 0) {
			dm_game_draw_snapped_bg(&gp);
		}
	}

	// 瓶、パネルを描画
	switch(evs_gamesel) {
	case GSL_2PLAY: case GSL_VSCPU: case GSL_2DEMO: {
		int bak = st->frame_move_count;
		bool visBoard = st->frame_move_count < FRAME_MOVE_MAX;
		bool visScore = !st->bg_snapping;

		if(debug_flag) break;

		if(st->bg_snapping) {
			st->frame_move_count = FRAME_MOVE_MAX;
		}

		// ビンを描画
		dm_calc_bottle_2p();
		if(visBoard) {
			dm_draw_bottle_2p(&gp);
		}

		// パネルを描画
		i = (FRAME_MOVE_MAX - st->frame_move_count) * SCREEN_HT / 2 / FRAME_MOVE_MAX;
		if(evs_story_flg) {
			draw_story_board(&gp, 0, -i, visBoard, visScore);
		}
		else {
			draw_vsmode_board(&gp, 0, -i, visBoard, visScore);
		}

		st->frame_move_count = bak;
		} break;
	}

	// カプセルを描画
	switch(evs_gamesel) {
	case GSL_2PLAY: case GSL_VSCPU: case GSL_2DEMO:
	case GSL_4PLAY: case GSL_4DEMO:
		if(debug_flag) break;
		if(st->bg_snapping) break;

		for(i = 0; i < evs_playcnt; i++) {
			dm_virus_anime(&game_state_data[i], game_map_data[i]);
			dm_game_graphic_p(&game_state_data[i], i, game_map_data[i]);
		}
		break;
	}

	switch(evs_gamesel) {
	case GSL_1PLAY:
	case GSL_1DEMO:
		if(debug_flag) break;
		if(st->bg_snapping) break;

		disp_logo_setup(&gp);

		// コーヒーブレーク
		if(st->coffee_break_flow > 0) {
			draw_coffee_break(&gp, st->coffee_break_level, st->coffee_break_shard,
				st->coffee_break_flow > 1 ? 1 : 0);
			msgWnd_draw(&st->msgWnd, &gp);

			if(st->coffee_break_timer == 360) {
				push_any_key_draw(128, 192);
			}
			break;
		}

		// マリオを描画
		animeState_initDL(&game_state_data[0].anime, &gp);
		animeState_draw(&game_state_data[0].anime, &gp, MARIO_STAND_X, MARIO_STAND_Y, 1, 1);

		// カプセルを描画
		dm_virus_anime(&game_state_data[0], game_map_data[0]);
		dm_game_graphic_1p(&game_state_data[0], 0, game_map_data[0]);

		// コインを描画
		_disp_coin_logo(&gp, st->retry_coin);

		gSPDisplayList(gp++, alpha_texture_init_dl);

		// 最高得点を描画
		draw_count_number(&gp, 0, 7, evs_high_score, 29, 39);

		// 現在得点を描画
		draw_count_number(&gp, 0, 7, game_state_data[0].game_score, 29, 67);

		gSPDisplayList(gp++, alpha_texture_init_dl);

		switch(evs_gamemode) {
		// ウィルスレベル数を描画
		case GMD_NORMAL:
			draw_virus_number(&gp, game_state_data[0].virus_level, 254, 126, 1, 1);
			break;
		}

		switch(evs_gamemode) {
		// 時間を描画
		case GMD_NORMAL:
		case GMD_TaiQ:
			draw_time(&gp, evs_game_time, _posLsTime[0], _posLsTime[1]);
			break;

		// タイムアタックの場合カウントダウンタイムを描画
		case GMD_TIME_ATTACK:
			i = MAX(0, (int)SCORE_ATTACK_TIME_LIMIT - (int)evs_game_time);
			i += FRAME_PAR_MSEC - 1;
			draw_time2(&gp, i, _posLsTime[0], _posLsTime[1]);
			break;
		}

		// ウィルス数を描画
		switch(evs_gamemode) {
		case GMD_TaiQ:
			i = game_state_data[0].total_erase_count;
			break;
		default:
			i = game_state_data[0].virus_number;
			break;
		}
		draw_virus_number(&gp, i, 254, 210, 1, 1);

		// ウィルスキャラを描画
		dm_draw_big_virus(&gp);

		// WIN LOSE DRAW 連鎖数等の描画
		dm_game_graphic_effect(&game_state_data[0],0,0);

		// 
		if(st->big_virus_stop_count) {
			disp_timestop_logo(&gp, 0);
		}

		break;

	case GSL_2PLAY:
	case GSL_VSCPU:
	case GSL_2DEMO:
		if(debug_flag) break;
		if(st->bg_snapping) break;

		disp_logo_setup(&gp);
		gSPDisplayList(gp++, alpha_texture_init_dl);

		// ウィルス数を描画
		for(i = 0; i < 2; i++) {
			draw_virus_number(&gp, game_state_data[i].virus_number,
				_posP2VirusNum[i][0], _posP2VirusNum[i][1], 1, 1);
		}

		// 時間を描画
		if(evs_gamemode == GMD_TIME_ATTACK) {
			i = MAX(0, (int)SCORE_ATTACK_TIME_LIMIT - (int)evs_game_time);
			i += FRAME_PAR_MSEC - 1;
			draw_time2(&gp, i, 154, 167);
		}
		else {
			draw_time(&gp, evs_game_time, 154, 167);
		}

		// ストーリーモードの場合
		if(evs_story_flg) {
//			// パネルを描画
//			i = (FRAME_MOVE_MAX - st->frame_move_count) * SCREEN_HT / 2 / FRAME_MOVE_MAX;
//			draw_story_board(&gp, 0, -i, game_state_data[0].game_score, evs_story_level);

			// 星の下地
			for(i = cached = 0; i < ARRAY_SIZE(_posStStar); i++) {
				if(st->win_count[i] == 0) {
					draw_star_base(&gp, _posStStar[i][0], _posStStar[i][1], cached);
					cached++;
				}
			}
		}
		else {
//			// パネルを描画
//			i = (FRAME_MOVE_MAX - st->frame_move_count) * SCREEN_HT / 2 / FRAME_MOVE_MAX;
//			draw_vsmode_board(&gp, 0, -i);

			// 勝利数を描画
			for(i = 0; i < 2; i++) {
				static const int _x[] = { 119, 182 };
				draw_count_number(&gp, 0, 2, st->vs_win_total[i], _x[i], 152);
			}

			// 星の下地
			for(i = cached = 0; i < 2; i++) {
				for(j = st->win_count[i]; j < evs_vs_count; j++) {
					draw_star_base(&gp, _posP2StarX[i], _posP2StarY[evs_vs_count - 1][j], cached);
					cached++;
				}
			}
		}

		// FLASH
		if(evs_gamemode == GMD_FLASH) {
			texC = st->texP2 + TEX_bottom_flash_panel;
		}
		// VIRUS
		else {
			texC = st->texP2 + TEX_bottom_virus_panel;
		}
		texA = st->texP2 + TEX_bottom_virus_flash_panel_alpha;
		StretchAlphaTexBlock(&gp, texC->size[0], texC->size[1],
			texC->addr[1], texC->size[0], texA->addr[1], texA->size[0],
			131, 181, texC->size[0], texC->size[1]);

		// 星を描画
		starForce_calc(&st->starForce, st->star_count);
		starForce_draw(&st->starForce, &gp, st->star_count);

		// キャラアニメを描画
		for(i = 0; i < 2; i++) {
			static const dir[] = { -1, 1 };

			animeState_initDL(&game_state_data[i].anime, &gp);
			animeState_draw(&game_state_data[i].anime, &gp,
				_posP2CharFrm[i][0], _posP2CharFrm[i][1], dir[i], 1);
		}

		// WIN LOSE DRAW 連鎖数等の描画
		for(i = 0; i < 2; i++) {
			dm_game_graphic_effect(&game_state_data[i], i, 0);
		}

		gSPDisplayList(gp++, alpha_texture_init_dl);

		break;

	case GSL_4PLAY:
	case GSL_4DEMO:
		if(debug_flag) break;
		if(st->bg_snapping) break;

		// キャラアニメを描画
		for(i = 0; i < 4; i++) {
			animeState_initDL(&game_state_data[i].anime, &gp);
			animeState_draw(&game_state_data[i].anime, &gp,
				_posP4CharBase[i][0] + 20,
				_posP4CharBase[i][1] + 40, 1, 1);
		}

		disp_logo_setup(&gp);
		gSPDisplayList(gp++, alpha_texture_init_dl);

		// ウィルス数を描画
		for(i = 0; i < 4; i++) {
			draw_virus_number(&gp, game_state_data[i].virus_number,
				_posP4CharBase[i][0] + 50,
				_posP4CharBase[i][1] + 29, 1, 1);
		}

		// ストーリーモーの場合
		if(evs_story_flg) {
			// タイム
			draw_time(&gp, evs_game_time, 59, 18);

			// 星の下地
			for(i = cached = 0; i < ARRAY_SIZE(_posStP4StarX); i++) {
				if(st->win_count[i] == 0) {
					draw_star_base(&gp, _posStP4StarX[i], _posStP4StarY, cached);
					cached++;
				}
			}
		}
		// チーム対抗戦
		else if(st->vs_4p_team_flg) {
			static const int color2index[] = { 1, 0, 2 };
			int tx, ty, tw, th, color;

			// ストックカプセルを描画
			texC = st->texP4 + TEX_P4_TOPPANEL_STOCKLAMP;
			texA = st->texP4 + TEX_P4_TOPPANEL_STOCKLAMP_ALPHA;
			tw = texC->size[0] / 2;
			th = texC->size[1] / 3;

			for(i = 0; i < 2; i++) {
				for(j = 0; j < 4; j++) {
					color = st->story_4p_stock_cap[i][j];

					if(color < 0) {
						continue;
					}

					tx = tw * i;
					ty = th * color2index[color];

					StretchAlphaTexTile(&gp,
						texC->size[0], texC->size[1],
						texC->addr[1], texC->size[0],
						texA->addr[1], texA->size[0],
						tx, ty, tw, th,
						_posP4StockCap[i][0] + j * 9,
						_posP4StockCap[i][1],
						tw, th);
				}
			}

			// 星の下地
			for(i = cached = 0; i < 2; i++) {
				for(j = st->win_count[i]; j < evs_vs_count; j++) {
					draw_star_base(&gp, _posP4TeamStarX[evs_vs_count - 1][i][j], _posP4TeamStarY, cached);
					cached++;
				}
			}
		}
		// バトルロイヤル
		else {
			// 星の下地
			for(i = cached = 0; i < 4; i++) {
				for(j = st->win_count[i]; j < evs_vs_count; j++) {
					draw_star_base(&gp, _posP4CharStarX[evs_vs_count - 1][i][j], _posP4CharStarY, cached);
					cached++;
				}
			}
		}

		// 星を描画
		starForce_calc(&st->starForce, st->star_count);
		starForce_draw(&st->starForce, &gp, st->star_count);

		// WIN LOSE DRAW 連鎖数等の描画
		for(i = 0; i < 4; i++) {
			dm_game_graphic_effect(&game_state_data[i],i,1);
		}

		// 攻撃エフェクトを描画
		disp_attack_effect(&gp);

		break;
	}

	if(!st->bg_snapping) {
//	if(1) {
		// デバッグ画面を描画
		switch(game_state_data[0].mode_now) {
		case dm_mode_debug:
			DebugMode_Draw(&gp);
			DebugMode_Update();
			break;
		case dm_mode_debug_config:
			DebugConfig_Update();
			DebugConfig_Draw(&gp);
			break;
		}

		// デモ表示 (Dr.マリオ, DEMO PLAY, press any key)
		switch(evs_gamesel) {
		case GSL_1DEMO:
		case GSL_2DEMO:
		case GSL_4DEMO:
			draw_demo_logo(&gp, 106, 170);
			gDPPipeSync(gp++);
			break;
		}

		// リプレイロゴを表示
		if(st->replayFlag && !dm_check_one_game_finish()) {
			int x, y;

			switch(evs_gamesel) {
			case GSL_1PLAY:
			case GSL_4PLAY:
				x = 30; y = 20;
				break;

			case GSL_VSCPU:
			case GSL_2PLAY:
				x = 128; y = 144;
				break;
			}

			draw_replay_logo(&gp, x, y);
		}

		// パスワード
		if(st->passWnd.alpha > 0) {
			texC = st->texAL + TEX_popup_window;

			gSPDisplayList(gp++, normal_texture_init_dl);
			gDPSetCombineLERP(gp++,
				0,0,0,PRIMITIVE, TEXEL0,0,PRIMITIVE,0,
				0,0,0,PRIMITIVE, TEXEL0,0,PRIMITIVE,0);
			gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
			gDPSetPrimColor(gp++, 0,0, 255,255,200, st->passWnd.alpha);
			gDPSetTextureLUT(gp++, G_TT_NONE);

			gDPLoadTextureBlock_4b(gp++, texC->addr[1], G_IM_FMT_I, texC->size[0], texC->size[1], 0,
				G_TX_NOMIRROR, G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

			drawCursorPattern(&gp, texC->size[0], texC->size[1], 16, 16,
				st->passWnd.posX - 6, st->passWnd.posY - 6,
				st->passWnd.colStep * PASSWORD_LENGTH + 12,
				st->passWnd.rowStep * 3 + 12);

			msgWnd_update(&st->passWnd);
			msgWnd_draw(&st->passWnd, &gp);
		}
		st->passWnd.alpha = CLAMP(0, 255, st->passWnd.alpha + st->passAlphaStep);

		// カーテンを描画
		st->curtain_count = CLAMP(CURTAIN_UP, CURTAIN_DOWN, st->curtain_count + st->curtain_step);
		curtain_proc(&gp, st->curtain_count);
	}

#if defined(DEBUG)
	if(!st->bg_snapping && _disp_meter_flag) {
//	if(_disp_meter_flag) {
		for(i = 0; i < evs_playcnt; i++) {
			game_state *state = &game_state_data[i];
			int x, y, s;
			if(!PLAYER_IS_CPU(state, state->player_no)) continue;

			s = state->map_item_size >> 2;
			x = state->map_x + s + state->map_item_size * (state->ai.aiEX - 1);
			y = state->map_y + s + state->map_item_size * state->ai.aiEY;
			s = state->map_item_size - (s << 1);

			FillRectRGB(&gp, x, y, s, s, 255, 255, 255);
		}
	}
#endif

#ifdef NN_SC_PERF
	if(!st->bg_snapping && _disp_meter_flag) {
//	if(_disp_meter_flag) {
		nnScWritePerfMeter(&gp, 32, 180);
	}
#endif

	if(st->bg_snapping) {
		st->bg_snapping = false;
	}
}

//## スワップ直後の処理
void dm_game_graphic_onDoneSawp()
{
	Game *st = watchGame;
	OSMesg msg;
	OSMesgQueue msgQ;
	OSTimer timer;
	int i, wait, stepCnt;

#ifdef NN_SC_PERF
	nnScCountUpPerfMeter(NN_SC_PERF_OVER, NN_SC_PERF_INFO_Count);
#endif

	// キー入力までのウェイトを取得
	wait = JOYrTIMEvm;

	// タイマーからのメッセージを待つ
	wait -= OS_CYCLES_TO_USEC(osGetTime());
	if(wait > 50) {
		osCreateMesgQueue(&msgQ, &msg, 1);
		osSetTimer(&timer, OS_USEC_TO_CYCLES(wait), 0, &msgQ, 0);
		osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
	}

	// EEPRomへアクセス中はパッドを読まない
	if(st->eep_rom_flg) {
		for(i = 0; i < MAXCONTROLLERS; i++) {
			joynew[i] = joyold[i] = joyupd[i] = joycur[i] = 0;
		}
	}
	else {
		joyProcCore();
	}

#ifdef NN_SC_PERF
	nnScCountUpPerfMeter(NN_SC_PERF_CPU, NN_SC_PERF_INFO_Count);
#endif

	// キー操作
	for(stepCnt = 0; stepCnt < evs_gamespeed; stepCnt++) {
		bool pause = false;
		bool finish = dm_check_one_game_finish();

		dm_make_key();

		// ポーズ中?
		for(i = 0; i < evs_playcnt; i++) {
			if(game_state_data[i].cnd_static == dm_cnd_pause) {
				pause = true;
				break;
			}
		}

		if(!pause && st->started_game_flg) {
			aiCOM_MissTake();
		}

		// リプレーデータを取得
		if(st->replayFlag && !pause && !finish && st->started_game_flg) {
			for(i = 0; i < evs_playcnt; i++) {
				joygam[i] = replay_play(i);
			}
			// リプレイバッファーがあふれたか?
			for(i = 0; i < evs_playcnt; i++) {
				if(joygam[i] & CONT_START) {
					break;
				}
			}
			if(i < evs_playcnt) {
				dm_seq_set_volume(SEQ_VOL_LOW);
				resume_game_state(1);
			}
		}

		// ウィルス回転処理
		i = (L_TRIG | R_TRIG);
		st->big_virus_no_wait = (evs_gamemode == GMD_TaiQ) && (joygam[0] & i);

		switch(evs_gamesel) {
		case GSL_1PLAY:
			key_control_main(&game_state_data[0], game_map_data[0], 0, main_joy[0]);
			break;

		case GSL_VSCPU:
		case GSL_2PLAY:
			for(i = 0; i < 2; i++) {
				key_control_main(&game_state_data[i], game_map_data[i], i, main_joy[i]);
			}
			break;

		default:
			for(i = 0; i < 4; i++) {
				key_control_main(&game_state_data[i], game_map_data[i], i, main_joy[i]);
			}
			break;
		}

		// リプレイ用キーデータを出力
		if(!st->replayFlag && !pause) {
			for(i = 0; i < evs_playcnt; i++) {
				replay_record(i, joygam[i]);
			}
		}
	}

#ifdef NN_SC_PERF
	nnScCountUpPerfMeter(NN_SC_PERF_OVER, NN_SC_PERF_INFO_Count);
#endif
}

//## タスク完了直後の処理
void dm_game_graphic_onDoneTask()
{
}

//////////////////////////////////////////////////////////////////////////////
//{### ストーリーモード all clear 後のメッセージ

//## メイン
int main_techmes(void *arg)
{
	Game *st;
	OSMesgQueue msgQ;
	OSMesg msgbuf[MAX_MESGS];
	NNScClient client;
	bool loop = true;
	int i;

	// メッセージキューの作成
	osCreateMesgQueue(&msgQ, msgbuf, MAX_MESGS);

	// クライアントに登録
	nnScAddClient((NNSched *)arg, &client, &msgQ);

	// ヒープを初期化
	dm_game_init_heap();
	st = watchGame;

	// グラフィックデータの読込み(背景等)
	dm_game_init_static();

	// 背景
	heapTop = (void *)init_menu_bg((u32)heapTop, 0);

	// メッセージ
	msgWnd_init2(&st->msgWnd, &heapTop, 0x1000, 18, 16, 52, 34);
	msgWnd_addStr(&st->msgWnd, EndingLastMessage);
	st->msgWnd.fontType = 1;
#if LOCAL==AMERICA
	st->msgWnd.centering = 1;
#elif LOCAL==CHINA
	st->msgWnd.centering = 1;
#endif

	// ゲームの異初期化
	i = evs_seqence;
	evs_seqence = 0;
	dm_game_init(0);
	evs_seqence = i;
	dm_seq_play_in_game(SEQ_Coffee);

	while(loop) {
		osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
		joyProcCore();

		if ( GameHalt ) {
			graphic_no = GFX_NULL;
			dm_audio_update();
			continue;
		}

		switch(st->coffee_break_flow) {
		case 0:
			if(st->coffee_break_timer == 255) {
				st->coffee_break_flow++;
			}
			else {
				st->coffee_break_timer = MIN(255, st->coffee_break_timer + 4);
			}
			break;
		case 1:
			if(msgWnd_isEnd(&st->msgWnd)) {
				st->coffee_break_flow++;
			}
			else {
				msgWnd_update(&st->msgWnd);
			}
			break;
		case 2:
			if(_getKeyTrg(0) & DM_ANY_KEY) {
				st->coffee_break_flow++;
			}
			break;
		case 3:
			if(st->coffee_break_timer == 0) {
				loop = false;
			}
			else {
				st->coffee_break_timer = MAX(0, st->coffee_break_timer - 4);
			}
			break;
		}

		dm_audio_update();
		graphic_no = GFX_TECHMES;
	}

	// 描画処理を停止
	graphic_no = GFX_NULL;

	// シーケンスを停止
	dm_audio_stop();

	// グラフィック・オーディオタスク終了待ち
	while(!dm_audio_is_stopped() || pendingGFX) {
		osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
		dm_audio_update();
	}

	// クライアントの抹消
	nnScRemoveClient((NNSched *)arg, &client);

	return MAIN_TITLE;
}

//## 描画
void graphic_techmes()
{
	Game *st = watchGame;
	Mtx *mp;
	Vtx *vp;
	int i;

	gp = gfx_glist[gfx_gtask_no];
	mp = dm_get_mtx_buf();
	vp = dm_get_vtx_buf();

	F3RCPinitRtn();
	F3ClearFZRtn(0);

	gDPSetEnvColor(gp++, 0,0,0, 255);
	gDPSetPrimColor(gp++, 0,0, 255,255,255, 255);

//	draw_ending_mess_bg(&gp);
	draw_menu_bg(&gp, 0, -SCREEN_HT / 2);

	dm_draw_KaSaMaRu(&gp, &mp, &vp, msgWnd_isSpeaking(&st->msgWnd), 200, 128, 1, 255);

	msgWnd_draw(&st->msgWnd, &gp);

	switch(st->coffee_break_flow) {
	case 2:
		push_any_key_draw(128, 192);
		break;
	}

	FillRectRGBA(&gp, 0,0,SCREEN_WD,SCREEN_HT, 0,0,0, 255 - st->coffee_break_timer);

	st->blink_count++;

	gDPFullSync(gp++);
	gSPEndDisplayList(gp++);
	osWritebackDCacheAll();

	gfxTaskStart(&gfx_task[gfx_gtask_no], gfx_glist[gfx_gtask_no],
		(s32)(gp - gfx_glist[gfx_gtask_no]) * sizeof(Gfx),
		GFX_GSPCODE_F3DEX, NN_SC_SWAPBUFFER);
}

//////////////////////////////////////////////////////////////////////////////
//{### EOF
