
#ifndef __dm_game_def_h__
#define	__dm_game_def_h__

#include "aidef.h"
#include "char_anime.h"

//////////////////////////////////////////////////////////////////////////////
// 宣言

#define cap_flg_on				1
#define cap_flg_off				0

#define cap_size_10				10		// カプセル通常サイズ
#define cap_size_8				8		// カプセル４Ｐサイズ

#define cap_size_big			0		// 通常サイズ
#define cap_size_small			1		// ４Ｐサイズ

#define cap_disp_flg			0		// 表示フラグ
#define cap_down_flg			1		// 落下フラグ
#define cap_condition_flg		2		// 状態フラグ
#define cap_col_flg				3		// 色フラグ
#define cap_virus_flg			4		// ウイルスの番号

#define cap_map_flg				2		// マップ番号フラグ
#define cap_throw_flg			2		// 投げ状態フラグ

#define dm_retire_flg			0		// リタイアフラグ
#define dm_game_over_flg		1		// 黒上がり終了確認フラグ
#define dm_training_flg			2		// トレーニングフラグ
#define dm_static_cnd			3		// 絶対的コンディション


#define cap_turn_r				1		// 時計回り(右移動)
#define cap_turn_l				-1		// 反時計回り(左移動)

#define dm_mode_now				0		// 現在の配列番号を指す
#define dm_mode_old				1		// 保存用の配列番号を指す

typedef enum {
	dm_cnd_null,
	dm_cnd_wait,			// 何もしない待ち状態
	dm_cnd_init,			// 初期化
	dm_cnd_stage_clear,		// ステージクリア
	dm_cnd_game_over,		// ゲームオーバー
//	dm_cnd_game_over_retry,	// ゲームオーバーでリトライ待ち
	dm_cnd_win,				// WINのみ
	dm_cnd_win_retry,		// WINでリトライ待ち
	dm_cnd_win_retry_sc,	// WIN & リトライ & 得点
//	dm_cnd_win_any_key,		// WINでキー待ち
//	dm_cnd_win_any_key_sc,	// WIN & ANY KEY & 得点
	dm_cnd_lose,			// LOSEのみ
	dm_cnd_lose_retry,		// LOSEでリトライ待ち
	dm_cnd_lose_retry_sc,	// LOSE & リトライ & 得点
//	dm_cnd_lose_sc,			// LOSE & 得点
//	dm_cnd_lose_any_key,	// LOSEでキー待ち
	dm_cnd_draw,			// DRAWのみ
	dm_cnd_draw_retry,		// DRAWのみ & リトライ
//	dm_cnd_draw_any_key,	// DRAWでキー待ち
	dm_cnd_pause,			// ポーズ
	dm_cnd_pause_re,		// ポーズ & リトライ
	dm_cnd_pause_re_sc,		// ポーズ & リトライ & 得点
//	dm_cnd_pause_tar,		// ポーズ & 攻撃相手
//	dm_cnd_pause_tar_re,	// ポーズ & 攻撃相手 & リトライ
//	dm_cnd_pause_tar_re_sc,	// ポーズ & 攻撃相手 & リトライ & 得点
	dm_cnd_retire,			// リタイア
	dm_cnd_tr_chack,		// 練習確認
	dm_cnd_training,		// 練習
//	dm_cnd_manual,			// 操作説明
	dm_cnd_clear_wait,
	dm_cnd_clear_result,
	dm_cnd_gover_wait,
	dm_cnd_gover_result,
	dm_cnd_retire_wait,
	dm_cnd_retire_result,
	dm_cnd_debug_config,	// コンフィグ
	dm_cnd_debug,			// デバック
} DM_GAME_CONDITION;

typedef enum {
	dm_mode_null,
	dm_mode_init,			// 初期化
//	dm_mode_4p_story,		// 4P ストーリー時の顔表示
	dm_mode_wait,			// 待ち
	dm_mode_throw,			// 投げ
	dm_mode_down,			// カプセル落下
	dm_mode_down_wait,		// 着地ウェイト
	dm_mode_erase_chack,	// 消滅判定
	dm_mode_erase_anime,	// 消滅アニメ
	dm_mode_ball_down,		// 消滅後の落下
	dm_mode_cap_set,		// 新カプセルセット
//	dm_mode_get_coin,		// クリアデモ & コインデモ
	dm_mode_stage_clear,	// クリアデモ
	dm_mode_game_over,		// ゲームオーバーデモ
//	dm_mode_game_over_retry,// ゲームオーバーデモ & リトライ
//	dm_mode_use_coin,		// ゲームオーバーデモ & リトライ & コインデモ
//	dm_mode_black_up,		// 下から暗くなる処理
	dm_mode_bottom_up,		// 瓶の中身が一段上がる処理
	dm_mode_win,			// WINデモ
	dm_mode_win_retry,		// WINデモ ＆リトライ
	dm_mode_lose,			// LOSEデモ
	dm_mode_lose_retry,		// LOSEデモ＆リトライ
	dm_mode_draw,			// DRAWデモ
	dm_mode_draw_retry,		// DRAWデモ＆リトライ
	dm_mode_tr_chaeck,		// 練習判断
	dm_mode_training,		// 練習初期設定
	dm_mode_tr_erase_chack,	// 練習用消滅判定
	dm_mode_tr_cap_set,		// 練習用新カプセルセット
	dm_mode_pause,			// ポーズ
	dm_mode_pause_retry,	// ポーズ&リトライ
	dm_mode_no_action,		// ポーズ&リトライ
	dm_mode_clear_wait,
	dm_mode_clear_result,
	dm_mode_gover_wait,
	dm_mode_gover_result,
	dm_mode_retire_wait,
	dm_mode_retire_result,
	dm_mode_debug_config,	// コンフィグ
	dm_mode_debug,			// デバック
} DM_GAME_MODE;

typedef enum {
	dm_ret_game_end   =  -2, // 完全終了
	dm_ret_game_over  =  -1, // ゲームオーバー
	dm_ret_null       =   0,
	dm_ret_next_stage =   1, // 次のステージへ
	dm_ret_retry      =   2, // 次のステージへ
	dm_ret_virus_wait =   3, // ウイルス配置終了
	dm_ret_pause      =   4, // ポーズ
	dm_ret_black_up   =   5, // 下から暗くなる処理終了
	dm_ret_clear      =   6, // クリア
	dm_ret_tr_a       =   7, // 練習攻撃無し
	dm_ret_tr_b       =   8, // 練習攻撃あり
	dm_ret_replay     =   9, // リプレイ
	dm_ret_end        = 100, // ループ脱出
} DM_GAME_RET;

#if 0 // 削除
/*
#define dm_1_VS_3				0x0e	// 判定用 1 対 1 のとき
#define dm_2_VS_2				0x0c	// 判定用 2 対 2 のとき
#define dm_3_VS_1				0x08	// 判定用 3 対 1のとき
*/
#endif

#define v_anime_def_speed		8		// ウイルスアニメーション速度
#define v_anime_def_speed_4p	12		// ウイルスアニメーション速度(4P)
#define v_anime_speed			1		// ウイルスアニメーション速度(ゲームオーバー時)
#define v_anime_speed_4p		4		// ウイルスアニメーション速度(4P)(ゲームオーバー時)

#define dm_bound_wait			18		// 着地ウェイト
#define dm_erace_speed_1		10		// ウイルスの消滅アニメーションの１段階目までのカウント
#define dm_erace_speed_2		27		// ウイルスの消滅アニメーションの２段階目までのカウント
#define dm_down_speed			14		// 攻撃カプセルと粒上カプセルの落下速度
#define dm_black_up_speed		6		// 下から黒くなる処理の速度

#define dm_wold_x				118		// 1P用基準Ｘ座標
#define dm_wold_y				46		// 1P用基準Ｙ座標

#if 0 // 削除
/*
#define dm_next_y				36		// ネクストカプセルのＹ座標(1P・2P)
*/
#endif

#define dm_wold_x_vs_1p			28		// 2P用 1P 基準Ｘ座標
#define dm_wold_x_vs_2p			212		// 2P用 2P 基準Ｘ座標

#define dm_wold_x_4p			20		// 4P用基準Ｘ座標
#define dm_wold_y_4p			46		// 4P用基準Ｙ座標

#if 0 // 削除
/*
#define dm_next_y_4p			44		// 4PネクストカプセルのＹ座標
*/
#endif

#define dm_wold_x_4p_1p			20		// 4P用 1P 基準Ｘ座標
#define dm_wold_x_4p_2p			92		// 4P用 2P 基準Ｘ座標
#define dm_wold_x_4p_3p			164		// 4P用 3P 基準Ｘ座標
#define dm_wold_x_4p_4p			236		// 4P用 4P 基準Ｘ座標

enum {
	capsel_u,				// カプセル	上
	capsel_d,				// 			下
	capsel_l,				// 			左
	capsel_r,				// 			右
	capsel_b,				// 			ボール
	erase_cap_a,			// 			消滅１
	erase_cap_b,			// 			消滅２
	virus_a1,				// ウイルス	a1
	virus_a2,				// 			a2
	virus_b1,				// 			b1
	virus_b2,				// 			b2
	virus_c1,				// 			c1
	virus_c2,				// 			c2
	erase_virus_a,			// 			消滅１
	erase_virus_b,			// 			消滅２
	no_item,				// 何も無し
	virus_a3,				// 			a3
	virus_a4,				// 			a4
	virus_b3,				// 			b3
	virus_b4,				// 			b4
	virus_c3,				// 			c3
	virus_c4				// 			c4
};

enum {
	capsel_red,				// カプセル	赤
	capsel_yellow,			// 			黄
	capsel_blue,			// 			青
	capsel_b_red,			// 		暗い赤
	capsel_b_yellow,		// 		暗い黄
	capsel_b_blue,			// 		暗い青
};

//////////////////////////////////////////////////////////////////////////////
// ゲーム情報用構造体

// 瓶の中の情報サイズ
#define GAME_MAP_W 8
#define GAME_MAP_H 16

typedef struct {
	s8	pos_m_x,pos_m_y;	// 表示座標(8 x 16)
	s8	capsel_m_g;			// カプセルのグラフィック番号
	s8	capsel_m_p;			// カプセルのパレット番号
	s8	capsel_m_flg[6];	// カプセルの表示フラグと落下判定フラグ
} game_map;

// 落下カプセル用構造体
typedef struct {
	s8	pos_x[2],pos_y[2];	// 表示座標(8 x 16)
	s8	capsel_g[2];		// カプセルのグラフィック番号
	s8	capsel_p[2];		// カプセルのパレット番号
	s8	capsel_flg[4];		// カプセルの表示フラグと落下判定フラグ
} game_cap;

// 攻撃カプセル用構造体
typedef struct {
	s8	pos_a_x,pos_a_y;	// 表示座標(8 x 16)
	s8	capsel_a_p;			// カプセルのパレット番号
	s8	capsel_a_flg[3];	// カプセルの表示フラグと落下判定フラグ
} game_a_cap;

// ゲームの制御用構造体
typedef struct {
	u32 game_score; // 得点
	u16 game_retry; // リトライ

	s16 map_x, map_y;  // マップの座標
	s8  map_item_size; // マップのアイテムの大きさ

	DM_GAME_MODE mode_now; // 内部処理番号
	DM_GAME_MODE mode_old; // 内部処理番号保存変数

	DM_GAME_CONDITION cnd_now;      // その時の状態
	DM_GAME_CONDITION cnd_old;      // その時の状態保存変数
	DM_GAME_CONDITION cnd_training; // 変化しない状況変数(トレーニング中だと０以外)
	DM_GAME_CONDITION cnd_static;   // 不変的状態 win lose pause wait

	u8 virus_order_number;// 配置中のウィルス番号
	u8 virus_number;      // ウイルス数
	u8 virus_level;       // ウイルスレベル
	u8 virus_anime;       // ウイルスアニメーション番号
	s8 virus_anime_vec;   // ウイルスアニメーション進行方向
	u8 virus_anime_count; // ウイルスアニメーションカウンタ
	u8 virus_anime_max;   // ウイルスアニメーションの最大コマ数
	u8 virus_anime_spead; // ウイルスアニメーション速度

	u8 cap_def_speed; // カプセル速度(SPEED_?(LOW/MID/HIGH/MAX))
		#define SPEED_LOW  0
		#define SPEED_MID  1
		#define SPEED_HIGH 2
		#define SPEED_MAX  3
	u8 cap_speed;         // カプセル速度
	u8 cap_count;         // カプセル落下数
	u8 cap_speed_count;   // カプセル落下用カウンタ
	u8 cap_speed_vec;     // カプセル落下用カウンタ増加値
	u8 cap_speed_max;     // カプセル落下速度(カウンタがこの数値以上になると１段落下)
	u8 cap_magazine_cnt;  // カプセルマガジン参照用変数
	u8 cap_magazine_save; // カプセルマガジン参照用保存変数
	s8 cap_move_se_flg;   // カプセル左右移動時のＳＥを鳴らすためのフラグ

	u8 erase_anime;       // 消滅アニメーションコマ数
	u8 erase_anime_count; // 消滅アニメーションカウンタ, 粒落下のカウンタも兼ねる
	u8 erase_virus_count; // 消滅ウイルス数
	u8 erase_virus_count_old;

	u8 chain_count;    // 連鎖数
	u8 chain_line;     // 消滅列数
	u8 chain_line_max; // 最大消滅列数
	u8 chain_color[4];
		// 0: 赤
		// 1: 黄
		// 2: 青 消した色をカウントする。
		// 3: 連鎖開始時に消した色のビットを立てる
		//    0x01:赤
		//    0x02:黄
		//    0x04:青
		//    ウイルス含む場合 0x80 のビットを立てる

	// 黒上がり処理カウンタ
	u8 black_up_count;

	// 警戒音を鳴らすためのカウンタ
	int warning_se_flag;

	u8 flg_retire;    // リタイアフラグ
	u8 flg_game_over; // ゲームオーバー
	u8 flg_training;  // トレーニングフラグ

	u8 player_no;   // プレイヤーの番号
	u8 player_type; // プレイヤーの種類
		#define PUF_PlayerMAN  0
		#define PUF_PlayerCPU  1
	u8 think_type;  // 敵思考キャラ
	u8 think_level; // ＣＰＵレベル
		#define PMD_CPU0 0
		#define PMD_CPU1 1
		#define PMD_CPU2 2
	u8 team_no;     // チーム番号
		#define TEAM_MARIO  0
		#define TEAM_ENEMY  1
		#define TEAM_ENEMY2 2
		#define TEAM_ENEMY3 3

	// 0:自分に落下する攻撃カプセルのワーク(2bitで制御,01:赤,10:黄,11:青,00:無し)
	// 1:攻撃してきた相手
	#define DAMAGE_MAX 0x10
	u16 cap_attack_work[DAMAGE_MAX][2];

	int         charNo;   // キャラクタ番号
	SAnimeState anime;    // アニメーション制御

	// フラッシュウィルスの位置 [0]:X, [1]:Y, [2]:COLOR
	#define FLASH_VIRUS_MAX 9
	int flash_virus_pos[FLASH_VIRUS_MAX][3];
	int flash_virus_bak[FLASH_VIRUS_MAX];
	int flash_virus_count;

	// 瓶の中身を一段上げる時のカウンタ
	int bottom_up_scroll;

	// "フラッシュ", "耐久", "タイアタ" 時の難易度
	int game_level;

	int total_erase_count; // 現在のウィルス消し数
	int total_chain_count; // 合計連鎖回数

	game_cap    now_cap;  // 現在操作するカプセル
	game_cap    next_cap; // 次のカプセル

	AIWORK ai; // COM思考用ﾜｰｸ
	u8     pn; // ﾌﾟﾚｲﾔｰ no.
	u8     gs; // ｹﾞｰﾑｽﾃｰﾀｽ
	u8     lv; // 設定ﾚﾍﾞﾙ(0-29)
	u8     vs; // ｳｲﾙｽ残数(ｶｳﾝﾀではなく、毎ｲﾝﾄごとにﾏｯﾌﾟﾃﾞｰﾀ上から探した数字が入る)

	AIBLK  blk[MFieldY+1][MFieldX]; //ﾌﾞﾛｯｸﾜｰｸ
	AICAPS cap;
} game_state;

// ウイルス配置用構造体
typedef struct {
	s8 virus_type;  // ウイルスの種類(色も兼ねる)
	u8 x_pos,y_pos; // Ｘ・Ｙ座標
} virus_map;

//////////////////////////////////////////////////////////////////////////////

#endif // __dm_game_def_h__
